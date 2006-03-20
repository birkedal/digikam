/* ============================================================
 * Author: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date  : 2004-11-17
 * Description : image properties side bar using data from 
 *               digiKam database.
 *
 * Copyright 2004-2006 by Gilles Caulier
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
 
#include <qrect.h>
#include <qsplitter.h>

// KDE includes.

#include <klocale.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kcursor.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kiconloader.h>

// Local includes.

#include "dimg.h"
#include "albumiconitem.h"
#include "albumiconview.h"
#include "imagepropertiesmetadatatab.h"
#include "imagepropertiescolorstab.h"
#include "imagedescedittab.h"
#include "navigatebarwidget.h"
#include "imagepropertiessidebardb.h"

namespace Digikam
{

class ImagePropertiesSideBarDBPriv
{
public:

    ImagePropertiesSideBarDBPriv()
    {
        currentView       = 0;
        currentItem       = 0;
        desceditTab       = 0;
        dirtyDesceditTab  = false;
    }

    bool              dirtyDesceditTab;

    AlbumIconView    *currentView;
    
    AlbumIconItem    *currentItem;

    ImageDescEditTab *desceditTab;
};

ImagePropertiesSideBarDB::ImagePropertiesSideBarDB(QWidget *parent, const char *name, QSplitter *splitter, 
                                                   Side side, bool mimimizedDefault, bool navBar)
                        : Digikam::ImagePropertiesSideBar(parent, name, splitter, side, mimimizedDefault, navBar)
{
    d = new ImagePropertiesSideBarDBPriv;
    d->desceditTab = new ImageDescEditTab(parent, navBar);
   
    appendTab(d->desceditTab, SmallIcon("imagecomment"), i18n("Comments && Tags"));

    // ----------------------------------------------------------
    
    connect(m_metadataTab, SIGNAL(signalFirstItem()),
            this, SIGNAL(signalFirstItem()));
                    
    connect(m_metadataTab, SIGNAL(signalPrevItem()),
            this, SIGNAL(signalPrevItem()));
    
    connect(m_metadataTab, SIGNAL(signalNextItem()),
            this, SIGNAL(signalNextItem()));

    connect(m_metadataTab, SIGNAL(signalLastItem()),
            this, SIGNAL(signalLastItem()));

    connect(m_colorTab, SIGNAL(signalFirstItem()),
            this, SIGNAL(signalFirstItem()));
                    
    connect(m_colorTab, SIGNAL(signalPrevItem()),
            this, SIGNAL(signalPrevItem()));
    
    connect(m_colorTab, SIGNAL(signalNextItem()),
            this, SIGNAL(signalNextItem()));

    connect(m_colorTab, SIGNAL(signalLastItem()),
            this, SIGNAL(signalLastItem()));
                            
    connect(d->desceditTab, SIGNAL(signalFirstItem()),
            this, SIGNAL(signalFirstItem()));
                    
    connect(d->desceditTab, SIGNAL(signalPrevItem()),
            this, SIGNAL(signalPrevItem()));
    
    connect(d->desceditTab, SIGNAL(signalNextItem()),
            this, SIGNAL(signalNextItem()));

    connect(d->desceditTab, SIGNAL(signalLastItem()),
            this, SIGNAL(signalLastItem()));
            
    connect(this, SIGNAL(signalChangedTab(QWidget*)),
            this, SLOT(slotChangedTab(QWidget*)));
}

ImagePropertiesSideBarDB::~ImagePropertiesSideBarDB()
{
    delete d;
}

void ImagePropertiesSideBarDB::itemChanged(const KURL& url, QRect *rect, DImg *img,
                                           AlbumIconView* view, AlbumIconItem* item)
{
    if (!url.isValid())
        return;
    
    m_currentURL        = url;
    m_currentRect       = rect;
    m_image             = img;
    m_dirtyMetadataTab  = false;
    m_dirtyColorTab     = false;
    d->currentView      = view;
    d->currentItem      = item;
    d->dirtyDesceditTab = false;
        
    slotChangedTab( getActiveTab() );    
}

void ImagePropertiesSideBarDB::slotNoCurrentItem(void)
{
    ImagePropertiesSideBar::slotNoCurrentItem();
    d->currentItem = 0;
    d->desceditTab->setItem();
    d->dirtyDesceditTab = false;
}

void ImagePropertiesSideBarDB::populateTags(void)
{
    d->desceditTab->populateTags();
}

void ImagePropertiesSideBarDB::slotChangedTab(QWidget* tab)
{
    setCursor(KCursor::waitCursor());

    // No database data available, for example in the case of image editor is 
    // launched from camera GUI.
    if (!d->currentView || !d->currentItem)
    {
        if (tab == m_metadataTab && !m_dirtyMetadataTab)
        {
            if (m_image)
                m_metadataTab->setCurrentData(m_image->getExif(), m_image->getIptc(), 
                                              m_currentURL.fileName(),
                                              NavigateBarWidget::ItemCurrent);
            else
                m_metadataTab->setCurrentURL(m_currentURL, NavigateBarWidget::ItemCurrent);

            m_dirtyMetadataTab = true;
        }
        else if (tab == m_colorTab && !m_dirtyColorTab)
        {
            m_colorTab->setData(m_currentURL, m_currentRect, m_image, NavigateBarWidget::ItemCurrent);
            m_dirtyColorTab = true;
        }
        else if (tab == d->desceditTab && !d->dirtyDesceditTab)
        {
            // Do nothing here. We cannot get data from database !
            d->desceditTab->setItem();
            d->dirtyDesceditTab = true;
        }
    }
    else    // Data from database available...
    {
        int currentItemType = NavigateBarWidget::ItemCurrent;
        
        if (d->currentView->firstItem() == d->currentItem)
            currentItemType = NavigateBarWidget::ItemFirst;
        else if (d->currentView->lastItem() == d->currentItem)
            currentItemType = NavigateBarWidget::ItemLast;
        
        if (tab == m_metadataTab && !m_dirtyMetadataTab)
        {
            if (m_image)
                m_metadataTab->setCurrentData(m_image->getExif(), m_image->getIptc(), 
                                              m_currentURL.fileName(), currentItemType);
            else
                m_metadataTab->setCurrentURL(m_currentURL, currentItemType);

            m_dirtyMetadataTab = true;
        }
        else if (tab == m_colorTab && !m_dirtyColorTab)
        {
            m_colorTab->setData(m_currentURL, m_currentRect, m_image, currentItemType);
            m_dirtyColorTab = true;
        }
        else if (tab == d->desceditTab && !d->dirtyDesceditTab)
        {
           d->desceditTab->setItem(d->currentItem, currentItemType);
           d->dirtyDesceditTab = true;
        }
    }

    setCursor( KCursor::arrowCursor() );
}

void ImagePropertiesSideBarDB::slotAssignRating(int rating)
{
    d->desceditTab->assignRating(rating);
}

void ImagePropertiesSideBarDB::slotAssignRatingNoStar()
{
    d->desceditTab->assignRating(0);
}

void ImagePropertiesSideBarDB::slotAssignRatingOneStar()
{
    d->desceditTab->assignRating(1);
}

void ImagePropertiesSideBarDB::slotAssignRatingTwoStar()
{
    d->desceditTab->assignRating(2);
}

void ImagePropertiesSideBarDB::slotAssignRatingThreeStar()
{
    d->desceditTab->assignRating(3);
}

void ImagePropertiesSideBarDB::slotAssignRatingFourStar()
{
    d->desceditTab->assignRating(4);
}

void ImagePropertiesSideBarDB::slotAssignRatingFiveStar()
{
    d->desceditTab->assignRating(5);
}

}  // NameSpace Digikam

#include "imagepropertiessidebardb.moc"
