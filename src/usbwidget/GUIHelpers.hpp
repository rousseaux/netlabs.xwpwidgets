/*****************************************************************************\
* GUIHelpers.hpp -- NeoWPS * USB Widget                                       *
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
 * This module contains a few several wrapper functions for the GUI System.
 * It is used to simplify things.
 */


#ifndef     __GUIHELPERS_HPP__
#define     __GUIHELPERS_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "ModuleGlobals.hpp"
#include    "ecomedia.h"

#include    "Window.hpp"
#include    "Dialog.hpp"

class   GUIHelpers {

    public:
    ulong   centerWindow(ulong this_window, ulong to_window);
    ulong   hideWindow(ulong window);
};

class   Monitor : public Object {

    public:
    Monitor();
    ~Monitor();

    ulong   start();
    ulong   stop();
    ulong   block();
    ulong   unblock();
    ulong   suspend();
    ulong   resume();

    ulong   setStatus();
    ulong   captureStatus();
    ulong   getStatus();

    ulong   eventStatusChanged();

};

class   DriveMonitor : public Monitor {
};

class   UsbMonitor : public Monitor {
};



// Window class was here

// Dialog class was here

/**
 * This dialog is only created when the debug-setting is on; which can be influenced by changing
 * the settings for the widget.
 * Settings get loaded when the widget is created.
 */
class   DebugDialog : public Dialog {

    private:

    public:
    DebugDialog();
    ~DebugDialog();
    ulong   create();
    ulong   destroy();


    ulong   redraw();

    ulong   msgInitDialog(ulong mp1, ulong mp2);
    ulong   msgCommand(ulong mp1, ulong mp2);

    ulong   commandDrawButton();
    ulong   commandDestroyButton();

    //~ static ulong EXPENTRY DebugDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    static ulong DebugDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
};

#define     PF_FIX_SIZE     1
#define     PF_FIX_HSECS    2
#define     PF_FIX_DIRTY    3


/**
 * This is the new dialog to fix an LVM hostile disk-layout.
 * The OO approach of DebugDialog seems to work good enough to implement it
 * this way instead of the usual like the normal DebugDialog.
 */
class   ProblemFixerDialog : public Dialog {

    private:
    char    dletter;
    int     pdisk;
    ulong   dskstat;
    ulong   fixmask;
    ulong   effmask;
    ulong   tofixmap;

    int     fixcommands_count;
    char    fixcommands[5][256];
    ulong   fixes[5];
    ulong   action_taken;


    void    init();
    int     mle_cmdname(int id);
    int     set_fix_state();
    int     set_close_state(int fixed);

    ulong   get_fixmask();
    ulong   set_fixmask(ulong fixmask);
    ulong   get_effmask();
    ulong   set_effmask(ulong effmask);
    ulong   get_tofixmap();
    ulong   set_tofixmap(ulong tofixmap);

    public:
    ProblemFixerDialog();
    ~ProblemFixerDialog();
    ulong   create();
    ulong   process();
    ulong   destroy();


    ulong   redraw();

    ulong   msgInitDialog(ulong mp1, ulong mp2);
    ulong   msgCommand(ulong mp1, ulong mp2);

    ulong   commandDrawButton();
    ulong   commandDestroyButton();

    void    mleAppendText(char* text);
    void    mleClear();

    char    getDriveLetter();
    char    setDriveLetter(char dletter);
    int     getPhysDisk();
    int     setPhysDisk(int pdisk);
    ulong   getDiskStatus();
    ulong   setDiskStatus(ulong dskstat);

    void    help();
    void    noDFSVOS2();
    void    setupFixCommands();

    ulong   instanceMessageHandler2(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    ulong   instanceMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    //~ static ulong EXPENTRY ProblemFixerDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    static ulong ProblemFixerDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
};




class   Notifier : public Object {
    private:
    Window* myNotifierWindow;

    Notifier();
    ~Notifier();

    ulong   show();
    ulong   hide();

};





extern      HBITMAP     hbmUSBThumb;
extern      HBITMAP     hbmIconInfo;
extern      HBITMAP     hbmIconWarning;

/* Prototypes */
void    GUIHelpers(void);

ULONG   MessageBox(PSZ title, PSZ text);                // Simple wrapper around WinMessageBox
void    LoadPictures(void);
BOOL    CenterWindow(HWND toCenterTo, HWND thisWindow); // Grabed from PPSERIAL.CPP, replace with XWP-version later
BOOL    OpenDriveView(char drive);




/*
// Macro to draw a rectangle around a window.
*/
#define DrawRect(x1,y1,x2,y2,color)  ptl.x = (LONG) (x1); \
                                     ptl.y = (LONG) (y1); \
                                     GpiSetCurrentPosition(hps,&ptl); \
                                     ptl.x = (LONG) (x1+x2); \
                                     ptl.y = (LONG) (y1+y2); \
                                     GpiSetColor(hps,color); \
                                     GpiBox(hps,DRO_OUTLINE,&ptl,0L,0L);



//#include  "USBHelpers.hpp"

#ifdef      __cplusplus
    }
#endif

#endif // __GUIHELPERS_HPP__
