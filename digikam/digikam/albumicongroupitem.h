/* ============================================================
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Date  : 2005-04-25
 * Copyright 2005 by Renchi Raju
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
 * ============================================================ */

#ifndef ALBUMICONGROUPITEM_H
#define ALBUMICONGROUPITEM_H

#include "icongroupitem.h"

class AlbumIconView;

class AlbumIconGroupItem : public IconGroupItem
{
public:

    AlbumIconGroupItem(AlbumIconView* view, int albumID);
    ~AlbumIconGroupItem();

protected:

    void paintBanner();

private:

    int            m_albumID;
    AlbumIconView* m_view;
};

#endif /* ALBUMICONGROUPITEM_H */
