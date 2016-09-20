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


///: ----------------------------------------------------------- [NotebookPage]

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
    this->notebook = NULL;
    this->prev = NULL;
    this->next = NULL;
}

NotebookPage::NotebookPage(Notebook* notebook) {
    this->debugMe();
    MessageBox("NotebookPage","CONSTRUCTOR-2");
    this->notebook = notebook;
}

NotebookPage::~NotebookPage() {
    MessageBox("NotebookPage","DESTRUCTOR");
}

int     NotebookPage::init(void) {
    MessageBox("NotebookPage","init");
    this->idResource = NB_PAGE_DEFAULT;
    this->pageStyle = BKA_MAJOR|BKA_STATUSTEXTON;
    this->pageOrder = BKA_LAST;
    this->tabTitle = "DEFAULT PAGE";
    this->statusText = "THIS IS A DEFAULT PAGE";
    this->hwndParent = this->notebook->hwndSelf;
    this->dlgProc = NotebookPageHandler;

    return 0;
}

int     NotebookPage::initItems(void) {
    MessageBox("NotebookPage","initItems");

    return 0;
}

int     NotebookPage::test123(void) {
    MessageBox("NotebookPage","test123");
    return 0;
}

///: ---------------------------------------------------------- [NotebookPage1]

NotebookPage1::NotebookPage1(Notebook* notebook) {
    this->debugMe();
    MessageBox("NotebookPage1","CONSTRUCTOR");
    this->notebook = notebook;
}

NotebookPage1::~NotebookPage1() {
    MessageBox("NotebookPage1","DESTRUCTOR");
}

int     NotebookPage1::init(void) {

    /* Call parent method to do default initialization */
    this->NotebookPage::init();

    MessageBox("NotebookPage1","init");

    /* Initialize this page */
    this->idResource = NB_PAGE_1;
    this->tabTitle = "Page #1";
    this->statusText = "This is the first page";

    return 0;
}

int     NotebookPage1::initItems(void) {


    MessageBox("NotebookPage1","initItems");

    Button* but = new Button(this->hwndSelf, NB_PAGE_1_PB_1);

    /* Wrap button, set text and unwrap it */
    if (but) {
        but->setText("Maximize");
        delete but;
    }
    but = NULL;

    return 0;
}

/* Handle Command Messages */
MRESULT NotebookPage1::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;

    MessageBox("NotebookPage1","wmCommand");

    switch (SHORT1FROMMP(mp1)) {

        /* Test Button #1 */
        case NB_PAGE_1_PB_1: {
            MessageBox("NotebookPage1","Test Button #1");
            this->maximize();
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

///: ---------------------------------------------------- [NotebookPageHandler]

MRESULT EXPENTRY NotebookPageHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

    /* Locals */
    BOOL    brc = FALSE;
    MRESULT mresReply = 0;
    CHAR    buf[512] = {0};

    /*
    // Get pointer to Class Instance.
    // Can be NULL if WM_INITDLG has not been invoked yet.
    // Using this pointer, messages can be deferred to member-functions.
    */
    NotebookPage*   pNbkPg = (NotebookPage*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Message Switch */
    switch (msg) {

        /* Initialize Dialog */
        case WM_INITDLG: {
            /* Get pointer to Class Instance from the CreateParams passed to WinLoadDlg() */
            pNbkPg = ((NotebookPage*)((WND_CLASS_INSTANCE*)mp2)->pvClassInstance);
            /* Assign the pointer to QWL_USER so it can be retrieved in message-cases */
            brc =   WinSetWindowPtr(
                        hwnd,
                        QWL_USER,
                        pNbkPg
                    );
            pNbkPg->test123();
            mresReply = FALSE;
            break;
        }

        /* Defer Command Messages to Class Instance */
        case WM_COMMAND: {
            mresReply = pNbkPg ? pNbkPg->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("NotebookPageHandler","pNbkPg is NULL !!");
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
