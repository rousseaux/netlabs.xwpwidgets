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
    __ctorb();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
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
    __ctore();
}

NotebookPage::NotebookPage(Notebook* notebook) {
    __ctorb();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    this->notebook = notebook;
    __ctore();
}

NotebookPage::~NotebookPage() {
    __dtorb();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    __dtore();
}

int     NotebookPage::init(void) {
    __mthd();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
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
    __mthd();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    return 0;
}

int     NotebookPage::test123(void) {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    return 0;
}

///: ---------------------------------------------------- [NotebookPageHandler]

MRESULT EXPENTRY NotebookPageHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

    /* Locals */
    BOOL    brc = FALSE;
    MRESULT mresReply = 0;
    CHAR    buf[512] = {0};

    /*
    // Get pointer to C++ Object.
    // Can be NULL if WM_INITDLG has not been invoked yet.
    // Using this pointer, messages can be deferred to member-functions.
    */
    NotebookPage*   pNbkPg = (NotebookPage*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Message Switch */
    switch (msg) {

        /* Initialize Dialog */
        case WM_INITDLG: {
            printf("%s::%s hwnd:%08X, msg:%08X, mp1:%08X, mp2:%08X\n", __FILE__, __FUNCTION__, hwnd, msg, mp1, mp2);
            /* Get pointer to C++ Object from the CreateParams passed to WinLoadDlg() */
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

        /* Defer Command Messages to C++ Object */
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
