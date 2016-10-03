/*****************************************************************************\
* usbsimpl.hpp -- NeoWPS * USB Widget                                         *
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

/******************************************************************************
* This module holds the (legacy) implementation the USB Widget
* -----------------------------------------------------------------------------
* It contains the main program logic which will gradually be moved to the
* UsbWidget Class.
*
* This module is based on the original sources created by eCoSoft.
* These original sources are in turn based on the Widget Template sources
* created by Ulrich M”ller that accompany the XWorkplace source package.
*
*/

/*
// Phantom Drives
// --------------
// When an usb-device with a drive mounted on it is removed without properly
// ejecting it, the drive does not disappear. Ejecting the drive will make it
// disappear, but only until the removable media are rescanned, after which it
// will reappear. Such a drive is called a *phantom* drive. Because the backing
// medium is missing, such a drive is of course not accessible. However,
// reinserting the _exact_ same usb-device will make the drive functional
// again. But, insering a _different_ usb-device will most probably cause a
// crash in LVM. The widget is capable of recovering phantom drives when the
// correct usb-device is interted again.
*/

#ifndef     __USBSIMPL_HPP__
#define     __USBSIMPL_HPP__


#ifdef      __cplusplus
    extern "C" {
#endif


#include  "Master.hpp"
//~ #include  "ModuleGlobals.hpp"
//~ #include    "USBHelpers.hpp"

#include    "setup.h"       // Public eCenter interfaces
#include    "shared\center.h"       // Public eCenter interfaces
#include    "shared\kernel.h"       // Public eCenter interfaces


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
#endif  // __USBSIMPL_HPP__

