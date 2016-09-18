/*****************************************************************************\
* WidgetSettingsDialog.cpp -- NeoWPS * USB Widget                             *
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

/******************************************************************************
* This module holds the new Widget Settings Dialog
* -----------------------------------------------------------------------------
* It contains a Notebook Control so that settings can be grouped according to
* their category. From the C++ point-of-view, an instance wraps around a Dialog
* and messages are deferred to instance-members.
*
*/


#include    "WidgetSettingsDialog.hpp"


/*
// WidgetSettingsDialog
*/
WidgetSettingsDialog::WidgetSettingsDialog() {
    this->debugMe();
    this->notebook = NULL;
    //~ MessageBox("WidgetSettingsDialog","CONSTRUCTOR");
    sprintf(this->buf, "WidgetSettingsDialog(): this=%08X", this);
    MessageBox("WIDGETSETTINGSDIALOG", this->buf);
}

WidgetSettingsDialog::~WidgetSettingsDialog() {
    //~ MessageBox("WidgetSettingsDialog","DESTRUCTOR");
}

int WidgetSettingsDialog::create() {
    //~ MessageBox("WidgetSettingsDialog","CREATE");

    /* Load the WidgetSettings Dialog */
    this->hwndSelf =    WinLoadDlg(
                            HWND_DESKTOP,
                            NULL,
                            //~ (PFNWP) WidgetSettingsDialog::classMessageHandler,
                            //~ (PFNWP) MyDialogHandler_1,
                            (PFNWP) DlgProcWidgetSettingsDialog,
                            hmodMe,
                            DLG_ID_WIDGETSETTINGS,
                            //~ ID_DEBUG_DIALOG,
                            &this->wci
                        );

    /* Show message if creation failed */
    if (this->hwndSelf) {
        this->hwndParent = HWND_DESKTOP;
        //~ MessageBox("WinLoadDlg", "OK");
    } else {
        MessageBox("WinLoadDlg", "NULL");
    }

    /* Create a new Notebook to manage the Notebook Control */
    this->notebook = new Notebook();

    /* Populate the Notebook with Pages */
    if (this->notebook) {

        /* Initialize Notebook */
        this->notebook->init(this->hwndSelf, DLG_ID_WIDGETSETTINGS_NOTEBOOK);

        /* Append the pages */
        this->notebook->appendPages();
    }

    return NULL;
}

int WidgetSettingsDialog::process() {
    MessageBox("WidgetSettingsDialog","PROCESS");
    int reply = NULL;
    reply = WinProcessDlg(this->hwndSelf);
    return reply;
}

int WidgetSettingsDialog::destroy() {
    MessageBox("WidgetSettingsDialog","DESTROY");
    if (this->notebook) delete this->notebook;
    this->notebook = NULL;
    if (this->hwndSelf) WinDestroyWindow(this->hwndSelf);
    this->hwndSelf = NULL;
    this->hwndParent = NULL;
    this->hwndSelf = NULL;
    return NULL;
}

int WidgetSettingsDialog::test123() {
    MessageBox("WidgetSettingsDialog","test123");
    return 0;
}

MRESULT WidgetSettingsDialog::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;
    switch (SHORT1FROMMP(mp1)) {

        /* Widget Settings Close Button */
        case DLG_ID_WIDGETSETTINGS_CLOSEBUTTON: {
            MessageBox("WidgetSettingsDialog","Dismissing Dialog");
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
    return (MRESULT) mresReply;
}


/* Override */
MRESULT WidgetSettingsDialogEx::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;
    switch (SHORT1FROMMP(mp1)) {

        /* Close Button */
        case DLG_ID_WIDGETSETTINGS_CLOSEBUTTON: {
            MessageBox("WidgetSettingsDialogEx","Dismissing Dialog -- wmCommand Override !!");
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
    return (MRESULT) mresReply;
}

/* Redirect messages for the WidgetSettingsDialog */
MRESULT DlgProcWidgetSettingsDialog(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

    /* Locals */
    BOOL    brc = FALSE;
    MRESULT mresReply = 0;
    CHAR    buf[512] = {0};

    /*
    // Get pointer to Class Instance.
    // Can be NULL if WM_INITDLG has not been invoked yet.
    // Using this pointer, messages can be deferred to member-functions.
    */
    WidgetSettingsDialog*   pWsd = (WidgetSettingsDialog*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Message Switch */
    switch (msg) {

        /* Initialize Dialog */
        case WM_INITDLG: {
            //~ MessageBox("DlgProcWidgetSettingsDialog","pWsd is NULL !!");
            /* Get pointer to Class Instance from the CreateParams passed to WinLoadDlg() */
            pWsd = ((WidgetSettingsDialog*)((WND_CLASS_INSTANCE*)mp2)->pvClassInstance);
            /* Assign the pointer to QWL_USER so it can be retrieved in message-cases */
            brc =   WinSetWindowPtr(
                        hwnd,
                        QWL_USER,
                        pWsd
                    );
            pWsd->test123();
            mresReply = FALSE;
            break;
        }

        /* Defer Command Messages to Class Instance */
        case WM_COMMAND: {
            mresReply = pWsd ? pWsd->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DlgProcWidgetSettingsDialog","pWsd is NULL !!");
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
