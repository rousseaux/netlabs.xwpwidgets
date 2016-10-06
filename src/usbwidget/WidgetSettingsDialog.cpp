/*****************************************************************************\
* WidgetSettingsDialog.cpp -- NeoWPS * USB Widget                             *
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

/******************************************************************************
* This module holds the new Widget Settings Dialog
* -----------------------------------------------------------------------------
* It contains a Notebook Control so that settings can be grouped according to
* their category. From the C++ point-of-view, an instance wraps around a Dialog
* and messages are deferred to instance-members.
*
*/


#include    "WidgetSettingsDialog.hpp"


/*
// WidgetSettingsDialog
*/
WidgetSettingsDialog::WidgetSettingsDialog() {
    this->debugMe();
    this->notebook = NULL;
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
}

WidgetSettingsDialog::~WidgetSettingsDialog() {
    //~ MessageBox("WidgetSettingsDialog","DESTRUCTOR");
    this->destroy();
}

int WidgetSettingsDialog::create() {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Load dialog is not already loaded */
    if (this->hwndSelf == NULL) {

        /* Load the WidgetSettings Dialog */
        this->hwndSelf =    WinLoadDlg(
                                HWND_DESKTOP,
                                NULL,
                                //~ (PFNWP) WidgetSettingsDialog::classMessageHandler,
                                //~ (PFNWP) MyDialogHandler_1,
                                (PFNWP) DlgProcWidgetSettingsDialog,
                                hmodMe,
                                IDD_WIDGETSETTINGS,
                                //~ IDD_DEBUG,
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

    /* Create a new Notebook to manage the Notebook Control */
    if (this->notebook == NULL) this->notebook = new WidgetSettingsNotebook();

    /* Populate the Notebook with Pages */
    if (this->notebook) {

        /* Initialize Notebook */
        this->notebook->init(this->hwndSelf, IDNB_WIDGETSETTINGS);

        /* Append the pages */
        this->notebook->appendPages();
    }
    else {
        MessageBox("Notebook", "NULL");
    }

    return NULL;
}

int WidgetSettingsDialog::process() {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    int reply = NULL;
    reply = WinProcessDlg(this->hwndSelf);
    return reply;
}

int WidgetSettingsDialog::destroy() {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    if (this->notebook || this->hwndSelf) {
        if (this->notebook) delete this->notebook;
        this->notebook = NULL;
        if (this->hwndSelf) WinDestroyWindow(this->hwndSelf);
        this->hwndSelf = NULL;
        this->hwndParent = NULL;
    }

    return NULL;
}

int WidgetSettingsDialog::test123() {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    return 0;
}

/* Handle Command Messages */
MRESULT WidgetSettingsDialog::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;
    switch (SHORT1FROMMP(mp1)) {

        /* Widget Settings Close Button */
        case IDB_WIDGETSETTINGS_CLOSE: {
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
    return (MRESULT) mresReply;
}


/* Override */
MRESULT WidgetSettingsDialogEx::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;
    switch (SHORT1FROMMP(mp1)) {

        /* Close Button */
        case IDB_WIDGETSETTINGS_CLOSE: {
            MessageBox("WidgetSettingsDialogEx","Dismissing Dialog -- wmCommand Override !!");
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
    return (MRESULT) mresReply;
}

///: ------------------------------------------------- [WidgetSettingsNotebook]

WidgetSettingsNotebook::WidgetSettingsNotebook() {
    this->debugMe();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    this->idResource = NULL;
    this->hwndParent = NULL;
    this->hwndSelf = NULL;
    this->pages = NULL;
}

WidgetSettingsNotebook::~WidgetSettingsNotebook() {
    this->removePages();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
}

void    WidgetSettingsNotebook::init(HWND parent, ULONG id) {

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

void    WidgetSettingsNotebook::appendPage(NotebookPage* page) {
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

    //~ page->hwndDebugListbox = WinWindowFromID(page->hwndSelf, NB_PAGE_1_LB_1);

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

void    WidgetSettingsNotebook::appendPages() {
    NotebookPage*   nbp = NULL;
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Create and insert pages */
    do {

        /* Create a new page */
        nbp = new WidgetSettingsNotebookPage1(this);

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

void    WidgetSettingsNotebook::removePage(NotebookPage* page) {
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

void    WidgetSettingsNotebook::removePages() {
    NotebookPage*   tnbp = this->pages;     // Temporary ptr
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    /* Traverse linked list and remove each page */
    while (tnbp) {
        this->removePage(tnbp);
        tnbp = tnbp->next;
    }
}

int     WidgetSettingsNotebook::test123(void) {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    return 0;
}



///: -------------------------------------------- [WidgetSettingsNotebookPage1]

WidgetSettingsNotebookPage1::WidgetSettingsNotebookPage1(Notebook* notebook) {
    this->debugMe();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    this->notebook = notebook;
}

WidgetSettingsNotebookPage1::~WidgetSettingsNotebookPage1() {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
}

int     WidgetSettingsNotebookPage1::init(void) {

    /* Call parent method to do default initialization */
    this->NotebookPage::init();

    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Initialize this page */
    this->idResource = IDD_WIDGETSETTINGS_NBP_1;
    this->tabTitle = "Page #1";
    this->statusText = "This is the first page";

    return 0;
}

int     WidgetSettingsNotebookPage1::initItems(void) {

    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    Button::setText(this->hwndSelf, IDD_WIDGETSETTINGS_NBP_1_PB_1, "Maximize");
    Button::setText(this->hwndSelf, IDD_WIDGETSETTINGS_NBP_1_PB_2, "Test123");
    Button::setText(this->hwndSelf, IDD_WIDGETSETTINGS_NBP_1_PB_3, "Test456");
    Button::setText(this->hwndSelf, IDD_WIDGETSETTINGS_NBP_1_PB_4, "Test789");
    Button::setText(this->hwndSelf, IDD_WIDGETSETTINGS_NBP_1_PB_7, "__debug");

    return 0;
}

/* Handle Command Messages */
MRESULT WidgetSettingsNotebookPage1::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;

    //~ MessageBox("WidgetSettingsNotebookPage1","wmCommand");

    switch (SHORT1FROMMP(mp1)) {

        /* Test Button #1 */
        case IDD_WIDGETSETTINGS_NBP_1_PB_1: {
            MessageBox("WidgetSettingsNotebookPage1","Test Button #1");
            this->maximize();
            break;
        }

        /* Test Button #2 */
        case IDD_WIDGETSETTINGS_NBP_1_PB_2: {
            MessageBox("WidgetSettingsNotebookPage1","Test Button #2");
            break;
        }

        /* Test Button #3 */
        case IDD_WIDGETSETTINGS_NBP_1_PB_3: {
            MessageBox("WidgetSettingsNotebookPage1","Test Button #3");
            break;
        }

        /* Test Button #4 */
        case IDD_WIDGETSETTINGS_NBP_1_PB_4: {
            MessageBox("WidgetSettingsNotebookPage1","Test Button #4");
            break;
        }

        /* Test Button #5 */
        case IDD_WIDGETSETTINGS_NBP_1_PB_5: {
            MessageBox("WidgetSettingsNotebookPage1","Test Button #5");
            break;
        }

        /* Test Button #6 */
        case IDD_WIDGETSETTINGS_NBP_1_PB_6: {
            MessageBox("WidgetSettingsNotebookPage1","Test Button #6");
            break;
        }

        /* Test Button #7 */
        case IDD_WIDGETSETTINGS_NBP_1_PB_7: {
            //~ MessageBox("WidgetSettingsNotebookPage1","Test Button #7");
            _debug("_debug test");
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


/* Redirect messages for the WidgetSettingsDialog */
MRESULT DlgProcWidgetSettingsDialog(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

    /* Locals */
    BOOL    brc = FALSE;
    MRESULT mresReply = 0;
    CHAR    buf[512] = {0};

    //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
    printf("%s:%s hwnd:%08X, msg:%08X, mp1:%08X, mp1:%08X\n", __FILE__, __FUNCTION__, msg, mp1, mp2);

    /*
    // Get pointer to C++ Object.
    // Can be NULL if WM_INITDLG has not been invoked yet.
    // Using this pointer, messages can be deferred to member-functions.
    */
    WidgetSettingsDialog*   pWsd = (WidgetSettingsDialog*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Message Switch */
    switch (msg) {

        /* Initialize Dialog */
        case WM_INITDLG: {
            //~ MessageBox("DlgProcWidgetSettingsDialog","pWsd is NULL !!");
            /* Get pointer to C++ Object from the CreateParams passed to WinLoadDlg() */
            pWsd = ((WidgetSettingsDialog*)((WND_CLASS_INSTANCE*)mp2)->pvClassInstance);
            /* Assign the pointer to QWL_USER so it can be retrieved in message-cases */
            brc =   WinSetWindowPtr(
                        hwnd,
                        QWL_USER,
                        pWsd
                    );
            pWsd->test123();
            mresReply = FALSE;
            break;
        }

        /* Defer Command Messages to C++ Object */
        case WM_COMMAND: {
            mresReply = pWsd ? pWsd->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DlgProcWidgetSettingsDialog","pWsd is NULL !!");
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
