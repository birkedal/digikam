/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-05-19
 * Description : Find Duplicates View.
 *
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

#define ICONSIZE 64

// Qt includes.

#include <QLayout>
#include <QPainter>
#include <QPushButton>
#include <QProgressBar>
#include <QTreeWidget>
#include <QHeaderView>

// KDE include.

#include <klocale.h>
#include <kdialog.h>

// Local includes.

#include "album.h"
#include "albummanager.h"
#include "albumdb.h"
#include "databaseaccess.h"
#include "ddebug.h"
#include "imagelister.h"
#include "findduplicatesalbumitem.h"
#include "findduplicatesview.h"
#include "findduplicatesview.moc"

namespace Digikam
{

class FindDuplicatesViewPriv
{

public:

    FindDuplicatesViewPriv()
    {
        listView           = 0;
        scanDuplicatesBtn  = 0;
        updateFingerPrtBtn = 0;
        progressBar        = 0;
        thumbLoadThread    = 0;
    }

    QPushButton         *scanDuplicatesBtn;
    QPushButton         *updateFingerPrtBtn;

    QProgressBar        *progressBar;

    QTreeWidget         *listView;

    ThumbnailLoadThread *thumbLoadThread;
};

FindDuplicatesView::FindDuplicatesView(QWidget *parent)
                  : QWidget(parent)
{
    d = new FindDuplicatesViewPriv;
    d->thumbLoadThread = ThumbnailLoadThread::defaultThread();

    setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout *grid = new QGridLayout(this);
    d->listView       = new QTreeWidget(this);
    d->listView->setRootIsDecorated(false);
    d->listView->setSelectionMode(QAbstractItemView::SingleSelection);
    d->listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->listView->setAllColumnsShowFocus(true);
    d->listView->setIconSize(QSize(ICONSIZE, ICONSIZE));
    d->listView->setSortingEnabled(true);
    d->listView->setColumnCount(2);
    d->listView->setHeaderLabels(QStringList() << i18n("Ref. images") << i18n("Items"));
    d->listView->header()->setResizeMode(QHeaderView::Stretch);
    d->listView->setWhatsThis(i18n("<p>This shows all duplicates items found in whole collections."));

    d->updateFingerPrtBtn = new QPushButton(i18n("Update finger-prints"), this);
    d->updateFingerPrtBtn->setIcon(KIcon("run-build"));
    d->updateFingerPrtBtn->setWhatsThis(i18n("<p>Use this button to scan whole collection to find all "
                                              "duplicates items."));

    d->scanDuplicatesBtn = new QPushButton(i18n("Find duplicates"), this);
    d->scanDuplicatesBtn->setIcon(KIcon("system-search"));
    d->scanDuplicatesBtn->setWhatsThis(i18n("<p>Use this button to scan whole collection to find all "
                                            "duplicates items."));

    d->progressBar = new QProgressBar(this);

    grid->addWidget(d->listView,           0, 0, 1, 3);
    grid->addWidget(d->updateFingerPrtBtn, 1, 0, 1, 3);
    grid->addWidget(d->scanDuplicatesBtn,  2, 0, 1, 1);
    grid->addWidget(d->progressBar,        2, 1, 1, 2);
    grid->setRowStretch(0, 10);
    grid->setColumnStretch(1, 10);
    grid->setMargin(KDialog::spacingHint());
    grid->setSpacing(KDialog::spacingHint());

    // ---------------------------------------------------------------

    connect(d->updateFingerPrtBtn, SIGNAL(clicked()),
            this, SIGNAL(signalUpdateFingerPrints()));

    connect(d->scanDuplicatesBtn, SIGNAL(clicked()),
            this, SLOT(slotFindDuplicates()));

    connect(d->listView, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
            this, SLOT(slotDuplicatesAlbumActived(QTreeWidgetItem*, int)));

    connect(d->thumbLoadThread, SIGNAL(signalThumbnailLoaded(const LoadingDescription&, const QPixmap&)),
            this, SLOT(slotThumbnailLoaded(const LoadingDescription&, const QPixmap&)));
}

FindDuplicatesView::~FindDuplicatesView()
{
    delete d;
}

void FindDuplicatesView::populateTreeView()
{
    const AlbumList& aList = AlbumManager::instance()->allSAlbums();

    for (AlbumList::const_iterator it = aList.begin(); it != aList.end(); ++it)
    {
        SAlbum* salbum = dynamic_cast<SAlbum*>(*it);
        if (salbum && salbum->isDuplicatesSearch())
        {
            FindDuplicatesAlbumItem *item = new FindDuplicatesAlbumItem(d->listView, salbum);
            ThumbnailLoadThread::defaultThread()->find(item->refUrl().path());
        }
    }

    d->listView->sortByColumn(1, Qt::DescendingOrder);
    d->listView->resizeColumnToContents(0);
}

void FindDuplicatesView::slotThumbnailLoaded(const LoadingDescription& desc, const QPixmap& pix)
{
    QTreeWidgetItemIterator it(d->listView);
    while (*it)
    {
        FindDuplicatesAlbumItem* item = dynamic_cast<FindDuplicatesAlbumItem*>(*it);
        if (item->refUrl().path() == desc.filePath)
        {
            if (pix.isNull())
                item->setThumb(SmallIcon("image-x-generic", ICONSIZE, KIconLoader::DisabledState));
            else
                item->setThumb(pix.scaled(ICONSIZE, ICONSIZE, Qt::KeepAspectRatio));

            return;
        }
        ++it;
    }
}

void FindDuplicatesView::slotFindDuplicates()
{
    d->listView->clear();
    d->scanDuplicatesBtn->setEnabled(false);
    d->updateFingerPrtBtn->setEnabled(false);
    d->progressBar->setEnabled(true);

    AlbumList albums = AlbumManager::instance()->allPAlbums();
    QStringList idsStringList;
    foreach(Album *a, albums)
        idsStringList << QString::number(a->id());

    KIO::Job *job = ImageLister::startListJob(DatabaseUrl::searchUrl(-1));
    job->addMetaData("duplicates", "true");
    job->addMetaData("albumids", idsStringList.join(","));
    job->addMetaData("threshold", QString::number(0.5));

    connect(job, SIGNAL(result(KJob*)),
            this, SLOT(slotDuplicatesSearchResult(KJob*)));

    connect(job, SIGNAL(totalAmount(KJob *, KJob::Unit, qulonglong)),
            this, SLOT(slotDuplicatesSearchTotalAmount(KJob *, KJob::Unit, qulonglong)));

    connect(job, SIGNAL(processedAmount(KJob *, KJob::Unit, qulonglong)),
            this, SLOT(slotDuplicatesSearchProcessedAmount(KJob *, KJob::Unit, qulonglong)));
}

void FindDuplicatesView::slotDuplicatesSearchTotalAmount(KJob*, KJob::Unit, qulonglong amount)
{
    d->progressBar->setMinimum(0);
    d->progressBar->setMaximum(amount);
}

void FindDuplicatesView::slotDuplicatesSearchProcessedAmount(KJob*, KJob::Unit, qulonglong amount)
{
    d->progressBar->setValue(amount);
}

void FindDuplicatesView::slotDuplicatesSearchResult(KJob*)
{
    d->scanDuplicatesBtn->setEnabled(true);
    d->updateFingerPrtBtn->setEnabled(true);
    d->progressBar->reset();
    d->progressBar->setEnabled(false);
    populateTreeView();
}

void FindDuplicatesView::slotDuplicatesAlbumActived(QTreeWidgetItem* item, int)
{
    FindDuplicatesAlbumItem* sitem = dynamic_cast<FindDuplicatesAlbumItem*>(item);
    if (sitem)
        AlbumManager::instance()->setCurrentAlbum(sitem->album());
}

}  // NameSpace Digikam
