/* ============================================================
 * Author: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *         J�rn Ahrens <joern.ahrens@kdemail.net>
 * Date  : 2005-02-06
 * Copyright 2005 by Renchi Raju, J�rn Ahrens
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

#ifndef UNDOACTION_H
#define UNDOACTION_H

#include <klocale.h>

namespace Digikam
{
class ImlibInterface;
}

class UndoAction
{
public:

    UndoAction(Digikam::ImlibInterface* iface);
    virtual ~UndoAction();

    virtual void rollBack() = 0;
    virtual void execute()  = 0;

    QString getTitle() const;
    
protected:

    Digikam::ImlibInterface* m_iface;
    QString                  m_title;
};

class UndoActionRotate : public UndoAction
{
public:

    enum Angle
    {
        R90,
        R180,
        R270
    };
    
    UndoActionRotate(Digikam::ImlibInterface* iface, Angle angle);
    ~UndoActionRotate();

    void rollBack();
    void execute();

private:

    int m_angle;
};

class UndoActionFlip : public UndoAction
{
public:

    enum Direction
    {
        Horizontal,
        Vertical
    };
    
    UndoActionFlip(Digikam::ImlibInterface* iface, Direction dir);
    ~UndoActionFlip();

    void rollBack();
    void execute();

private:

    int m_dir;
};

class UndoActionBCG : public UndoAction
{
public:

    UndoActionBCG(Digikam::ImlibInterface* iface,
                  double oldGamma, double oldBrightness,
                  double oldContrast, double newGamma,
                  double newBrightness, double newContrast);
    ~UndoActionBCG();

    void rollBack();
    void execute();    

private:

    double m_oldGamma;
    double m_oldBrightness;
    double m_oldContrast;
    double m_newGamma;
    double m_newBrightness;
    double m_newContrast;
};

class UndoActionIrreversible : public UndoAction
{
public:

    UndoActionIrreversible(Digikam::ImlibInterface* iface, 
                           const QString &caller=i18n("unknown"));
    ~UndoActionIrreversible();

    void rollBack();
    void execute();

};

#endif /* UNDOACTION_H */
