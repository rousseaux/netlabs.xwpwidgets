/*****************************************************************************\
* NotebookPage.cpp -- NeoWPS * USB Widget                                     *
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
 * This module contains the NotebookPage Class Implementation.
 */

#include    "GUIHelpers.hpp"

#include    "NotebookPage.hpp"


/*
// NotebookPage
*/
NotebookPage::NotebookPage() {
    this->debugMe();
    MessageBox("NotebookPage","CONSTRUCTOR");
    this->idPage = 0;
    this->idResource = 0;
    this->hwndParent = NULL;
    this->hwndSelf = NULL;
    this->dlgProc = WinDefDlgProc;
    this->pageStyle = NULL;
    this->pageOrder = NULL;
    this->tabTitle = NULL;
    this->statusText = NULL;
    this->prev = NULL;
    this->next = NULL;
}

NotebookPage::~NotebookPage() {
    MessageBox("NotebookPage","DESTRUCTOR");
}

int     NotebookPage::test123(void) {
    MessageBox("NotebookPage","test123");
    return 0;
}

/* Handle Command Messages */
MRESULT NotebookPage1::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;
    switch (SHORT1FROMMP(mp1)) {

        /* Test Button #1 */
        case NB_PAGE_1_PB_1: {
            MessageBox("NotebookPage1","Test Button #1");
            break;
        }

        /* Test Button #2 */
        case NB_PAGE_1_PB_2: {
            MessageBox("NotebookPage1","Test Button #2");
            break;
        }

        /* Test Button #3 */
        case NB_PAGE_1_PB_3: {
            MessageBox("NotebookPage1","Test Button #3");
            break;
        }

        /* Test Button #4 */
        case NB_PAGE_1_PB_4: {
            MessageBox("NotebookPage1","Test Button #4");
            break;
        }

        /* Test Button #5 */
        case NB_PAGE_1_PB_5: {
            MessageBox("NotebookPage1","Test Button #5");
            break;
        }

        /* Test Button #6 */
        case NB_PAGE_1_PB_6: {
            MessageBox("NotebookPage1","Test Button #6");
            break;
        }

        /* Test Button #7 */
        case NB_PAGE_1_PB_7: {
            MessageBox("NotebookPage1","Test Button #7");
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

/* This one is not used -- active handler is in Notebook Class */
MRESULT EXPENTRY NotebookPageHandler2(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

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
