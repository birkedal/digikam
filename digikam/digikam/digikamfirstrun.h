/* ============================================================
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *         Gilles Caulier <caulier dot gilles at free.fr>
 * Date  : 2003-02-01
 * Description : 
 * 
 * Copyright 2003-2004 by Renchi Raju
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

#ifndef DIGIKAMFIRSTRUN_H
#define DIGIKAMFIRSTRUN_H

// Qt includes.

#include <kdialogbase.h>

class KConfig;
class KURLRequester;
class DigikamFirstFirstRunWidget;

class DigikamFirstRun : public KDialogBase
{
    Q_OBJECT

public:
    DigikamFirstRun( KConfig* config,
                     QWidget* parent = 0,
                     const char* name = 0,
                     bool modal = true,
                     WFlags fl = WDestructiveClose );
    ~DigikamFirstRun();

private:
    KConfig*     config_;
    DigikamFirstFirstRunWidget *ui;

protected slots:
    void slotOk();
};

#endif // DIGIKAMFIRSTRUN_H
