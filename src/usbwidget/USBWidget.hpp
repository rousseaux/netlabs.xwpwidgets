/*****************************************************************************\
* USBWidget.hpp -- NeoWPS * USB Widget                                        *
*                                                                             *
* Copyright (c) RDP Engineering                                               *
* Portions Copyright (c) eCoSoft                                              *
* Portions Copyright (c) 2000-2001 Ulrich M”ller                              *
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
 * This the public header file for it's corrsponding source. (USBWidget.cpp)
 */


#ifndef     __USBWIDGET_HPP__
#define     __USBWIDGET_HPP__


#ifdef      __cplusplus
    extern "C" {
#endif


#include  "Master.hpp"
//~ #include  "ModuleGlobals.hpp"
//~ #include    "USBHelpers.hpp"

#include    "shared\center.h"       // Public eCenter interfaces




#define     TXFSDC_EJECT                    0x02    // eject removable

/*
// These are used in the window-procedure to select the indicator.
// The window-id should be used in the code (switch) instead of these.
*/
#define     SHOW_DEV_INSERT                 1
#define     SHOW_SAFE_EJECT                 2
#define     SHOW_DEV_REMOVE                 3
#define     SHOW_DRIVES_CHANGED             4

/*
// ID's of the indicator windows.
*/
#define     ID_INDI_DEV_INSERT              7053
#define     ID_INDI_SAFE_EJECT              7054
#define     ID_INDI_FORCE_REMOVE            7055
#define     ID_INDI_DRIVES_CHANGED          7056

/*
// ID's of the timers of the indicator windows.
// These control the time such a window is visible.
*/
#define     TIMER_SHOW_BANNER_DEV_INSERT        8001
#define     TIMER_SHOW_BANNER_SAFE_EJECT        8002
#define     TIMER_SHOW_BANNER_DEV_REMOVE        8003
#define     TIMER_SHOW_BANNER_DRIVES_CHANGED    8004

/*
// The timeouts for each of the indicator windows.
*/
#define     TIMEOUT_BANNER_DEV_INSERT       3000
#define     TIMEOUT_BANNER_SAFE_EJECT       3000
#define     TIMEOUT_BANNER_DEV_REMOVE       5000
#define     TIMEOUT_BANNER_DRIVES_CHANGED   3000


//#include  "setup.h"


static void internal_GetStaticTextSize(HWND hwnd, ULONG ulID, int *piCX, int *piCY);

/* Window & Dialog handlers */
MRESULT EXPENTRY fnwpIndiClient( HWND , ULONG , MPARAM , MPARAM  );
MRESULT EXPENTRY fnwpSampleWidget(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY fnwpSettingsDlg(HWND hwnd, ULONG msg0, MPARAM mp1, MPARAM mp2);



#ifndef     max
    #define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

// generic headers
#define     DONT_REPLACE_MALLOC     // in case mem debug is enabled


// #pragma hdrstop                  // VAC++ keeps crashing otherwise

/* ******************************************************************
 *
 *  Private definitions
 *
 ********************************************************************/

#define     WIDGET_VERSION  "7.1"

/* Define the percentages when battery images are changed. */
#define     FULL_PERCENT        95
#define     HIGH_PERCENT        65
#define     MEDIUM_PERCENT      35
#define     LOW_PERCENT         25
#define     CRITICAL_PERCENT    15



VOID cmnLogToPipe(const char*, ULONG, const char*, const char*, ...);
VOID cmnLogToFile(const char*, ULONG, const char*, const char*, ...);
int StopLVMThread(void);
int StopThread(void);


ULONG EXPENTRY  WgtInitModule(HAB hab,                               // XCenter's anchor block
                                HMODULE hmodPlugin,                 // module handle of the widget DLL
                                HMODULE hmodXFLDR,                  // XFLDR.DLL module handle
                                PXCENTERWIDGETCLASS *ppaClasses,
                                PSZ pszErrorMsg);

VOID EXPENTRY   WgtQueryVersion(PULONG pulMajor,
                              PULONG pulMinor,
                              PULONG pulRevision);

VOID EXPENTRY   WgtUnInitModule();




#ifdef __cplusplus
    }
#endif
#endif  // __USBWIDGET_HPP__

