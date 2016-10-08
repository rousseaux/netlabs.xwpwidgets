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
    __ctorb();
    this->debugMe();
    this->notebook = NULL;
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    __ctore();
}

/*
// Destructor
*/
DebugDialog::~DebugDialog() {
    __dtorb();
    //~ MessageBox("DebugDialog","DESTRUCTOR");
    this->destroy();
    __dtore();
}

/*
// create :: Create a new Dialog
*/
int DebugDialog::create() {
    __mthd();

    /* Load dialog is not already loaded */
    if (this->hwndSelf == NULL) {

        /* Load the Debug Dialog */
        this->hwndSelf =    WinLoadDlg(
                                HWND_DESKTOP,
                                NULL,
                                (PFNWP) DebugDialogProc,
                                hmodMe,
                                IDD_DEBUG,
                                &this->wci
                            );
    }

    /* Show dialog if creation succeeded */
    if (this->hwndSelf) {
        this->hwndParent = HWND_DESKTOP;
        this->initItems();
        this->show();
        //~ MessageBox("WinLoadDlg", "OK");
    }
    /* Show message if creation failed */
    else {
        MessageBox("WinLoadDlg", "NULL");
    }

    /* Create a new Notebook to manage the Notebook Control */
    if (this->notebook == NULL) this->notebook = new DebugNotebook();

    /* Populate the Notebook with Pages */
    if (this->notebook) {

        /* Initialize Notebook */
        this->notebook->init(this->hwndSelf, IDNB_DEBUG);

        /* Maximize it */
        this->notebook->maximize();

        /* Append the pages */
        this->notebook->appendPages();
    }
    else {
        MessageBox("Notebook", "NULL");
    }

    return NULL;
}

int DebugDialog::process() {
    __mthd();
    int reply = NULL;
    reply = WinProcessDlg(this->hwndSelf);
    return reply;
}

int DebugDialog::destroy() {
    __mthd();
    if (this->notebook || this->hwndSelf) {
        if (this->notebook) delete this->notebook;
        this->notebook = NULL;
        if (this->hwndSelf) WinDestroyWindow(this->hwndSelf);
        this->hwndSelf = NULL;
        this->hwndParent = NULL;
    }

    return NULL;
}

int DebugDialog::test123() {
    __mthd();
    return 0;
}

int DebugDialog::initItems() {
    BOOL    brc = FALSE;

    /* Align BLDLEVEL text to top */
    do {
        RECTL   rself;
        RECTL   rctrl;
        WPOINT  wpoint;
        brc = WinSetDlgItemText(this->hwndSelf, ID_BLDLEVEL, bldlevel);
        brc = (BOOL) WinSendMsg(this->hwndSelf, WM_QUERYBORDERSIZE, (MPARAM) &wpoint, (MPARAM) NULL);
        brc = WinQueryWindowRect(this->hwndSelf, &rself);
        brc = WinQueryWindowRect(WinWindowFromID(this->hwndSelf, ID_BLDLEVEL), &rctrl);
        brc = WinSetWindowPos(
            WinWindowFromID(this->hwndSelf, ID_BLDLEVEL),
            HWND_TOP,
            rctrl.xLeft,
            rself.yTop - WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) - (rctrl.yTop - rctrl.yBottom) - wpoint.y,
            rself.xRight - rself.xLeft - wpoint.x,
            rctrl.yTop - rctrl.yBottom,
            SWP_MOVE | SWP_SIZE
        );
    } while (0);

    return 0;
}

MRESULT DebugDialog::wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    BOOL brc = FALSE;
    /* Default processing dismisses the dialog but does not destroy it */
    return this->wmDefault(hwnd, msg, mp1, mp2);
    //~ return (MRESULT) 0;
}

MRESULT DebugDialog::wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) 0;
}

/* Handle Command Messages */
MRESULT DebugDialog::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthdb();
    MRESULT mresReply = 0;
    switch (SHORT1FROMMP(mp1)) {

        /* Widget Settings Close Button */
        case IDB_DEBUG_CLOSE: {
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
    __mthde();
    return (MRESULT) mresReply;
}

MRESULT DebugDialog::wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    BOOL    brc = FALSE;
    HPS     hps = (HPS) mp1;
    PRECTL  prectl = (PRECTL) mp2;

    //~ brc = WinFillRect(hps, prectl, CLR_WHITE);

    return (MRESULT) TRUE;
}

MRESULT DebugDialog::wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    //~ __mthd();
    BOOL    brc = FALSE;
    HPS     hps = NULL;
    RECTL    rectl;

    /* Invalidate the given rectangle */
    do {
        break;
        hps = WinBeginPaint(hwnd, NULL, &rectl);
        brc = WinFillRect(hps, &rectl, CLR_WHITE);
        brc = WinEndPaint(hps);
    } while (0);

    return this->wmDefault(hwnd, msg, mp1, mp2);
    //~ return (MRESULT) TRUE;
}

MRESULT DebugDialog::wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    //~ __mthd();
    return (MRESULT) WinDefDlgProc(hwnd, msg, mp1, mp2);
}

///: ---------------------------------------------------------- [DebugNotebook]

DebugNotebook::DebugNotebook() {
    __ctorb();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    this->idResource = NULL;
    this->hwndParent = NULL;
    this->hwndSelf = NULL;
    this->pages = NULL;
    __ctore();
}

DebugNotebook::~DebugNotebook() {
    __dtorb();
    this->removePages();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    __dtore();
}

void    DebugNotebook::init(HWND parent, ULONG id) {
    __mthd();
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

void    DebugNotebook::appendPage(NotebookPage* page) {
    __mthd();
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

void    DebugNotebook::appendPages() {
    __mthd();
    NotebookPage*   nbp = NULL;
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Create and insert pages */
    do {

        /* Create a new page */
        nbp = new DebugNotebookPage1(this);

        /* Initialize the page and append it to the notebook */
        if (nbp) {
            nbp->init();
            this->appendPage(nbp);
        }

        /* Create a new page */
        nbp = new DebugNotebookPage2(this);

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

void    DebugNotebook::removePage(NotebookPage* page) {
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

void    DebugNotebook::removePages() {
    NotebookPage*   tnbp = this->pages;     // Temporary ptr
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    /* Traverse linked list and remove each page */
    while (tnbp) {
        this->removePage(tnbp);
        tnbp = tnbp->next;
    }
}

int     DebugNotebook::test123(void) {
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    return 0;
}

int     DebugNotebook::maximize() {
    __mthd();
    BOOL    brc = FALSE;
    RECTL   rectl;
    WPOINT  wpoint;

    if (this->hwndParent) {

        /* Get window dimensions of parent */
        brc = WinQueryWindowRect(this->hwndParent, &rectl);
        brc = (BOOL) WinSendMsg(this->hwndParent, WM_QUERYBORDERSIZE, (MPARAM) &wpoint, (MPARAM) NULL);

        /* Calculate rectangle minus borders and frame-controls */
        //! TODO: More checks on parent (has frame-controls, has menubar, etc.)
        rectl.xLeft += wpoint.x;
        rectl.yBottom += wpoint.y;
        rectl.xRight -= 2 * wpoint.x;
        rectl.yTop -= 2 * wpoint.y + WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) + 16;

        /* Move and Size the Notebook */
        brc = WinSetWindowPos(
            this->hwndSelf,
            HWND_TOP,
            rectl.xLeft,
            rectl.yBottom,
            rectl.xRight,
            rectl.yTop,
            SWP_MOVE |
            SWP_SIZE
        );
    }

    return this->hwndSelf;
}

MRESULT EXPENTRY DebugDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {

    /* Locals */
    BOOL    brc = FALSE;
    MRESULT mrc = 0;
    CHAR    buf[512] = {0};

    //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
    printf("%s::%s hwnd:%08X, msg:%08X, mp1:%08X, mp2:%08X\n", __FILE__, __FUNCTION__, hwnd, msg, mp1, mp2);

    /*
    // Get pointer to C++ Object.
    // Can be NULL if WM_INITDLG has not been invoked yet.
    // Using this pointer, messages can be deferred to member-functions.
    */
    DebugDialog*   pWnd = (DebugDialog*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Message Switch */
    switch (msg) {

        /* Initialize Dialog */
        case WM_INITDLG: {
            printf("%s::%s WM_INITDLG: hwnd:%08X, msg:%08X, mp1:%08X, mp2:%08X\n", __FILE__, __FUNCTION__, hwnd, msg, mp1, mp2);
            //~ MessageBox("DlgProcWidgetSettingsDialog","pWnd is NULL !!");
            /* Get pointer to C++ Object from the CreateParams passed to WinLoadDlg() */
            pWnd = ((DebugDialog*)((WND_CLASS_INSTANCE*)mp2)->pvClassInstance);
            /* Assign the pointer to QWL_USER so it can be retrieved in message-cases */
            brc =   WinSetWindowPtr(
                        hwnd,
                        QWL_USER,
                        pWnd
                    );
            pWnd->test123();
            mrc = (MRESULT) TRUE;
            break;
        }

        /* Delegate the messages below */
        case WM_CLOSE:
            mrc = pWnd ? pWnd->wmClose(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_CLOSE","pWnd is NULL !!");
            break;
        case WM_DESTROY:
            mrc = pWnd ? pWnd->wmDestroy(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_DESTROY","pWnd is NULL !!");
            break;
        case WM_COMMAND:
            mrc = pWnd ? pWnd->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_COMMAND","pWnd is NULL !!");
            break;
        case WM_ERASEBACKGROUND:
            //~ return WinDefDlgProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmEraseBackground(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_ERASEBACKGROUND","pWnd is NULL !!");
            break;
        case WM_PAINT:
            //~ return WinDefDlgProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmPaint(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DebugDialog::WM_PAINT","pWnd is NULL !!");
            break;

        /* Use WinDefDlgProc if the C++ Object pointer is not set yet */
        default: {
            if (pWnd) {
                //~ return WinDefDlgProc(hwnd, msg, mp1, mp2);
                mrc = pWnd->wmDefault(hwnd, msg, mp1, mp2);
            }
            else {
                //~ MessageBox("DialogWindow::WM_DEFAULT","pWnd is NULL !!");
                mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
            }
            break;
        }
    }

    return (MRESULT) mrc;
}

///: ----------------------------------------------------- [DebugNotebookPage1]

DebugNotebookPage1::DebugNotebookPage1(Notebook* notebook) {
    __ctorb();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    this->notebook = notebook;
    __ctore();
}

DebugNotebookPage1::~DebugNotebookPage1() {
    __dtorb();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    __dtore();
}

int     DebugNotebookPage1::init(void) {
    __mthd();
    /* Call parent method to do default initialization */
    this->NotebookPage::init();
    //~ this->pageStyle &= ~BKA_STATUSTEXTON;

    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Initialize this page */
    this->idResource = IDD_DEBUG_NBP_1;
    //~ this->tabTitle = "Page #1";
    this->tabTitle = "Actions";
    this->statusText = "Actions on LVM and other thingies";

    return 0;
}

int     DebugNotebookPage1::initItems(void) {
    __mthd();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    return 0;
}

/* Handle Command Messages */
MRESULT DebugNotebookPage1::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;

    //~ MessageBox("DebugNotebookPage1","wmCommand");

    switch (SHORT1FROMMP(mp1)) {

        case IDB_MAXIMIZE:
            //~ WinSetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_MAXIMIZE|SWP_ZORDER);
            this->maximize();
            break;

        /* Default */
        default: {
            mresReply = 0;
            //mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);           // NO DEFAULT HANDLING OF COMMANDS !!
            break;
        }

    } // switch
    return (MRESULT) mresReply;
}

///: ----------------------------------------------------- [DebugNotebookPage2]

DebugNotebookPage2::DebugNotebookPage2(Notebook* notebook) {
    __ctorb();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    this->notebook = notebook;
    __ctore();
}

DebugNotebookPage2::~DebugNotebookPage2() {
    __dtorb();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    __dtore();
}

int     DebugNotebookPage2::init(void) {
    __mthd();
    /* Call parent method to do default initialization */
    this->NotebookPage::init();
    //~ this->pageStyle &= ~BKA_STATUSTEXTON;

    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);

    /* Initialize this page */
    this->idResource = IDD_DEBUG_NBP_2;
    //~ this->tabTitle = "Page #2";
    this->tabTitle = "Testing";
    this->statusText = "Testing Components and other thingies";

    return 0;
}

int     DebugNotebookPage2::initItems(void) {
    __mthd();
    sprintf(this->buf, "[%s]\t[%04d@%08X] %s\n", __FILE__, sizeof(*this), (unsigned)this, __FUNCTION__);
    _debug(this->buf);
    return 0;
}

/* Handle Command Messages */
MRESULT DebugNotebookPage2::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;

    //~ MessageBox("DebugNotebookPage2","wmCommand");

    switch (SHORT1FROMMP(mp1)) {

        case IDB_MAXIMIZE:
            //~ WinSetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_MAXIMIZE|SWP_ZORDER);
            this->maximize();
            break;

        /* Default */
        default: {
            mresReply = 0;
            //mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);           // NO DEFAULT HANDLING OF COMMANDS !!
            break;
        }

    } // switch
    return (MRESULT) mresReply;
}
