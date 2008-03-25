/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2008-03-13
 * Description : image files selector dialog.
 * 
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com> 
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

#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

// KDE includes.

#include <kurl.h>
#include <kpreviewwidgetbase.h>


// Local includes.

#include "loadingdescription.h"
#include "digikam_export.h"

namespace Digikam
{

class ImageDialogPrivate;
class ImageDialogPreviewPrivate;

class DIGIKAM_EXPORT ImageDialogPreview : public KPreviewWidgetBase
{
    Q_OBJECT

public:

    ImageDialogPreview(QWidget *parent=0);
    ~ImageDialogPreview();

    QSize sizeHint() const;

private slots:

    void showPreview();
    void showPreview(const KUrl &url);
    void slotThumbnail(const LoadingDescription& desc, const QPixmap& pix);
    void clearPreview();

private:

    void resizeEvent(QResizeEvent *e);

private:

    class ImageDialogPreviewPrivate *d;
};

// ------------------------------------------------------------------------

class DIGIKAM_EXPORT ImageDialog
{

public:

    ImageDialog(QWidget* parent, const KUrl url);
    ~ImageDialog();

    KUrl::List urls() const;

    QString fileformats() const;

    static KUrl::List getImageURLs(QWidget* parent, const KUrl url);

private:

    ImageDialogPrivate* d;
};

} // namespace Digikam

#endif /* IMAGEDIALOG_H */
