/*****************************************************************************\
* WidgetSettings.cpp -- NeoWPS * USB Widget                                   *
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

// From GUIHelpers.cpp

/**
 * This module contains the new WidgetSettings Notebook.
 */


#include    "WidgetSettings.hpp"


void    WidgetSettings(void) {
}

/*
// WidgetSettings
*/
WidgetSettings::WidgetSettings() {
    MessageBox("WidgetSettings","CONSTRUCTOR");
}

WidgetSettings::~WidgetSettings() {
    MessageBox("WidgetSettings","DESTRUCTOR");
}

int WidgetSettings::create() {
    MessageBox("WidgetSettings","CREATE");
    this->handle = WinLoadDlg(
                    HWND_DESKTOP,
                    NULL,
                    //~ (PFNWP) WidgetSettings::classMessageHandler,
                    //~ (PFNWP) MyDialogHandler_1,
                    (PFNWP) WidgetSettingsHandler,
                    hmodMe,
                    DLG_ID_WIDGETSETTINGS,
                    //~ ID_DEBUG_DIALOG,
                    NULL);

    if (this->handle) {
        MessageBox("WinLoadDlg", "OK");
    } else {
        MessageBox("WinLoadDlg", "NULL");
    }


    return NULL;
}

//~ int WidgetSettings::show() {
    //~ MessageBox("Widget Settings #1","Widget Settings #2");
    //~ return NULL;
//~ }

int WidgetSettings::process() {
    MessageBox("WidgetSettings","PROCESS");
    int reply = NULL;
    reply = WinProcessDlg(this->handle);
    return reply;
}

int WidgetSettings::destroy() {
    MessageBox("WidgetSettings","DESTROY");
    WinDestroyWindow(this->handle);
    this->handle = NULL;
    return NULL;
}




//~ ulong   WidgetSettings::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2) {
MRESULT EXPENTRY WidgetSettingsHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

    MRESULT mresReply = 0;
    HWND    hwndNB = NULL;

    switch (msg) {

        //!: Init dialog (Debug)
        case WM_INITDLG: {
            hwndNB = WinWindowFromID(hwnd, DLG_ID_WIDGETSETTINGS_NOTEBOOK);

            mresReply = FALSE;
            break;
        }

        case WM_COMMAND: {
            switch (SHORT1FROMMP(mp1)) {

                /* Close Button */
                case 9003: {
                    mresReply = (MRESULT) WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
                    break;
                }

                /* Default */
                default: {
                    mresReply = 0;
                    //mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);           // NO DEFAULT HANDLING OF COMMANDS !!
                    break;
                }

            } // switch
            break;
        }
        //~: Handle default message (debug dialog)
        default: {
            mresReply = 0;
            mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
        }
    }
    return mresReply;
}
