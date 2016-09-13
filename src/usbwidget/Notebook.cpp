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


#include    "Notebook.hpp"


/*
// Notebook
*/
Notebook::Notebook() {
    MessageBox("Notebook","CONSTRUCTOR");
    this->idResource = NULL;
    this->hwndParent = NULL;
    this->hwndSelf = NULL;
    this->pages = NULL;
}

Notebook::~Notebook() {
    this->removePages();
    MessageBox("Notebook","DESTRUCTOR");
}

void    Notebook::appendPage(NotebookPage* page) {
    MessageBox("Notebook","appendPage");

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

    /* Load the page */
    page->hwndSelf =    WinLoadDlg(
                            this->hwndSelf,
                            this->hwndSelf,
                            WinDefDlgProc,
                            hmodMe,
                            page->idResource,
                            NULL
                        );

    /* Associate page-dialog with notebook-page */
    WinSendMsg(
        this->hwndSelf,
        BKM_SETPAGEWINDOWHWND,
        MPFROMLONG(page->idPage),
        MPFROMHWND(page->hwndSelf)
    );

    /* Append the page to the list */
    if (this->pages == NULL) {
        page->prev = NULL;      // Is first page so there is no previous
        page->next = NULL;      // Is first page so there is no next
        this->pages = page;     // Link the first page
    } else {
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
    MessageBox("Notebook","appendPages");

    /* Create and insert pages */
    do {
        nbp = new NotebookPage();
        if (nbp == NULL) break;

        /* Insert first page */
        nbp->idPage = 0;
        nbp->idResource = NB_PAGE_1;
        nbp->dlgProc = WinDefDlgProc;
        nbp->pageStyle = BKA_MAJOR|BKA_STATUSTEXTON;
        nbp->pageOrder = BKA_LAST;
        nbp->tabTitle = "Page #1";
        nbp->statusText = "This is the first page";
        this->appendPage(nbp);

        nbp = new NotebookPage();
        if (nbp == NULL) break;

        /* Insert second page */
        nbp->idPage = 0;
        nbp->idResource = NB_PAGE_2;
        nbp->dlgProc = WinDefDlgProc;
        nbp->pageStyle = BKA_MAJOR|BKA_STATUSTEXTON;
        nbp->pageOrder = BKA_LAST;
        nbp->tabTitle = "Page #2";
        nbp->statusText = "This is the second page";
        this->appendPage(nbp);

        nbp = new NotebookPage();
        if (nbp == NULL) break;

        /* Insert third page */
        nbp->idPage = 0;
        nbp->idResource = NB_PAGE_3;
        nbp->dlgProc = WinDefDlgProc;
        nbp->pageStyle = BKA_MAJOR|BKA_STATUSTEXTON;
        nbp->pageOrder = BKA_LAST;
        nbp->tabTitle = "Page #3";
        nbp->statusText = "This is the third page";
        this->appendPage(nbp);

    } while (0);
}

void    Notebook::removePage(NotebookPage* page) {
    NotebookPage*   tnbp = this->pages;     // Temporary ptr
    MessageBox("Notebook","removePage");

    //! NO NEED TO TRAVERSE !!
    /* Traverse linked list and delete page */
    while (tnbp) {
        if (tnbp == page) {                     // Found page
            if (page->prev == NULL) {           // Is first page ?
                this->pages = page->next;       // Unlink it
            } else {                            // Is not first page
                page->prev->next = page->next;  // Unlink it
            }
            delete page;                        // Delete the page
            tnbp = NULL;                        // Cause loop to end
        } else {
            tnbp = tnbp->next;                  // Try next page
        }
    }
}

void    Notebook::removePages() {
    NotebookPage*   tnbp = this->pages;     // Temporary ptr
    MessageBox("Notebook","removePages");
    /* Traverse linked list and remove each page */
    while (tnbp) {
        this->removePage(tnbp);
        tnbp = tnbp->next;
    }
}

MRESULT EXPENTRY NotebookHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

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
