/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-09-14
 * Description : a presentation tool.
 *
 * Copyright (C) 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
 * Copyright (C) 2012-2016 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "presentationaudiolist.h"

// Qt includes

#include <QString>
#include <QWidget>
#include <QEvent>
#include <QFileInfo>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QIcon>
#include <QBrush>
#include <QMimeData>
#include <QUrl>
#include <QMessageBox>
#include <QApplication>
#include <QMediaMetaData>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class PresentationAudioListItem::Private
{

public:

    Private()
    {
        mediaObject = 0;
    }

    QUrl          url;
    QString       artist;
    QString       title;
    QTime         totalTime;
    QMediaPlayer* mediaObject;
};

PresentationAudioListItem::PresentationAudioListItem(QListWidget* const parent, const QUrl& url)
    : QListWidgetItem(parent),
      d(new Private)
{
    d->url = url;
    setIcon(QIcon::fromTheme(QString::fromLatin1("audio-x-generic")).pixmap(48, QIcon::Disabled));

    d->totalTime   = QTime(0, 0, 0);
    d->mediaObject = new QMediaPlayer();

    connect(d->mediaObject, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(slotMediaStateChanged(QMediaPlayer::MediaStatus)));

    connect(d->mediaObject, SIGNAL(error(QMediaPlayer::Error)),
            this, SLOT(slotPlayerError(QMediaPlayer::Error)));

    d->mediaObject->setMedia(url);
}

PresentationAudioListItem::~PresentationAudioListItem()
{
    delete d;
}

QUrl PresentationAudioListItem::url() const
{
    return d->url;
}

void PresentationAudioListItem::setName(const QString& text)
{
    setText(text);
}

QString PresentationAudioListItem::artist() const
{
    return d->artist;
}

QString PresentationAudioListItem::title() const
{
    return d->title;
}

QTime PresentationAudioListItem::totalTime() const
{
    return d->totalTime;
}

void PresentationAudioListItem::slotPlayerError(QMediaPlayer::Error err)
{
    if (err != QMediaPlayer::NoError)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "An error as occured while playing (" << err << ")";
        showErrorDialog();
    }
}

void PresentationAudioListItem::slotMediaStateChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::UnknownMediaStatus ||
        status == QMediaPlayer::NoMedia            ||
        status == QMediaPlayer::InvalidMedia)
    {
        showErrorDialog();
        return;
    }

    qint64 total = d->mediaObject->duration();
    int hours      = (int)(total  / (long int)( 60 * 60 * 1000 ));
    int mins       = (int)((total / (long int)( 60 * 1000 )) - (long int)(hours * 60));
    int secs       = (int)((total / (long int)1000) - (long int)(hours * 60 * 60) - (long int)(mins * 60));
    d->totalTime   = QTime(hours, mins, secs);
    d->artist      = (d->mediaObject->metaData(QMediaMetaData::Author)).toStringList().join(QString::fromLatin1(","));
    d->title       = (d->mediaObject->metaData(QMediaMetaData::Title)).toString();

    if ( d->artist.isEmpty() && d->title.isEmpty() )
        setText(d->url.fileName());
    else
        setText(i18nc("artist - title", "%1 - %2", artist(), title()));

    emit signalTotalTimeReady(d->url, d->totalTime);
}

void PresentationAudioListItem::showErrorDialog()
{
    QMessageBox msgBox(QApplication::activeWindow());
    msgBox.setWindowTitle(i18n("Error"));
    msgBox.setText(i18n("%1 may not be playable.", d->url.fileName()));
    msgBox.setDetailedText(d->mediaObject->errorString());
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.exec();

    d->artist = d->url.fileName();
    d->title  = i18n("This file may not be playable.");
    setText(i18nc("artist - title", "%1 - %2", artist(), title()));
    setBackground(QBrush(Qt::red));
    setForeground(QBrush(Qt::white));
    QFont errorFont = font();
    errorFont.setBold(true);
    errorFont.setItalic(true);
    setFont(errorFont);
}

// ------------------------------------------------------------------

PresentationAudioList::PresentationAudioList(QWidget* const parent)
    : QListWidget(parent)
{
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    setSortingEnabled(false);
    setIconSize(QSize(32, 32));
}

void PresentationAudioList::dragEnterEvent(QDragEnterEvent* e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void PresentationAudioList::dragMoveEvent(QDragMoveEvent* e)
{
    if (e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void PresentationAudioList::dropEvent(QDropEvent* e)
{
    QList<QUrl> list = e->mimeData()->urls();
    QList<QUrl> urls;

    foreach(const QUrl &url, list)
    {
        QFileInfo fi(url.toLocalFile());

        if (fi.isFile() && fi.exists())
            urls.append(QUrl(url));
    }

    e->acceptProposedAction();

    if (!urls.isEmpty())
        emit signalAddedDropItems(urls);
}

QList<QUrl> PresentationAudioList::fileUrls()
{
    QList<QUrl> files;

    for (int i = 0; i < count(); ++i)
    {
        PresentationAudioListItem* const sitem = dynamic_cast<PresentationAudioListItem*>(item(i));

        if (sitem)
        {
            files << QUrl(sitem->url());
        }
    }

    return files;
}

}  // namespace Digikam
