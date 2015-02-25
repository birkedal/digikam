/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2006-01-20
 * Description : core image editor GUI implementation private data.
 *
 * Copyright (C) 2006-2015 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef EDITORWINDOWPRIVATE_H
#define EDITORWINDOWPRIVATE_H

// Qt includes

#include <QList>
#include <QString>
#include <QSignalMapper>
#include <QMenu>

// KDE includes

#include <kconfiggroup.h>
#include <kservice.h>

// Libkdcraw includes

#include <KDCRAW/RWidgetUtils>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "editorwindow.h"
#include "versionmanager.h"

class QAction;
class QDialog;
class QEventLoop;
class QLabel;
class QToolButton;
class QWidgetAction;

class KActionCollection;

using namespace KDcrawIface;

namespace Digikam
{

class ActionCategorizedView;
class DZoomBar;
class EditorToolIface;
class ExposureSettingsContainer;
class ICCSettingsContainer;
class PreviewToolBar;

class EditorWindow::Private
{

public:

    Private() :
        cmViewIndicator(0),
        underExposureIndicator(0),
        overExposureIndicator(0),
        infoLabel(0),
        imagepluginsActionCollection(0),
        copyAction(0),
        cropAction(0),
        autoCropAction(0),
        filePrintAction(0),
        flipHorizAction(0),
        flipVertAction(0),
        rotateLeftAction(0),
        rotateRightAction(0),
        selectAllAction(0),
        selectNoneAction(0),
        slideShowAction(0),
        softProofOptionsAction(0),
        zoomFitToSelectAction(0),
        zoomMinusAction(0),
        zoomPlusAction(0),
        zoomTo100percents(0),
        undoSignalMapper(0),
        redoSignalMapper(0),
        formatMenuActionMapper(0),
        waitingLoop(0),
        currentWindowModalDialog(0),
        zoomFitToWindowAction(0),
        viewCMViewAction(0),
        viewSoftProofAction(0),
        viewUnderExpoAction(0),
        viewOverExpoAction(0),
        showMenuBarAction(0),
        selectToolsActionView(0),
        ICCSettings(0),
        zoomBar(0),
        previewToolBar(0),
        exposureSettings(0),
        toolIface(0)
    {
    }

    ~Private()
    {
    }

    void legacyUpdateSplitterState(KConfigGroup& group);
    void plugNewVersionInFormatAction(EditorWindow* const q, QMenu* const menuAction, const QString& text, const QString& format);
    void addPageUpDownActions(EditorWindow* const q, QWidget* const w);

public:

    static const QString         configAutoZoomEntry;
    static const QString         configBackgroundColorEntry;
    static const QString         configJpeg2000CompressionEntry;
    static const QString         configJpeg2000LossLessEntry;
    static const QString         configJpegCompressionEntry;
    static const QString         configJpegSubSamplingEntry;
    static const QString         configPgfCompressionEntry;
    static const QString         configPgfLossLessEntry;
    static const QString         configPngCompressionEntry;
    static const QString         configSplitterStateEntry;
    static const QString         configTiffCompressionEntry;
    static const QString         configUnderExposureColorEntry;
    static const QString         configUnderExposureIndicatorEntry;
    static const QString         configUnderExposurePercentsEntry;
    static const QString         configOverExposureColorEntry;
    static const QString         configOverExposureIndicatorEntry;
    static const QString         configOverExposurePercentsEntry;
    static const QString         configExpoIndicatorModeEntry;
    static const QString         configUseRawImportToolEntry;
    static const QString         configUseThemeBackgroundColorEntry;
    static const QString         configVerticalSplitterSizesEntry;
    static const QString         configVerticalSplitterStateEntry;

    QToolButton*                 cmViewIndicator;
    QToolButton*                 underExposureIndicator;
    QToolButton*                 overExposureIndicator;

    RAdjustableLabel*            infoLabel;

    KActionCollection*           imagepluginsActionCollection;

    QAction*                     copyAction;
    QAction*                     cropAction;
    QAction*                     autoCropAction;
    QAction*                     filePrintAction;
    QAction*                     flipHorizAction;
    QAction*                     flipVertAction;
    QAction*                     rotateLeftAction;
    QAction*                     rotateRightAction;
    QAction*                     selectAllAction;
    QAction*                     selectNoneAction;
    QAction*                     slideShowAction;
    QAction*                     softProofOptionsAction;
    QAction*                     zoomFitToSelectAction;
    QAction*                     zoomMinusAction;
    QAction*                     zoomPlusAction;
    QAction*                     zoomTo100percents;

    QSignalMapper*               undoSignalMapper;
    QSignalMapper*               redoSignalMapper;
    QSignalMapper*               formatMenuActionMapper;

    QEventLoop*                  waitingLoop;
    QDialog*                     currentWindowModalDialog;

    QAction*                     zoomFitToWindowAction;
    QAction*                     viewCMViewAction;
    QAction*                     viewSoftProofAction;
    QAction*                     viewUnderExpoAction;
    QAction*                     viewOverExpoAction;
    QAction*                     showMenuBarAction;

    ActionCategorizedView*       selectToolsActionView;

    ICCSettingsContainer*        ICCSettings;

    DZoomBar*                    zoomBar;
    PreviewToolBar*              previewToolBar;

    ExposureSettingsContainer*   exposureSettings;

    EditorToolIface*             toolIface;

    VersionManager               defaultVersionManager;

    QList<int>                   fullscreenSizeBackup;

    QMap<QString, KService::Ptr> servicesMap;
};

const QString EditorWindow::Private::configAutoZoomEntry(QLatin1String("AutoZoom"));
const QString EditorWindow::Private::configBackgroundColorEntry(QLatin1String("BackgroundColor"));
const QString EditorWindow::Private::configJpeg2000CompressionEntry(QLatin1String("JPEG2000Compression"));
const QString EditorWindow::Private::configJpeg2000LossLessEntry(QLatin1String("JPEG2000LossLess"));
const QString EditorWindow::Private::configJpegCompressionEntry(QLatin1String("JPEGCompression"));
const QString EditorWindow::Private::configJpegSubSamplingEntry(QLatin1String("JPEGSubSampling"));
const QString EditorWindow::Private::configPgfCompressionEntry(QLatin1String("PGFCompression"));
const QString EditorWindow::Private::configPgfLossLessEntry(QLatin1String("PGFLossLess"));
const QString EditorWindow::Private::configPngCompressionEntry(QLatin1String("PNGCompression"));
const QString EditorWindow::Private::configSplitterStateEntry(QLatin1String("SplitterState"));
const QString EditorWindow::Private::configTiffCompressionEntry(QLatin1String("TIFFCompression"));
const QString EditorWindow::Private::configUnderExposureColorEntry(QLatin1String("UnderExposureColor"));
const QString EditorWindow::Private::configUnderExposureIndicatorEntry(QLatin1String("UnderExposureIndicator"));
const QString EditorWindow::Private::configUnderExposurePercentsEntry(QLatin1String("UnderExposurePercentsEntry"));
const QString EditorWindow::Private::configOverExposureColorEntry(QLatin1String("OverExposureColor"));
const QString EditorWindow::Private::configOverExposureIndicatorEntry(QLatin1String("OverExposureIndicator"));
const QString EditorWindow::Private::configOverExposurePercentsEntry(QLatin1String("OverExposurePercentsEntry"));
const QString EditorWindow::Private::configExpoIndicatorModeEntry(QLatin1String("ExpoIndicatorMode"));
const QString EditorWindow::Private::configUseRawImportToolEntry(QLatin1String("UseRawImportTool"));
const QString EditorWindow::Private::configUseThemeBackgroundColorEntry(QLatin1String("UseThemeBackgroundColor"));
const QString EditorWindow::Private::configVerticalSplitterSizesEntry(QLatin1String("Vertical Splitter Sizes"));
const QString EditorWindow::Private::configVerticalSplitterStateEntry(QLatin1String("Vertical Splitter State"));

void EditorWindow::Private::legacyUpdateSplitterState(KConfigGroup& group)
{
    // Check if the thumbnail size in the config file is splitter based (the
    // old method), and convert to dock based if needed.
    if (group.hasKey(configSplitterStateEntry))
    {
        // Read splitter state from config file
        QByteArray state;
        state = QByteArray::fromBase64(group.readEntry(configSplitterStateEntry, state));

        // Do a cheap check: a splitter state with 3 windows is always 34 bytes.
        if (state.count() == 34)
        {
            // Read the state in streamwise fashion.
            QDataStream stream(state);

            // The first 8 bytes are resp. the magic number and the version
            // (which should be 0, otherwise it's not saved with an older
            // digiKam version). Then follows the list of window sizes.
            qint32     marker;
            qint32     version = -1;
            QList<int> sizesList;

            stream >> marker;
            stream >> version;

            if (version == 0)
            {
                stream >> sizesList;

                if (sizesList.count() == 3)
                {
                    qCDebug(DIGIKAM_GENERAL_LOG) << "Found splitter based config, converting to dockbar";
                    // Remove the first entry (the thumbbar) and write the rest
                    // back. Then it should be fine.
                    sizesList.removeFirst();
                    QByteArray newData;
                    QDataStream newStream(&newData, QIODevice::WriteOnly);
                    newStream << marker;
                    newStream << version;
                    newStream << sizesList;
                    char s[24];
                    int numBytes = stream.readRawData(s, 24);
                    newStream.writeRawData(s, numBytes);
                    group.writeEntry(configSplitterStateEntry, newData.toBase64());
                }
            }
        }
    }
}

void EditorWindow::Private::plugNewVersionInFormatAction(EditorWindow* const q, QMenu* const menuAction,
                                                         const QString& text, const QString& format)
{
    if (!formatMenuActionMapper)
    {
        formatMenuActionMapper = new QSignalMapper(q);

        connect(formatMenuActionMapper, SIGNAL(mapped(QString)),
                q, SLOT(saveNewVersionInFormat(QString)));
    }

    QAction* const action = new QAction(text, q);

    connect(action, SIGNAL(triggered()),
            formatMenuActionMapper, SLOT(map()));

    formatMenuActionMapper->setMapping(action, format);
    menuAction->addAction(action);
}

void EditorWindow::Private::addPageUpDownActions(EditorWindow* const q, QWidget* const w)
{
    defineShortcut(w, Qt::Key_Down,  q, SLOT(slotForward()));
    defineShortcut(w, Qt::Key_Right, q, SLOT(slotForward()));
    defineShortcut(w, Qt::Key_Up,    q, SLOT(slotBackward()));
    defineShortcut(w, Qt::Key_Left,  q, SLOT(slotBackward()));
}

}  // namespace Digikam

#endif /* EDITORWINDOWPRIVATE_H */
