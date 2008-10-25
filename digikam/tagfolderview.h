/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-22
 * Description : tags folder view.
 *
 * Copyright (C) 2005-2006 by Joern Ahrens <joern.ahrens@kdemail.net>
 * Copyright (C) 2006-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef TAGFOLDERVIEW_H
#define TAGFOLDERVIEW_H

// Qt includes.

#include <QDropEvent>
#include <QPixmap>

// Local includes.

#include "folderview.h"

class QDropEvent;
class QDrag;

namespace Digikam
{

class Album;
class TAlbum;
class TagFolderViewItem;
class TagFolderViewPriv;

class TagFolderView : public FolderView
{
    Q_OBJECT

public:

    TagFolderView(QWidget *parent);
    ~TagFolderView();

    void tagNew();
    void tagEdit();
    void tagDelete();

    void selectItem(int id);

    void refresh();

signals:

    void signalProgressBarMode(int, const QString&);
    void signalProgressValue(int);
    void signalTextTagFilterMatch(bool);

public slots:

    void slotTextTagFilterChanged(const QString&);

protected:

    void contentsDropEvent(QDropEvent *e);
    bool acceptDrop(const QDropEvent *e) const;

private slots:

    void slotAlbumAdded(Album*);
    void slotSelectionChanged();
    void slotAlbumDeleted(Album*);
    void slotAlbumRenamed(Album*);
    void slotAlbumsCleared();
    void slotAlbumIconChanged(Album* album);
    void slotAlbumMoved(TAlbum* tag, TAlbum* newParent);
    void slotContextMenu(Q3ListViewItem*, const QPoint&, int);
    void slotABCContextMenu();
    void slotGotThumbnailFromIcon(Album *album, const QPixmap& thumbnail);
    void slotThumbnailLost(Album *album);
    void slotReloadThumbnails();
    void slotRefresh(const QMap<int, int>&);

private:

    void tagNew(TagFolderViewItem *item, const QString& _title=QString(),
                const QString& _icon=QString() );
    void tagEdit(TagFolderViewItem *item);
    void tagDelete(TagFolderViewItem *item);
    void setTagThumbnail(TAlbum *album);
    QDrag* makeDragObject();

private:

    TagFolderViewPriv *d;
};

}  // namespace Digikam

#endif // TAGFOLDERVIEW_H
