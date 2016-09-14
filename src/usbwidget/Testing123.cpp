/*****************************************************************************\
* Testing123.cpp -- NeoWPS * USB Widget                                       *
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
 * This module is contains code that is under test and will either be moved
 * to other modules or removed if not needed.
 */

#include    "GUIHelpers.hpp"

#include    "Testing123.hpp"


/* Declare external so we can show the BLDLEVEL in the Debug Dialog */
extern  char    bldlevel[];


VOID    ShowDriveMap(VOID) {
    APIRET  ulrc = -1;
    ULONG   drives = 0;
    CHAR    buf[256] = {"\0"};

    buf[0] = '\0';

    //MessageBox("DEBUG-FROM-Testing123", "SHOW_DRIVE_MAP");
    drives = drive_map_dqcd();
    /*
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " drives=%08X, drive_count=%d", drives, drive_count(drives));
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " DQCD_drive_letters=%s", drive_letters(buf, drives, ));
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " DQCD_drive_digits=%s", drive_digits(buf, drives, ));
    */
}

VOID    ShowDriveMap2(VOID) {
    APIRET  ulrc = -1;
    ULONG   disknum = 0;
    ULONG   drives = 0;
    CHAR    buf[256] = {"\0"};

    buf[0] = '\0';

    //MessageBox("DEBUG-FROM-Testing123", "SHOW_DRIVE_MAP2");

    Open_LVM_Engine(TRUE, &ulrc);
    /* Driveletters assigned to usb-drives seem to be available according to this function ! */
    drives = Get_Available_Drive_Letters(&ulrc);

    /*
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " drives=%08X, drive_count=%d", drives, drive_count(drives));
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " GADL_drive_letters=%s", drive_letters(drives, buf));
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " GADL_drive_digits=%s", drive_digits(drives, buf));
    */
    drives = ~drives;                                                           // Invert to get used drive-letters.
    drives &= DRIVE_MAP_MASK;                                                   // Only 26 drives so clear upper 6 bits.
    /*
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " drives=%08X, drive_count=%d", drives, drive_count(drives));
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " GNADL_drive_letters=%s", drive_letters(drives, buf));
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " GADL_drive_digits=%s", drive_digits(buf, drives, ));
    */
    drives = Get_Reserved_Drive_Letters(&ulrc);
    /*
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " drives=%08X, drive_count=%d", drives, drive_count(drives));
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " GRDL_drive_letters=%s", drive_letters(drives, buf));
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " GADL_drive_digits=%s", drive_digits(drives, buf));
    * */
    Close_LVM_Engine();
}

VOID    ShowVolumes(VOID) {
    //MessageBox("DEBUG-FROM-Testing123", "SHOW_VOLUMES");
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " VOID");
}

VOID    ShowVolumes2(VOID) {
    //MessageBox("DEBUG-FROM-Testing123","SHOW_VOLUMES2");
    cmnLog(__FILE__, __LINE__, __FUNCTION__, " VOID");
}

// SEPARATOR

VOID    ShowDialogs(VOID) {
    HWND    hwndParent = NULL;
    HWND    hwndOwner = NULL;
    HWND    hwndDialog = NULL;
    ERRORID eid = NULL;
    BOOL    brc = FALSE;
    ULONG   ulReply;

    //MessageBox("DEBUG-FROM-Testing123", "SHOW_DIALOGS");

    hwndParent = HWND_DESKTOP;
    hwndOwner = HWND_DESKTOP;

    /* These all correspond to the address of the default dialog-handler */
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " MyDialogHandler=%08X", DebugDialogHandler);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " &MyDialogHandler=%08X", &DebugDialogHandler);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " *MyDialogHandler=%08X", *DebugDialogHandler);

    hwndDialog = WinLoadDlg(hwndParent,
                            hwndOwner,
                            DebugDialogHandler,
                            hmodMe,
                            ID_DIALOG_1,
                            NULL);

    //eid = WinGetLastError(myhab);

    ulReply = WinProcessDlg(hwndDialog);

    brc = WinShowWindow(hwndDialog, TRUE);

    brc = WinDestroyWindow(hwndDialog);

}

VOID    HideDialogs(VOID) {
    //MessageBox("DEBUG-FROM-Testing123", "HIDE_DIALOGS");
}

VOID    ShowWindows(VOID) {
    //MessageBox("DEBUG-FROM-Testing123","SHOW_WINDOWS");
}

VOID    HideWindows(VOID) {
    //MessageBox("DEBUG-FROM-Testing123", "HIDE_WINDOWS");
}






MRESULT EXPENTRY MyDialogHandler_2(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = NULL;
    APIRET  ulrc = NULL;
    BOOL    brc = TRUE;
    CHAR    buf[256] = {"\0"};
    int     i = 0;

    sprintf(buf, "  >>> DLGPROC-2: msg:%08lX, mp1:%08lX, mp2:%08lX", msg, mp1, mp2);
    __debug("Debug Dialog", buf, DBG_AUX);

    switch (msg) {

        case WM_INITDLG: {
            //~ WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "WM_INITDLG START");
            hdlgDebugDialog = hwnd;
            //~ sprintf(buf, "%08X", hwnd);
            //~ WinSetDlgItemText(hwnd, ID_HWND_DLG, buf);
            CenterWindow(HWND_DESKTOP, hwnd);
            //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " WM_INITDLG: hwnd=%08X, msg=%08X, mp1=%08X, mp2=%08X", hwnd, msg, mp1, mp2);
            //~ WinSendMsg(hwnd, WM_INITDLG2, NULL, NULL);
            //~ WinPostMsg(hwnd, WM_INITDLG2, NULL, NULL);
            //~ WinSendMsg(hwndListBox, LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "WM_INITDLG END");
            mresReply = (MRESULT) FALSE;
            break;
        }

        case WM_INITDLG2: {
            int     i = 0;

            mresReply = 0;
            break;
        }

        case WM_COMMAND: {                                 // 0x0020
            switch (SHORT1FROMMP(mp1)) {

                /* Close Dialog */
                case DID_OK: {
                    mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);
                    break;
                }

                /* Default */
                default: {
                    mresReply = 0;
                    //mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);
                    break;
                }

            } // switch
            break;
        }

        default: {
            mresReply = 0;
            mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);
            break;
        }
    }

    return mresReply;
}
