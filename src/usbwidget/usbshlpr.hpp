/*****************************************************************************\
* usbshlpr.hpp -- Support DAEMON for the NeoWPS * USB Widget                  *
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


#ifndef     __USBSHLPR_HPP__
#define     __USBSHLPR_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

/* Defines */
#define     DAEMON_CLI      0   // Start the daemon as a cli-program
#define     DAEMON_GUI      1   // Start the daemon as a gui-program (/pm)

/* System Includes */
#include    <conio.h>

/* Module Includes */
#include    "Master.hpp"

/* Component Includes */
#include    "Object.hpp"
#include    "Window.hpp"
#include    "Dialog.hpp"

/* IDs */
#include    "usbshlpr.ids"

/* Prototypes */
MRESULT EXPENTRY    GuiDaemonDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    GuiDaemonWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    ClientWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY    FrameWndProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
//! FIXME: Duplicated here because of ugly dependencies
ULONG   MessageBox(PSZ title, PSZ text);
ULONG   MessageBox2(PSZ title, PSZ text);
BOOL    CenterWindow(HWND toCenterTo, HWND thisWindow);
void    __debug(char* title, char* message, unsigned long flags);
extern HMODULE hmodMe;

/* Class Declarations */
class   Daemon;
class   CliDaemon;
class   GuiDaemon;
class   DialogWindow;
class   FrameWindow;
class   ClientWindow;

// ----------------------------------------------------------------------------
// Daemon :: Kinda provides the interface with no real implementation
// ----------------------------------------------------------------------------
class   Daemon {

    public:

    /* Constructor(s) and Destructor */
    Daemon();
    virtual ~Daemon();

    /* Public Methods */
    virtual int     doAction();     // Determines the action to take
    virtual int     start();        // Start the Daemon
    virtual int     stop();         // Stop the Daemon

    /* Public Attributes */
    int             argc;           // Argument count passed from main()
    char**          argv;           // Array of argument pointers passed from main()
    HEV             wait4daemon;    // UsbWidget blocks on this until Daemon ready
    HEV             wait4stop;      // Daemon blocks on this (/stop posts it)

    protected:
    virtual void    clear();        // Clear instance attributes

    private:
};

// ----------------------------------------------------------------------------
// CliDaemon :: Provides the implementation for the Daemon
// ----------------------------------------------------------------------------
// This class is used when the daemon is started without the /gui flag
// If not started detached, messages will be output to stdout.
// ----------------------------------------------------------------------------
class   CliDaemon : public Daemon {
    public:
    CliDaemon(int argc, char* argv[]);
    virtual ~CliDaemon();
    virtual int     start();        // Override default implementation
    virtual int     stop();         // Override default implemenation
    PMASTERMEMPOOL  pmmp;           // Pointer to shared memory
    protected:
    virtual void    clear();        // Clear instance attributes
    private:
    char    buf[512];               // Work buffer
};

// ----------------------------------------------------------------------------
// GuiDaemon :: Provides the implementation for the Daemon
// ----------------------------------------------------------------------------
// This class is used when the daemon is started witn the /pm flag.
// It will present a GUI and do its work on a separate thread.
// ----------------------------------------------------------------------------
class   GuiDaemon : public Daemon {

    public:

    /* Public constructor(s) and destructor */
    GuiDaemon(int argc, char* argv[]);
    virtual ~GuiDaemon();

    /* Public Methods */
    virtual int     start();        // Override default implementation
    virtual int     stop();         // Override default implemenation

    /* Public Attributes */
    PMASTERMEMPOOL  pmmp;           // Pointer to shared memory
    HWND            hwndApp;        // Handle for thread1 Application Window
    HAB             hab;            // Handle for thread1 Anchor Block
    HMQ             hmq;            // Handle for thread1 Message Queue
    QMSG            qmsg;           // Message Queue for thread1

    protected:
    virtual void    clear();                    // Clear instance attributes
    virtual void    changeProcessTypeToPM();    // Enable PM usage
    virtual int     enterMessageLoop();         // Returns WM_QUIT.mp1 value
    virtual int     doAfterPeek();              // Peek and remove after WM_QUIT
    virtual int     startAsDialog();            // Use WinLoadDlg()
    virtual int     startAsFrameWindow();       // Use WinCreateWindow()

    private:
    char    buf[512];               // Work buffer
};

// ----------------------------------------------------------------------------
// DialogWindow :: Presents the Application Window (as a dialog)
// ----------------------------------------------------------------------------
// For now we'll it a bit quick-n-dirty.
// Later we'll morph to a proper AplicationWindow class.
// ----------------------------------------------------------------------------
class   DialogWindow: public Dialog {

    public:

    /* Public Constuctor(s) and Destructor */
    DialogWindow();
    virtual ~DialogWindow();

    /* Public Methods */
    virtual void    init(GuiDaemon* gd);
    virtual int     create();
    virtual int     destroy();
    virtual void    dump();
    virtual HWND    addButton(ULONG id, ULONG x, ULONG y, ULONG cx, ULONG cy);

    /* Public Message Handlers */
    virtual MRESULT wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Public Attributes */
    PSZ         pszTitle;       // Window Title
    PFNWP       dlgProc;        // Dialog Procedure
    GuiDaemon*  gd;             // C++ Daemon Object

    protected:

    /* Protected Methods */
    virtual void    clear();

    private:

};

// ----------------------------------------------------------------------------
// FrameWindow :: Presents the Application Window (as a regular window)
// ----------------------------------------------------------------------------
// For now we'll it a bit quick-n-dirty.
// Later we'll morph to a proper AplicationWindow class.
// ----------------------------------------------------------------------------
class   FrameWindow : public Window {

    public:

    /* Public Constuctor(s) and Destructor */
    FrameWindow();
    virtual ~FrameWindow();

    /* Public Methods */
    virtual void    init(GuiDaemon* gd);
    virtual void    regclass();
    virtual int     create();
    virtual int     updateFrame(unsigned fids);
    virtual int     destroy();
    virtual void    dump();
    virtual HWND    addButton(ULONG id, ULONG x, ULONG y, ULONG cx, ULONG cy);

    /* Public Message Handlers */
    virtual MRESULT wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Public Attributes */
    ULONG       flStyle;            // Style Flags
    ULONG       flCreateFlags;      // Creation Flags
    PSZ         pszClassFrame;      // Window Class Name
    PSZ         pszTitle;           // Window Title
    ULONG       flStyleClient;      // Style for Client Window
    PFNWP       wndProc;            // Window Procedure
    FRAMECDATA  fcd;                // Frame Control Data
    GuiDaemon*  gd;                 // C++ Daemon Object

    protected:

    /* Protected Methods */
    virtual void    clear();

    /* Protected Attributes */
    ClientWindow*   client;

    private:

};

// ----------------------------------------------------------------------------
// ClientWindow :: Wraps the Application Window
// ----------------------------------------------------------------------------
// For now we'll it a bit quick-n-dirty.
// Later we'll morph to a proper AplicationWindow class.
// ----------------------------------------------------------------------------
class   ClientWindow : public Window {

    public:

    /* Public Constuctor(s) and Destructor */
    ClientWindow();
    virtual ~ClientWindow();

    /* Public Methods */
    virtual void    init();
    virtual void    regclass();
    virtual int     create();
    virtual int     destroy();
    virtual void    dump();
    virtual HWND    addButton(ULONG id, ULONG x, ULONG y, ULONG cx, ULONG cy);

    /* Public Message Handlers */
    virtual MRESULT wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Public Attributes */
    ULONG   flStyle;            // Style Flags
    PSZ     pszClassClient;     // Window Class Name
    PSZ     pszTitle;           // Window Title
    PFNWP   wndProc;            // Window Procedure
    FrameWindow*    parent;     // C++ Parent (container)

    protected:

    /* Protected Methods */
    virtual void    clear();

    private:

};

#ifdef      __cplusplus
    }
#endif

#endif // __USBSHLPR_HPP__
