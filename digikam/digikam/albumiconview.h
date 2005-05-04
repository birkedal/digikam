//////////////////////////////////////////////////////////////////////////////
//
//    ALBUMICONVIEW.H
//
//    Copyright (C) 2002-2004 Renchi Raju <renchi at pooh.tam.uiuc.edu>
//                            Gilles Caulier <caulier dot gilles at free.fr>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ALBUMICONVIEW_H
#define ALBUMICONVIEW_H

// KDE includes.

#include <qrect.h>
#include <qfont.h>
#include <kfileitem.h>

#include "iconview.h"

// Local includes.

#include "imageinfo.h"
#include "albumitemhandler.h"

class KFileItem;
class KFileItemMetaInfo;
class QMouseEvent;
class QResizeEvent;
class QDragMoveEvent;
class QDropEvent;
class QPoint;
class QString;
class QPainter;
class QPixmap;

class AlbumIconItem;
class AlbumSettings;
class AlbumIconViewPrivate;
class ThumbnailSize;
class Album;

class AlbumIconView : public IconView,
                      public AlbumItemHandler
{
    Q_OBJECT

public:

    AlbumIconView(QWidget* parent);
    ~AlbumIconView();

    void setAlbum(Album* album);
    void setThumbnailSize(const ThumbnailSize& thumbSize);
    ThumbnailSize thumbnailSize();

    void applySettings(const AlbumSettings* settings);
    const AlbumSettings* settings();

    void    refreshIcon(AlbumIconItem* item);

    AlbumIconItem* firstSelectedItem();

    KURL::List allItems();
    KURL::List selectedItems();

    void refresh();
    void refreshItems(const KURL::List& itemList);

    QRect    itemRect() const;
    QRect    itemDateRect() const;
    QRect    itemPixmapRect() const;
    QRect    itemNameRect() const;
    QRect    itemCommentsRect() const;
    QRect    itemFileCommentsRect() const;
    QRect    itemResolutionRect() const;
    QRect    itemSizeRect() const;
    QRect    itemTagRect() const;
    QRect    bannerRect() const;

    QPixmap* itemBaseRegPixmap() const;
    QPixmap* itemBaseSelPixmap() const;
    QPixmap  bannerPixmap() const;

    QFont    itemFontReg() const;
    QFont    itemFontCom() const;
    QFont    itemFontXtra() const;

    void     setInFocus(bool val);

    AlbumIconItem* findItem(const QPoint& pos);
    AlbumIconItem* findItem(const QString& url) const;

protected:

    void resizeEvent(QResizeEvent* e);

    void focusInEvent(QFocusEvent* e);
    void drawFrame(QPainter* p);
    
    // DnD
    void startDrag();
    void contentsDragMoveEvent(QDragMoveEvent *e);
    void contentsDropEvent(QDropEvent *e);

    bool acceptToolTip(IconItem *item, const QPoint &mousePos);
    
private:

    AlbumIconViewPrivate *d;

    void    updateBannerRectPixmap();
    void    updateItemRectsPixmap();
    bool    showMetaInfo();

private slots:

    void slotImageListerNewItems(const ImageInfoList& itemList);
    void slotImageListerDeleteItem(ImageInfo* item);
    void slotImageListerClear();
    void slotImageListerCompleted();

    void slotDoubleClicked(IconItem *item);
    void slotRightButtonClicked(IconItem *item, const QPoint& pos);

    void slotGotThumbnail(const KURL& url, const QPixmap& pix,
                          const KFileMetaInfo* metaInfo);
    void slotFailedThumbnail(const KURL& url);
    void slotFinishedThumbnail();
    void slotSelectionChanged();

    void slotFilesModified();
    void slotFilesModified(const KURL& url);

    void slotContentsMoving(int x, int y);
    void slotShowToolTip(IconItem* item);

    void slotThemeChanged();

    void slotAssignTag(int tagID);
    void slotRemoveTag(int tagID);

public slots:

    void slotEditImageComments(AlbumIconItem* item);
    void slotSetExifOrientation(int orientation);
    void slotRename(AlbumIconItem* item);
    void slotDeleteSelectedItems();
    void slotDisplayItem(AlbumIconItem *item=0);
    void slotProperties(AlbumIconItem* item);
    void slotAlbumModified();
    void slotSetAlbumThumbnail(AlbumIconItem *iconItem);
    
signals:

    void signalItemsAdded();
    void signalInFocus();

};

#endif // ALBUMICONVIEW_H
