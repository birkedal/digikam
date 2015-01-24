/** ===========================================================
 * @file
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * @date   2010-06-21
 * @brief  CLI test program for digiKam DB init
 *
 * @author Copyright (C) 2014-2015 by Gilles Caulier
 *         <a href="mailto:caulier dot gilles at gmail dot com">caulier dot gilles at gmail dot com</a>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt includes

#include <QApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QDBusConnection>
#include <QString>
#include <QTimer>
#include <QCommandLineParser>
#include <QDebug>

// KDE includes

#include <kaboutdata.h>
#include <klocalizedstring.h>

// digiKam includes

#include "daboutdata.h"
#include "albummanager.h"
#include "collectionlocation.h"
#include "collectionmanager.h"
#include "databaseaccess.h"
#include "databaseparameters.h"
#include "scancontroller.h"
#include "setup.h"
#include "thumbnaildatabaseaccess.h"
#include "digikam_version.h"

namespace Digikam
{

bool Setup::execSinglePage(Page)
{
    return true;
}

}

using namespace Digikam;

int main(int argc, char** argv)
{
    KAboutData aboutData("digikam",
                         0,
                         ki18n("digiKam"),
                         digiKamVersion().toAscii(),
                         DAboutData::digiKamSlogan(),
                         KAboutLicense::GPL,
                         DAboutData::copyright(),
                         additionalInformation(),
                         DAboutData::webProjectUrl().url().toUtf8());

    QApplication app(argc, argv);
    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    //PORTING SCRIPT: adapt aboutdata variable if necessary
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    DatabaseParameters params;
    params.databaseType = DatabaseParameters::SQLiteDatabaseType();
    params.setDatabasePath(QDir::currentPath() + "/digikam-test.db");
    params.setThumbsDatabasePath(QDir::currentPath() + "/digikam-thumbs-test.db");

    params.legacyAndDefaultChecks();

    QDBusConnection::sessionBus().registerService("org.kde.digikam.startup-" +
                     QString::number(QCoreApplication::instance()->applicationPid()));

    // initialize database
    bool b = AlbumManager::instance()->setDatabase(params, false, "/media/fotos/Digikam Sample/");

    qDebug() << "Database initialization done: " << b;

    QTimer::singleShot(500, &app, SLOT(quit()));
    app.exec();

    ScanController::instance()->shutDown();

    DatabaseAccess::cleanUpDatabase();
    ThumbnailDatabaseAccess::cleanUpDatabase();

    return 0;
}
