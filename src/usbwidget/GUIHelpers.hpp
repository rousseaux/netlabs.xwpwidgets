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

// Monitor class was here

// DriveMonitor class was here

// UsbMonitor class was here

// Window class was here

// Dialog class was here

// DebugDialog class was here

#define     PF_FIX_SIZE     1
#define     PF_FIX_HSECS    2
#define     PF_FIX_DIRTY    3


// ProblemFixerDialog class was here

// Notifier class was here





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
