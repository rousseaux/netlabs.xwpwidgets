/*****************************************************************************\
* Debug.cpp -- NeoWPS * USB Widget                                            *
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
 * This module add several debugging fascilities.
 * It's purpose is to log debug information to none or more outputs.
 */

#include    "Debug.hpp"

//! FIXME: THE DELAY CULPRIT !!!!
///
/// When there are no PM applications or Windowed Sessions running, it
/// takes a *long* time for the dialog to be loaded. As soons as an
/// app is running the delay vanishes.
///
/// It is the WM_INITDLG2 custom message that causes the delay.
/// This message initializes the controls, but even when returning 0,
/// the delay stays.
///
/// It is the creation of the controls that take a long time.
///
/// Nope, it's just the creation of the dialog.
/// Why would that take a long time when no apps are running ?
///
/// Relocated creation to Widget Window Creation.
void    CreateDebugDialog() {

    /*
    ** Create the Debug Dialog if it does not exist yet.
    ** It could exist because it is currently destroyed in Module Uninit.
    ** So a widget delete will not destroy it.
    */
    if (!hdlgDebugDialog) {
        hdlgDebugDialog = WinLoadDlg(
            HWND_DESKTOP,           // Parent
            HWND_DESKTOP,           // Owner
            MyDialogHandler_1,      // Dialog Procedure
            hmodMe,                 // Module
            ID_DEBUG_DIALOG,        // Dialog ID
            NULL                    // Create Parameters
        );
    }
}

void    DestroyDebugDialog() {
    if (hdlgDebugDialog) {
        WinDestroyWindow(hdlgDebugDialog);
        hdlgDebugDialog = NULL;
    }
}



void    __debug(char* title, char* message, unsigned long flags) {
#if     DEBUG & DBG_ON
    unsigned long   lflags      = 0;
    char            buf[2048]   = "\0";
    char            buf2[256]   = "\0";
    unsigned long   ulrc        = -1;
    unsigned long   i           = 0;

    lflags = flags ? flags : DEBUG;


    // Create mutex semaphore to control access to serial port.
    if (!hmtxAuxDebug) {
        ulrc = DosCreateMutexSem(
            NULL,
            &hmtxAuxDebug,
            DC_SEM_SHARED,
            FALSE
        );
    }

    sprintf(buf, "[%08lX]", lflags);

    if (lflags) {

        /* Insert title if any and trail it with ": " */
        if (title) {
            //~ strcpy(buf, title);
            strcat(buf, title);
            strcat(buf, ": ");
        }

        /* Append the message if any */
        if (message) {
            strcat(buf, message);
        }

        /* Output to stdout */
        if (lflags & DBG_STDOUT) {
            fprintf(stdout, "%s\n", buf);
            fflush(stdout);
        }
        /* Output to stderr */
        if (lflags & DBG_STDERR) {
            fprintf(stderr, "%s\n", buf);
            fflush(stderr);
        }

        /* Show popup */
        if (lflags & DBG_POPUP) {
            WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, buf, "Debug Popup", NULL, MB_OK);
        }

        /* Output to ListBox of Debug Dialog */
        if (lflags & DBG_LBOX) {
            if (lflags & DBG_USE_POST) {
                WinPostMsg(
                    WinWindowFromID(hdlgDebugDialog, ID_LB_1),
                    LM_INSERTITEM,
                    (MPARAM) LIT_END,
                    (MPARAM) buf
                );
                //~ strcat(buf," [posted]");
                WinPostMsg(WinWindowFromID(hdlgDebugDialog, ID_LB_1), LM_SETTOPINDEX, (MPARAM) 32767, (MPARAM) 0);
            }
            else {
                WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_LB_1), LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) buf);
                WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_LB_1), LM_SETTOPINDEX, (MPARAM) 32767, (MPARAM) 0);
            }
        }

        /* Output to MLE of Debug Dialog */
        if (lflags & DBG_MLE) {
            if (lflags & DBG_USE_POST) {
                //~ strcat(buf, " [posted]");
                WinPostMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) buf, (MPARAM) NULL);
                WinPostMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) "\n", (MPARAM) NULL);
            }
            else {
                //~ strcat(buf, " [sent]");
                WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) buf, (MPARAM) NULL);
                WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) "\n", (MPARAM) NULL);
            }
            /// scroll to end message must come here
        }

        /* Output to Debug Pipe */
        if (lflags & DBG_PIPE) {
            // Not Implemented Yet
        }

        /* Output to Debug File */
        if (lflags & DBG_FILE) {
            // Not Implemented Yet
        }


/** Only do Aux Debug if globally enabled **/
#if     AUX_DEBUG & DBG_ON

        /*
        // Output to Com Port using mutex semaphore.
        // The message is lost if the semaphore cannot be obtained
        // within 100ms.
        */
        if (lflags & DBG_AUX) {
            FILE*   haux        = NULL;

            /* Request ownership of the semaphore */
            ulrc = DosRequestMutexSem(hmtxAuxDebug, 100);

            /* Log the line to the com-port */
            if (ulrc == NO_ERROR) {
                haux = fopen("com1", "w");
                fprintf(haux, buf);
                fprintf(haux, "\n");
                fflush(haux);
                fclose(haux);
                ulrc = DosReleaseMutexSem(hmtxAuxDebug);
            }
        }
#endif  // AUX_DEBUG

    }
#endif  // DEBUG
    return;
}


