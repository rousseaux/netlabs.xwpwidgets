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

#include    "APIHelpers.hpp"
#include    "GUIHelpers.hpp"
#include    "LVMHelpers.hpp"
#include    "USBHelpers.hpp"
#include    "ProblemFixer.hpp"
#include    "ProblemFixerDialog.hpp"
#include    "WidgetSettingsDialog.hpp"

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
            DebugDialogHandler,     // Dialog Procedure
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


//! BOOKMARK: Handler for Debug Dialog Messages
/**
 * Dialog Handler
 */
MRESULT EXPENTRY DebugDialogHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply           = NULL;
    HWND    hwndListBox         = NULL;
    HWND    hwndMle             = NULL;
    HWND    hwndDriveLetters    = NULL;
    HWND    hwndUnits           = NULL;

    BOOL    brc = TRUE;
    CHAR    buf[256] = {"\0"};
    static int count=0;                                         // Shared between dialog-instances !
    int     i = 0;

    //~ sprintf(buf, "  >>> DLGPROC: msg:%08lX, mp1:%08lX, mp2:%08lX", msg, mp1, mp2);
    //~ __debug("Debug Dialog", buf, DBG_AUX);


    hwndListBox = WinWindowFromID(hwnd, ID_LB_1);               // ListBox for messages
    hwndMle = WinWindowFromID(hwnd, ID_MLE_1);                  // MLE Cotrol
    hwndDriveLetters = WinWindowFromID(hwnd, ID_DRIVE_LETTERS); // ComboBox with drive-letters
    hwndUnits = WinWindowFromID(hwnd, ID_UNIT_NUMBERS);         // ComboBox with unit-numbers


    //~: Switch on message (debug dialog)
    switch (msg) {

        /*
        // We don't have a complete window yet so we postpone other stuff,
        // like adding controls, to a second stage.
        */
        case WM_INITDLG: {                                      // 0x003b

            WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "WM_INITDLG START");
            hdlgDebugDialog = hwnd;
            sprintf(buf, "%08X", hwnd);
            WinSetDlgItemText(hwnd, ID_HWND_DLG, buf);
            WinSetDlgItemText(hwnd, ID_BLDLEVEL, bldlevel);
            CenterWindow(HWND_DESKTOP, hwnd);
            //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " WM_INITDLG: hwnd=%08X, msg=%08X, mp1=%08X, mp2=%08X", hwnd, msg, mp1, mp2);
            WinSendMsg(hwnd, WM_INITDLG2, NULL, NULL);
            //~ WinPostMsg(hwnd, WM_INITDLG2, NULL, NULL);
            WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "WM_INITDLG END");
            mresReply = (MRESULT) FALSE;
            break;
        }

        /*
        ** Second stage initialization.
        ** Here we create our controls.
        */
        case WM_INITDLG2: {
            /* Local Variables */
            int     i = 0;

            WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "WM_INITDLG2 START");

            //!: Labels for the action-buttons
            PCHAR   action_names[] = {
                "RediscoverPRMs",   // ID_ACTION_1
                "OpenLVM",
                "RefreshLVM",
                "Disks",
                "Volumes",
                "CloseLVM",
                "IOCtlDump",
                "Vendor Name",
                "DeviceList",
                "QueryBootDrive",   // ID_ACTION_10
                "UsbDevInfo",
                "TermLVMthread",    // ID_ACTION_12
                "Device Insert",
                "Safe Eject",
                "Device Remove",
                "Drives Changed",
                //~ "StartFFasSess",
                //~ "StartFFasProg",
                "QueryDrive",       // ID_ACTION_17
                "PurgeDrive",       // ID_ACTION_18
                "PhysOpen",
                "PhysClose",        // ID_ACTION_20
                "DasdOpen",
                "DasdClose",
                "Create",
                "Show",
                "Hide",
                "Destroy",          // ID_ACTION_26
                "DfsVOS2",
                "FixDialog",
                NULL
            };

            /* Set the labels, NULL terminates loop */
            for (i=0; action_names[i]; i++) {
                WinSetDlgItemText(hwnd, ID_ACTION_1+i, action_names[i]);
            }


            /* Empty the units combobox */
            mresReply = WinSendMsg(hwndUnits, LM_DELETEALL, (MPARAM) NULL, (MPARAM) NULL);

            /* Insert the unit-letters */
            for (i=0; i<17; i++) {
                PhysDisks[i] = -1;
                buf[0] = i+'0';
                buf[1] = '\0';
                WinSendMsg(hwndUnits, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) buf);
            }


            /* Empty the diveletters combobox */
            mresReply = WinSendMsg(hwndDriveLetters, LM_DELETEALL, (MPARAM) NULL, (MPARAM) NULL);

            /* Insert the drive-letters */
            for (i=0; i<26; i++) {
                LogDrives[i] = -1;
                buf[0] = i+'A';
                buf[1] = '\0';
                WinSendMsg(hwndDriveLetters, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) buf);
            }

            WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "WM_INITDLG2 END");

            mresReply = 0;
            break;
        }


        //~: Handle command messages
        case WM_COMMAND: {
                                                    // 0x0020
            switch (SHORT1FROMMP(mp1)) {


                /* WM_QUIT Received so close dialog, client should handle destroy */
                case WM_QUIT: {
                    sprintf(buf, " QUITTING DIALOG");
                    __debug(NULL, buf, DBG_LBOX);
                    WinPostMsg(hwndListBox, LM_SETTOPINDEX, (MPARAM) 32767, (MPARAM) 0);

                    mresReply = (MRESULT) WinDismissDlg(hwnd, 88);
                    break;
                }

                /* Close Dialog */
                // BOOKMARK: Close Command (debug dialog)
                case DID_OK: {
                    mresReply = 0;
                    //mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);


                    sprintf(buf, " CLOSING DIALOG");
                    __debug(NULL, buf, DBG_LBOX);
                    WinPostMsg(hwndListBox, LM_SETTOPINDEX, (MPARAM) 32767, (MPARAM) 0);

                    mresReply = (MRESULT) WinDismissDlg(hwnd, TRUE);
                    //return (0L);
                    break;
                }


                /* Clear Output Windows */
                case ID_CLEAR: {
                    mresReply = 0;
                    mresReply = WinSendMsg(hwndListBox, LM_DELETEALL, (MPARAM) NULL, (MPARAM) NULL);
                    mresReply = WinSendMsg(hwndMle, MLM_SETSEL, (MPARAM) NULL, (MPARAM) 50000);
                    mresReply = WinSendMsg(hwndMle, MLM_CLEAR, (MPARAM) NULL, (MPARAM) NULL);
                    break;
                }


                /* RediscoverPRMs */
                case ID_ACTION_1: {
                    int     i = 0;
                    CARDINAL32  rc = -1;

                    Rediscover_PRMs(&rc);
                    sprintf(buf, "Rediscover_PRMs: rc=%d", rc);
                    __debug(NULL, buf, DBG_LBOX);
                    if (rc) {
                        sprintf(buf, "The LVM-Engine has to be closed for this action to succeed.", rc);
                        __debug(NULL, buf, DBG_LBOX);
                    }
                    mresReply = 0;

                    break;
                }


                /* OpenLVM */
                case ID_ACTION_2: {
                    int     i = 0;
                    CARDINAL32  rc = -1;

                    Open_LVM_Engine(TRUE, &rc);
                    sprintf(buf, "OpenLVM: rc=%d", rc);
                    __debug(NULL, buf, DBG_LBOX);
                    mresReply = 0;
                    break;
                }


                /* RefreshLVM */
                case ID_ACTION_3: {
                    int     i = 0;
                    CARDINAL32  rc = -1;

                    //~ Refresh_LVM_Engine(&rc);
                    rc = RefreshLVMEngine();
                    sprintf(buf, "RefreshLVM: rc=%d", rc);
                    __debug(NULL, buf, DBG_LBOX);
                    mresReply = 0;
                    break;
                }


                /* DumpDrives */
                case ID_ACTION_4: {
                    int     i = 0;
                    char    txtbuf[32000] = "\0";
                    mresReply = 0;

                    DumpDrives(txtbuf);
                    WinSendMsg(hwndMle, MLM_INSERT, (MPARAM) txtbuf, (MPARAM) NULL);

                    break;
                }


                /* DumpVolumes */
                case ID_ACTION_5: {
                    int     i = 0;
                    char    txtbuf[32000] = "\0";
                    mresReply = 0;

                    DumpVolumes(txtbuf);
                    WinSendMsg(hwndMle, MLM_INSERT, (MPARAM) txtbuf, (MPARAM) NULL);

                    break;
                }


                /* CloseLVM */
                case ID_ACTION_6: {
                    int     i = 0;
                    CARDINAL32  rc = -1;

                    Close_LVM_Engine();
                    sprintf(buf, "CloseLVM: OK");
                    __debug(NULL, buf, DBG_LBOX);
                    mresReply = 0;
                    break;
                }


                /* IOCtlDump */
                case ID_ACTION_7: {
                    CHAR    buf[256]    = "\0";
                    CHAR    buf2[256]   = "\0";
                    HWND    hwndDriveLetters    = NULL;



                    WinQueryDlgItemText(hwnd, ID_DRIVE_LETTERS, sizeof(buf2), buf2);
                    //sprintf(buf, "DRIVE: %c:", buf2[0]);
                    //__debug(NULL, buf, DBG_LBOX);
                    IOCtlIsBlockRemovable(buf2[0], buf, sizeof(buf));
                    __debug(NULL, buf, DBG_LBOX);

                    IOCtlGetDeviceParams(buf2[0], buf, sizeof(buf));
                    __debug(NULL, buf, DBG_LBOX);

                    IOCtlGetLockStatus(buf2[0], buf, sizeof(buf));
                    __debug(NULL, buf, DBG_LBOX);
                    IOCtlPurge(buf2[0], buf, sizeof(buf));
                    __debug(NULL, buf, DBG_LBOX);
                    sprintf(buf, "LETTER_TO_MAP: %c -> %08X", buf2[0], drive_letter_to_map(buf2[0]));
                    __debug(NULL, buf, DBG_LBOX);


                    mresReply = 0;
                    break;
                }


                /* GetVendorString */
                case ID_ACTION_8: {
                    char    buf[256] = "\0";
                    char*   p = NULL;
                    strcpy(buf, "0204:6025");
                    p = GetUsbVendorString(buf, buf, sizeof(buf));
                    __debug(NULL, buf, DBG_LBOX);
                    mresReply = 0;

                    break;
                }

                /* DumpDeviceList */
                case ID_ACTION_9: {
                    int     i = 0;

                    __debug(NULL, "", DBG_LBOX);
                    WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "START-OF-CURRENT-LIST");
                    DumpDeviceList(hwndListBox, pws->currentUSBDevicesList);
                    WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "END-OF-CURRENT-LIST");
                    __debug(NULL, "", DBG_LBOX);
                    __debug(NULL, "", DBG_LBOX);
                    __debug(NULL, "", DBG_LBOX);
                    __debug(NULL, "", DBG_LBOX);
                    WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "START-OF-PREVIOUS-LIST");
                    //~ DumpDeviceList(hwndListBox, pws->previousUSBDevicesList);
                    WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "END-OF-PREVIOUS-LIST");
                    /*
                    WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "START-OF-REMOVED-LIST");
                    DumpDeviceList(hwndListBox, pws->removedUSBDevicesList);
                    WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "END-OF-REMOVED-LIST");
                    */
                    mresReply = 0;

                    break;
                }


                /* QueryBootDrive */
                case ID_ACTION_10: {
                    int     i = 0;
                    char    buf[256] = "\0";
                    char    buf2[256] = "\0";

                    buf2[0] = QueryBootDrive();
                    buf2[1] = ':';
                    buf2[2] = '\0';

                    sprintf(buf, "QueryBootDrive: %s", buf2);
                    __debug(NULL, buf, DBG_LBOX);

                    mresReply = 0;
                    break;
                }


                /* UsbDevInfo */
                case ID_ACTION_11: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";

                    mresReply = 0;

                    WinQueryDlgItemText(hwnd, ID_DRIVE_LETTERS, sizeof(buf), buf);
                    drive = buf[0];
                    WinQueryDlgItemText(hwnd, ID_UNIT_NUMBERS, sizeof(buf), buf);
                    unit = buf[0];

                    UsbDevInfo(drive, unit);

                    break;
                }


                // BOOKMARK: STOP LVM-THREAD
                /* Stop LVMthread */
                case ID_ACTION_12: {
                    int     i           = 0;
                    APIRET  ulrc        = -1;
                    char    buf[256]    = "\0";

                    mresReply = 0;

                    lvm_thread_working = FALSE;
                    ulrc = DosPostEventSem(hevPushLVMthread);
                    //~ DosBlock(100);
                    MyDosSleep(100);


                    sprintf(buf, "ulrc: %d", ulrc);
                    __debug("DosPostEventSem", buf, DBG_LBOX);

                    for (i=0; i<10; i++) {
                        ulrc = DosWaitThread(&tid_lvm, DCWW_NOWAIT);
                        if (ulrc == 0 || ulrc == ERROR_INVALID_THREADID)
                            break;
                        sprintf(buf, "i:%02d, tid:%08lX, ulrc:%d", i, tid_lvm, ulrc);
                        __debug("DosWaitThread-xx", buf, DBG_LBOX);
                        //~ DosBlock(100);
                        MyDosSleep(100);
                    }

                    if (ulrc == NO_ERROR || ERROR_INVALID_THREADID) {
                        sprintf(buf, "-- lvm_thread stopped: ulrc=%ld, tid_lvm=%08X", ulrc, tid_lvm);
                        __debug(NULL, buf, DBG_LBOX);
                    }

                    break;
                }


                /* DevInsertPopUp */
                case ID_ACTION_13: {
                    int     i = 0;
                    mresReply = 0;
                    PopUpChangedDevices(SHOW_DEV_INSERT, NULL);
                    WinSetWindowPos(hwndIndiDevInsert, HWND_TOP, 0, 0, 0, 0, SWP_SHOW | SWP_ZORDER);
                    break;
                }


                /* DevSafeEjectPopUp */
                case ID_ACTION_14: {
                    int     i = 0;
                    mresReply = 0;
                    PopUpChangedDevices(SHOW_SAFE_EJECT, NULL);
                    WinSetWindowPos(hwndIndiSafeEject, HWND_TOP, 0, 0, 0, 0, SWP_SHOW | SWP_ZORDER);
                    break;
                }


                /* DeviceRemovePopUp */
                case ID_ACTION_15: {
                    int     i = 0;
                    mresReply = 0;
                    PopUpChangedDevices(SHOW_DEV_REMOVE, NULL);
                    break;
                }


                /* DrivesChangedPopUp */
                case ID_ACTION_16: {
                    int     i = 0;
                    mresReply = 0;
                    //DosSleep(1500);                   // blokkeert de message-queue.
                    PopUpChangedDevices(SHOW_DRIVES_CHANGED, NULL);
                    break;
                }

                ///
                //!: QUERY DRIVE -- USE HERE TO TACKLE PHANTOMS (message)
                ///
                /* QueryDrive (old StartFFasSesion) */
                case ID_ACTION_17: {
                    int     i = 0;
                    char    drive = '*';
                    mresReply = 0;
                    char    buf[256] = "\0";
                    //~ StartFireFoxAsSession("http://www.ecomstation.com");

                    __debug(NULL, "", DBG_LBOX);
                    WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "## QueryDrive ##");
                    WinQueryDlgItemText(hwnd, ID_DRIVE_LETTERS, sizeof(buf), buf);
                    drive = buf[0];

                    QueryDrive(drive);

                    break;
                }

                ///
                //!: PURGE DRIVE -- USE HERE TO TACKLE PHANTOMS (message)
                ///
                /* PurgeDrive (old StartFFasProgram) */
                case ID_ACTION_18: {
                    int     i = 0;
                    char    drive = '*';
                    char    buf[256] = "\0";
                    mresReply = 0;
                    //~ StartFireFoxAsProgram("http://www.ecomstation.com");

                    __debug(NULL, "", DBG_LBOX);
                    WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "## PurgeDrive ##");
                    WinQueryDlgItemText(hwnd, ID_DRIVE_LETTERS, sizeof(buf), buf);
                    drive = buf[0];

                    PurgeDrive(drive);

                    break;


                }


                /* PhysOpen */
                case ID_ACTION_19: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";

                    mresReply = 0;

                    WinQueryDlgItemText(hwnd, ID_UNIT_NUMBERS, sizeof(buf), buf);
                    unit = buf[0];

                    PhysOpen(unit);

                    break;
                }


                /* PhysClose */
                case ID_ACTION_20: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";

                    mresReply = 0;

                    WinQueryDlgItemText(hwnd, ID_UNIT_NUMBERS, sizeof(buf), buf);
                    unit = buf[0];

                    PhysClose(unit);

                    break;
                }


                /* DasdOpen */
                case ID_ACTION_21: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";

                    mresReply = 0;

                    WinQueryDlgItemText(hwnd, ID_DRIVE_LETTERS, sizeof(buf), buf);
                    drive = buf[0];

                    DasdOpen(drive);
                    ShowLogicalDriveHandles();

                    break;
                }


                /* DasdClose */
                case ID_ACTION_22: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";

                    mresReply = 0;

                    WinQueryDlgItemText(hwnd, ID_DRIVE_LETTERS, sizeof(buf), buf);
                    drive = buf[0];

                    DasdClose(drive);
                    ShowLogicalDriveHandles();

                    break;
                }


                /* Create */
                case ID_ACTION_23: {
                    BOOL                brc = TRUE;
                    ULONG               ulReply = 0;
                    int                 i = 0;
                    char                drive = '*';
                    char                unit = '0';
                    char                buf[256] = "\0";
                    ulong               dskstat = 0;
                    USBDeviceReport*    usbdev = NULL;
                    FSDTYPE             fsdt = FSDT_UNKNOWN;
                    BOOL                hsecs0      = -1;

                    mresReply = 0;

                    /* Create a new WidgetSettingsDialog */
                    do {
                        /* Create instance if not existing yet */
                        if (!g_WidgetSettingsDialog) g_WidgetSettingsDialog = new WidgetSettingsDialog();

                        /* Create dialog itself */
                        if (g_WidgetSettingsDialog) {
                            g_WidgetSettingsDialog->create();
                            g_WidgetSettingsDialog->show();
                            //~ g_WidgetSettingsDialog->process();
                            //~ g_WidgetSettingsDialog->hide();
                            //~ g_WidgetSettingsDialog->destroy();
                            //~ delete g_WidgetSettingsDialog;
                            //~ g_WidgetSettingsDialog = NULL;
                        }
                    } while (0);
                    break;

                    //~ if (!g_myDebugDialog) {
                        //~ g_myDebugDialog = new DebugDialog();
                        //~ g_myDebugDialog->create();
                        //~ g_myDebugDialog->show();
                    //~ }

                    ///
                    WinQueryDlgItemText(hwnd, ID_DRIVE_LETTERS, sizeof(buf), buf);
                    drive = buf[0];
                    usbdev = DriveLetterToUsbDevice(drive);
                    dskstat = CheckDeviceSanity(usbdev);
                    ///

                    sprintf(buf, "Mask:%08lX, Disk:%d, Corrupted:%d, Beyond:%d, BigFloppy:%d, HiddenSecsZero:%d, FS:%d",
                        (ULONG) dskstat,
                        usbdev->diskindex,
                        !!(dskstat & DSKSTAT_CORRUPTED),
                        !!(dskstat & DSKSTAT_BEYONDENDDISK),
                        !!(dskstat & DSKSTAT_BIGFLOPPY),
                        !!(dskstat & DSKSTAT_HIDDENSECSZERO),
                        (dskstat & DSKSTAT_FILESYS) >> 8
                        );
                    __debug("XX CheckDeviceSanity XX", buf, DBG_MLE);

                    if (!g_ProblemFixerDialog) {
                        g_ProblemFixerDialog = new ProblemFixerDialog();
                        g_ProblemFixerDialog->create();
                        g_ProblemFixerDialog->setDiskStatus(dskstat);
                        g_ProblemFixerDialog->setDriveLetter(drive);
                        g_ProblemFixerDialog->setupFixCommands();
                        g_ProblemFixerDialog->help();
                        g_ProblemFixerDialog->show();
                        ulReply = g_ProblemFixerDialog->process();
                        sprintf(buf, "WinProcessDlg: ulReply:%08lX", ulReply);
                        __debug("ProblemFixerDialog", buf, DBG_MLE);
                    }



                    //AllocSharedMem();

                    break;
                }


                /* Show */
                case ID_ACTION_24: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";

                    mresReply = 0;


                    /* Show WidgetSettingsDialog */
                    do {
                        /* Create instance if not existing yet */
                        if (!g_WidgetSettingsDialog) g_WidgetSettingsDialog = new WidgetSettingsDialog();

                        /* Show it */
                        if (g_WidgetSettingsDialog) {
                            g_WidgetSettingsDialog->show();
                            //~ g_WidgetSettingsDialog->process();
                            //~ g_WidgetSettingsDialog->hide();
                            //~ g_WidgetSettingsDialog->destroy();
                            //~ delete g_WidgetSettingsDialog;
                            //~ g_WidgetSettingsDialog = NULL;
                        }
                    } while (0);
                    break;


                    //~ if (g_myDebugDialog) {
                        //~ g_myDebugDialog->show();
                    //~ }

                    if (g_ProblemFixerDialog) {
                        g_ProblemFixerDialog->show();
                    }

                    //GetSharedMem();

                    break;
                }

                /* Hide */
                case ID_ACTION_25: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";

                    mresReply = 0;

                    /* Hide WidgetSettingsDialog */
                    do {
                        /* Create instance if not existing yet */
                        if (!g_WidgetSettingsDialog) g_WidgetSettingsDialog = new WidgetSettingsDialog();

                        /* Hide it */
                        if (g_WidgetSettingsDialog) {
                            //~ g_WidgetSettingsDialog->show();
                            //~ g_WidgetSettingsDialog->process();
                            g_WidgetSettingsDialog->hide();
                            //~ g_WidgetSettingsDialog->destroy();
                            //~ delete g_WidgetSettingsDialog;
                            //~ g_WidgetSettingsDialog = NULL;
                        }
                    } while (0);
                    break;

                    //~ if (g_myDebugDialog) {
                        //~ g_myDebugDialog->hide();
                    //~ }

                    if (g_ProblemFixerDialog) {
                        g_ProblemFixerDialog->hide();
                    }

                    //FreeSharedMem();

                    break;
                }


                /* Destroy */
                case ID_ACTION_26: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";

                    mresReply = 0;

                    /* Destroy WidgetSettingsDialog */
                    do {
                        /* Create instance if not existing yet */
                        if (!g_WidgetSettingsDialog) g_WidgetSettingsDialog = new WidgetSettingsDialog();

                        /* Destroy it */
                        if (g_WidgetSettingsDialog) {
                            //~ g_WidgetSettingsDialog->show();
                            //~ g_WidgetSettingsDialog->process();
                            //~ g_WidgetSettingsDialog->hide();
                            g_WidgetSettingsDialog->destroy();
                            //~ delete g_WidgetSettingsDialog;
                            //~ g_WidgetSettingsDialog = NULL;
                        }
                    } while (0);
                    break;

                    //~ if (g_myDebugDialog) {
                        //~ g_myDebugDialog->destroy();
                        //~ delete g_myDebugDialog;
                        //~ g_myDebugDialog = NULL;
                    //~ }

                    if (g_ProblemFixerDialog) {
                        //~ g_ProblemFixerDialog->destroy();
                        delete g_ProblemFixerDialog;
                        g_ProblemFixerDialog = NULL;
                    }

                    break;
                }

                /* RunCommandAndCaptureOutput */
                case ID_ACTION_27: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";
                    char    buf2[256] = "\0";
                    char*   output = NULL;
                    char*   s = NULL;
                    int     disk = 0;
                    ULONG   dskstat = 0;
                    mresReply = 0;

                    //~ EnumPartitions(1);

                    WinQueryDlgItemText(hwnd, ID_UNIT_NUMBERS, sizeof(buf), buf);
                    disk = buf[0] - '0';

                    dskstat = GetDiskStatus(disk);

                    sprintf(buf, "Mask:%08lX", (ULONG) dskstat);
                    __debug("GetDiskStatus", buf, DBG_MLE);

                    sprintf(buf, "DiskNr:%d", disk);
                    __debug("GetDiskStatus", buf, DBG_MLE);

                    sprintf(buf, "IsPRM:%d", !!(dskstat & DSKSTAT_PRM));
                    __debug("GetDiskStatus", buf, DBG_MLE);

                    sprintf(buf, "IsCorrupted:%d, IsUnusable:%d, IO_Error:%d, IsBigFloppy:%d",
                        !!(dskstat & DSKSTAT_CORRUPTED),
                        !!(dskstat & DSKSTAT_UNUSABLE),
                        !!(dskstat & DSKSTAT_IOERROR),
                        !!(dskstat & DSKSTAT_BIGFLOPPY)
                    );
                    __debug("GetDiskStatus", buf, DBG_MLE);

                    sprintf(buf, "PartitionCount:%d", dskstat & DSKSTAT_PARTCOUNT);
                    __debug("GetDiskStatus", buf, DBG_MLE);

                    break;
                }

                /* FixDialog */
                case ID_ACTION_28: {
                    int     i = 0;
                    char    drive = '*';
                    char    unit = '0';
                    char    buf[256] = "\0";
                    ULONG   ulres = -1;

                    ulres = WinDlgBox(hdlgProblemFixerDialog, NULL, WinDefDlgProc, hmodMe, ID_PROBLEMFIXER_DIALOG, NULL);

                    break;
                }

                default: {
                    mresReply = 0;
                    break;
                }

            } // switch command
            break;
        } // case WM_COMMAND

/*
        case WM_CONTROL: {                                                      // 0x0030
            mresReply = 0;
            cmnLog(__FILE__, __LINE__, __FUNCTION__, " WM_CONTROL: hwnd=%08X, msg=%08X, mp1=%08X, mp2=%08X", hwnd, msg, mp1, mp2);
            break;
        }

        case WM_HITTEST: {                                                      // 0x000c
            mresReply = 0;
            mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);
            break;
        }

        case WM_CONTROLPOINTER: {                                                   // 0x0038
            mresReply = 0;
            mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);
            break;
        }

        case WM_MOUSEFIRST: {                                                   // 0x0070
            mresReply = 0;
            mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);
            break;
        }

        // Some Timer (XCenter?) -- Increases frequency when dialog has focus
        case 0x0692: {
            mresReply = 0;
            break;
        }
*/

        /* Pass on unhandled messages to default handler */
        default: {
            mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);
            break;
        }
    } // switch message (msg)


    //~ mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);
    return mresReply;
}
