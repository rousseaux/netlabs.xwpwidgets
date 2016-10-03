/*****************************************************************************\
* Notebook.cpp -- NeoWPS * USB Widget                                         *
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

// From NotebookPage.cpp

/**
 * This module contains the Notebook Class Implementation.
 */

#include    "GUIHelpers.hpp"

#include    "Notebook.hpp"


///: --------------------------------------------------------------- [Notebook]

Notebook::Notebook() {
    this->debugMe();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    this->idResource = NULL;
    this->hwndParent = NULL;
    this->hwndSelf = NULL;
    this->pages = NULL;
}

Notebook::~Notebook() {
    this->removePages();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
}

void    Notebook::init(HWND parent, ULONG id) {

    /* Set some important attributes */
    this->hwndParent = parent;
    this->idResource = id;

    /* Get the handle of the Notebook Control */
    this->hwndSelf = WinWindowFromID(       // Handle of the Notebook Control
                        this->hwndParent,   // Window or Dialog parent
                        this->idResource    // ID of the Notebook Control
                    );

    /* Set dimensions of Major Tabs -- can resize Notebook */
    WinSendMsg(
        this->hwndSelf,
        BKM_SETDIMENSIONS,
        MPFROM2SHORT(80, 40),
        MPFROMLONG(BKA_MAJORTAB)
    );

    /* Set dimensions of Minor Tabs -- can resize Notebook */
    //~ WinSendMsg(
        //~ this->hwndSelf,
        //~ BKM_SETDIMENSIONS,
        //~ MPFROM2SHORT(80, 25),
        //~ MPFROMLONG(BKA_MINORTAB)
    //~ );

    /* Set dimensions of prev/next page button */
    //~ WinSendMsg(
        //~ this->hwndSelf,
        //~ BKM_SETDIMENSIONS,
        //~ MPFROM2SHORT(30, 30),
        //~ MPFROMLONG(BKA_PAGEBUTTON)
    //~ );


    /* Set background color for page */
    WinSendMsg(
        this->hwndSelf,
        BKM_SETNOTEBOOKCOLORS,
        MPFROMLONG(CLR_PALEGRAY),
        MPFROMLONG(BKA_BACKGROUNDPAGECOLORINDEX)
    );

    /* Set background color for Major Tabs */
    WinSendMsg(
        this->hwndSelf,
        BKM_SETNOTEBOOKCOLORS,
        MPFROMLONG(CLR_PALEGRAY),
        MPFROMLONG(BKA_BACKGROUNDMAJORCOLORINDEX)
    );
}

void    Notebook::appendPage(NotebookPage* page) {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Insert the page into the notebook */
    page->idPage = (ULONG)   WinSendMsg(
                                this->hwndSelf,
                                BKM_INSERTPAGE,
                                (MPARAM) NULL,
                                MPFROM2SHORT(page->pageStyle, page->pageOrder)
                            );

    /* Set the tab-text for the page */
    WinSendMsg(
        this->hwndSelf,
        BKM_SETTABTEXT,
        (MPARAM) page->idPage,
        (MPARAM) page->tabTitle
    );

    /* Set the status-text for the page */
    WinSendMsg(
        this->hwndSelf,
        BKM_SETSTATUSLINETEXT,
        (MPARAM) page->idPage,
        (MPARAM) page->statusText
    );

    /* Load the page dialog */
    page->hwndSelf =    WinLoadDlg(
                            this->hwndSelf,
                            this->hwndSelf,
                            page->dlgProc,
                            hmodMe,     //! FIXME: this is a global variable !
                            page->idResource,
                            &page->wci
                        );

    page->hwndDebugListbox = WinWindowFromID(page->hwndSelf, NB_PAGE_1_LB_1);

    /* Associate page-dialog with notebook-page */
    WinSendMsg(
        this->hwndSelf,
        BKM_SETPAGEWINDOWHWND,
        MPFROMLONG(page->idPage),
        MPFROMHWND(page->hwndSelf)
    );

    page->initItems();

    /* Append the page to the list */
    if (this->pages == NULL) {
        page->prev = NULL;      // Is first page so there is no previous
        page->next = NULL;      // Is first page so there is no next
        this->pages = page;     // Link the first page
    }
    else {
        NotebookPage*   tnbp = this->pages;     // Temporary ptr
        while (tnbp->next) {                    // If there is a next page...
            tnbp = tnbp->next;                  // point to it
        }
        /* Link the page */
        tnbp->next = page;      // Create forward link on old last page
        page->prev = tnbp;      // Create backward on new last page
        page->next = NULL;      // Indicate last page
    }
}

void    Notebook::appendPages() {
    NotebookPage*   nbp = NULL;
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Create and insert pages */
    do {

        /* Create a new page */
        nbp = new NotebookPage1(this);

        /* Initialize the page and append it to the notebook */
        if (nbp) {
            nbp->init();
            this->appendPage(nbp);
        }

        /* Create a new page */
        nbp = new NotebookPage(this);

        /* Initialize the page and append it to the notebook */
        if (nbp) {
            nbp->init();
            this->appendPage(nbp);
        }

        /* Create a new page */
        nbp = new NotebookPage(this);

        /* Initialize the page and append it to the notebook */
        if (nbp) {
            nbp->init();
            this->appendPage(nbp);
        }

    } while (0);
}

void    Notebook::removePage(NotebookPage* page) {
    NotebookPage*   tnbp = this->pages;     // Temporary ptr
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Traverse linked list and delete page */
    while (tnbp) {
        if (tnbp == page) {                     // Found page
            if (page->prev == NULL) {           // Is first page ?
                this->pages = page->next;       // Unlink it
            }
            else {                              // Is not first page
                page->prev->next = page->next;  // Unlink it
            }
            delete page;                        // Delete the page
            tnbp = NULL;                        // Cause loop to end
        }
        else {
            tnbp = tnbp->next;                  // Try next page
        }
    }
}

void    Notebook::removePages() {
    NotebookPage*   tnbp = this->pages;     // Temporary ptr
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    /* Traverse linked list and remove each page */
    while (tnbp) {
        this->removePage(tnbp);
        tnbp = tnbp->next;
    }
}

int     Notebook::test123(void) {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    return 0;
}

///: -------------------------------------------------------- [NotebookHandler]

/* Probably obsolete because a Notebook is a Control */
MRESULT EXPENTRY NotebookHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

    /* Locals */
    BOOL    brc = FALSE;
    MRESULT mresReply = 0;
    CHAR    buf[512] = {0};

    /*
    // Get pointer to C++ Object.
    // Can be NULL if WM_INITDLG has not been invoked yet.
    // Using this pointer, messages can be deferred to member-functions.
    */
    Notebook*   pNbk = (Notebook*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Message Switch */
    switch (msg) {

        /* Initialize Dialog */
        case WM_INITDLG: {
            /* Get pointer to C++ Object from the CreateParams passed to WinLoadDlg() */
            pNbk = ((Notebook*)((WND_CLASS_INSTANCE*)mp2)->pvClassInstance);
            /* Assign the pointer to QWL_USER so it can be retrieved in message-cases */
            brc =   WinSetWindowPtr(
                        hwnd,
                        QWL_USER,
                        pNbk
                    );
            pNbk->test123();
            mresReply = FALSE;
            break;
        }

        /* Defer Command Messages to C++ Object */
        case WM_COMMAND: {
            pNbk ? pNbk->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("NotebookHandler","pNbk is NULL !!");
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
