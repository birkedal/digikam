/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2011-08-03
 * Description : digital camera thumbnails controller
 *
 * Copyright (C) 2011-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "camerathumbsctrl.h"

// Qt includes

#include <QCache>
#include <QPair>
#include <QUrl>

// KDE includes

#include <kio/previewjob.h>

// Local includes

#include "digikam_debug.h"
#include "cameracontroller.h"
#include "thumbnailsize.h"
#include "iccsettings.h"
#include "iccmanager.h"
#include "iccprofile.h"

namespace Digikam
{

class CameraThumbsCtrlStaticPriv
{
public:

    CameraThumbsCtrlStaticPriv()
    {
        profile = IccProfile::sRGB();
    }

public:

    IccProfile profile;
};

Q_GLOBAL_STATIC(CameraThumbsCtrlStaticPriv, static_d)

// ------------------------------------------------------------------------------------------

class CameraThumbsCtrl::Private
{

public:

    Private()
        : controller(0),
          kdeJob(0)
    {
    }

    QCache<QUrl, CachedItem> cache;  // Camera info/thumb cache based on item url keys.

    QList<QUrl>               pendingItems;

    CameraController*        controller;

    QList<CamItemInfo>       kdeTodo;
    QHash<QUrl, CamItemInfo> kdeJobHash;
    KIO::PreviewJob*         kdeJob;
};

// --------------------------------------------------------

CameraThumbsCtrl::CameraThumbsCtrl(CameraController* const ctrl, QWidget* const parent)
    : QObject(parent), d(new Private)
{
    d->controller     = ctrl;
    static_d->profile = IccManager::displayProfile(parent);

    connect(d->controller, SIGNAL(signalThumbInfo(QString,QString,CamItemInfo,QImage)),
            this, SLOT(slotThumbInfo(QString,QString,CamItemInfo,QImage)));

    connect(d->controller, SIGNAL(signalThumbInfoFailed(QString,QString,CamItemInfo)),
            this, SLOT(slotThumbInfoFailed(QString,QString,CamItemInfo)));

    setCacheSize(200);
}

CameraThumbsCtrl::~CameraThumbsCtrl()
{
    clearCache();
}

CameraController* CameraThumbsCtrl::cameraController() const
{
    return d->controller;
}

bool CameraThumbsCtrl::getThumbInfo(const CamItemInfo& info, CachedItem& item) const
{
    if (hasItemFromCache(info.url()))
    {
        // We look if items are not in cache.

        item = *retrieveItemFromCache(info.url());

        // Color Managed view rules.

        if (IccSettings::instance()->useManagedPreviews())
        {
            QImage img  = item.second.toImage();
            IccManager::transformForDisplay(img, static_d->profile);
            item.second = QPixmap::fromImage(img);
        }

        return true;
    }
    else if (!d->pendingItems.contains(info.url()))
    {
        // We look if items are not in pending list.

        d->pendingItems << info.url();
        d->controller->getThumbsInfo(CamItemInfoList() << info, ThumbnailSize::maxThumbsSize());
    }

    item = CachedItem(info, d->controller->mimeTypeThumbnail(info.name).pixmap(ThumbnailSize::maxThumbsSize()));

    return false;
}

void CameraThumbsCtrl::updateThumbInfoFromCache(const CamItemInfo& info)
{
    removeItemFromCache(info.url());
    CachedItem item;
    getThumbInfo(info, item);
}

void CameraThumbsCtrl::slotThumbInfo(const QString&, const QString& file, const CamItemInfo& info, const QImage& thumb)
{
    QImage thumbnail = thumb;

    if (thumb.isNull())
    {
        thumbnail = d->controller->mimeTypeThumbnail(file).pixmap(ThumbnailSize::maxThumbsSize()).toImage();
    }

    putItemToCache(info.url(), info, QPixmap::fromImage(thumbnail));
    d->pendingItems.removeAll(info.url());
    emit signalThumbInfoReady(info);
}

void CameraThumbsCtrl::slotThumbInfoFailed(const QString& /*folder*/, const QString& file, const CamItemInfo& info)
{
    if (d->controller->cameraDriverType() == DKCamera::UMSDriver)
    {
        putItemToCache(info.url(), info, QPixmap());
        loadWithKDE(info);
    }
    else
    {
        QPixmap pix = d->controller->mimeTypeThumbnail(file).pixmap(ThumbnailSize::maxThumbsSize());
        putItemToCache(info.url(), info, pix);
        d->pendingItems.removeAll(info.url());
        emit signalThumbInfoReady(info);
    }
}

void CameraThumbsCtrl::loadWithKDE(const CamItemInfo& info)
{
    d->kdeTodo << info;
    startKdePreviewJob();
}

void CameraThumbsCtrl::startKdePreviewJob()
{
    if (d->kdeJob || d->kdeTodo.isEmpty())
    {
        return;
    }

    d->kdeJobHash.clear();
    QList<QUrl> list;

    foreach(const CamItemInfo& info, d->kdeTodo)
    {
        QUrl url = info.url();
        list << url;
        d->kdeJobHash[url] = info;
    }

    d->kdeTodo.clear();

    KFileItemList items;

    for (QList<QUrl>::iterator it = list.begin() ; it != list.end() ; ++it)
    {
        if ((*it).isValid())
            items.append(KFileItem(*it));
    }

    d->kdeJob = KIO::filePreview(items, QSize(ThumbnailSize::Huge, ThumbnailSize::Huge));

    connect(d->kdeJob, SIGNAL(gotPreview(KFileItem,QPixmap)),
            this, SLOT(slotGotKDEPreview(KFileItem,QPixmap)));

    connect(d->kdeJob, SIGNAL(failed(KFileItem)),
            this, SLOT(slotFailedKDEPreview(KFileItem)));

    connect(d->kdeJob, SIGNAL(finished(KJob*)),
            this, SLOT(slotKdePreviewFinished(KJob*)));
}

void CameraThumbsCtrl::slotGotKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    procressKDEPreview(item, pix);
}

void CameraThumbsCtrl::slotFailedKDEPreview(const KFileItem& item)
{
    procressKDEPreview(item, QPixmap());
}

void CameraThumbsCtrl::procressKDEPreview(const KFileItem& item, const QPixmap& pix)
{
    CamItemInfo info = d->kdeJobHash.value(item.url());
    QUrl url         = info.url();

    if (info.isNull())
    {
        return;
    }

    QString file   = item.url().fileName();
    QString folder = item.url().toLocalFile().remove(QString("/") + file);
    QPixmap thumb;

    if (pix.isNull())
    {
        // This call must be run outside Camera Controller thread.
        thumb = d->controller->mimeTypeThumbnail(file).pixmap(ThumbnailSize::maxThumbsSize());
        qCDebug(LOG_IMPORTUI) << "Failed thumb from KDE Preview : " << item.url();
    }
    else
    {
        thumb = pix;
        qCDebug(LOG_IMPORTUI) << "Got thumb from KDE Preview : " << item.url();
    }

    putItemToCache(url, info, thumb);
    d->pendingItems.removeAll(url);
    emit signalThumbInfoReady(info);
}

void CameraThumbsCtrl::slotKdePreviewFinished(KJob*)
{
    d->kdeJob = 0;
    startKdePreviewJob();
}

// -- Cache management methods ------------------------------------------------------------

const CachedItem* CameraThumbsCtrl::retrieveItemFromCache(const QUrl& url) const
{
    return d->cache[url];
}

bool CameraThumbsCtrl::hasItemFromCache(const QUrl& url) const
{
    return d->cache.contains(url);
}

void CameraThumbsCtrl::putItemToCache(const QUrl& url, const CamItemInfo& info, const QPixmap& thumb)
{
    int infoCost  = sizeof(info);
    int thumbCost = thumb.width() * thumb.height() * thumb.depth() / 8;
    d->cache.insert(url, new CachedItem(info, thumb), infoCost + thumbCost);
}

void CameraThumbsCtrl::removeItemFromCache(const QUrl& url)
{
    d->cache.remove(url);
}

void CameraThumbsCtrl::clearCache()
{
    d->cache.clear();
}

void CameraThumbsCtrl::setCacheSize(int numberOfItems)
{
    d->cache.setMaxCost((numberOfItems * ThumbnailSize::maxThumbsSize() * ThumbnailSize::maxThumbsSize() * QPixmap::defaultDepth() / 8) +
                        (numberOfItems * 1024 * 2));
}

}  // namespace Digikam
