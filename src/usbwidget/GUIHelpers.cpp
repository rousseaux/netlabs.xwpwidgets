/*****************************************************************************\
* GUIHelpers.cpp -- NeoWPS * USB Widget                                       *
*                                                                             *
* Copyright (c) 2014 RDP Engineering                                          *
*                                                                             *
* Author: Ben Rietbroek <rousseau.ecsdev@gmail.com>                           *
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
 * This module is supposed to contain implementation independent code.
 * The separation between abstract functionality and implementation code will
 * be made in the new implementation of the Widget.
 */


#include    "GUIHelpers.hpp"


void    GUIHelpers(void) {
}






/*
// WINDOW
*/
Window::Window() {
    this->handle = NULL;
    sprintf(this->buf, "Window() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
}

Window::~Window() {
    sprintf(this->buf, "~Window() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
}

ulong   Window::show() {
    if (this->handle) {
        WinSetWindowPos(this->handle, HWND_TOP, 0, 0, 0, 0, SWP_SHOW|SWP_ZORDER);
        WinShowWindow(this->handle, true);
        WinSetFocus(HWND_DESKTOP, this->handle);
    }
    return this->handle;
}

void    Window::center() {
    if (this->handle) {
        CenterWindow(WinQueryWindow(this->handle, QW_PARENT), this->handle);
    }
}

void    Window::centerToDesktop() {
    if (this->handle) {
        CenterWindow(HWND_DESKTOP, this->handle);
    }
}

void    Window::centerToOther(ulong toCenterTo) {
    if (this->handle) {
        CenterWindow(toCenterTo, this->handle);
    }
}

ulong   Window::hide() {
    if (this->handle) {
        WinShowWindow(this->handle, false);
    }
    return this->handle;
}

ulong   Window::getHandle() {
    return this->handle;
}


/*
// DIALOG
*/
Dialog::Dialog() {
    sprintf(this->buf, "Dialog() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
}

Dialog::~Dialog() {
    sprintf(this->buf, "~Dialog() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
}




/**
 * ===========
 * DebugDialog :: Contains line oriented and text oriented consoles.
 * ===========
 */

/*
// Constructor
*/
DebugDialog::DebugDialog() {
    sprintf(this->buf, "DebugDialog() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
}

/*
// Destructor
*/
DebugDialog::~DebugDialog() {
    sprintf(this->buf, "~DebugDialog() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
    this->destroy();
}

/*
// create :: Create a new Dialog
*/
ulong   DebugDialog::create() {

    /*
    // This buffer contains a pointer to the instance of this Dialog.
    // It is passed as a max-64KB buffer with the 16-bit word at offset 0 containing
    // the length of the data.
    */
    char    myself[sizeof(short)+sizeof(Root*)];        // Allocate the buffer; all classes descend from Root.

    /* Initialize it */
    myself[0]               = (short) sizeof(myself);   // Size of the buffer as a 16-bit word at offset 0.
    myself[sizeof(short)]   = (ulong) this;             // The pointer to this Dialog instance.

    /* Load the Dialog */
    this->handle =  WinLoadDlg(
                        hdlgDebugDialog,                // parent
                        hdlgDebugDialog,                // owner
                        (PFNWP) classMessageHandler,    // handler
                        //MyDialogHandler_3,
                        hmodMe,                         // module; GLOBAL VAR!! CHANGE THIS !!
                        ID_NOTIFICATION_DIALOG,         // id
                        myself                          // parameters
                    );

    /* Show it */
    this->show();

    /* Debug Info */
    if (this->debugMe()) {
        sprintf(this->buf, "DebugDialog::create() : hdlg=%08X, myself=%08X", this->handle, myself);
        __debug(NULL, this->buf, DBG_LBOX);
    }

    /* Return the handle of the new Dialog */
    return this->handle;
}

/*
// detroy :: Destroy the Dialog and any resources it uses.
*/
ulong   DebugDialog::destroy() {
    if (this->handle) {
        WinDestroyWindow(this->handle);
        this->handle = NULL;
    }
    return true;
}

/*
// redraw :: Repaint the content of the Dialog.
*/
ulong   DebugDialog::redraw() {
    return true;
}

/*
// msgInitDialog :: Handles the WM_INITDLG message sent on window-creation.
*/
ulong   DebugDialog::msgInitDialog(ulong mp1, ulong mp2) {
    return true;
}

/*
// msgCommand :: Handles the WM_COMMAND messages sent by controls.
*/
ulong   DebugDialog::msgCommand(ulong mp1, ulong mp2) {
    return true;
}

/*
// commandDrawButton :: Handles a specific (user defined) command-message.
*/
ulong   DebugDialog::commandDrawButton() {
    return true;
}

/*
// commandDestroyButton :: Handles a specific (user defined) command-message.
*/
ulong   DebugDialog::commandDestroyButton() {
    return true;
}


/*
// classMessageHandler :: Handles all messages.
// This is the actual window-procedure which is invoked as a callback.
// Therefore it's declaration is static because it get's called externally from the dialog-instance.
// Also, it's the type EXPENTRY (_Pascal FAR) because it get's called from outside this module.
// Note that FAR is void in 32-bit mode and FAR32 is used for 32-bit far pointers.
// But we don't need that in flat 32-bit mode.
// The parameter mp2 contains a structure with a pointer to the dialog-instance that invoked this handler
// when the message WM_INITDLG is received.
*/
ulong   DebugDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2) {

    //return MyDialogHandler_3(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);

    MRESULT mresReply = 0;

    switch (msg) {

        //!: Init dialog (Debug)
        case WM_INITDLG: {
            mresReply = FALSE;
            break;
        }

        case WM_COMMAND: {
            switch (SHORT1FROMMP(mp1)) {

                /* Close Dialog */
                case DID_OK: {
                    mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
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
    return (ulong) mresReply;
}



//!: 0---------------------------------> [ PROBLEMFIXER DIALOG IMPLEMENTATION ]

/**
 * ===========
 * ProblemFixerDialog :: Used to fix LVM hostile disk layouts
 * ===========
 */

/*
// Constructor
*/
ProblemFixerDialog::ProblemFixerDialog() {
    sprintf(this->buf, "ProblemFixerDialog() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);

    this->init();
}

/*
// Destructor
*/
ProblemFixerDialog::~ProblemFixerDialog() {
    sprintf(this->buf, "~ProblemFixerDialog() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
    this->destroy();
}

void    ProblemFixerDialog::init() {
    int     i = 0;
    this->dletter = '*';
    this->pdisk = 0;
    this->dskstat = 0;
    this->fixmask = 0;
    this->effmask = 0;
    this->tofixmap = 0;
    this->fixcommands_count = 0;
    for (i=0; i<5; i++) {
        this->fixcommands[i][0] = '\0';
    }
    for (i=0; i<5; i++) {
        this->fixes[i] = 0;
    }
    this->action_taken = DID_CLOSE;
}


int     ProblemFixerDialog::mle_cmdname(int id) {
    int     l = 0;
    char    buf[256] = "\0";
    char    buf2[256] = "\0";
    char    *p = buf;

    WinQueryDlgItemText(this->handle, id, sizeof(buf), buf);
    sprintf(buf2, "# %s #\n", buf);

    l = strlen(buf2);
    memset(p, '#', l-1);
    p += l-1;
    *p++ = '\n';
    *p++ = '\0';
    strcat(buf, buf2);
    p = buf + strlen(buf);
    memset(p, '#', l-1);
    p += l-1;
    *p++ = '\n';
    *p++ = '\0';

    this->mleAppendText(buf);

    return strlen(buf);
}

int     ProblemFixerDialog::set_fix_state() {
    WinShowWindow(WinWindowFromID(this->handle, DID_FIX), TRUE);
    WinShowWindow(WinWindowFromID(this->handle, DID_CANCEL), TRUE);
    WinShowWindow(WinWindowFromID(this->handle, DID_HIDE), TRUE);
    WinShowWindow(WinWindowFromID(this->handle, DID_CLOSE), FALSE);
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(this->handle, DID_CANCEL));   // Stack Trap (werkt nu na txtbuf verkleining)
    //~ WinFocusChange(HWND_DESKTOP, WinWindowFromID(this->handle, DID_CANCEL), NULL);
    return 0;
}

int     ProblemFixerDialog::set_close_state(int fixed) {
    char    buf[256] = "\0";
    RGB2    rgb2;

    if (fixed) {
        rgb2.bRed = 0;
        rgb2.bGreen = 128;
        rgb2.bBlue = 0;
        rgb2.fcOptions = 0;
        WinSetPresParam(WinWindowFromID(this->handle, ID_FIX_WARNING), PP_FOREGROUNDCOLOR, (ULONG) sizeof(RGB2), (PVOID) &rgb2);
        WinSetDlgItemText(this->handle, ID_FIX_WARNING, "The fix(es) have been applied, click on the Close Button and wait for the Safe Eject message");
    }
    else {
        WinSetDlgItemText(this->handle, ID_FIX_WARNING, "There are no fixes that can be applied");
    }

    WinShowWindow(WinWindowFromID(this->handle, DID_FIX), FALSE);
    WinShowWindow(WinWindowFromID(this->handle, DID_CANCEL), FALSE);
    WinShowWindow(WinWindowFromID(this->handle, DID_HIDE), FALSE);
    WinShowWindow(WinWindowFromID(this->handle, DID_CLOSE), TRUE);
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(this->handle, DID_CLOSE));    // Stack Trap (werkt nu na txtbuf verkleining)
    //~ WinFocusChange(HWND_DESKTOP, WinWindowFromID(this->handle, DID_CLOSE), NULL);

    sprintf(buf, "buf:%08lX", buf);
    __debug("BUF-ON-STACK-IN-MEMBER", buf, DBG_MLE);
    return 0;
}

ulong   ProblemFixerDialog::get_fixmask() {
    return this->fixmask;
}

ulong   ProblemFixerDialog::set_fixmask(ulong fixmask) {
    this->fixmask = fixmask;
    return this->fixmask;
}

ulong   ProblemFixerDialog::get_effmask() {
    return this->effmask;
}

ulong   ProblemFixerDialog::set_effmask(ulong effmask) {
    this->effmask = effmask;
    return this->effmask;
}

ulong   ProblemFixerDialog::get_tofixmap() {
    return this->tofixmap;
}

ulong   ProblemFixerDialog::set_tofixmap(ulong tofixmap) {
    this->tofixmap = tofixmap;
    return this->tofixmap;
}

/*
// create :: Create a new Dialog
*/
ulong   ProblemFixerDialog::create() {

    char    buf[256] = "\0";

    /*
    // This buffer contains a pointer to the instance of this Dialog.
    // It is passed as a max-64KB buffer with the 16-bit word at offset 0 containing
    // the length of the data.
    */
    char    myself[sizeof(short)+sizeof(Root*)];        // Allocate the buffer; all classes descend from Root.
    short*  sp = (short*) &myself[0];
    ulong*  lp = (ulong*) &myself[2];

    /* Initialize it */
    *sp     = (short) sizeof(myself);   // Size of the buffer as a 16-bit word at offset 0.
    *lp     = (ulong) this;             // The pointer to this Dialog instance.

    sprintf(buf, "this: %08lX", this);
    __debug("Create", buf, DBG_MLE);

    /* Load the Dialog */
    this->handle =  WinLoadDlg(
                        HWND_DESKTOP,                   // parent
                        NULL,                           // owner
                        (PFNWP) classMessageHandler,    // handler
                        //MyDialogHandler_3,
                        hmodMe,                         // module; GLOBAL VAR!! CHANGE THIS !!
                        ID_PROBLEMFIXER_DIALOG,         // id
                        myself                          // parameters
                    );

    /* Center dialog to Parent (Desktop) */
    this->center();

    /* Debug Info */
    if (this->debugMe()) {
        sprintf(this->buf, "ProblemFixerDialog::create() : hdlg=%08X, myself=%08X", this->handle, myself);
        __debug(NULL, this->buf, DBG_LBOX);
    }

    /* Return the handle of the new Dialog */
    return this->handle;
}


/*
// process :: Process the Dialog to get a return value.
*/
ulong   ProblemFixerDialog::process() {
    return WinProcessDlg(this->handle);
}


/*
// detroy :: Destroy the Dialog and any resources it uses.
*/
ulong   ProblemFixerDialog::destroy() {
    if (this->handle) {
        //~ WinDismissDlg(this->handle, 0x88);
        WinDestroyWindow(this->handle);
        this->handle = NULL;
    }
    return true;
}

/*
// redraw :: Repaint the content of the Dialog.
*/
ulong   ProblemFixerDialog::redraw() {
    return true;
}

/*
// msgInitDialog :: Handles the WM_INITDLG message sent on window-creation.
*/
ulong   ProblemFixerDialog::msgInitDialog(ulong mp1, ulong mp2) {
    return true;
}

/*
// msgCommand :: Handles the WM_COMMAND messages sent by controls.
*/
ulong   ProblemFixerDialog::msgCommand(ulong mp1, ulong mp2) {
    return true;
}

/*
// commandDrawButton :: Handles a specific (user defined) command-message.
*/
ulong   ProblemFixerDialog::commandDrawButton() {
    return true;
}

/*
// commandDestroyButton :: Handles a specific (user defined) command-message.
*/
ulong   ProblemFixerDialog::commandDestroyButton() {
    return true;
}

void    ProblemFixerDialog::mleAppendText(char* text) {
    WinSendMsg(WinWindowFromID(this->handle, ID_MLE_1), MLM_INSERT, (MPARAM) text, (MPARAM) NULL);
    //~ WinSendMsg(WinWindowFromID(this->handle, ID_MLE_1), MLM_INSERT, (MPARAM) "\n", (MPARAM) NULL);
}


void    ProblemFixerDialog::mleClear() {
    MRESULT mresReply   = 0;
    mresReply = WinSendMsg(WinWindowFromID(this->handle, ID_MLE_1), MLM_SETSEL, (MPARAM) NULL, (MPARAM) 50000);
    mresReply = WinSendMsg(WinWindowFromID(this->handle, ID_MLE_1), MLM_CLEAR, (MPARAM) NULL, (MPARAM) NULL);
    mresReply = WinSendMsg(WinWindowFromID(this->handle, ID_MLE_1), MLM_SETSEL, (MPARAM) NULL, (MPARAM) 50000);
    mresReply = WinSendMsg(WinWindowFromID(this->handle, ID_MLE_1), MLM_CLEAR, (MPARAM) NULL, (MPARAM) NULL);
    mresReply = WinSendMsg(WinWindowFromID(this->handle, ID_MLE_1), MLM_SETSEL, (MPARAM) NULL, (MPARAM) 50000);
    mresReply = WinSendMsg(WinWindowFromID(this->handle, ID_MLE_1), MLM_CLEAR, (MPARAM) NULL, (MPARAM) NULL);
}


char    ProblemFixerDialog::getDriveLetter() {
    return this->dletter;
}

char    ProblemFixerDialog::setDriveLetter(char dletter) {
    USBDeviceReport*    usbdev = NULL;
    char                buf[256] = "\0";

    this->dletter = dletter;
    usbdev = DriveLetterToUsbDevice(dletter);
    if (usbdev) {
        this->setPhysDisk(usbdev->diskindex);
    }
    else {
        this->setPhysDisk(0);
    }

    sprintf(buf, "Drive %c:", this->dletter);
    WinSetDlgItemText(this->handle, ID_DRIVE_LETTER, buf);

    return this->dletter;
}

int     ProblemFixerDialog::getPhysDisk() {
    return this->pdisk;
}

int     ProblemFixerDialog::setPhysDisk(int pdisk) {

    this->pdisk = pdisk;

    sprintf(buf, "Physical Disk #%d", this->pdisk);
    WinSetDlgItemText(this->handle, ID_DISK_NUMBER, buf);

    return this->pdisk;
}

ulong   ProblemFixerDialog::getDiskStatus() {
    return this->dskstat;
}

ulong   ProblemFixerDialog::setDiskStatus(ulong dskstat) {
    return (this->dskstat = dskstat);
}


void    ProblemFixerDialog::help() {
    char    buf[256] = "\0";

    this->mleAppendText("INFORMATION\n");
    this->mleAppendText("===========\n");
    this->mleAppendText("This dialog is presented because eComStation has found problems with\n");
    this->mleAppendText("the usb storage device you just inserted.\n");
    this->mleAppendText("Usually these problems are related to the layout of the storage device.\n");
    this->mleAppendText("With this dialog you might be able to solve these problems and make the\n");
    this->mleAppendText("usb storage device usable for eComStation.\n");
    this->mleAppendText("If you don't want to try to fix the problems then just press the Cancel button.\n");
    this->mleAppendText("The medium will then be ejected without any alteration.\n");
    this->mleAppendText("If however you want to try to fix the problems, then be sure you have a working backup\n");
    this->mleAppendText("of all the data on the device.\n");
    this->mleAppendText("\n");

    this->mleAppendText("PROBLEMS AND FIXING\n");
    this->mleAppendText("===================\n");
    this->mleAppendText("Click on the \"Show Problems\" item on the right side of this dialog.\n");
    this->mleAppendText("This will show the problems that were encountered and their possible cause.\n");
    this->mleAppendText("Then click on the \"Show Fix Commands\" item.\n");
    this->mleAppendText("This will show the DFSVOS2 commands that will be run to try to fix the problems.\n");
    this->mleAppendText("DFSVOS2 is a limited version of the DFSee disk management tool licenced for eCS users.\n");
    this->mleAppendText("If you are \"Feeling Lucky\", press the \"Fix\" button in the lower right corner.\n");
    this->mleAppendText("This will apply the fixes and hopefully solve the access problems.\n");
    this->mleAppendText("\n");

    this->mleAppendText("MORE INFORMATION\n");
    this->mleAppendText("================\n");
    this->mleAppendText("Above the bold items are several items you can use to obtain more information about\n");
    this->mleAppendText("the mass storage device you just inserted.\n");
    this->mleAppendText("These items may be of use for people that have a deeper knowledge about disk layouts\n");
    this->mleAppendText("and may be of help to determine if the proposed fixes make any sense.\n");
    this->mleAppendText("\n");

    this->mleAppendText("SAVING THIS LOGWINDOW's CONTENT\n");
    this->mleAppendText("===============================\n");
    this->mleAppendText("You can use the mouse to select all text.\n");
    this->mleAppendText("Then use Ctrl-Ins to put it on the ClipBoard.\n");
    this->mleAppendText("Now you can paste it into your favorite editor and save it.\n");
    this->mleAppendText("True save functionality will be implemented at a later time.\n");
    //~ this->mleAppendText("\n");

    //~ this->mleAppendText("!! M A K E   S U R E  Y O U  H A V E  A  W O R K I N G  B A C K U P !!\n");
}

void    ProblemFixerDialog::noDFSVOS2() {
    this->mleClear();
    this->mleAppendText("The DFSVOS2 backend could not be found!\n");
    this->mleAppendText("There should be an DFSVOS2.BIN in ?:\\eCS\\System\\eWPS\\Plugins\\XCenter\n");
    this->mleAppendText("-- or --\n");
    this->mleAppendText("An an DFSVOS2.EXE in ?:\\eCS\\Install !\n");
    this->mleAppendText("\n");
    this->mleAppendText("This Problem Fixer does not work until the DFSVOS2 backend is available.\n");
}


void    ProblemFixerDialog::setupFixCommands() {
    ULONG   dskstat;
    ULONG   fixmask;
    ULONG   effmask;
    ULONG   tofixmap;
    CHAR    buf[256] = "\0";
    int     i = 0;

    /* Get the status of the disk */
    dskstat = this->getDiskStatus();

    /* Create the fixmask to isolate what we are looking for */
    fixmask =   DSKSTAT_BEYONDENDDISK |
                DSKSTAT_CORRUPTED |
                DSKSTAT_DIRTY |
                DSKSTAT_HIDDENSECSZERO |
                DSKSTAT_NOACCESS;
    this->set_fixmask(fixmask);

    /*
    // Create the effective mask by removing things we are not looking for.
    // An effmask of 1 will indicate a fixable disk.
    */
    effmask =   dskstat & ~fixmask & ~DSKSTAT_VALID &
                ~DSKSTAT_PRM & ~DSKSTAT_RESIZE & ~DSKSTAT_FILESYS;
    this->set_effmask(effmask);

    /* Set the tofixmap, this contains flags for each problem to fix */
    tofixmap = dskstat & fixmask;
    this->set_tofixmap(tofixmap);

    sprintf(
        buf,
        "dskstat: %08lX, fixmask: %08lX, effmask: %08lX, tofixmap:%08lX\n",
        dskstat,
        fixmask,
        effmask,
        tofixmap
    );
    this->mleAppendText(buf);
    this->mleAppendText("\n");

    if (dskstat | DSKSTAT_VALID) {

        if (effmask == 1) {
            if (dskstat & DSKSTAT_BEYONDENDDISK) {
                sprintf(
                    this->fixcommands[this->fixcommands_count],
                    "%s part %d#resize -free:1",
                    g_dfsvos2,
                    QueryPartitionNumer(this->getPhysDisk())
                );
                this->fixes[this->fixcommands_count] = PF_FIX_SIZE;
                this->fixcommands_count++;
            }
            // only fix for fat16
            if ((dskstat & DSKSTAT_HIDDENSECSZERO) && (((dskstat & DSKSTAT_FILESYS) >> 8) == FSDT_FAT)) {
                sprintf(
                    this->fixcommands[this->fixcommands_count],
                    "%s part %d#fixpbr -!",
                    g_dfsvos2,
                    QueryPartitionNumer(this->getPhysDisk())
                );
                this->fixes[this->fixcommands_count] = PF_FIX_HSECS;
                this->fixcommands_count++;
            }
            // don't run chkdsk for fat32
            if ((dskstat & DSKSTAT_NOACCESS) && (((dskstat & DSKSTAT_FILESYS) >> 8) != FSDT_FAT32)){
                sprintf(
                    this->fixcommands[this->fixcommands_count],
                    "CHKDSK %c: /F",
                    this->getDriveLetter()
                );
                this->fixes[this->fixcommands_count] = PF_FIX_DIRTY;
                this->fixcommands_count++;
            }
        }
        else {
            if (dskstat | DSKSTAT_BIGFLOPPY)
                this->action_taken = DID_FIX;
            this->set_close_state(0);
        }
    }
}


ulong   ProblemFixerDialog::instanceMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2) {
    MRESULT             mresReply   = 0;
    char                buf[4096]   = "\0";
    char                buf2[4096]  = "\0";
    char*               output      = NULL;
    int                 i = 0;


    //~ {
        //~ char    buf[8192];
        //~ buf[0000] = '1';
        //~ buf[4095] = '2';
        //~ buf[4096] = '3';
        //~ buf[8191] = '4';
    //~ }



    switch (msg) {

        case WM_INITDLG2: {
            int     i = 0;
            //!: Labels for the action-buttons
            PCHAR   action_names[] = {
                "Disk Map",             // 1
                "Partition Info",       // 2
                "Volume Info",          // 3
                "LVM Info",             // 4
                "MBR Info",             // 5
                "PBR Info",             // 6

                "Dump MBR",             // 7
                "Dump PBR",             // 8
                "Dump USB Info",        // 9

                "Show Help",            // 10
                "Show Problems",        // 11
                "Show Fix Commands",    // 12
                NULL
            };

            __debug("WM_INITDLG2", "From Dynamic", DBG_MLE);

            /* Set the labels, NULL terminates loop */
            for (i=0; action_names[i]; i++) {
                WinSetDlgItemText(this->handle, ID_ACTION_1+i, action_names[i]);
            }

            //~ mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
            break;
        }

        case WM_COMMAND: {
            switch (SHORT1FROMMP(mp1)) {

                /*
                // BUTTON ACTIONS
                */
                case DID_FIX: {
                    int     i = 0;
                    char*   output = NULL;

                    this->mleClear();
                    this->mleAppendText("####################\n");
                    this->mleAppendText("# Applying Fix(es) #\n");
                    this->mleAppendText("####################\n");

                    sprintf(buf, "dskstat:%08lX\n", this->dskstat);
                    this->mleAppendText(buf);
                    this->mleAppendText("\n");

                    if (this->fixcommands_count > 0) {
                        for (i=0; i<this->fixcommands_count; i++) {
                            switch (this->fixes[i]) {
                                case PF_FIX_SIZE:
                                    sprintf(buf, "FIX COMMAND: %s\n", this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    output = RunCommandAndCaptureOutput(this->fixcommands[i]);
                                    this->mleAppendText(output+4);
                                    this->mleAppendText("\n");
                                    break;
                                case PF_FIX_HSECS:
                                    sprintf(buf, "FIX COMMAND: %s\n", this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    output = RunCommandAndCaptureOutput(this->fixcommands[i]);
                                    this->mleAppendText(output+4);
                                    this->mleAppendText("\n");
                                    break;
                                case PF_FIX_DIRTY:
                                    sprintf(buf, "FIX COMMAND: %s\n", this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    output = RunCommandAndCaptureOutput(this->fixcommands[i]);
                                    this->mleAppendText(output+4);
                                    this->mleAppendText("\n");
                                    break;
                            }
                        }
                        this->action_taken = DID_FIX;
                    }

                    WinShowWindow(WinWindowFromID(this->handle, ID_SHOW_PROBLEMS), FALSE);
                    for (i=ID_ACTION_1; i<=ID_ACTION_12; i++) {
                        WinShowWindow(WinWindowFromID(this->handle, i), FALSE);
                    }
                    WinShowWindow(WinWindowFromID(this->handle, ID_IMAGE_BACKUP), FALSE);
                    WinShowWindow(WinWindowFromID(this->handle, ID_IMAGE_RESTORE), FALSE);
                    this->set_close_state(1);
                    //~ mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
                    break;
                }

                case DID_CANCEL: {
                    mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
                    break;
                }

                case DID_HIDE: {
                    this->hide();
                    mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
                    break;
                }

                case DID_CLOSE: {
                    mresReply = (MRESULT) 0x1234;
                    //~ mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
                    WinDismissDlg(this->handle, this->action_taken);
                    break;
                }


                /*
                // HELPER ACTIONS
                */
                case ID_ACTION_1: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    //~ sprintf(buf, "DFSVOS2 -a -B- -e- map -d:%d", this->getPhysDisk());
                    sprintf(buf, "%s map -d:%d", g_dfsvos2, this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    break;
                }

                case ID_ACTION_2: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "%s part -d:%d", g_dfsvos2, this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    break;
                }

                case ID_ACTION_3: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "%s vol %c: -d:%d", g_dfsvos2, this->getDriveLetter(), this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    break;
                }

                case ID_ACTION_4: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "%s lvm -list -d:%d", g_dfsvos2, this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    break;
                }

                case ID_ACTION_5: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "%s disk %d", g_dfsvos2, this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    break;
                }

                case ID_ACTION_6: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "%s vol %c:#0 -d:%d", g_dfsvos2, this->getDriveLetter(), this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    break;
                }

                case ID_ACTION_7: {
                    CARDINAL32  rc          = -1;
                    CARDINAL32  rc2         = -1;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    /* Get the MBR */
                    rc = ReadMBR(this->getPhysDisk(), buf, sizeof(buf));

                    /* Dump the sector */
                    dump_page(buf, buf2, 0);
                    this->mleAppendText(buf2);
                    this->mleAppendText("\n");
                    dump_page(buf+256, buf2, 256);
                    this->mleAppendText(buf2);
                    this->mleAppendText("\n");
                    this->mleAppendText("\n");

                    mresReply = 0;
                    break;
                }

                case ID_ACTION_8: {
                    APIRET      ulrc        = -1;
                    APIRET      ulrc2       = -1;
                    HFILE       fhandle     = NULL;
                    ULONG       bread       = 0;
                    ULONG       ulAction    = -1;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    /* Get the PBR */
                    ulrc = ReadPBR(this->getDriveLetter(), buf, sizeof(buf));

                    /* Dump the sector */
                    dump_page(buf, buf2, 0);
                    this->mleAppendText(buf2);
                    this->mleAppendText("\n");
                    dump_page(buf+256, buf2, 256);
                    this->mleAppendText(buf2);
                    this->mleAppendText("\n");
                    this->mleAppendText("\n");

                    mresReply = 0;
                    break;
                }

                case ID_ACTION_9: {
                    USBDeviceReport*    usbdev = NULL;
                    struct usb_device_descriptor* usbdescr = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    /* Clear buffer */
                    memset(buf, 0, sizeof(buf));
                    sprintf(buf, "NO USB DATA AVAILABLE\n");

                    /* Get the corresponding USB device */
                    usbdev = DriveLetterToUsbDevice(this->getDriveLetter());

                    /* Show Action */
                    this->mleAppendText("** [widget] **\n");

                    /* Prepare info in buffer */
                    if (usbdev) {
                        sprintf(buf,
                            "New Device         : %d\n"
                            "Drive Map          : %08lX\n"
                            "Physical Disk      : %d\n"
                            "Removed            : %d\n"
                            "Safely Ejected     ; %d\n"
                            "Eject Pending      ; %d\n"
                            "Compared           ; %d\n"
                            "Dasd               ; %d\n"
                            "Zip Drive          ; %d\n"
                            "Was Phantom        ; %d\n"
                            "Drive Letter       : %c:\n"
                            "",
                            usbdev->iamnew,
                            usbdev->drivemap,
                            usbdev->diskindex,
                            usbdev->removed,
                            usbdev->safe_eject,
                            usbdev->eject_pending,
                            usbdev->compared,
                            usbdev->dasd,
                            usbdev->zipdrive,
                            usbdev->was_phantom,
                            usbdev->dletter
                        );
                    }

                    /* Show USB Info */
                    this->mleAppendText(buf);

                    /* Show Action */
                    this->mleAppendText("** [device] **\n");

                    /* Prepare info in buffer */
                    if (usbdev) {
                        usbdescr = (struct usb_device_descriptor*) usbdev->data;
                        sprintf(buf,
                            "Product ID         : %04X\n"
                            "Vendor ID          : %04x\n"
                            "Device Class       : %d\n"
                            "Device Subclass    : %d\n"
                            "Device Protocol    ; %d\n"
                            "Num Configurations : %d\n"
                            "",
                            usbdescr->idProduct,
                            usbdescr->idVendor,
                            usbdescr->bDeviceClass,
                            usbdescr->bDeviceSubClass,
                            usbdescr->bDeviceProtocol,
                            usbdescr->bNumConfigurations
                        );
                    }

                    /* Show USB Info */
                    this->mleAppendText(buf);
                    this->mleAppendText("\n");

                    mresReply = 0;
                    break;
                }

                case ID_ACTION_10: {
                    /* Show Action */
                    this->mleClear();
                    this->help();
                    mresReply = 0;
                    break;
                }

                case ID_SHOW_PROBLEMS:
                case ID_ACTION_11: {
                    /* Show Action */
                    this->mleClear();
                    this->mle_cmdname(SHORT1FROMMP(ID_ACTION_11));

                    sprintf(buf, "dskstat: %08lX, fixmask: %08lX, effmask: %08lX, tofixmap:%08lX\n",
                        this->dskstat,
                        this->fixmask,
                        this->effmask,
                        this->tofixmap
                    );
                    this->mleAppendText(buf);
                    this->mleAppendText("\n");

                    mresReply = 0;

                    if (this->getDiskStatus() & DSKSTAT_BIGFLOPPY) {
                        sprintf(buf, "%s", pfd_big_floppy_problem);
                        this->mleAppendText(buf);
                        break;
                    }

                    if ((this->getDiskStatus() & DSKSTAT_PARTCOUNT) > 1) {
                        sprintf(buf, "%s", pfd_too_many_partitions_problem);
                        this->mleAppendText(buf);
                        break;
                    }

                    if (this->get_effmask() == 1) {
                        for (i=0; i<this->fixcommands_count; i++) {
                            switch (this->fixes[i]) {
                                case PF_FIX_SIZE:
                                    sprintf(buf, pfd_corrupted_problem, this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    break;
                                case PF_FIX_HSECS:
                                    sprintf(buf, pfd_bad_hiddensecs_problem, this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    break;
                                case PF_FIX_DIRTY:
                                    sprintf(buf, pfd_dirty_fs_problem, this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    break;
                            }
                        }
                        break;
                    }


                    sprintf(buf, "%s", pfd_unusable_problem);
                    this->mleAppendText(buf);
                    sprintf(buf, "%s", pfd_ioerror_problem);
                    this->mleAppendText(buf);
                    sprintf(buf, "%s", pfd_corrupted_problem);
                    this->mleAppendText(buf);

                    sprintf(buf, "%s", pfd_bad_hiddensecs_problem);
                    this->mleAppendText(buf);
                    sprintf(buf, "%s", pfd_dirty_fs_problem);
                    this->mleAppendText(buf);

                    this->mleAppendText("\n");

                    break;
                }

                case ID_ACTION_12: {
                    /* Show Action */
                    this->mleClear();
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(
                        buf,
                        "dskstat: %08lX, fixmask: %08lX, effmask: %08lX, tofixmap:%08lX\n",
                        this->dskstat,
                        this->fixmask,
                        this->effmask,
                        this->tofixmap
                    );
                    this->mleAppendText(buf);
                    this->mleAppendText("\n");

                    mresReply = 0;

                    if (this->getDiskStatus() & DSKSTAT_BIGFLOPPY) {
                        sprintf(buf, "%s", pfd_big_floppy_fix);
                        this->mleAppendText(buf);
                        break;
                    }

                    if ((this->getDiskStatus() & DSKSTAT_PARTCOUNT) > 1) {
                        sprintf(buf, "%s", pfd_too_many_partitions_fix);
                        this->mleAppendText(buf);
                        break;
                    }

                    if (this->get_effmask() == 1) {
                        for (i=0; i<this->fixcommands_count; i++) {
                            switch (this->fixes[i]) {
                                case PF_FIX_SIZE:
                                    sprintf(buf, pfd_corrupted_fix, this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    break;
                                case PF_FIX_HSECS:
                                    sprintf(buf, pfd_bad_hiddensecs_fix, this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    break;
                                case PF_FIX_DIRTY:
                                    sprintf(buf, pfd_dirty_fs_fix, this->fixcommands[i]);
                                    this->mleAppendText(buf);
                                    break;
                            }
                        }
                        break;
                    }

                    sprintf(buf, "%s", pfd_unusable_fix);
                    this->mleAppendText(buf);
                    sprintf(buf, "%s", pfd_ioerror_fix);
                    this->mleAppendText(buf);
                    sprintf(buf, "%s", pfd_corrupted_fix);
                    this->mleAppendText(buf);
                    sprintf(buf, "%s", pfd_big_floppy_fix);
                    this->mleAppendText(buf);
                    sprintf(buf, "%s", pfd_bad_hiddensecs_fix);
                    this->mleAppendText(buf);
                    sprintf(buf, "%s", pfd_dirty_fs_fix);
                    this->mleAppendText(buf);

                    this->mleAppendText("\n");
                    mresReply = 0;
                    break;
                }



                /*
                // TESTING
                */
                case ID_TEST_1: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText("\n");
                    WinSetDlgItemText(this->handle, ID_FIX_WARNING, "There are no fixes that can be applied");
                    mresReply = 0;
                    break;
                }

                case ID_TEST_2: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText("\n");
                    WinSetDlgItemText(this->handle, ID_FIX_WARNING, "The fix(es) have been applied, click on the Close button to eject the medium");
                    mresReply = 0;
                    break;
                }

                case ID_TEST_3: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText("\n");

                    ParseDisk(this->getPhysDisk(), g_dfsvos2_ecsdisks_beyondendisk, buf);
                    this->mleAppendText(buf);
                    this->mleAppendText("\n");

                    sprintf(buf, "Partition: %d\n", QueryPartitionNumer(this->getPhysDisk()));
                    this->mleAppendText(buf);
                    this->mleAppendText("\n");

                    mresReply = 0;
                    break;
                }

                case ID_TEST_4: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText("\n");

                    this->set_fix_state();

                    mresReply = 0;
                    break;
                }

                case ID_TEST_5: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText("\n");

                    this->set_close_state(0);

                    mresReply = 0;
                    break;
                }


                case ID_TEST_6: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText("\n");


                    mresReply = 0;
                    break;
                }

                case ID_TEST_7: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText("\n");

                    sprintf(buf, "%s part -xml -file- -d:%d", g_dfsvos2, this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    DosFreeMem(output);

                    mresReply = 0;
                    break;
                }


                /*
                // BACKUP & RESTORE ACTIONS
                */
                case ID_IMAGE_BACKUP: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText(
                        "== Sorry, this function has not been implemented yet ==\n"
                        "The idea is to dump the complete raw usb storage device to a file to obtain a mirror.\n"
                        "Should the fix fail and corrupt the device, this mirror can be restored to recreate\n"
                        "the disk excactly as it was before applying the fix.\n"
                        "This action will require enough disk space to hold the complete device,\n"
                        "which could be several Giga Bytes.\n"
                        "Also the destination FS needs to be JFS to hold such large files.\n"
                    );
                    this->mleAppendText("\n");
                    mresReply = 0;
                    break;
                }

                case ID_IMAGE_RESTORE: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText(
                        "== Sorry, this function has not been implemented yet ==\n"
                        "When implemented it would restore an image file to the usb storage device.\n"
                        "Like the Image Backup procedure it will take a considerable amount of time to\n"
                        "complete such an image restore, depending on the size of the device.\n"
                    );
                    this->mleAppendText("\n");
                    mresReply = 0;
                    break;
                }



                /*
                // MISC ACTIONS
                */
                case ID_CLEAR_LOG_WINDOW: {
                    this->mleClear();
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
            break;
        }

    }

    return (ulong) mresReply;
}

/*
// Used to investigate the stack problem.
*/
ulong   ProblemFixerDialog::instanceMessageHandler2(ulong hwnd, ulong msg, ulong mp1, ulong mp2) {
    MRESULT             mresReply   = 0;
    char                buf[1024]   = "\0";

    switch (msg) {

        case WM_INITDLG2: {
            int     i = 0;
            //!: Labels for the action-buttons
            PCHAR   action_names[] = {
                "Disk Map",             // 1
                "Partition Info",       // 2
                "Volume Info",          // 3
                "LVM Info",             // 4
                "MBR Info",             // 5
                "PBR Info",             // 6

                "Dump MBR",             // 7
                "Dump PBR",             // 8
                "Dump USB Info",        // 9

                "Show Help",            // 10
                "Show Problems",        // 11
                "Show Fix Commands",    // 12
                NULL
            };

            __debug("WM_INITDLG2", "From Dynamic", DBG_MLE);

            /* Set the labels, NULL terminates loop */
            for (i=0; action_names[i]; i++) {
                WinSetDlgItemText(this->handle, ID_ACTION_1+i, action_names[i]);
            }

            //~ mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
            //~ break;
        }
        break;


        case WM_COMMAND: {
            switch (SHORT1FROMMP(mp1)) {

                /*
                // BUTTON ACTIONS
                */
                case DID_OK: {
                    mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
                    //~ break;
                }
                break;

                case DID_FIX: {
                    mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
                    //~ break;
                }
                break;

                case DID_CANCEL: {
                    mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
                    //~ break;
                }
                break;

                case DID_HIDE: {
                    this->hide();
                    //~ break;
                }
                break;

                case DID_CLOSE: {
                    mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
                    //~ break;
                }
                break;

                /*
                // HELPER ACTIONS
                */
                case ID_ACTION_1: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "DFSVOS2 -a -B- -e- map -d:%d", this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_ACTION_2: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "DFSVOS2 -a -B- -e- part -d:%d", this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_ACTION_3: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "DFSVOS2 -a -B- -e- vol %c: -d:%d", this->getDriveLetter(), this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_ACTION_4: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "DFSVOS2 -a -B- -e- lvm -list -d:%d", this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_ACTION_5: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "DFSVOS2 -a -B- -e- disk %d", this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_ACTION_6: {
                    char*   output  = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    sprintf(buf, "DFSVOS2 -a -B- -e- vol %c:#0 -d:%d", this->getDriveLetter(), this->getPhysDisk());
                    output = RunCommandAndCaptureOutput(buf);
                    this->mleAppendText(output+4);
                    this->mleAppendText("\n");
                    DosFreeMem(output);
                    mresReply = 0;
                    //~ break;
                }
                break;


///:! WITH ONLY 6 ACTIONS BLDLEVEL FAILS TO EMBED (DLL IS WORKING HOWEVER)
// ADDING ACTION 7 RESTORES BUILDLEVEL

                case ID_ACTION_7: {
                    CARDINAL32  rc          = -1;
                    CARDINAL32  rc2         = -1;
                    char        buf[1024]   = "\0";
                    char        buf2[4096]  = "\0";

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    /* Clear buffer */
                    memset(buf, 0, sizeof(buf));
                    sprintf(buf, "NO MBR DATA READ");

                    /* Read the first sector from the disk */
                    Open_LVM_Engine(TRUE, &rc);
                    if (rc == LVM_ENGINE_NO_ERROR || rc == LVM_ENGINE_ALREADY_OPEN) {
                        Read_Sectors(this->getPhysDisk(), 0L, 1, buf, &rc2);
                    }

                    /* Only close if it was sucessfully opened; don't close if it was already open */
                    if (rc == LVM_ENGINE_NO_ERROR)
                        Close_LVM_Engine();

                    /* Dump the sector */
                    dump_page(buf, buf2, 0);
                    this->mleAppendText(buf2);
                    this->mleAppendText("\n");
                    dump_page(buf+256, buf2, 256);
                    this->mleAppendText(buf2);
                    this->mleAppendText("\n");
                    this->mleAppendText("\n");

                    mresReply = 0;
                    //~ break;
                }
                break;


                case ID_ACTION_8: {
                    APIRET      ulrc        = -1;
                    APIRET      ulrc2       = -1;
                    char        buf[1024]   = "\0";
                    char        buf2[4096]  = "\0";
                    HFILE       fhandle     = NULL;
                    ULONG       bread       = 0;
                    ULONG       ulAction    = -1;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    /* Clear buffer */
                    memset(buf, 0, sizeof(buf));
                    sprintf(buf, "NO PBR DATA READ");

                    /* Misuse buf2 to construct drive-letter as string */
                    sprintf(buf2, "%c:", this->getDriveLetter());

                    /* Open DASD */
                    ulrc = DosOpen(buf2,
                                    &fhandle,
                                    &ulAction,
                                    0,
                                    FILE_NORMAL,
                                    OPEN_ACTION_OPEN_IF_EXISTS,
                                    OPEN_FLAGS_DASD | OPEN_SHARE_DENYWRITE | OPEN_ACCESS_READONLY,
                                    NULL);

                    /* Read first sector from volume */
                    ulrc2 = DosRead(fhandle, buf, 512, &bread);

                    /* Close Volume */
                    DosClose(fhandle);


                    /* Dump the sector */
                    dump_page(buf, buf2, 0);
                    this->mleAppendText(buf2);
                    this->mleAppendText("\n");
                    dump_page(buf+256, buf2, 256);
                    this->mleAppendText(buf2);
                    this->mleAppendText("\n");
                    this->mleAppendText("\n");

                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_ACTION_9: {
                    USBDeviceReport*    usbdev = NULL;
                    char                buf[4096] = "\0";
                    struct usb_device_descriptor* usbdescr = NULL;

                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));

                    /* Clear buffer */
                    memset(buf, 0, sizeof(buf));
                    sprintf(buf, "NO USB DATA AVAILABLE\n");

                    /* Get the corresponding USB device */
                    usbdev = DriveLetterToUsbDevice(this->getDriveLetter());

                    /* Show Action */
                    this->mleAppendText("** [widget] **\n");

                    /* Prepare info in buffer */
                    if (usbdev) {
                        sprintf(buf,
                            "New Device         : %d\n"
                            "Drive Map          : %08lX\n"
                            "Physical Disk      : %d\n"
                            "Removed            : %d\n"
                            "Safely Ejected     ; %d\n"
                            "Eject Pending      ; %d\n"
                            "Compared           ; %d\n"
                            "Dasd               ; %d\n"
                            "Zip Drive          ; %d\n"
                            "Was Phantom        ; %d\n"
                            "Drive Letter       : %c:\n"
                            "",
                            usbdev->iamnew,
                            usbdev->drivemap,
                            usbdev->diskindex,
                            usbdev->removed,
                            usbdev->safe_eject,
                            usbdev->eject_pending,
                            usbdev->compared,
                            usbdev->dasd,
                            usbdev->zipdrive,
                            usbdev->was_phantom,
                            usbdev->dletter
                        );
                    }

                    /* Show USB Info */
                    this->mleAppendText(buf);

                    /* Show Action */
                    this->mleAppendText("** [device] **\n");

                    /* Prepare info in buffer */
                    if (usbdev) {
                        usbdescr = (struct usb_device_descriptor*) usbdev->data;
                        sprintf(buf,
                            "Product ID         : %04X\n"
                            "Vendor ID          : %04x\n"
                            "Device Class       : %d\n"
                            "Device Subclass    : %d\n"
                            "Device Protocol    ; %d\n"
                            "Num Configurations : %d\n"
                            "",
                            usbdescr->idProduct,
                            usbdescr->idVendor,
                            usbdescr->bDeviceClass,
                            usbdescr->bDeviceSubClass,
                            usbdescr->bDeviceProtocol,
                            usbdescr->bNumConfigurations
                        );
                    }

                    /* Show USB Info */
                    this->mleAppendText(buf);
                    this->mleAppendText("\n");

                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_ACTION_10: {
                    /* Show Action */
                    //~ this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->help();
                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_SHOW_PROBLEMS:
                case ID_ACTION_11: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(ID_ACTION_11));
                    this->mleAppendText("\n");
                    sprintf(buf, "dskstat:%08lX\n", this->dskstat);
                    this->mleAppendText(buf);
                    mresReply = 0;
                    //~ break;
                }
                break;

                case ID_ACTION_12: {
                    /* Show Action */
                    this->mle_cmdname(SHORT1FROMMP(mp1));
                    this->mleAppendText("\n");
                    mresReply = 0;
                    //~ break;
                }
                break;


                /* Default */
                default: {
                    mresReply = 0;
                    //mresReply = WinDefDlgProc(hwnd, msg, mp1, mp2);           // NO DEFAULT HANDLING OF COMMANDS !!
                    //~ break;
                }
                break;

            } // switch
            //~ break;
        }
        break;

        //~: Handle default message (debug dialog)
        default: {
            mresReply = 0;
            mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
        }
        break;

    }

    return (ulong) mresReply;
}


/*
// classMessageHandler :: Handles all messages.
// This is the actual window-procedure which is invoked as a callback.
// Therefore it's declaration is static because it get's called externally from the dialog-instance.
// Is serves as a common entry-point for messages sent or posted to this class.
// When an instance of the class is created, the this-pointer is passed to the WM_INITDLG message.
// There it is put in the WindowULong of the window instance.
// Everytime this WindowULong is retrieved and if non-zero, the message is delegated to
// the instance method. This was multiple instances can have their own instance data.
// Actual initialization is handled in WM_INITDLG2, which is posted from WM_INITDLG.
*/
ulong   ProblemFixerDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2) {

    MRESULT             mresReply   = 0;
    char                buf[256]    = "\0";
    ProblemFixerDialog* myself      = NULL;

    /* Get the this-pointer of the instance */
    myself = (ProblemFixerDialog*) WinQueryWindowULong(hwnd, QWL_USER);


    /* If it is non-zero then delegate the message to the instance handler */
    if (myself) {
        mresReply = (MRESULT) myself->instanceMessageHandler(hwnd, msg, mp1, mp2);
    }
    /* If zero then WM_INITDLG was not processed yet, so handle msgs here */
    else {
        switch (msg) {

            case WM_INITDLG: {
                /* Get the this-pointer from mp2 and put it in the WindowULong of this window instance */
                WinSetWindowULong(hwnd, QWL_USER, (ULONG) *(ulong*)(mp2+2));
                sprintf(buf, "From Static -- myself: %08lX, WindowLong: %08lX", myself, WinQueryWindowULong(hwnd, QWL_USER));
                __debug("WM_INITDLG", buf, DBG_MLE);
                /* Delegate further initialization to the instance handler */
                WinPostMsg(hwnd, WM_INITDLG2, NULL, NULL);
                mresReply = FALSE;
                break;
            }

            case WM_INITDLG2: {
                __debug("WM_INITDLG2", "From Static", DBG_MLE);
                mresReply = FALSE;
                break;
            }

            //~: Handle default message (debug dialog)
            default: {
                mresReply = 0;
                mresReply = WinDefDlgProc(hwnd, msg, (MPARAM) mp1, (MPARAM) mp2);
            }
        }
        /// DEZE MOET HIER ANDERS GEEN GOEIE MSG AFHANDELING !
        //~ return (ulong) mresReply;
    }
    return (ulong) mresReply;
}
