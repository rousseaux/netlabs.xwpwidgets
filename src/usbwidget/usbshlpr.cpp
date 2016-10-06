/*****************************************************************************\
* usbshlpr.cpp -- Support DAEMON for the NeoWPS * USB Widget                  *
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
* This module implements a DAEMON that keeps shared memory from being released
* -----------------------------------------------------------------------------
* It is not directly possible to identify which usb-device holds the mass
* storage device where one or more drives are mounted on. These relations are
* held in a list which is stored in the shared memory. If the WPS is restarted,
* the Widget can re-use the list to know which drives are mounted on which
* usb-devices.
*
* The shared memory is initialized at the start with a custom structure which
* is also understood by the Widget. The rest is initialized by DosSetSubMem to
* enable the use of DosSubAllocMem by the Widget. The Widget attaches to this
* named shared memory and has it's C malloc and free functions overridden to
* use DosSubAllocMem. Also, the C++ new and delete operators are overloaded
* to use the overridden malloc and free functions. This enables the Widget to
* retain it's state between WPS restarts.
*
*/

/*
// Window Procedures and C++ Instance Methods
// ------------------------------------------
// Normally, a Window Procedure is associated with a specific registered
// Window Class. This means that every window created from that class uses the
// same Window Procedure. To change the behavior at the instance level, the
// complete window-procedure must be subclassed.
//
// Here, a different approach is taken.
// A Window Class is wrapped in a C++ Class which contains methods to handle
// messages. When a Window is created, a pointer to the C++ Object is passed.
// This pointer is then used to delegate the messages to the member
// functions of the C++ Object. Different behavior is obtained by regular
// C++ inheritance, where methods can override the methods of the parent class.
// Whe using composition instead of inheritance, method delegation is used.
// With this method, functionality is closely related to the C++ Object and
// can be changed with member-function granularity. This means that the
// Window Procedure is now very generic, with its only function to capture the
// pointer to the C++ Object and then delegate window messages to it.
*/


//! ----------------------------------------------------------------- [includes]

/* Stuff to manage the shared memory */
#include    "AllocMem.hpp"

#include    "DebugDialog.hpp"
#include    "WidgetSettingsDialog.hpp"
//~ #include    "ProblemFixerDialog.hpp"

/* The public include file for this module */
#include    "usbshlpr.hpp"

/*
// This includes the BLDLEVEL information.
// It defines a it defines a global character string so it is a source file
// and not an include file. The character string holds the BLDLEVEL information
// and is thus embedded in the resulting object file.
*/
#include    "usbshlpr.lvl"

//! ----------------------------------------------------------------- [globals]

/* Pointer to this Daemon Application */
Daemon*                 g_daemon    =   NULL;
DebugDialog*            g_dbd       =   NULL;
WidgetSettingsDialog*   g_wsd       =   NULL;
ProblemFixerDialog*     g_pfd       =   NULL;
//! FIXME: Get this one dynamically
HMODULE hmodMe              = NULL;

//! ------------------------------------------------------------------ [Daemon]

/* Constructor */
Daemon::Daemon() {
    __ctorb();
    this->clear();
    __ctore();
}

//~ /* Constructor */
//~ Daemon::Daemon(int argc, char* argv[]) {
    //~ __ctorb();
    //~ this->clear();
    //~ this->argc = argc;
    //~ this->argv = argv;
    //~ __ctore();
//~ }

/* Destrcutor */
Daemon::~Daemon() {
    __dtorb();
    this->clear();
    __dtore();
}

/* Clear Attributes */
void    Daemon::clear() {
    __mthd();
    this->argc          = 0;
    this->argv          = NULL;
    this->wait4daemon   = NULL;
    this->wait4stop     = NULL;
}

/* Determine which action to take */
int     Daemon::doAction() {
    __mthdb();

    int     rc      = 0;
    BOOL    stop    = FALSE;
    BOOL    start   = FALSE;
    int     action  = 0;

    printf("argc      : %d\n", this->argc);
    printf("argv[0]   : %s\n", this->argv[0]);
    printf("wait4stop : %08X\n", this->wait4stop);

    /* See if '/stop' is somewhere on the command-line */
    do {
        int i = 0;
        int c = this->argc;
        while (c--) {
            stop = !strcmpi(this->argv[i++], "/stop");
            if (stop) break;
        }
    } while (0);

    /* Select Action */
    rc = stop ? this->stop() : this->start();

    __mthde();
    return rc;
}

/* Start the Daemon */
int     Daemon::start() {
    __mthd();
    return 0;
}

/* Stop the Daemon */
int     Daemon::stop() {
    __mthd();
    return 0;
}

//! --------------------------------------------------------------- [CliDaemon]

/* Constructor */
CliDaemon::CliDaemon(int argc, char* argv[]) {
    __ctorb();
    this->clear();
    this->argc = argc;
    this->argv = argv;
    __ctore();
}

/* Destructor */
CliDaemon::~CliDaemon() {
    __dtorb();
    this->clear();
    __dtore();
}

/* Clear Attributes */
void    CliDaemon::clear() {
    __mthd();
    this->pmmp          = NULL;
    //~ this->Daemon::clear();
}

/* Start the Daemon */
int     CliDaemon::start() {
    __mthd();
    int     rc      = 0;
    APIRET  ulrc    = -1;

    /* Try to allocate the named shared memory */
    ulrc =  DosAllocSharedMem(
                (PPVOID) &this->pmmp,
                SHMEM_NAME,
                SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL),
                PAG_COMMIT | PAG_READ | PAG_WRITE
            );

    if (ulrc) {
        switch (ulrc) {
            case ERROR_ALREADY_EXISTS: {
                PVOID   p = NULL;
                ulrc =  DosGetNamedSharedMem(
                            &p,
                            SHMEM_NAME,
                            PAG_READ | PAG_WRITE
                        );
                printf("The Shared Memory (%s) already exists at: 0x%08x.\n", SHMEM_NAME, p);
                ulrc = -1;
                return 0;
                break;
            }
            default: {
                printf("Error %d occured, aborting.\n", ulrc);
                return 0;
                break;
            }
        }
    }

    printf(
        "DosAllocSharedMem: size: %d, ulrc: %d\n",
        SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL),
        ulrc
    );

    /* Open the semaphore that the Widget created */
    ulrc = DosOpenEventSem(EVSEM_WAIT4DAEMON, &this->wait4daemon);
    printf("DosOpenEventSem: ulrc: %d\n", ulrc);

    /* First try to allocate the Master Pool */
    if (!ulrc) {
        //~ this->pmmp = (PMASTERMEMPOOL) shmem;          // Cast pointer to access master structure
        this->pmmp->signature     = SHMEM_SIGNATURE;  // Set signature
        this->pmmp->poolsize      = SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL);
        this->pmmp->pooltype      = PT_USE_NAMED;     // Set the pooltype
        this->pmmp->submempool    = &this->pmmp[1];         // Sub Memory Pool starts here
        this->pmmp->pwidgetstate  = NULL;             // Pointer to the Widget State


        printf(
            "Master Pool: address:%08lX, signature:%08lX, poolsize:%d, pooltype:%d, submempool:%08lX, widgetstate: %08lX\n",
            this->pmmp,
            this->pmmp->signature,
            this->pmmp->poolsize,
            this->pmmp->pooltype,
            this->pmmp->submempool,
            this->pmmp->pwidgetstate);

        /* Now try to create the sub-allocation system */
        ulrc = DosSubSetMem(
                    this->pmmp->submempool,
                    DOSSUB_INIT,
                    SHMEM_SIZE+sizeof(SUBMEMPOOL)
                );

        printf("DosSubSetMem: size: %d, ulrc: %d\n", SHMEM_SIZE+sizeof(SUBMEMPOOL), ulrc);
        if (!ulrc) {
            //~ printf("Press a key to deallocate the shared memory...\n");
            //~ _getch();                               // Wait for a key press
            //!: SubSetMem Beep
            //~ DosBeep(3000,200);
            ulrc = DosPostEventSem(this->wait4daemon);
            printf("DosPostEventSem: ulrc: %d\n", ulrc);
            ulrc = DosCloseEventSem(this->wait4daemon);
            printf("DosCloseEventSem: ulrc: %d\n", ulrc);

            ulrc = DosCreateEventSem(EVSEM_WAIT4STOP, &this->wait4stop, DC_SEM_SHARED, FALSE);
            printf("DosCreateEventSem: ulrc: %d\n", ulrc);

            //!: BOOKMARK: Block on semaphore
            printf("DosWaitEventSem: blocking...\n");
            fflush(stdout);
            ulrc = DosWaitEventSem(this->wait4stop, SEM_INDEFINITE_WAIT);
            printf("DosWaitEventSem: ulrc: %d\n", ulrc);

            ulrc = DosCloseEventSem(this->wait4stop);
            printf("DosCloseEventSem: ulrc: %d\n", ulrc);

            //~ DosSleep(120000);
            //!: SubSetMem Beep 2
            //~ DosBeep(1000,200);
            ulrc = DosSubUnsetMem(this->pmmp->submempool); // Cleanup sub-allocation system
            printf("DosSubUnsetMem: ulrc: %d\n", ulrc);

            ulrc = DosFreeMem(this->pmmp);               // Free the Master Pool
            printf("DosFreeMem: ulrc: %d\n", ulrc);

            this->pmmp = NULL;
            rc = ulrc;

        }
        /* Setting up the sub-allocation system failed */
        else {
            DosFreeMem(this->pmmp);          // Free the Master Pool
            printf("DosSubSetMem: ulrc: %d\n", ulrc);

            this->pmmp = NULL;
            rc = ulrc;
        }
    }
    /* Allocating the Master Pool failed */
    else {
        this->pmmp = NULL;
        rc = ulrc;
    }

    return rc;
}

/* Stop the aemon */
int     CliDaemon::stop() {
    __mthd();
    APIRET  ulrc    = -1;

    ulrc = DosOpenEventSem(EVSEM_WAIT4STOP, &this->wait4stop);
    printf("DosOpenEventSem  : ulrc: %d\n", ulrc);
    ulrc = DosPostEventSem(this->wait4stop);
    printf("DosPostEventSem  : ulrc: %d\n", ulrc);
    ulrc = DosCloseEventSem(this->wait4stop);
    printf("DosCloseEventSem : ulrc: %d\n", ulrc);

    return 0;
}

//! --------------------------------------------------------------- [GuiDaemon]

/* Constructor */
GuiDaemon::GuiDaemon(int argc, char* argv[]) {
    __ctorb();
    this->clear();
    this->argc  = argc;
    this->argv  = argv;
    __ctore();
}

/* Destructor */
GuiDaemon::~GuiDaemon() {
    __dtorb();
    this->clear();
    __dtore();
}

/* Clear Attributes */
void    GuiDaemon::clear() {
    __mthd();
    this->pmmp      = NULL;
    this->hwndApp   = NULL;
    this->hab       = NULL;
    this->hmq       = NULL;
    memset(&this->qmsg, sizeof(QMSG), 0);
    //~ this->Daemon::clear();
}

/* Change process-type to enable usage of PM */
void    GuiDaemon::changeProcessTypeToPM() {
    PTIB   ptib;
    PPIB   ppib;
    if(DosGetInfoBlocks(&ptib, &ppib) == 0) {
        ppib->pib_ultype = 3;
        printf("** NOW PM-APP !!\n");
    }
}

/* Start processing messages */
int     GuiDaemon::enterMessageLoop() {
    BOOL    brc = FALSE;
    int     rc = 0;

    printf("** ENTERING LOOP !!\n");

    /* The message loop */
    while (WinGetMsg(this->hab, &this->qmsg, 0, 0, 0))
        WinDispatchMsg(this->hab, &this->qmsg);

    printf("** WM_QUIT (%08X) !!\n", WM_QUIT);

    /* Return value from WM_QUIT.mp1 */
    rc = (this->qmsg.msg == WM_QUIT) ? (int) this->qmsg.mp1 : (int) -1;

    return rc;
}

/* Peek which messages were sent after (1st) WM_QUIT was processed */
int     GuiDaemon::doAfterPeek() {
    BOOL    brc = FALSE;
    int     rc = 0;
    int     pcnt = 10;

    /* WM_QUIT should be last message */
    printf(
        "BEFORE-PEEK: msg: rc:%d, hwnd:%08X, msg:%08X, mp1:%08X, mp2:%08X\n",
        rc,
        this->qmsg.hwnd,
        this->qmsg.msg,
        this->qmsg.mp1,
        this->qmsg.mp2
    );

    /* Read last messages from queue -- max 10 */
    do {
        brc = WinPeekMsg(
            this->hab,
            &this->qmsg,
            NULL,
            0,
            0,
            PM_REMOVE
        );
        printf(
            "AFTER-PEEK: brc:%d, msg: rc:%d, hwnd:%08X, msg:%08X, mp1:%08X, mp2:%08X\n",
            brc,
            rc,
            this->qmsg.hwnd,
            this->qmsg.msg,
            this->qmsg.mp1,
            this->qmsg.mp2
        );
    } while (brc && --pcnt);

    if (!pcnt) printf("AFTER-PEEK: More than 10 messages were in queue\n");

    return pcnt;
}

/* Start the Daemon */
int     GuiDaemon::start() {
    __mthd();
    int     rc      = 0;
    int     brc     = FALSE;
    APIRET  ulrc    = -1;

    /* Enable PM usage for WINDOWCOMPAT exe-modules -- enables printf usage */
    this->changeProcessTypeToPM();

    /* Initialize the Graphics System */
    this->hab = WinInitialize(0);
    this->hmq = WinCreateMsgQueue(this->hab, 0);

    /* Different ways to start */
    this->startAsDialog();
    //~ this->startAsFrameWindow();

    /* Destroy the Application Window if not already destroyed */
    if (this->hwndApp) {
        brc = WinDestroyWindow(this->hwndApp);
        this->hwndApp = NULL;
    }

    /* Terminate PM usage */
    brc = WinDestroyMsgQueue(this->hmq);
    brc = WinTerminate(this->hab);

    /* Return to caller */
    return rc;
}

/* Present a DialogWindow */
int     GuiDaemon::startAsDialog() {
    __mthd();
    int     rc          =   0;
    int     brc         =   FALSE;
    APIRET  ulrc        =   0;
    DialogWindow*   dw  =   NULL;

    dw = new DialogWindow();        // Create new C++ Object
    dw->init(this);                 // Initialize it
    dw->create();                   // Create the Window

    this->hwndApp = dw->hwndSelf;   // Assign to ApplicationWindow handle

    printf("this->hwndApp:%08X\n", this->hwndApp);

    /* Set size position and Z-order */
    brc = WinSetWindowPos(
        dw->hwndSelf,
        HWND_TOP,
        0,
        0,
        800,
        600,
        SWP_SIZE | SWP_MOVE | SWP_ZORDER
    );

    /* Position and Activate */
    dw->centerToDesktop();
    brc = WinSetActiveWindow(HWND_DESKTOP, this->hwndApp);
    brc = WinShowWindow(this->hwndApp, TRUE);

    /* Dump C++ Object Data */
    dw->dump();

    /* Start processing messages until WM_QUIT is received */
    rc = this->enterMessageLoop();

    /* Peek and remove remaining messages after WM_QUIT is received */
    rc = this->doAfterPeek();

    /* Destroy Window */
    dw->destroy();

    /* Destroy C++ Object */
    if (dw) delete dw;
    dw = NULL;

    return rc;
}

/* Present a FrameWindow with ClientWindow */
int     GuiDaemon::startAsFrameWindow() {
    __mthd();
    int     rc          =   0;
    int     brc         =   FALSE;
    APIRET  ulrc        =   -1;
    FrameWindow*    fw  =   NULL;

    fw = new FrameWindow();         // Create new C++ Object
    fw->init(this);                 // Initialize it
    //~ fw->regclass();             // Register its class
    fw->create();                   // Create the Window

    this->hwndApp = fw->hwndSelf;   // Assign to ApplicationWindow handle

    /* Set size position and Z-order */
    brc = WinSetWindowPos(
        fw->hwndSelf,
        HWND_TOP,
        0,
        0,
        800,
        600,
        SWP_SIZE | SWP_MOVE | SWP_ZORDER
    );

    /* Position and Activate */
    fw->centerToDesktop();
    WinSetActiveWindow(HWND_DESKTOP, fw->hwndSelf);
    WinShowWindow(fw->hwndSelf, TRUE);

    /* Dump C++ Object Data */
    fw->dump();

    /* Start processing messages until WM_QUIT is received */
    rc = this->enterMessageLoop();

    /* Peek and remove remaining messages after WM_QUIT is received */
    rc = this->doAfterPeek();

    /* Destroy Window */
    fw->destroy();

    /* Destroy C++ Object */
    if (fw) delete fw;
    fw = NULL;

    return rc;
}

/* Stop the Daemon */
int     GuiDaemon::stop() {
    __mthd();
    APIRET  ulrc    = -1;
    return 0;
}

//! ------------------------------------------------------------ [DialogWindow]

/*Constructor */
DialogWindow::DialogWindow() {
    __ctorb();
    this->clear();
    __ctore();
}

/* Destructor */
DialogWindow::~DialogWindow() {
    __dtorb();
    this->clear();
    __dtore();
}

/* Initialize Window Parameters */
void    DialogWindow::init(GuiDaemon* gd) {
    __mthd();
    //~ this->flStyle = WS_CLIPCHILDREN;
    //~ this->pszClassClient = "GuiDaemon";
    this->pszTitle = "GuiDaemon :: Dialog Window";
    this->dlgProc = GuiDaemonDlgProc;
    this->gd = gd;
}

/* Create the Window */
int     DialogWindow::create() {
    __mthd();
    BOOL brc        =   FALSE;
    MRESULT mrc     =   NULL;


    this->hwndSelf =
    WinLoadDlg(
        HWND_DESKTOP,       // Parent
        NULL,               // Owner
        this->dlgProc,      // Dialog Procedure
        NULL,               // Module Handle to load from
        1,                  // Dialog Template ID
        &this->wci          // Pass structure containing pointer to C++ Object
    );

    /* Create Button on DialogWindow */
    //~ this->addButton(456, 20,20,150,80);

    return 0;
}

/* Destroy the Window */
int     DialogWindow::destroy() {
    __mthd();
    BOOL brc = FALSE;
    brc = WinDestroyWindow(this->hwndSelf);
    return 0;
}

void    DialogWindow::dump() {
    __mthd();
    printf("this->hwndParent=%08X\n", this->hwndParent);
    printf("this->hwndSelf=%08X\n", this->hwndSelf);
    printf("this->wci=%08X\n", this->wci);
    printf("this->wci.pvClassInstance=%08X\n", this->wci.pvClassInstance);
}

/* Add a button to the Window */
HWND    DialogWindow::addButton(ULONG id, ULONG x, ULONG y, ULONG cx, ULONG cy) {
    __mthd();
    HWND    hwndButton = NULL;

    do {
        //~ break;
        hwndButton = WinCreateWindow(
            this->hwndSelf,                 // Parent (client-window)
            WC_BUTTON,                      // We want a window of class WC_BUTTON
            (PSZ)"DialogButton",            // The button-text
            WS_VISIBLE      |               // Make it visible
            BS_PUSHBUTTON   |               // It's a pushbutton
            BS_NOPOINTERFOCUS,              // No initial focus
            x,                              // The x-pos from llc
            y,                              // The y-pos from llc
            cx,                             // Width of the button
            cy,                             // Height of the button
            this->hwndSelf,                 // Owner (client-window)
            HWND_TOP,                       // Z-order
            id,                             // Window ID
            NULL,                           // Control Data (none)
            NULL                            // Presentation Parameters (none)
        );
    } while (0);

    return hwndButton;
}

/* Clear Attributes */
void    DialogWindow::clear() {
    __mthd();
    this->pszTitle  = NULL;
    this->hwndSelf  = NULL;
    this->dlgProc   = WinDefDlgProc;
    this->gd        = NULL;
}

MRESULT DialogWindow::wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    BOOL brc = FALSE;
    /*
    // For a non-modal Dialog, WM_CLOSE does not post a WM_QUIT message after
    // it has been processed. So we do it here and this makes it possible to
    // return a value in WM_QUIT.mp1, which can then be retrieved after the
    // message-loop has ended.
    */
    brc = WinPostMsg(hwnd, WM_QUIT, (MPARAM) 88, (MPARAM) NULL);
    /* Default processing dismisses the dialog but does not destroy it */
    return this->wmDefault(hwnd, msg, mp1, mp2);
    //~ return (MRESULT) 0;
}

MRESULT DialogWindow::wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) 0;
}

MRESULT DialogWindow::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();

    printf("hwnd:%08X, msg:%08X, mp1:%08X (%04d), mp2:%08X (%04d)\n", hwnd, msg, mp1, mp1, mp2, mp2);

    switch (SHORT1FROMMP(mp1)) {
        case IDM_QUIT:
            WinPostMsg(hwnd, WM_CLOSE, (MPARAM) NULL, (MPARAM) NULL);
            break;
        case IDM_SM2NEWDBD:
            do {
                if (!g_dbd) {
                    g_dbd = new DebugDialog();
                    g_dbd->create();
                    g_dbd->center();
                    g_dbd->show();
                    //~ g_dbd->process();
                }
                else {
                    g_dbd->center();
                    g_dbd->show();
                }
            } while (0);
            break;
        case IDM_SM2NEWWSD:
            do {
                if (!g_wsd) {
                    g_wsd = new WidgetSettingsDialog();
                    g_wsd->create();
                    g_wsd->center();
                    g_wsd->show();
                    //~ g_wsd->process();
                }
                else {
                    g_wsd->center();
                    g_wsd->show();
                }
            } while (0);
            break;
        //~ case IDM_SM2PFD:
            //~ do {
                //~ if (!g_pfd) {
                    //~ g_pfd = new ProblemFixerDialog();
                    //~ g_pfd->create();
                    //~ g_pfd->center();
                    //~ g_pfd->show();
                    //~ g_pfd->process();
                //~ }
                //~ else {
                    //~ g_pfd->center();
                    //~ g_pfd->show();
                //~ }
            //~ } while (0);
            //~ break;
        case IDM_SM3DESTROYDBD:
            do {
                if (g_dbd) {
                    g_dbd->destroy();
                    delete g_dbd;
                    g_dbd = NULL;
                }
            } while (0);
            break;
        case IDM_SM3DESTROYWSD:
            do {
                if (g_wsd) {
                    g_wsd->destroy();
                    delete g_wsd;
                    g_wsd = NULL;
                }
            } while (0);
            break;
    }

    return (MRESULT) 0;
}

MRESULT DialogWindow::wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) TRUE;
}

MRESULT DialogWindow::wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    //~ __mthd();
    BOOL    brc = FALSE;
    HPS     hps = NULL;
    RECTL    rcl;

    /* Invalidate the given rectangle */
    do {
        break;
        hps = WinBeginPaint(hwnd, NULL, &rcl);
        brc = WinEndPaint(hps);
    } while (0);

    return this->wmDefault(hwnd, msg, mp1, mp2);
    //~ return NULL;
}

MRESULT DialogWindow::wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    //~ __mthd();
    return (MRESULT) WinDefDlgProc(hwnd, msg, mp1, mp2);
}

//! ------------------------------------------------------------- [FrameWindow]

/* Constructor */
FrameWindow::FrameWindow() {
    __ctorb();
    this->clear();
    __ctore();
}

/* Destructor */
FrameWindow::~FrameWindow() {
    __dtorb();
    this->clear();
    __dtore();
}

/* Create the Window */
int     FrameWindow::create() {
    __mthd();
    BOOL brc        =   FALSE;
    MRESULT mrc     =   NULL;

    /* Create FrameWindow */
    do {
        //~ break;
        this->hwndSelf = WinCreateWindow(
            this->hwndParent,
            this->pszClassFrame,
            this->pszTitle,
            this->flStyle,
            0,
            0,
            0,
            0,
            this->hwndOwner,
            HWND_TOP,
            3,
            &this->fcd,
            NULL
        );
    } while (0);

    /* Create Button on FrameWindow */
    //~ this->addButton(456, 20,20,150,80);

    /* Create ClientWindow */
    do {
        //~ break;
        this->client = new ClientWindow();
        this->client->parent = this;
        this->client->init();
        this->client->regclass();
        this->client->hwndParent = this->hwndSelf;
        this->client->hwndOwner = this->hwndSelf;
        this->client->create();

    } while (0);

    if (this->client) {
        /* Create Button on ClientWindow */
        //~ this->client ? this->client->addButton(456, 120,120,150,80):NULL;
        printf("%s -- this->hwndSelf:%08X\n", __FUNCTION__, this->hwndSelf);
        printf("%s -- this->client->hwndSelf:%08X\n", __FUNCTION__, this->client->hwndSelf);
        printf("%s -- FID_CLIENT:%08X\n", __FUNCTION__, WinWindowFromID(this->hwndSelf, FID_CLIENT));
        printf("%s -- QWL_USER:%08X\n", __FUNCTION__, WinQueryWindowPtr(this->client->hwndSelf, QWL_USER));
    }

    this->updateFrame(FID_CLIENT);

    return 0;
}

/* Update Frame Controls */
int     FrameWindow::updateFrame(unsigned fids) {
    WinSendMsg(this->hwndSelf, WM_UPDATEFRAME, (MPARAM) fids, (MPARAM) NULL);
    return 0;
}

/* Destroy the Window */
int     FrameWindow::destroy() {
    __mthd();
    BOOL brc = FALSE;
    brc = WinDestroyWindow(this->hwndSelf);
    if (this->client) {
        this->client->destroy();
        delete this->client;
        this->client = NULL;
    }
    return 0;
}

/* Clear Attributes */
void    FrameWindow::clear() {
    __mthd();
    this->flStyle       = NULL;
    this->flCreateFlags = NULL;
    this->pszTitle      = NULL;
    this->hwndSelf      = NULL;
    this->wndProc       = WinDefWindowProc;
    this->gd            = NULL;
    this->client        = NULL;
}

/* Initialize Window Parameters */
void    FrameWindow::init(GuiDaemon* gd) {
    __mthd();
    this->flStyle = 0;
    this->flCreateFlags = (
        FCF_SYSMENU     |   // Create a system-menu button
        FCF_TITLEBAR    |   // Create a title-bar
        FCF_MINMAX      |   // Create the min-max buttons (and close)
        FCF_CLOSEBUTTON |   // Actually redundant, implied by FCF_MINMAX
        FCF_SIZEBORDER  |   // Draw a sizing-border
        FCF_NOBYTEALIGN |   // Make horizontal positioning smooth
        FCF_MENU        |   // Load a menu
        FCF_TASKLIST        // Put the beast in the task-list
    );
    //~ this->pszClassFrame = "MyFrameWindow";
    this->pszClassFrame = WC_FRAME;
    this->pszTitle = "GuiDaemon :: Frame Window";
    this->wndProc = FrameWndProc;
    this->fcd.cb = sizeof(FRAMECDATA);
    this->fcd.flCreateFlags = this->flCreateFlags;
    this->fcd.hmodResources = NULL;
    this->fcd.idResources = 1;
    this->hwndParent = HWND_DESKTOP;
    this->hwndOwner = NULL;
    this->gd = gd;
}

/* Register Window Class */
void    FrameWindow::regclass() {
    __mthd();
    BOOL    brc = FALSE;
    brc = WinRegisterClass(
        this->gd->hab,
        this->pszClassFrame,
        this->wndProc,
        //~ CS_FRAME,
        CS_SIZEREDRAW,
        //~ NULL,
        4   // If 0 on client windows then QWL_USER is unavailable
    );
    printf("%s:%d\n", __FUNCTION__, brc);
}

/* Dump Object Info */
void    FrameWindow::dump() {
    __mthd();
    printf("this->gd->hab=%08X\n", this->gd->hab);
    printf("this->gd->hmq=%08X\n", this->gd->hmq);
    printf("this->hwndSelf=%08X\n", this->hwndSelf);
    this->client ? printf("this->client->hwndSelf=%08X\n", this->client->hwndSelf):NULL;
    printf("this->wci=%08X\n", this->wci);
    printf("this->wci.pvClassInstance=%08X\n", this->wci.pvClassInstance);
}

/* Add a button to the Window */
HWND    FrameWindow::addButton(ULONG id, ULONG x, ULONG y, ULONG cx, ULONG cy) {
    __mthd();
    HWND    hwndButton = NULL;

    do {
        //~ break;
        hwndButton = WinCreateWindow(
            this->hwndSelf,                 // Parent (client-window)
            WC_BUTTON,                      // We want a window of class WC_BUTTON
            (PSZ)"FrameButton",             // The button-text
            WS_VISIBLE      |               // Make it visible
            BS_PUSHBUTTON   |               // It's a pushbutton
            BS_NOPOINTERFOCUS,              // No initial focus
            x,                              // The x-pos from llc
            y,                              // The y-pos from llc
            cx,                             // Width of the button
            cy,                             // Height of the button
            this->hwndSelf,                 // Owner (client-window)
            HWND_TOP,                       // Z-order
            id,                             // Window ID
            NULL,                           // Control Data (none)
            NULL                            // Presentation Parameters (none)
        );
    } while (0);

    return hwndButton;
}

MRESULT FrameWindow::wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    BOOL brc = FALSE;
    //~ brc = WinPostMsg(hwnd, WM_QUIT, (MPARAM) NULL, (MPARAM) NULL);
    return (MRESULT) 0;
}

MRESULT FrameWindow::wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) 0;
}

MRESULT FrameWindow::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    printf("%s: %04d (wnd)\n", __FUNCTION__, SHORT1FROMMP(mp1));
    return (MRESULT) 0;
}

MRESULT FrameWindow::wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) TRUE;
}

MRESULT FrameWindow::wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    //~ HPS     hps = NULL;
    //~ RECTL    rcl;
    //~ hps = WinBeginPaint(hwnd, NULL, &rcl);
    //~ brc = WinInvalidateRect(hwnd, &rcl, TRUE);
    //~ brc = WinEndPaint(hps);
    return (MRESULT) this->wmDefault(hwnd, msg, mp1, mp2);
}

MRESULT FrameWindow::wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) WinDefWindowProc(hwnd, msg, mp1, mp2);
}

//! ------------------------------------------------------------ [ClientWindow]

/* Constructor */
ClientWindow::ClientWindow() {
    __ctorb();
    this->clear();
    __ctore();
}

/* Destructor */
ClientWindow::~ClientWindow() {
    __dtorb();
    this->clear();
    __dtore();
}

/* Clear Attributes */
void    ClientWindow::clear() {
    __mthd();
}

/* Initialize Window Parameters */
void    ClientWindow::init() {
    __mthd();
    this->flStyle = WS_CLIPCHILDREN;
    this->pszClassClient = "GuiDaemon";
    this->pszTitle = "GuiDaemon :: Frame Window";
    this->wndProc = ClientWndProc;
}

/* Register Window Class */
void    ClientWindow::regclass() {
    __mthd();
    BOOL    brc = FALSE;
    brc =   WinRegisterClass(
                this->parent->gd->hab,
                this->pszClassClient,
                this->wndProc,
                //~ WinDefWindowProc,
                //~ CS_FRAME,
                CS_SIZEREDRAW,
                //~ NULL,
                4   // If 0 on client windows then QWL_USER is unavailable
            );
    printf("%s:%d\n", __FUNCTION__, brc);
}

/* Create the Window */
int     ClientWindow::create() {
    __mthd();
    BOOL brc    = FALSE;
    MRESULT mrc = NULL;

    this->hwndSelf = WinCreateWindow(
        this->hwndParent,
        this->pszClassClient,
        this->pszTitle,
        this->flStyle,
        0,
        0,
        0,
        0,
        this->hwndOwner,
        HWND_TOP,
        FID_CLIENT,
        &this->wci,
        NULL
    );

    return 0;
}

/* Destroy the Window */
int     ClientWindow::destroy() {
    __mthd();
    BOOL brc = FALSE;
    brc = WinDestroyWindow(this->hwndSelf);
    return 0;
}

/* Dump Object Info */
void    ClientWindow::dump() {
    __mthd();
    printf("this->hwndParent=%08X\n", this->hwndParent);
    printf("this->hwndSelf=%08X\n", this->hwndSelf);
    printf("this->wci=%08X\n", this->wci);
    printf("this->wci.pvClassInstance=%08X\n", this->wci.pvClassInstance);
}

/* Add a button to the Window */
HWND    ClientWindow::addButton(ULONG id, ULONG x, ULONG y, ULONG cx, ULONG cy) {
    __mthd();
    HWND    hwndButton = NULL;

    do {
        //~ break;
        hwndButton = WinCreateWindow(
            this->hwndSelf,                 // Parent (client-window)
            WC_BUTTON,                      // We want a window of class WC_BUTTON
            (PSZ)"ClientButton",            // The button-text
            WS_VISIBLE      |               // Make it visible
            BS_PUSHBUTTON   |               // It's a pushbutton
            BS_NOPOINTERFOCUS,              // No initial focus
            x,                              // The x-pos from llc
            y,                              // The y-pos from llc
            cx,                             // Width of the button
            cy,                             // Height of the button
            this->hwndSelf,                 // Owner (client-window)
            HWND_TOP,                       // Z-order
            id,                             // Window ID
            NULL,                           // Control Data (none)
            NULL                            // Presentation Parameters (none)
        );
    } while (0);

    return hwndButton;
}

MRESULT ClientWindow::wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    BOOL brc = FALSE;
    /*
    // While WM_CLOSE posts a WM_QUIT message after it has been processed,
    // doing it here early makes it possible to return a value in WM_QUIT.mp1,
    // which can then be retrieved after the message-loop has ended.
    // Then, doing a WinPeekMsg() will retrieve the WM_QUIT posted by WM_CLOSE,
    // where WM_QUIT.mp1 holds the window handle of the window that received
    // the WM_CLOSE message.
    */
    brc = WinPostMsg(hwnd, WM_QUIT, (MPARAM) 88, (MPARAM) NULL);
    /* Default processing posts the 2nd WM_QUIT */
    return this->wmDefault(hwnd, msg, mp1, mp2);
    //~ return (MRESULT) 0;
}

MRESULT ClientWindow::wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) 0;
}

MRESULT ClientWindow::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();

    printf("hwnd:%08X, msg:%08X, mp1:%08X (%04d), mp2:%08X (%04d)\n", hwnd, msg, mp1, mp1, mp2, mp2);

    switch (SHORT1FROMMP(mp1)) {
        case IDM_QUIT:
            WinPostMsg(hwnd, WM_CLOSE, (MPARAM) NULL, (MPARAM) NULL);
            break;
    }

    return (MRESULT) 0;
}

MRESULT ClientWindow::wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    __mthd();
    return (MRESULT) TRUE;
}

MRESULT ClientWindow::wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    //~ __mthd();
    BOOL    brc = FALSE;
    HPS     hps = NULL;
    RECTL    rcl;

    /* Invalidate the given rectangle */
    do {
        //~ break;
        hps = WinBeginPaint(hwnd, NULL, &rcl);
        brc = WinEndPaint(hps);
    } while (0);

    return NULL;
}

MRESULT ClientWindow::wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    //~ __mthd();
    return (MRESULT) WinDefWindowProc(hwnd, msg, mp1, mp2);
}

//! ---------------------------------------------------------------- [Handlers]

MRESULT     GuiDaemonDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    BOOL        brc     = FALSE;
    MRESULT     mrc     = (MPARAM) 0;

    //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
    //~ printf("%s:%s hwnd:%08X, msg:%08X, mp1:%08X, mp1:%08X\n", __FILE__, __FUNCTION__, msg, mp1, mp2);

    /* Get the pointer to the C++ Object */
    DialogWindow* pWnd = (DialogWindow*) WinQueryWindowPtr(hwnd, QWL_USER);

    switch (msg) {

        case WM_INITDLG: {
            printf("WM_INITDLG received (dlg)-- hwnd:%08X, mp1:%08X, mp2:%08X\n", hwnd, mp1, mp2);
            /* Add a Frame Menu */
            do {
                //~ break;
                HWND hwndMenu = WinLoadMenu(hwnd, NULL, 2);
                brc = WinSetWindowUShort(hwndMenu, QWS_ID, FID_MENU);
                mrc = WinSendMsg(hwnd, WM_UPDATEFRAME, (MPARAM) FCF_MENU, (MPARAM) NULL);
                printf("hwndMenu:%08X, brc:%d, mrc:%08X\n", hwndMenu, brc, mrc);
                mrc = 0;
            } while (0);
            //~ mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
            if (mp2)(((WND_CLASS_INSTANCE*)mp2)->cb)?WinSetWindowPtr(hwnd, QWL_USER, ((WND_CLASS_INSTANCE*)mp2)->pvClassInstance):NULL;
            mrc = (MRESULT) TRUE;
            break;
        }

        /* Delegate the messages below */
        case WM_CLOSE:
            mrc = pWnd ? pWnd->wmClose(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DialogWindow::WM_CLOSE","pWnd is NULL !!");
            break;
        case WM_DESTROY:
            mrc = pWnd ? pWnd->wmDestroy(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DialogWindow::WM_DESTROY","pWnd is NULL !!");
            break;
        case WM_COMMAND:
            mrc = pWnd ? pWnd->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DialogWindow::WM_COMMAND","pWnd is NULL !!");
            break;
        case WM_ERASEBACKGROUND:
            //~ return WinDefDlgProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmEraseBackground(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DialogWindow::WM_ERASEBACKGROUND","pWnd is NULL !!");
            break;
        case WM_PAINT:
            //~ return WinDefDlgProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmPaint(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("DialogWindow::WM_PAINT","pWnd is NULL !!");
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

    return mrc;
}

MRESULT     GuiDaemonWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    BOOL        brc     = FALSE;
    MRESULT     mrc     = (MPARAM) 0;

    switch (msg) {

        case WM_CREATE: {
            printf("WM_CREATE received (wnd)\n");
            mrc = WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;
        }

        case WM_CLOSE: {
            printf("WM_CLOSE received (wnd)\n");
            //~ mrc = WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;
        }

        case WM_DESTROY: {
            printf("WM_DESTROY received (wnd)\n");
            //~ mrc = WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;
        }

        case WM_COMMAND: {
            printf("%s:%s WM_COMMAND:%d -- (wnd)\n", __FILE__, __FUNCTION__, SHORT1FROMMP(mp1));
            switch (SHORT1FROMMP(mp1)) {
                case 123: {
                    printf("123 (wnd)\n");
                    //~ brc = WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                    mrc = 0;
                    break;
                }
                case 456: {
                    printf("456 (wnd)\n");
                    //~ WinDestroyWindow(hwnd);
                    //~ brc = WinPostMsg(hwnd, WM_QUIT, NULL, NULL);
                    mrc = 0;
                    break;
                }
            }
        }

        case WM_ERASEBACKGROUND: {
            printf("WM_ERASEBACKGROUND received (wnd)\n");
            mrc = (MRESULT) TRUE;
            break;
        }

        //~ case WM_PAINT: {
            //~ HPS     hps = NULL;
            //~ RECTL    rcl;
            //~ printf("WM_ERASEBACKGROUND received\n");
            //~ hps = WinBeginPaint(hwnd, NULL, &rcl);
            //~ brc = WinInvalidateRect(hwnd, &rcl, TRUE);
            //~ brc = WinEndPaint(hps);
            //~ mrc = 0;
            //~ break;
        //~ }

        default: {
            mrc = WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;
        }
    }

    return mrc;
}


/*
// This is the handler used for the FrameWindow version.
*/
MRESULT     ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    BOOL            brc     = FALSE;
    MRESULT         mrc     = (MPARAM) -1;

    //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
    //~ printf("%s:%s hwnd:%08X, msg:%08X, mp1:%08X, mp1:%08X\n", __FILE__, __FUNCTION__, msg, mp1, mp2);

    /* Get the pointer to the C++ Object */
    ClientWindow* pWnd = (ClientWindow*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Delegate Messages */
    switch (msg) {

        /* This one cannot be delegated because QWL_USER is not set yet */
        case WM_CREATE: {
            printf("mp1:%08X\n", mp1);
            printf("mp2:%08X\n", mp2);
            if (mp1)(((WND_CLASS_INSTANCE*)mp1)->cb)?WinSetWindowPtr(hwnd, QWL_USER, ((WND_CLASS_INSTANCE*)mp1)->pvClassInstance):NULL;
            mrc = FALSE;   // Continue window creation
            break;
        }

        /* Delegate the messages below */
        case WM_CLOSE:
            mrc = pWnd ? pWnd->wmClose(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("ClientWindow::WM_CLOSE","pWnd is NULL !!");
            break;
        case WM_DESTROY:
            mrc = pWnd ? pWnd->wmDestroy(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("ClientWindow::WM_DESTROY","pWnd is NULL !!");
            break;
        case WM_COMMAND:
            mrc = pWnd ? pWnd->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("ClientWindow::WM_COMMAND","pWnd is NULL !!");
            break;
        case WM_ERASEBACKGROUND:
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmEraseBackground(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("ClientWindow::WM_ERASEBACKGROUND","pWnd is NULL !!");
            break;
        case WM_PAINT:
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmPaint(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("ClientWindow::WM_PAINT","pWnd is NULL !!");
            break;

        /* Use WinDefWindowProc if the C++ Object pointer is not set yet */
        default: {
            if (pWnd) {
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
                mrc = pWnd->wmDefault(hwnd, msg, mp1, mp2);
            }
            else {
                MessageBox("ClientWindow::WM_DEFAULT","pWnd is NULL !!");
                mrc = WinDefWindowProc(hwnd, msg, mp1, mp2);
            }
            break;
        }
    }

    return mrc;
}

/*
// This handler can be used for FrameWindow subclassing -- not used now.
*/
MRESULT     FrameWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    BOOL            brc     = FALSE;
    MRESULT         mrc     = (MPARAM) -1;

    //~ printf("%s:%s hwnd:%08X, msg:%08X, mp1:%08X, mp1:%08X\n", __FILE__, __FUNCTION__, msg, mp1, mp2);

    /* Get the pointer to the C++ Object */
    FrameWindow* pWnd = (FrameWindow*) WinQueryWindowPtr(hwnd, QWL_USER);

    /* Delegate Messages */
    switch (msg) {

        /* This one cannot be delegated because QWL_USER is not set yet */
        case WM_CREATE: {
            CLASSINFO   ci;
            brc = WinQueryClassInfo(WinQueryAnchorBlock(hwnd),"MyFrameWindow",&ci);
            printf("flClassTyle:%08X, pfnWindowProc:%08X, cbWindowData:%08X\n", ci.flClassStyle, ci.pfnWindowProc, ci.cbWindowData);
            printf("mp1:%08X\n", mp1);
            printf("mp2:%08X\n", mp2);
            mrc = FALSE;   // Continue window creation
            break;
        }

        /* Delegate the messages below */
        case WM_CLOSE:
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmClose(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("FrameWindow::WM_CLOSE","pWnd is NULL !!");
            break;
        case WM_DESTROY:
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmDestroy(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("FrameWindow::WM_DESTROY","pWnd is NULL !!");
            break;
        case WM_COMMAND:
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmCommand(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("FrameWindow::WM_COMMAND","pWnd is NULL !!");
            break;
        case WM_ERASEBACKGROUND:
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmEraseBackground(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("FrameWindow::WM_ERASEACKGROUND","pWnd is NULL !!");
            break;
        case WM_PAINT:
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
            mrc = pWnd ? pWnd->wmPaint(hwnd, msg, mp1, mp2) : (MRESULT) MessageBox("FrameWindow::WM_PAINT","pWnd is NULL !!");
            break;

        /* Use WinDefWindowProc if the C++ Object pointer is not set yet */
        default: {
            if (pWnd) {
            //~ return WinDefWindowProc(hwnd, msg, mp1, mp2);
                mrc = pWnd->wmDefault(hwnd, msg, mp1, mp2);
            }
            else {
                MessageBox("ClientWindow::WM_DEFAULT","pWnd is NULL !!");
                mrc = WinDefWindowProc(hwnd, msg, mp1, mp2);
            }
            break;
        }
    }

    return mrc;
}

//! FIXME: Duplicated here because of ugly dependencies

/* Modal Message Box */
ULONG   MessageBox(PSZ title, PSZ text) {
    ULONG   ulresp = -1;

    ulresp =
    WinMessageBox(
        HWND_DESKTOP,
        NULL,
        text,
        title,
        NULL,
        MB_OK
    );
    return ulresp;
}

/* Non-modal Message Box */
ULONG   MessageBox2(PSZ title, PSZ text) {
    ULONG   ulresp = -1;
    MB2INFO mb2info;
    MB2D mb2d    = {"test", 999, BS_DEFAULT};

    mb2info.cb = sizeof(MB2INFO);
    mb2info.hIcon = NULL;
    mb2info.cButtons = 1;
    mb2info.flStyle = MB_NOICON|MB_NONMODAL;
    memcpy (&mb2info.mb2d, &mb2d, sizeof(MB2D));

    ulresp =
    WinMessageBox2(
        HWND_DESKTOP,
        NULL,
        text,
        title,
        NULL,
        &mb2info
    );
    return ulresp;
}

BOOL    CenterWindow(HWND toCenterTo, HWND thisWindow)
{
    POINTL  ptTo;               // Midpoint of window
    POINTL  ptThis;             // Midpoint of window
    POINTL  vector;             // 2D translation-vector (x,y)
    SWP     swpTo;              // Dimensions of window to be centered to
    SWP     swpThis;            // Dimensions of window to be centered to

    /* Query window-dimensions (also includes offset-vector from HWND_DESKTOP) */
    WinQueryWindowPos(toCenterTo, &swpTo);
    WinQueryWindowPos(thisWindow, &swpThis);

    /* Calculate mid-point of To Window */
    ptTo.x = swpTo.cx/2 + swpTo.x;
    ptTo.y = swpTo.cy/2 + swpTo.y;

    /* Calculate mid-point of This Window */
    ptThis.x = swpThis.cx/2 + swpThis.x;
    ptThis.y = swpThis.cy/2 + swpThis.y;

    /* Calculate translation-vector */
    vector.x = ptTo.x - ptThis.x;
    vector.y = ptTo.y - ptThis.y;

    /* Move the window */
    WinSetWindowPos(
        thisWindow,             // Window handle
        0,                      // Relative window-placement order
        swpThis.x+vector.x,     // Window position, x-coordinate
        swpThis.y+vector.y,     // Window position, y-coordinate
        0,                      // Window size
        0,                      // Window size
        SWP_MOVE                // Window-positioning options
    );

    return (BOOL) TRUE;
}


void    __debug(char* title, char* message, unsigned long flags) {
    /* Empty */
}




//! -------------------------------------------------------------------- [main]

/******************************************************************************
* main :: Main EntryPoint of the Daemon Program
* -----------------------------------------------------------------------------
* Normally the Daemon runs in detached mode and its only function is to
* prevent the system from deallocating the shared memory when the USB Widget
* is removed from the XCenter or when the WorkplaceShell is restarted.
*
* Testing a new build of the USB Widget requires restarting the WorkplaceShell,
* which becomes a bit annoying during development sessions. So, for the purpose
* of development, the Daemon also contains a GUI mode, which is a regular
* Presentation Manager program. This makes it easier to test rebuilt components
* because no WorkplaceShell restart is required.
*
* The GUI mode can also be of interest for end users, because it can display
* which events are taking place. These event-logs can help in tracking down
* problems on a particular system.
*
* Usage:
* ------
* - Starting from the command-line without arguments starts the Daemon
*   in CLI mode.
* - Starting from the command-line with the /gui flag starts the Daemon
*   in GUI mode.
* - Option /stop stops a running Daemon
*/
int     main(int argc, char* argv[]) {

    /* Local Variables */
    int     rc          = -1;           // Initial value to return to caller
    int     iStartType  = DAEMON_CLI;   // Default is to start the cli-daemon

    /* See if '/gui' is somewhere on the command-line */
    do {
        BOOL gui = FALSE;
        int i = 0;
        int c = argc;
        while (c--) {
            gui = !strcmpi(argv[i++], "/gui");
            if (gui) break;
        }
        iStartType = gui ? DAEMON_GUI : DAEMON_CLI;
    } while (0);

    /* Create and start a Daemon of the specified type (cli or pm) */
    switch (iStartType) {

        /* Create a CliDaemon Instance and give it control */
        case DAEMON_CLI: {
            g_daemon = new CliDaemon(argc, argv);
            if (g_daemon) {
                printf("-------------------------------------------------------------------------------\n");
                rc = g_daemon->doAction();
                printf("-------------------------------------------------------------------------------\n");
                delete g_daemon;
                g_daemon = NULL;
            }
            break;
        }

        /* Create a GuiDaemon Instance and give it control */
        case DAEMON_GUI: {
            g_daemon = new GuiDaemon(argc, argv);
            if (g_daemon) {
                printf("-------------------------------------------------------------------------------\n");
                rc = g_daemon->doAction();
                printf("-------------------------------------------------------------------------------\n");
                delete g_daemon;
                g_daemon = NULL;
            }
            break;
        }

        /* Something is wrong with command-line parsing...*/
        default: {
            rc = -1;
            break;
        }
    }   // switch (iStartType)

    /* Return to system */
    printf("usbshlpr.dmn: rc: %d\n", rc);
    return 0;
}
