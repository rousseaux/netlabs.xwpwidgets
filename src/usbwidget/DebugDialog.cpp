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
    this->debugMe();
    sprintf(this->buf, "DebugDialog() : hdlg=%08X", this->hwndSelf);
    __debug(NULL, this->buf, DBG_LBOX);
}

/*
// Destructor
*/
DebugDialog::~DebugDialog() {
    sprintf(this->buf, "~DebugDialog() : hdlg=%08X", this->hwndSelf);
    __debug(NULL, this->buf, DBG_LBOX);
    this->destroy();
}

/*
// create :: Create a new Dialog
*/
ulong   DebugDialog::create() {

    /*
    // This buffer contains a pointer to the instance of this Dialog.
    // It is passed as a max-64KB buffer with the 16-bit word at offset 0 containing
    // the length of the data.
    */
    char    myself[sizeof(short)+sizeof(Object*)];        // Allocate the buffer; all classes descend from Object.

    /* Initialize it */
    myself[0]               = (short) sizeof(myself);   // Size of the buffer as a 16-bit word at offset 0.
    myself[sizeof(short)]   = (ulong) this;             // The pointer to this Dialog instance.

    /* Load the Dialog */
    this->hwndSelf =    WinLoadDlg(
                            hdlgDebugDialog,                // parent
                            hdlgDebugDialog,                // owner
                            (PFNWP) classMessageHandler,    // handler
                            //MyDialogHandler_3,
                            hmodMe,                         // module; GLOBAL VAR!! CHANGE THIS !!
                            ID_NOTIFICATION_DIALOG,         // id
                            myself                          // parameters
                        );

    /* Show it */
    this->show();

    /* Debug Info */
    if (this->debugMe()) {
        sprintf(this->buf, "DebugDialog::create() : hdlg=%08X, myself=%08X", this->hwndSelf, myself);
        __debug(NULL, this->buf, DBG_LBOX);
    }

    /* Return the handle of the new Dialog */
    return this->hwndSelf;
}

/*
// detroy :: Destroy the Dialog and any resources it uses.
*/
ulong   DebugDialog::destroy() {
    if (this->hwndSelf) {
        WinDestroyWindow(this->hwndSelf);
        this->hwndSelf = NULL;
    }
    return true;
}

/*
// redraw :: Repaint the content of the Dialog.
*/
ulong   DebugDialog::redraw() {
    return true;
}

/*
// msgInitDialog :: Handles the WM_INITDLG message sent on window-creation.
*/
ulong   DebugDialog::msgInitDialog(ulong mp1, ulong mp2) {
    return true;
}

/*
// msgCommand :: Handles the WM_COMMAND messages sent by controls.
*/
ulong   DebugDialog::msgCommand(ulong mp1, ulong mp2) {
    return true;
}

/*
// commandDrawButton :: Handles a specific (user defined) command-message.
*/
ulong   DebugDialog::commandDrawButton() {
    return true;
}

/*
// commandDestroyButton :: Handles a specific (user defined) command-message.
*/
ulong   DebugDialog::commandDestroyButton() {
    return true;
}


/*
// classMessageHandler :: Handles all messages.
// This is the actual window-procedure which is invoked as a callback.
// Therefore it's declaration is static because it get's called externally from the dialog-instance.
// Also, it's the type EXPENTRY (_Pascal FAR) because it get's called from outside this module.
// Note that FAR is void in 32-bit mode and FAR32 is used for 32-bit far pointers.
// But we don't need that in flat 32-bit mode.
// The parameter mp2 contains a structure with a pointer to the dialog-instance that invoked this handler
// when the message WM_INITDLG is received.
*/
ulong   DebugDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2) {

    //return MyDialogHandler_3(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);

    MRESULT mresReply = 0;

    switch (msg) {

        //!: Init dialog (Debug)
        case WM_INITDLG: {
            mresReply = FALSE;
            break;
        }

        case WM_COMMAND: {
            switch (SHORT1FROMMP(mp1)) {

                /* Close Dialog */
                case DID_OK: {
                    mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
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
    return (ulong) mresReply;
}


