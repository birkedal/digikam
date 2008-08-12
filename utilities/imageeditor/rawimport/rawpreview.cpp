/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-08-04
 * Description : RAW preview widget.
 *
 * Copyright (C) 2008 Gilles Caulier <caulier dot gilles at gmail dot com>
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

// Qt includes.

#include <qstring.h>
#include <qpainter.h>
#include <qtoolbutton.h>
#include <qtooltip.h>
#include <qpixmap.h>
#include <qfileinfo.h>

// KDE includes.

#include <klocale.h>
#include <kcursor.h>
#include <kdatetbl.h>
#include <kiconloader.h>

// Local includes.

#include "ddebug.h"
#include "paniconwidget.h"
#include "managedloadsavethread.h"
#include "loadingdescription.h"
#include "themeengine.h"
#include "rawpreview.h"
#include "rawpreview.moc"

namespace Digikam
{

class RawPreviewPriv
{
public:

    RawPreviewPriv()
    {
        panIconPopup         = 0;
        panIconWidget        = 0;
        cornerButton         = 0;
        thread               = 0;
        url                  = 0;
        currentFitWindowZoom = 0;
    }

    double                 currentFitWindowZoom;

    QToolButton           *cornerButton;

    KPopupFrame           *panIconPopup;

    KURL                   url;

    PanIconWidget         *panIconWidget;

    DImg                   preview;

    ManagedLoadSaveThread *thread;

    LoadingDescription     loadingDesc;
};

RawPreview::RawPreview(QWidget *parent)
          : PreviewWidget(parent)
{
    d = new RawPreviewPriv;
    d->thread = new ManagedLoadSaveThread;

    setMinimumWidth(640);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->cornerButton = new QToolButton(this);
    d->cornerButton->setIconSet(SmallIcon("move"));
    d->cornerButton->hide();
    QToolTip::add(d->cornerButton, i18n("Pan the image to a region"));
    setCornerWidget(d->cornerButton);

    // ------------------------------------------------------------

    connect(d->thread, SIGNAL(signalImageLoaded(const LoadingDescription&, const DImg&)),
            this, SLOT(slotImageLoaded(const LoadingDescription&, const DImg&)));

    connect(d->thread, SIGNAL(signalLoadingProgress(const LoadingDescription&, float)),
            this, SLOT(slotLoadingProgress(const LoadingDescription&, float)));

    connect(d->cornerButton, SIGNAL(pressed()),
            this, SLOT(slotCornerButtonPressed()));

    connect(ThemeEngine::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));

    // ------------------------------------------------------------

    slotReset();
}

RawPreview::~RawPreview()
{
    delete d;
}

void RawPreview::setImage(const DImg& image)
{
    d->preview = image;

    updateZoomAndSize(false);

    viewport()->setUpdatesEnabled(true);
    viewport()->update();
}

DImg& RawPreview::getImage() const
{
    return d->preview;
}

void RawPreview::setDecodingSettings(const KURL& url, const DRawDecoding& settings)
{
    if (!url.isEmpty())
    {
        setCursor(KCursor::waitCursor());
        d->loadingDesc = LoadingDescription(url.path(), settings);
        d->thread->load(d->loadingDesc, ManagedLoadSaveThread::LoadingPolicyFirstRemovePrevious);
        emit signalLoadingStarted();
    }
}

void RawPreview::cancelLoading()
{
    d->thread->stopLoading(d->loadingDesc.filePath);
}

void RawPreview::slotLoadingProgress(const LoadingDescription& description, float progress)
{
    if (description.filePath != d->loadingDesc.filePath)
        return;

    emit signalLoadingProgress(progress);
}

void RawPreview::slotImageLoaded(const LoadingDescription& description, const DImg& image)
{
    if (description.filePath != d->loadingDesc.filePath)
        return;

    if (image.isNull())
    {
        QPixmap pix(visibleWidth(), visibleHeight());
        pix.fill(ThemeEngine::instance()->baseColor());
        QPainter p(&pix);
        p.setPen(QPen(ThemeEngine::instance()->textRegColor()));
        p.drawText(0, 0, pix.width(), pix.height(),
                   Qt::AlignCenter|Qt::WordBreak, 
                   i18n("Cannot decode RAW image for\n\"%1\"")
                   .arg(QFileInfo(d->loadingDesc.filePath).fileName()));
        p.end();
        // three copies - but the image is small
        setImage(DImg(pix.convertToImage()));
        emit signalLoadingFailed();
    }
    else
    {
        setImage(image);
        emit signalImageLoaded(image);
    }

    unsetCursor();
}

void RawPreview::slotThemeChanged()
{
    setBackgroundColor(ThemeEngine::instance()->baseColor());
}

void RawPreview::slotCornerButtonPressed()
{
    if (d->panIconPopup)
    {
        d->panIconPopup->hide();
        delete d->panIconPopup;
        d->panIconPopup = 0;
    }

    d->panIconPopup    = new KPopupFrame(this);
    PanIconWidget *pan = new PanIconWidget(d->panIconPopup);
    pan->setImage(180, 120, getImage());
    d->panIconPopup->setMainWidget(pan);

    QRect r((int)(contentsX()    / zoomFactor()), (int)(contentsY()     / zoomFactor()),
            (int)(visibleWidth() / zoomFactor()), (int)(visibleHeight() / zoomFactor()));
    pan->setRegionSelection(r);
    pan->setMouseFocus();

    connect(pan, SIGNAL(signalSelectionMoved(const QRect&, bool)),
            this, SLOT(slotPanIconSelectionMoved(const QRect&, bool)));

    connect(pan, SIGNAL(signalHiden()),
            this, SLOT(slotPanIconHiden()));

    QPoint g = mapToGlobal(viewport()->pos());
    g.setX(g.x()+ viewport()->size().width());
    g.setY(g.y()+ viewport()->size().height());
    d->panIconPopup->popup(QPoint(g.x() - d->panIconPopup->width(), 
                                  g.y() - d->panIconPopup->height()));

    pan->setCursorToLocalRegionSelectionCenter();
}

void RawPreview::slotPanIconHiden()
{
    d->cornerButton->blockSignals(true);
    d->cornerButton->animateClick();
    d->cornerButton->blockSignals(false);
}

void RawPreview::slotPanIconSelectionMoved(const QRect& r, bool b)
{
    setContentsPos((int)(r.x()*zoomFactor()), (int)(r.y()*zoomFactor()));

    if (b)
    {
        d->panIconPopup->hide();
        delete d->panIconPopup;
        d->panIconPopup = 0;
        slotPanIconHiden();
    }
}

void RawPreview::zoomFactorChanged(double zoom)
{
    updateScrollBars();

    if (horizontalScrollBar()->isVisible() || verticalScrollBar()->isVisible())
        d->cornerButton->show();
    else
        d->cornerButton->hide();

    PreviewWidget::zoomFactorChanged(zoom);
}

void RawPreview::resizeEvent(QResizeEvent* e)
{
    if (!e) return;

    QScrollView::resizeEvent(e);

    if (!d->loadingDesc.filePath.isEmpty())
        d->cornerButton->hide(); 

    updateZoomAndSize(false);
}

void RawPreview::updateZoomAndSize(bool alwaysFitToWindow)
{
    // Set zoom for fit-in-window as minimum, but dont scale up images
    // that are smaller than the available space, only scale down.
    double zoom = calcAutoZoomFactor(ZoomInOnly);
    setZoomMin(zoom);
    setZoomMax(zoom*12.0);

    // Is currently the zoom factor set to fit to window? Then set it again to fit the new size.
    if (zoomFactor() < zoom || alwaysFitToWindow || zoomFactor() == d->currentFitWindowZoom)
    {
        setZoomFactor(zoom);
    }

    // store which zoom factor means it is fit to window
    d->currentFitWindowZoom = zoom;

    updateContentsSize();
}

int RawPreview::previewWidth()
{
    return d->preview.width();
}

int RawPreview::previewHeight()
{
    return d->preview.height();
}

bool RawPreview::previewIsNull()
{
    return d->preview.isNull();
}

void RawPreview::resetPreview()
{
    d->preview     = DImg();
    d->loadingDesc = LoadingDescription();

    updateZoomAndSize(false);
}

void RawPreview::paintPreview(QPixmap *pix, int sx, int sy, int sw, int sh)
{
    DImg img     = d->preview.smoothScaleSection(sx, sy, sw, sh, tileSize(), tileSize());
    QPixmap pix2 = img.convertToPixmap();
    bitBlt(pix, 0, 0, &pix2, 0, 0);
}

}  // NameSpace Digikam
