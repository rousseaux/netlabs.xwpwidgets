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


/*
// WidgetSettingsDialog
*/
WidgetSettingsDialog::WidgetSettingsDialog() {
    this->hwndParent = NULL;
    this->hwndSelf = NULL;
    this->settings = NULL;
    //~ MessageBox("WidgetSettingsDialog","CONSTRUCTOR");
}

WidgetSettingsDialog::~WidgetSettingsDialog() {
    //~ MessageBox("WidgetSettingsDialog","DESTRUCTOR");
}

int WidgetSettingsDialog::create() {
    //~ MessageBox("WidgetSettingsDialog","CREATE");

    /* Load the WidgetSettings Dialog */
    this->handle =  WinLoadDlg(
                        HWND_DESKTOP,
                        NULL,
                        //~ (PFNWP) WidgetSettingsDialog::classMessageHandler,
                        //~ (PFNWP) MyDialogHandler_1,
                        (PFNWP) WidgetSettingsDialogHandler,
                        hmodMe,
                        DLG_ID_WIDGETSETTINGS,
                        //~ ID_DEBUG_DIALOG,
                        NULL
                    );

    if (this->handle) {
        this->hwndParent = HWND_DESKTOP;
        this->hwndSelf = this->handle;
        //~ MessageBox("WinLoadDlg", "OK");
    } else {
        MessageBox("WinLoadDlg", "NULL");
    }

    /* Create a new Notebook to manage the Notebook Control */
    this->settings = new Notebook();

    /* Populate the Notebook with Pages */
    if (this->settings) {
        this->settings->idResource = DLG_ID_WIDGETSETTINGS_NOTEBOOK;
        this->settings->hwndParent = this->hwndSelf;
        this->settings->hwndSelf = WinWindowFromID(this->hwndSelf, this->settings->idResource);

        /* Set dimensions of Major Tabs -- can resize Notebook */
        WinSendMsg(settings->hwndSelf,
            BKM_SETDIMENSIONS,
            MPFROM2SHORT(80, 40),
            MPFROMLONG(BKA_MAJORTAB)
        );

        /* Set dimensions of Minor Tabs -- can resize Notebook */
        //~ WinSendMsg(settings->hwndSelf,
            //~ BKM_SETDIMENSIONS,
            //~ MPFROM2SHORT(80, 25),
            //~ MPFROMLONG(BKA_MINORTAB)
        //~ );

        /* Set dimensions of prev/next page button */
        //~ WinSendMsg(settings->hwndSelf,
            //~ BKM_SETDIMENSIONS,
            //~ MPFROM2SHORT(30, 30),
            //~ MPFROMLONG(BKA_PAGEBUTTON)
        //~ );

        /* Append the pages */
        this->settings->appendPages();
    }

    return NULL;
}

int WidgetSettingsDialog::process() {
    MessageBox("WidgetSettingsDialog","PROCESS");
    int reply = NULL;
    reply = WinProcessDlg(this->handle);
    return reply;
}

int WidgetSettingsDialog::destroy() {
    MessageBox("WidgetSettingsDialog","DESTROY");
    if (this->settings) delete this->settings;
    this->settings = NULL;
    if (this->handle) WinDestroyWindow(this->handle);
    this->handle = NULL;
    this->hwndParent = NULL;
    this->hwndSelf = NULL;
    return NULL;
}




//~ ulong   WidgetSettingsDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2) {
MRESULT EXPENTRY WidgetSettingsDialogHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

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
