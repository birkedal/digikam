/* ============================================================
 * Author:  Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-07-24
 * Description : a dialog to select a camera folders.
 * 
 * Copyright 2006 by Gilles Caulier
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

#include <qlayout.h>
#include <qframe.h>

// KDE includes.

#include <klocale.h>
#include <kdebug.h>

// Local includes.

#include "camerafolderitem.h"
#include "camerafolderview.h"
#include "camerafolderdialog.h"

namespace Digikam
{

CameraFolderDialog::CameraFolderDialog(QWidget *parent, const QStringList& cameraFolderList,
                                       const QString& cameraName, const QString& cameraRootPath)
                  : KDialogBase(Plain, i18n("%1 - Select Camera Folder").arg(cameraName),
                                Help|Ok|Cancel, Ok,
                                parent, 0, true, true)
{
    setHelp("camerainterface.anchor", "digikam");
    resize(500, 400);

    QVBoxLayout* lay = new QVBoxLayout(plainPage(), 0, spacingHint());
    m_folderView = new CameraFolderView(plainPage());
    lay->addWidget(m_folderView);    
    m_folderView->addVirtualFolder(cameraName);

    for (QStringList::const_iterator it = cameraFolderList.begin();
         it != cameraFolderList.end(); ++it)
    {
        QString folder(*it);
        kdDebug() << folder << endl;
        if (folder != cameraRootPath && !folder.isEmpty())
        {
            QString root = folder.section( '/', 0, -2 );
            QString sub  = folder.section( '/', -1 );
            m_folderView->addFolder(root, sub);
        }
    }
}

CameraFolderDialog::~CameraFolderDialog()
{
}

QString CameraFolderDialog::selectedFolderPath()
{
    QListViewItem *item = m_folderView->currentItem();
    if (!item) return QString();

    CameraFolderItem *folderItem = static_cast<CameraFolderItem *>(item);
    if (folderItem->isVirtualFolder())
        return QString();

    return(folderItem->folderPath());
}

}  // namespace Digikam
