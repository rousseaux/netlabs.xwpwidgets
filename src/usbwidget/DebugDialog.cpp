/*****************************************************************************\
* DebugDialog.cpp -- NeoWPS * USB Widget                                      *
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


/**
 * This module contains the DebugDialog.
 */

#include    "Debug.hpp"

//~ #include    "GUIHelpers.hpp"

#include    "DebugDialog.hpp"

/*
// Constructor
*/
DebugDialog::DebugDialog() {
    __ctorb();
    this->debugMe();
    this->notebook = NULL;
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    __ctore();
}

/*
// Destructor
*/
DebugDialog::~DebugDialog() {
    __dtorb();
    //~ MessageBox("DebugDialog","DESTRUCTOR");
    this->destroy();
    __dtore();
}

/*
// create :: Create a new Dialog
*/
int DebugDialog::create() {
    __mthd();

    /* Load dialog is not already loaded */
    if (this->hwndSelf == NULL) {

        /* Load the WidgetSettings Dialog */
        this->hwndSelf =    WinLoadDlg(
                                HWND_DESKTOP,
                                NULL,
                                (PFNWP) DebugDialogProc,
                                hmodMe,
                                ID_DEBUG_DIALOG,
                                &this->wci
                            );
    }

    /* Show dialog if creation succeeded */
    if (this->hwndSelf) {
        this->hwndParent = HWND_DESKTOP;
        this->show();
        //~ MessageBox("WinLoadDlg", "OK");
    }
    /* Show message if creation failed */
    else {
        MessageBox("WinLoadDlg", "NULL");
    }
#if 0
    /* Create a new Notebook to manage the Notebook Control */
    if (this->notebook == NULL) this->notebook = new Notebook();

    /* Populate the Notebook with Pages */
    if (this->notebook) {

        /* Initialize Notebook */
        this->notebook->init(this->hwndSelf, DLG_ID_WIDGETSETTINGS_NOTEBOOK);

        /* Append the pages */
        this->notebook->appendPages();
    }
    else {
        MessageBox("Notebook", "NULL");
    }
#endif
    return NULL;
}

int DebugDialog::process() {
    __mthd();
    int reply = NULL;
    reply = WinProcessDlg(this->hwndSelf);
    return reply;
}

int DebugDialog::destroy() {
    __mthd();
    if (this->notebook || this->hwndSelf) {
        if (this->notebook) delete this->notebook;
        this->notebook = NULL;
        if (this->hwndSelf) WinDestroyWindow(this->hwndSelf);
        this->hwndSelf = NULL;
        this->hwndParent = NULL;
    }

    return NULL;
}

int DebugDialog::test123() {
    __mthd();
    return 0;
}

MRESULT DebugDialog::wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    BOOL brc = FALSE;
    /* Default processing dismisses the dialog but does not destroy it */
    return this->wmDefault(hwnd, msg, mp1, mp2);
    //~ return (MRESULT) 0;
}

MRESULT DebugDialog::wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) 0;
}

/* Handle Command Messages */
MRESULT DebugDialog::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthdb();
    MRESULT mresReply = 0;
    switch (SHORT1FROMMP(mp1)) {

        /* Widget Settings Close Button */
        case DLG_ID_WIDGETSETTINGS_CLOSEBUTTON: {
            //~ MessageBox("WidgetSettingsDialog","Dismissing Dialog");
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
    __mthde();
    return (MRESULT) mresReply;
}

MRESULT DebugDialog::wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) TRUE;
}

MRESULT DebugDialog::wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    //~ __mthd();
    BOOL    brc = FALSE;
    HPS     hps = NULL;
    RECTL    rcl;

    /* Invalidate the given rectangle */
    do {
        break;
        hps = WinBeginPaint(hwnd, NULL, &rcl);
        brc = WinEndPaint(hps);
    } while (0);

    return this->wmDefault(hwnd, msg, mp1, mp2);
    //~ return NULL;
}

MRESULT DebugDialog::wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    //~ __mthd();
    return (MRESULT) WinDefDlgProc(hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY DebugDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

    /* Locals */
    BOOL    brc = FALSE;
    MRESULT mrc = 0;
    CHAR    buf[512] = {0};

    /*
    // Get pointer to C++ Object.
    // Can be NULL if WM_INITDLG has not been invoked yet.
    // Using this pointer, messages can be deferred to member-functions.
    */
    DebugDialog*   pWnd = (DebugDialog*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Message Switch */
    switch (msg) {

        /* Initialize Dialog */
        case WM_INITDLG: {
            printf("%s::%s WM_INITDLG: %08X, %08X, %08X, %08X\n", __FILE__, __FUNCTION__, hwnd, msg, mp1, mp2);
            //~ MessageBox("DlgProcWidgetSettingsDialog","pWnd is NULL !!");
            /* Get pointer to C++ Object from the CreateParams passed to WinLoadDlg() */
            pWnd = ((DebugDialog*)((WND_CLASS_INSTANCE*)mp2)->pvClassInstance);
            /* Assign the pointer to QWL_USER so it can be retrieved in message-cases */
            brc =   WinSetWindowPtr(
                        hwnd,
                        QWL_USER,
                        pWnd
                    );
            pWnd->test123();
            mrc = (MRESULT) TRUE;
            break;
        }

        /* Delegate the messages below */
        case WM_CLOSE:
            mrc = pWnd ? pWnd->wmClose(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_CLOSE","pWnd is NULL !!");
            break;
        case WM_DESTROY:
            mrc = pWnd ? pWnd->wmDestroy(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_DESTROY","pWnd is NULL !!");
            break;
        case WM_COMMAND:
            mrc = pWnd ? pWnd->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_COMMAND","pWnd is NULL !!");
            break;
        case WM_ERASEBACKGROUND:
            //~ return WinDefDlgProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmEraseBackground(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_ERASEBACKGROUND","pWnd is NULL !!");
            break;
        case WM_PAINT:
            //~ return WinDefDlgProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmPaint(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_PAINT","pWnd is NULL !!");
            break;

        /* Use WinDefDlgProc if the C++ Object pointer is not set yet */
        default: {
            if (pWnd) {
                //~ return WinDefDlgProc(hwnd, msg, mp1, mp2);
                mrc = pWnd->wmDefault(hwnd, msg, mp1, mp2);
            }
            else {
                //~ MessageBox("DialogWindow::WM_DEFAULT","pWnd is NULL !!");
                mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
            }
            break;
        }
    }

    return (MRESULT) mrc;
}

