/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2013-02-25
 * Description : Table view column helpers: Thumbnail column
 *
 * Copyright (C) 2013 by Michael G. Hansen <mike at mghansen dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "tableview_column_thumbnail.moc"

// Qt includes

// KDE includes

// local includes

namespace Digikam
{

namespace TableViewColumns
{

ColumnThumbnail::ColumnThumbnail(
        TableViewShared* const tableViewShared,
        const TableViewColumnConfiguration& pConfiguration,
        QObject* const parent)
  : TableViewColumn(tableViewShared, pConfiguration, parent)
{
    connect(s->thumbnailLoadThread, SIGNAL(signalThumbnailLoaded(LoadingDescription, QPixmap)),
            this, SLOT(slotThumbnailLoaded(LoadingDescription, QPixmap)));
}

ColumnThumbnail::~ColumnThumbnail()
{

}

TableViewColumnDescription ColumnThumbnail::getDescription()
{
    return TableViewColumnDescription(QLatin1String("thumbnail"), QLatin1String("Thumbnail"));
}

TableViewColumn::ColumnFlags ColumnThumbnail::getColumnFlags() const
{
    return ColumnCustomPainting;
}

QString ColumnThumbnail::getTitle()
{
    return i18n("Thumbnail");
}

QVariant ColumnThumbnail::data(const QModelIndex& sourceIndex, const int role)
{
    Q_UNUSED(sourceIndex)
    Q_UNUSED(role)

    // we do not return any data, but paint(...) something
    return QVariant();
}

bool ColumnThumbnail::paint(QPainter* const painter, const QStyleOptionViewItem& option, const QModelIndex& sourceIndex) const
{
    /// @todo do we have to reset the column?
    const ImageInfo info = getImageInfo(sourceIndex);
    if (!info.isNull())
    {
        QSize size(60, 60);
        const QString path = info.filePath();
        QPixmap thumbnail;

        /// @todo handle unavailable thumbnails -> emit itemChanged(...) later
        if (s->thumbnailLoadThread->find(path, thumbnail, qMax(size.width() + 2, size.height() + 2)))
        {
            /// @todo Is slotThumbnailLoaded still called when the thumbnail is found right away?
            /// @todo remove borders
//                 thumbnail = thumbnail.copy(1, 1, thumbnail.size().width()-2, thumbnail.size().height()-2)
            const QSize availableSize = option.rect.size();
            const QSize pixmapSize    = thumbnail.size().boundedTo(availableSize);
            QPoint startPoint((availableSize.width() - pixmapSize.width()) / 2,
                              (availableSize.height() - pixmapSize.height()) / 2);
            startPoint += option.rect.topLeft();
            painter->drawPixmap(QRect(startPoint, pixmapSize), thumbnail, QRect(QPoint(0, 0), pixmapSize));

            return true;
        }
    }

    // we did not get to paint a thumbnail...
    return false;
}

QSize ColumnThumbnail::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& sourceIndex) const
{
    return QSize(60, 60);
}

void ColumnThumbnail::slotThumbnailLoaded(const LoadingDescription& loadingDescription, const QPixmap& thumb)
{
    if (thumb.isNull())
    {
        return;
    }

    const QModelIndex sourceIndex = s->imageFilterModel->indexForPath(loadingDescription.filePath);
    if (!sourceIndex.isValid())
    {
        return;
    }

    emit(signalDataChanged(sourceIndex));
}

} /* namespace TableViewColumns */

} /* namespace Digikam */

