/*****************************************************************************\
* WidgetSettings.hpp -- NeoWPS * USB Widget                                   *
*                                                                             *
* Copyright (c) RDP Engineering                                               *
*                                                                             *
* Author: Ben Rietbroek <rousseau.os2dev@gmx.com>                             *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,                *
*   MA 02110-1301, USA.                                                       *
*                                                                             *
\*****************************************************************************/

// From GUIHelpers.hpp

/**
 * This module contains the new WidgetSettings Notebook.
 */


#ifndef     __WIDGETSETINGS_HPP__
#define     __WIDGETSETINGS_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"
#include    "ModuleGlobals.hpp"
#include    "ecomedia.h"

#include    "Notebook.hpp"

/* Prototypes */
MRESULT EXPENTRY WidgetSettingsDialogHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

class   WidgetSettingsDialog : public Dialog {
    public:
    HWND        hwndParent;
    HWND        hwndSelf;
    Notebook*   settings;
    WidgetSettingsDialog();
    virtual ~WidgetSettingsDialog();
    virtual int create(void);
    virtual int process(void);
    virtual int destroy(void);

    virtual int test123(void);

    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);


// The _System qualifier can not be applied... (when a static method ?)
    //~ static ulong classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);

    private:
    DLG_CLASS_INSTANCE dci;
};


class   WidgetSettingsDialogEx : public WidgetSettingsDialog {
    public:
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
};


#ifdef      __cplusplus
    }
#endif

#endif // __WIDGETSETINGS_HPP__
