/* ============================================================
 * File  : imageplugin_antivignetting.cpp
 * Author: Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2004-12-25
 * Description : 
 * 
 * Copyright 2004 by Gilles Caulier
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

// KDE includes.
  
#include <klocale.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <kaction.h>
#include <kcursor.h>
#include <kdebug.h>

// Local includes.

#include "imageeffect_antivignetting.h"
#include "imageplugin_antivignetting.h"

K_EXPORT_COMPONENT_FACTORY( digikamimageplugin_antivignetting,
                            KGenericFactory<ImagePlugin_AntiVignetting>("digikamimageplugin_antivignetting"));

ImagePlugin_AntiVignetting::ImagePlugin_AntiVignetting(QObject *parent, const char*, const QStringList &)
                          : Digikam::ImagePlugin(parent, "ImagePlugin_AntiVignetting")
{
    new KAction(i18n("Anti Vignetting..."), 0, 
                this, SLOT(slotAntiVignetting()),
                actionCollection(), "imageplugin_antivignetting");
                
    
    kdDebug() << "ImagePlugin_AntiVignetting plugin loaded" << endl;
}

ImagePlugin_AntiVignetting::~ImagePlugin_AntiVignetting()
{
}

QStringList ImagePlugin_AntiVignetting::guiDefinition() const
{
    QStringList guiDef;
    guiDef.append("MenuBar/Menu/Fi&x/Fix/Action/imageplugin_antivignetting/ ");    
    return guiDef;
}

void ImagePlugin_AntiVignetting::slotAntiVignetting()
{
    DigikamAntiVignettingImagesPlugin::ImageEffect_AntiVignetting dlg(parentWidget());
    dlg.exec();
}


#include "imageplugin_antivignetting.moc"
