/*****************************************************************************\
* usbsimpl.cpp -- NeoWPS * USB Widget                                         *
*                                                                             *
* Copyright (c) RDP Engineering                                               *
* Portions Copyright (c) eCoSoft                                              *
* Portions Copyright (c) 2000-2001 Ulrich M�ller                              *
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
* created by Ulrich M�ller that accompany the XWorkplace source package.
*
*/

/**
 * USB Widget - Taken from miniwdgt.c
 *
 *    Monitor USB Devices
 *
 *@@added V0.9.9 (2001-02-06) [umoeller]
 *@@header "shared\center.h"
 */

/**
 * Copyright (C) 2000-2001 Ulrich M�ller.
 * This file is part of the XWorkplace binary release.
 * XWorkplace is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation, in version 2 as it comes in the
 * "COPYING" file of the XWorkplace main distribution.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#pragma strings(readonly)


#ifdef __cplusplus
extern "C" {
#endif



/*
 *  Suggested #include order:
 *  1)  os2.h
 *  2)  C library headers
 *  3)  setup.h (code generation and debugging options)
 *  4)  headers in helpers\
 *  5)  at least one SOM implementation header (*.ih)
 *  6)  dlgids.h, headers in shared\ (as needed)
 *  7)  headers in implementation dirs (e.g. filesys\, as needed)
 *  8)  #pragma hdrstop and then more SOM headers which crash with precompiled headers
 */


#include    "WidgetSettingsDialog.hpp"


#include    "Apm.h"
#include    "Dialogids.h"
#include    "lange.h"

#include    "AllocMem.hpp"
#include    "Debug.hpp"
#include    "CList.hpp"
#include    "Threads.hpp"
#include    "Testing123.hpp"

#include    "APIHelpers.hpp"
#include    "GUIHelpers.hpp"
#include    "LVMHelpers.hpp"
#include    "USBHelpers.hpp"

// Rousseau:
// When uncommented VAC40 (VACBLD) barks at multiple inclusion eventhough
// the .h is protected with #ifndef (?)
//#include  "ecomedia.h"

#include    "usbcalls.h"


// Rousseau: removed extern "C"
//extern "C" {
#include    "lvm_intr.h"
//}


#ifndef SETUP_HEADER_INCLUDED
#include    "setup.h"
#endif

// disable wrappers, because we're not linking statically
#ifdef WINH_STANDARDWRAPPERS
    #undef WINH_STANDARDWRAPPERS
#endif

// headers in /helpers
#include    "helpers\comctl.h"      // common controls (window procs)
#include    "helpers\dosh.h"        // Control Program helper routines
#include    "helpers\gpih.h"        // GPI helper routines
#include    "helpers\prfh.h"        // INI file helper routines;
// #include "helpers\standards.h"   // some standard macros
                                    // this include is required for some
                                    // of the structures in shared\center.h

#include    "helpers\stringh.h"     // string helper routines
// #include "helpers\threads.h"     // thread helpers
#include    "helpers\timer.h"       // replacement PM timers
#include    "helpers\winh.h"        // PM helper routines
#include    "helpers\xstring.h"     // extended string helpers
#include    "helpers\nls.h"         // PS++

// XWorkplace implementation headers
#include    "dlgids.h"              // all the IDs that are shared with NLS
#include    "shared\center.h"       // public XCenter interfaces
#include    "shared\common.h"       // the majestic XWorkplace include file


VOID EXPENTRY WwgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData);

#include    "usbshold.hpp"

#include    "usbsimpl.hpp"


#define     PIPE_NAME   "\\PIPE\\PIPE1"
#define     PIPE_SIZE   256


/*
// Variables for the notification windows.
// These should be moved to the window-objects.
*/
typedef struct _VARIABLES {
    ULONG devinsert_banner_width;
    ULONG devinsert_banner_height;
    ULONG safeeject_banner_width;
    ULONG safeeject_banner_height;
    ULONG devremove_banner_width;
    ULONG devremove_banner_height;
    ULONG driveschanged_banner_width;
    ULONG driveschanged_banner_height;
} VARIABLES;


/* Declare external so we can show the BLDLEVEL in the Debug Dialog */
extern  char    bldlevel[];



//#ifndef   SETUP_HEADER_INCLUDED
//#include "setup.h"
//#endif
#ifdef  COMMON_HEADER_INCLUDED
//#include "xworkplace\include\shared\common.h"
#endif



extern DRIVESTATUS drvstate[26];
extern char Drives[26][128];

//extern char** Drives;
//~ extern CList currentUSBDevicesList;
extern HEV hevPushLVMthread;
//extern DRIVESTATUS* drvstate;




PSZ         pszOSDirectory;
char        szNlsPath[512];

ULONG       cxDesktop, cyDesktop;
ULONG       IndicatorHeaderH=32;

char        qq1[256];
char        qq2[256];



// drive letter, we pass it to eject thread
// ULONG ejectdrive;

HPS         hpsIndi;


char        PgmMsg[][MSGSIZE] = {
    "Eject properties",                 //  0
    "Close",                            //  1
    "Rediscover PRM",                   //  2
    "Eject %c:  %s",                    //  3
    "Drive %s",                         //  4
    "can be removed",                   //  5
    "DosDevIOCtl error, rc = %d"        //  6
};


//~ HWND            hwndWidget;

PXCENTERWIDGET  MyWidget;
CHAR            szIndiClass[] = "CRC indicator class";



/*
// Variables for the notification windows.
*/
VARIABLES       variables;





/* ******************************************************************
 *
 *  XCenter widget class definition
 *
 ********************************************************************/

/*
 *    This contains the name of the PM window class and
 *    the XCENTERWIDGETCLASS definition(s) for the widget
 *    class(es) in this DLL.
 *
 *    The address of this structure (or an array of these
 *    structures, if there were several widget classes in
 *    this plugin) is returned by the "init" export
 *    (WgtInitModule).
 */

#define     WNDCLASS_WIDGET_USB "XWPUSBMonibiWidget"

static XCENTERWIDGETCLASS G_WidgetClasses[] = {
    WNDCLASS_WIDGET_USB,                            // PM window class name
    0,                                              // additional flag, not used here
    "USBMonibiWidget",                              // internal widget class name
    "NeoWPS * USB Widget",                          // widget class name displayed to user
    WGTF_UNIQUEPERXCENTER | WGTF_TOOLTIP,           // widget class flags
    WwgtShowSettingsDlg                             // settings dialog
};

/* ******************************************************************
 *
 *  Function imports from XFLDR.DLL
 *
 ********************************************************************/

/*
 *    To reduce the size of the widget DLL, it is
 *    compiled with the VAC subsystem libraries.
 *    In addition, instead of linking frequently
 *    used helpers against the DLL again, we import
 *    them from XFLDR.DLL, whose module handle is
 *    given to us in the INITMODULE export.
 *
 *    Note that importing functions from XFLDR.DLL
 *    is _not_ a requirement. We only do this to
 *    avoid duplicate code.
 *
 *    For each funtion that you need, add a global
 *    function pointer variable and an entry to
 *    the G_aImports array. These better match.
 */

// resolved function pointers from XFLDR.DLL
PCMNQUERYDEFAULTFONT            pcmnQueryDefaultFont            = NULL;
PCMNQUERYHELPLIBRARY            pcmnQueryHelpLibrary            = NULL;
PCMNQUERYMAINRESMODULEHANDLE    pcmnQueryMainResModuleHandle    = NULL;
PCMNSETCONTROLSFONT             pcmnSetControlsFont             = NULL;

PCTRFREESETUPVALUE              pctrFreeSetupValue              = NULL;
PCTRPARSECOLORSTRING            pctrParseColorString            = NULL;
PCTRSCANSETUPSTRING             pctrScanSetupString             = NULL;

PGPIHDRAW3DFRAME                pgpihDraw3DFrame                = NULL;
PGPIHSWITCHTORGB                pgpihSwitchToRGB                = NULL;

PNLSDATETIME                    pstrhDateTime                   = NULL; //PS++
PNLSTHOUSANDSULONG              pstrhThousandsULong             = NULL; //PS++

PTMRSTARTXTIMER                 ptmrStartXTimer                 = NULL;
PTMRSTOPXTIMER                  ptmrStopXTimer                  = NULL;

PWINHCENTERWINDOW               pwinhCenterWindow               = NULL;
PWINHFREE                       pwinhFree                       = NULL;
PWINHQUERYPRESCOLOR             pwinhQueryPresColor             = NULL;
PWINHQUERYWINDOWFONT            pwinhQueryWindowFont            = NULL;
PWINHSETWINDOWFONT              pwinhSetWindowFont              = NULL;

PXSTRCAT                        pxstrcat                        = NULL;
PXSTRCLEAR                      pxstrClear                      = NULL;
PXSTRINIT                       pxstrInit                       = NULL;


//!: --------------------------------------------------- [ Imports to resolve ]
RESOLVEFUNCTION G_aImports[] = {
    "cmnQueryDefaultFont",          (PFN*)&pcmnQueryDefaultFont,
    "cmnQueryHelpLibrary",          (PFN*)&pcmnQueryHelpLibrary,
    "cmnQueryMainResModuleHandle",  (PFN*)&pcmnQueryMainResModuleHandle,
    "cmnSetControlsFont",           (PFN*)&pcmnSetControlsFont,
    "ctrFreeSetupValue",            (PFN*)&pctrFreeSetupValue,
    "ctrParseColorString",          (PFN*)&pctrParseColorString,
    "ctrScanSetupString",           (PFN*)&pctrScanSetupString,
    "gpihDraw3DFrame",              (PFN*)&pgpihDraw3DFrame,
    "gpihSwitchToRGB",              (PFN*)&pgpihSwitchToRGB,
    "strhDateTime",                 (PFN*)&pstrhDateTime,
    "strhThousandsULong",           (PFN*)&pstrhThousandsULong,
    "tmrStartXTimer",               (PFN*)&ptmrStartXTimer,
    "tmrStopXTimer",                (PFN*)&ptmrStopXTimer,
    "winhCenterWindow",             (PFN*)&pwinhCenterWindow,
    "winhFree",                     (PFN*)&pwinhFree,
    "winhQueryPresColor",           (PFN*)&pwinhQueryPresColor,
    "winhQueryWindowFont",          (PFN*)&pwinhQueryWindowFont,
    "winhSetWindowFont",            (PFN*)&pwinhSetWindowFont,
    "xstrcat",                      (PFN*)&pxstrcat,
    "xstrClear",                    (PFN*)&pxstrClear,
    "xstrInit",                     (PFN*)&pxstrInit
};


//!: o---------------------------------------> [ Private Widget Instance Data ]
/**
 *@@ MONITORSETUP:
 *    instance data to which setup strings correspond.
 *    This is also a member of MONITORPRIVATE.
 *
 *    Putting these settings into a separate structure
 *    is no requirement, but comes in handy if you
 *    want to use the same setup string routines on
 *    both the open widget window and a settings dialog.
 */
typedef struct _MONITORSETUP
{
    LONG        lcolBackground;         // background color
    LONG        lcolBackgroundLow;      // background color for low battery
    LONG        lcolForeground;         // foreground color (for text)
    LONG        lcolForegroundAC;       // foreground color when plugged in

    char        displayIcon;            // Display icon
    char        displayText;            // Display text
    char        displayPercentage;      // Display text as percentage or time
    char        displayMinutes;         // Display time as minutes or hours and minutes

    ULONG       WhenCharged;            // When charged
    ULONG       WhenCharging;           // When charged
    ULONG       WhenDischarging;        // When charged

    BOOL        enablebeeps;
    BOOL        monitorzip;
    char        debugInfo;              // Debug info in tooltip
    double      timeDivider;            // Fudge factor for time in tooltip

    PSZ         pszFont;
    // if != NULL, non-default font (in "8.Helv" format);
    // this has been allocated using local malloc()!
} MONITORSETUP, *PMONITORSETUP;

/*
 *@@ MONITORPRIVATE:
 *    more window data for the various monitor widgets.
 *
 *    An instance of this is created on WM_CREATE in
 *    fnwpMonitorWidgets and stored in XCENTERWIDGET.pUser.
 */

typedef struct _MONITORPRIVATE
{
    PXCENTERWIDGET  pWidget;
            // reverse ptr to general widget data ptr; we need
            // that all the time and don't want to pass it on
            // the stack with each function call

    ULONG           ulType;
                // one of the following:
                // -- MWGT_DATE: date widget
                // -- MWGT_TIME: time widget
                // -- MWGT_SWAPPER: swap monitor widget
                // -- MWGT_MEMORY: memory monitor widget;
                // this is copied from the widget class on WM_CREATE

    ULONG           cxCurrent;
    ULONG           cyCurrent;

    // widget settings that correspond to a setup string
    MONITORSETUP    Setup;


    HPOINTER        hptrAC;             // "AC" icon
    HPOINTER        hptrBattery;        // "battery" icon
    HPOINTER        hptrBattery020;     // "20% battery" icon
    HPOINTER        hptrBattery025;     // "25% battery" icon
    HPOINTER        hptrBattery050;     // "50% battery" icon
    HPOINTER        hptrBattery075;     // "75% battery" icon
    HPOINTER        hptrBattery100;     // "100% battery" icon
    HPOINTER        hptrNoBattery;      // No battery" icon
    ULONG           ulMiniIconSize;

    HPOINTER        hptrBAT0;
    HPOINTER        hptrBAT10;
    HPOINTER        hptrBAT20;
    HPOINTER        hptrBAT30;
    HPOINTER        hptrBAT40;
    HPOINTER        hptrBAT50;
    HPOINTER        hptrBAT60;
    HPOINTER        hptrBAT70;
    HPOINTER        hptrBAT80;
    HPOINTER        hptrBAT90;
    HPOINTER        hptrBAT100;

    HPOINTER        hptrAC0;
    HPOINTER        hptrAC10;
    HPOINTER        hptrAC20;
    HPOINTER        hptrAC30;
    HPOINTER        hptrAC40;
    HPOINTER        hptrAC50;
    HPOINTER        hptrAC60;
    HPOINTER        hptrAC70;
    HPOINTER        hptrAC80;
    HPOINTER        hptrAC90;
    HPOINTER        hptrAC100;

    ULONG           ulTimerID;          // if != NULLHANDLE, update timer is running

} MONITORPRIVATE, *PMONITORPRIVATE;




/*
// # Log to Pipe #
*/
VOID cmnLogToPipe(const char *pcszSourceFile,   // in: source file name
            ULONG ulLine,                       // in: source line
            const char *pcszFunction,           // in: function name
            const char *pcszFormat,             // in: format string (like with printf)
            ...) {


}






/*
// # Log to File #
*/
/*
 *@@ cmnLog:
 *    logs a message to the XWorkplace log file
 *    in the root directory of the boot drive.
 *
 *@@added V0.9.2 (2000-03-06) [umoeller]
 */
VOID cmnLogToFile(const char *pcszSourceFile,   // in: source file name
            ULONG ulLine,                       // in: source line
            const char *pcszFunction,           // in: function name
            const char *pcszFormat,             // in: format string (like with printf)
            ...) {                              // in: additional stuff (like with printf)

    va_list     args;
    CHAR        szLogFileName[100];
    FILE        *fileLog = 0;
    CHAR        buf[16];
    CHAR        boot_drive = '\0';

    /* Rousseau: Hoe een functie met varargs overriden ? -- functie-pointers ? / macro's ? */
    //cmnLogToFile(pcszSourceFile, ulLine, pcszFunction, pcszFormat);
    //cmnLogToPipe(pcszSourceFile, ulLine, pcszFunction, pcszFormat);

    boot_drive = QueryBootDrive();
    szLogFileName[0] = boot_drive;
    szLogFileName[1] = '\0';
    strcat(szLogFileName, ":\\var\\log\\usbshold.log");

}







/* ******************************************************************
 *
 *  Widget setup management
 *
 ********************************************************************/

//!: << WgtClearSetup >>
/**
 *@@ WgtClearSetup:
 *    cleans up the data in the specified setup
 *    structure, but does not free the structure
 *    itself.
 */
void WgtClearSetup(PMONITORSETUP pSetup) {
    cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- Begin");

    if (pSetup) {
        if (pSetup->pszFont) {
            free(pSetup->pszFont);
            pSetup->pszFont = NULL;
        }
    }

    cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- End\n\n");
}

//!: << MwgtFreeSetup >>
/**
 *@@ MwgtFreeSetup:
 *    cleans up the data in the specified setup
 *    structure, but does not free the structure
 *    itself.
 */
VOID MwgtFreeSetup(PMONITORSETUP pSetup) {
    cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- Begin");

    if (pSetup) {
        if (pSetup->pszFont) {
            free(pSetup->pszFont);
            pSetup->pszFont = NULL;
        }
    }

    cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- End\n\n");
}

//!: << MwgtScanSetup >>
/**
 *@@ MwgtScanSetup:
 *    scans the given setup string and translates
 *    its data into the specified binary setup
 *    structure.
 *
 *    NOTE: It is assumed that pSetup is zeroed
 *    out. We do not clean up previous data here.
 */
VOID MwgtScanSetup(const char *pcszSetupString, PMONITORSETUP pSetup) {

    PSZ     p;
    char    buf[256];

    variables.safeeject_banner_width = 256+64;
    variables.safeeject_banner_height = 96;

    //cmnLog(__FILE__, __LINE__, __FUNCTION__, pcszSetupString);

    cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- Begin");

    // background color
    p = pctrScanSetupString(pcszSetupString, "BGNDCOL");
    if (p) {
        pSetup->lcolBackground = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else {
        // default color:
        pSetup->lcolBackground = WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0);
    }



    // background color for Low Battery
    p = pctrScanSetupString(pcszSetupString, "LOWCOL");
    if (p) {
        pSetup->lcolBackgroundLow = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else {
        // default color:
        pSetup->lcolBackgroundLow = WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0);
    }

    // text color:
    p = pctrScanSetupString(pcszSetupString, "TEXTCOL");
    if (p) {
        pSetup->lcolForeground = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else {
        pSetup->lcolForeground = WinQuerySysColor(HWND_DESKTOP, SYSCLR_WINDOWSTATICTEXT, 0);
    }

    // text color:
    p = pctrScanSetupString(pcszSetupString, "ACTEXTCOL");
    if (p) {
        pSetup->lcolForegroundAC = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else {
        pSetup->lcolForegroundAC = WinQuerySysColor(HWND_DESKTOP, SYSCLR_WINDOWSTATICTEXT, 0);
    }

    // font:
    // we set the font presparam, which automatically
    // affects the cached presentation spaces
    p = pctrScanSetupString(pcszSetupString, "FONT");
    if (p) {
        pSetup->pszFont = strdup(p);
        pctrFreeSetupValue(p);
    }
    // else: leave this field null

/*

    // Show Icon:
    p = pctrScanSetupString(pcszSetupString, "DISPLAY");
    if (p)
    {
        if (strcmp(p, "ICON") == 0)
        {
            pSetup->displayIcon = 1;
            pSetup->displayText = 0;
        }
        else
        {
            pSetup->displayText = 1;
            pSetup->displayIcon = 0;
        }
        pctrFreeSetupValue(p);
    }
    else
    {
        pSetup->displayIcon = 1;
        pSetup->displayText = 1;
    }

    // Show Percentage or time
    p = pctrScanSetupString(pcszSetupString, "STYLE");
    if (p)
    {
//PS++
        if (strcmp(p, "TIMEPERCENT") == 0)
             pSetup->displayPercentage = 2;
        else
        if (strcmp(p, "TIME") == 0)
        {
            pSetup->displayPercentage = 0;
        }
        else // if (strcmp(p, "TIME") == 0)
        {
            pSetup->displayPercentage = 1;
        }
        pctrFreeSetupValue(p);
    }
    else
    {
        pSetup->displayPercentage = 1;
    }

    // Show time as minutes or hours and minutes
    p = pctrScanSetupString(pcszSetupString, "TIMESTYLE");
    if (p)
    {
        if (strcmp(p, "MINUTES") == 0)
        {
            pSetup->displayMinutes = 1;
        }
        else // if (strcmp(p, "HOURSMINUTES") == 0)
        {
            pSetup->displayMinutes = 0;
        }
        pctrFreeSetupValue(p);
    }
    else
    {
        pSetup->displayMinutes = 1;
    }

    // Show Debug info:
    p = pctrScanSetupString(pcszSetupString, "DEBUG");
    if (p)
    {
        pSetup->debugInfo = 1;
        pctrFreeSetupValue(p);
    }
    else
    {
        pSetup->debugInfo = 0;
    }
*/
    //


    p = pctrScanSetupString(pcszSetupString, "BEEPS");
    if (p) {
        if (strcmp(p, "YES") == 0) {
            pSetup->enablebeeps = 1;
            enablebeeps=1;
        }
        else {
            pSetup->enablebeeps = 0;
            enablebeeps=0;
        }

        pctrFreeSetupValue(p);
    }
    else {
        pSetup->enablebeeps = 1;
        enablebeeps=1;
    }

/*
    p = pctrScanSetupString(pcszSetupString, "MONITORZIP");
    if (p) {
        if (strcmp(p, "YES") == 0) {
            pSetup->monitorzip = 1;
            monitorzip=1;
        }
        else {
            pSetup->monitorzip = 0;
            monitorzip=0;
        }

        pctrFreeSetupValue(p);
    }
    else {
        pSetup->monitorzip = 1;
        monitorzip=1;
    }
*/


/*
    sprintf(buf, "Load setup, Beeps=%d", pSetup->enablebeeps);
    cmnLog(__FILE__, __LINE__, __FUNCTION__, buf);

    //
    p = pctrScanSetupString(pcszSetupString, "TIMEDIVIDER");
    if (p)
    {
        pSetup->timeDivider = atof(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->timeDivider = 1;
*/


/*
    p = pctrScanSetupString(pcszSetupString, "WHENCHARGED");
    if (p)
    {
        pSetup->WhenCharged = atoi(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->WhenCharged = 6;


    p = pctrScanSetupString(pcszSetupString, "WHENCHARGING");
    if (p)
    {
        pSetup->WhenCharging = atoi(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->WhenCharging = 2;

    p = pctrScanSetupString(pcszSetupString, "WHENDISCHARGING");
    if (p)
    {
        pSetup->WhenDischarging = atoi(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->WhenDischarging = 5;

*/

    cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- End\n\n");
}


//!: << MwgtSaveSetup >>
/**
 *@@ MwgtSaveSetup:
 *    composes a new setup string.
 *    The caller must invoke xstrClear on the
 *    string after use.
 *
 *@@added V0.9.7 (2000-12-04) [umoeller]
 */

        // out: setup string (is cleared first)
VOID MwgtSaveSetup(PXSTRING pstrSetup, PMONITORSETUP pSetup)
{
    CHAR    szTemp[100];
    PSZ  psz = 0;

    cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- Begin");

    pxstrInit(pstrSetup, 100);

    sprintf(szTemp, "BGNDCOL=%06lX;", pSetup->lcolBackground);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "TEXTCOL=%06lX;", pSetup->lcolForeground);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "LOWCOL=%06lX;", pSetup->lcolBackgroundLow);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "ACTEXTCOL=%06lX;", pSetup->lcolForegroundAC);
    pxstrcat(pstrSetup, szTemp, 0);

    /*
    sprintf(szTemp, "WHENCHARGED=%d;",
            pSetup->WhenCharged);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "WHENCHARGING=%d;",
            pSetup->WhenCharging);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "WHENDISCHARGING=%d;",
            pSetup->WhenDischarging);
    pxstrcat(pstrSetup, szTemp, 0);
    */


    if (pSetup->pszFont) {
        // non-default font:
        sprintf(szTemp, "FONT=%s;", pSetup->pszFont);
        pxstrcat(pstrSetup, szTemp, 0);
    }



    if (pSetup->displayIcon && pSetup->displayText) {
    }
    else {
        if (pSetup->displayIcon) {
            pxstrcat(pstrSetup, "DISPLAY=ICON;", 0);
        }
        else {
            pxstrcat(pstrSetup, "DISPLAY=TEXT;", 0);
        }
    }
//PS+++
    if (pSetup->displayPercentage == 2 ) {
        pxstrcat(pstrSetup, "STYLE=TIMEPERCENT;", 0);
    }
    else {
        if (pSetup->displayPercentage) {
            pxstrcat(pstrSetup, "STYLE=PERCENT;", 0);
        }
        else {
            pxstrcat(pstrSetup, "STYLE=TIME;", 0);
        }
    }
    if (pSetup->displayMinutes) {
        pxstrcat(pstrSetup, "TIMESTYLE=MINUTES;", 0);
    }
    else {
        pxstrcat(pstrSetup, "TIMESTYLE=HOURSMINUTES;", 0);
    }



    if (pSetup->debugInfo) {
        pxstrcat(pstrSetup, "DEBUG=YES;", 0);
    }

    if (enablebeeps) {
        pxstrcat(pstrSetup, "BEEPS=YES;", 0);
    }
    else {
        pxstrcat(pstrSetup, "BEEPS=NO;", 0);
    }


    if (pSetup->timeDivider != 1) {
        sprintf(szTemp, "TIMEDIVIDER=%.1f;", pSetup->timeDivider);
        pxstrcat(pstrSetup, szTemp, 0);
    }

    cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- End\n\n");

}

/* ******************************************************************
 *
 *  Widget settings dialog
 *
 ********************************************************************/

// None currently.

/* ******************************************************************
 *
 *  Callbacks stored in XCENTERWIDGETCLASS
 *
 ********************************************************************/

#define     WMXINT_SETUP    WM_USER+1805

//!: << fnwpSettingsDialog >> (Dialog Procedure for the Widget Settings)
/**
 *@@ fnwpSettingsDlg:
 *    dialog proc for the winlist settings dialog.
 */
MRESULT EXPENTRY fnwpSettingsDlg(HWND hwnd, ULONG msg0, MPARAM mp1, MPARAM mp2) {

    MRESULT     mrc = 0;
    static      PWIDGETSETTINGSDLGDATA pData;
    CHAR        szTemp[3 + 1];
    char        temp[200];
    int         x, y ,dw, dy, yspace, x0, y0, width, height;
    UCHAR       msg[128], buf[128];
    int         iCX, iCY;


    switch(msg0) {
        case WM_INITDLG:
            pData=(PWIDGETSETTINGSDLGDATA)mp2;
            WinPostMsg(hwnd, WMXINT_SETUP, (MPARAM)0, (MPARAM)0); // otherwise all auto(radio)controls are resetted??
        break;

        case WMXINT_SETUP:
        {
            // setup-string-handling
            PMONITORSETUP pSetup=(PMONITORSETUP)malloc(sizeof(MONITORSETUP));


            if(pSetup)
            {
                memset(pSetup, 0, sizeof(MONITORSETUP));
                // store this in WIDGETSETTINGSDLGDATA
                pData->pUser = pSetup;

                MwgtScanSetup(pData->pcszSetupString, pSetup);

                // make backup for Undo
                //memcpy(pSetup);


//              sprintf(temp, "just checked debuginfo box   displayIcon=%d, displaytext=%d", pSetup->displayIcon, pSetup->displayText);

                // set max.length of entryfield to 3
                WinSendDlgItemMsg(hwnd, ID_TIME_DIVIDER,
                                  EM_SETTEXTLIMIT,
                                  MPFROMSHORT(3),
                                  (MPARAM)0);
//              sprintf(temp, "just set text length       displayIcon=%d, displaytext=%d", pSetup->displayIcon, pSetup->displayText);

                sprintf(szTemp, "%.1f;", pSetup->timeDivider);
//              sprintf(temp, "just fiddled with timedivider displayIcon=%d, displaytext=%d", pSetup->displayIcon, pSetup->displayText);
                WinSetDlgItemText(hwnd, ID_TIME_DIVIDER, szTemp);
//              sprintf(temp, "just set timedivider field   displayIcon=%d, displaytext=%d", pSetup->displayIcon, pSetup->displayText);

                /*
//PS++
                if (pSetup->displayPercentage == 2 )
                {
                    mrc = WinSendDlgItemMsg(hwnd, ID_LIFE_TIME_PERCENTAGE,
                                      BM_SETCHECK,
                                      MPFROM2SHORT (TRUE, 0), NULL);
                }
                else
                if (pSetup->displayPercentage)
                {
                    mrc = WinSendDlgItemMsg(hwnd, ID_LIFE_PERCENTAGE,
                                      BM_SETCHECK,
                                      MPFROM2SHORT (TRUE, 0), NULL);
                }
                else
                {
                    mrc = WinSendDlgItemMsg(hwnd, ID_LIFE_TIME,
                                      BM_SETCHECK,
                                      MPFROM2SHORT (TRUE, 0), NULL);
                }

                if (pSetup->displayMinutes)
                {
                    mrc = WinSendDlgItemMsg(hwnd, ID_LIFE_MINUTES,
                                      BM_SETCHECK,
                                      MPFROM2SHORT (TRUE, 0), NULL);
                }
                else
                {
                    mrc = WinSendDlgItemMsg(hwnd, ID_LIFE_HOURSMINUTES,
                                      BM_SETCHECK,
                                      MPFROM2SHORT (TRUE, 0), NULL);
                }
                */



                /*
                if (pSetup->displayText && pSetup->displayIcon)
                {
                    WinSendDlgItemMsg(hwnd, ID_ICON_AND_TEXT,
                                      BM_CLICK,
                                      MPFROMSHORT(TRUE),
                                      (MPARAM)0);
                }
                else
                {
                    if(pSetup->displayIcon)
                    {
                        WinSendDlgItemMsg(hwnd, ID_ICON_ONLY,
                                         BM_CLICK,
                                         MPFROMSHORT(TRUE),
                                         (MPARAM)0);
                    }
                    else
                    {
                        WinSendDlgItemMsg(hwnd, ID_TEXT_ONLY,
                                         BM_CLICK,
                                         MPFROMSHORT(TRUE),
                                         (MPARAM)0);
                    }

                }
                */

            // x, y ,dw, dy



            internal_GetStaticTextSize(hwnd, ID_MAXSTRING, &iCX, &iCY);

            ULONG cxDesktop, cyDesktop, cxBorder, cyTitle, cyBorder;
            cxDesktop=WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
            cyDesktop=WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);
            cxBorder=WinQuerySysValue(HWND_DESKTOP, SV_CXSIZEBORDER);
            cyBorder=WinQuerySysValue(HWND_DESKTOP, SV_CYSIZEBORDER );

            cyTitle=WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR);


            yspace=10;
            x0=50, y0=16;
            iCY=iCY+4;

            width=x0*2+iCX+cxBorder*2;

            //
            // UNDO ?
            //
            x=x0+20; y=iCY;

            LangeGetString(BatteryLange, "Undo", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,UNDO_BUTTON), (char*)msg);
/*#
            WinSetWindowPos(WinWindowFromID(hwnd, UNDO_BUTTON),
                  HWND_TOP,
                  x,
                  y,
                  120, y+18,
                  SWP_MOVE | SWP_SIZE);
*/

            //LangeGetString(BatteryLange, "Cancel", (char*)msg, sizeof(msg));
            //WinSetWindowText (WinWindowFromID(hwnd,DID_CANCEL), (char*)msg);


            LangeGetString(BatteryLange, "Default", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,DEFAULT_BUTTON), (char*)msg);
/*#
            WinSetWindowPos(WinWindowFromID(hwnd, DEFAULT_BUTTON),
                  HWND_TOP,
                  x+140,
                  y,
                  120, y+18,
                  SWP_MOVE | SWP_SIZE);
*/

            LangeGetString(BatteryLange, "USB widget Settings", (char*)msg, sizeof(msg));
            WinSetWindowText(hwnd, (PCSZ) msg);

            LangeGetString(BatteryLange, "Close", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,CLOSE_BUTTON), (char*)msg);


            LangeGetString(BatteryLange, "You can report problems to:", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,ID_WIDGET_PROBLEMS), (char*)msg);

            LangeGetString(BatteryLange, "Get the latest version at:", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,ID_WIDGET_UPDATES), (char*)msg);

            ////////////
            //
            // [x] Beeps
            //
            ///////////

            y=y+18+4*yspace;
            x=x0+8;

            //
            LangeGetString(BatteryLange, "Enable Beeps on attach/eject", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,ID_BEEPS_CONNECT), (char*)msg);
/*#
            WinSetWindowPos(WinWindowFromID(hwnd, ID_BEEPS_CONNECT),
                  HWND_TOP,
                  x,
                  y,
                  iCX, iCY,
                  SWP_MOVE | SWP_SIZE);
*/
            WinCheckButton(hwnd, ID_BEEPS_CONNECT, enablebeeps);
            //        pSetup->enablebeeps=1;
            //        enablebeeps=1;
            //WinCheckButton(WinWindowFromID(hwnd, ID_BEEPS_CONNECT), ID_BEEPS_CONNECT, enablebeeps);


            WinCheckButton(hwnd, ID_MONITOR_ZIP, enablebeeps);

/*

            ////////////
            //
            // ┬é┬« ┬ó├á┬Ñ┬¼├» ├á┬á┬í┬«├ó├½ ┬«├ó ┬í┬á├ó┬á├á┬Ñ┬¿
            // Combo-box
            //
            ///////////

            y=y+iCY+2*yspace;
            x=x0+8;

            for(int a=0; a<=5; a++) {
                sprintf((char*)buf, "WhenDischarging%02d", a);
                LangeGetString(BatteryLange, (char*)buf, (char*)msg, sizeof(msg));

                WinSendDlgItemMsg(hwnd,ID_COMBO_DISCHARGING, LM_INSERTITEM,
                  MPFROMSHORT(LIT_END),MPFROMP(  msg  ));
            }

            WinSetWindowPos(WinWindowFromID(hwnd, ID_COMBO_DISCHARGING),
                  HWND_TOP,
                  x,
                  y-120,
                  iCX, iCY+128,
                  SWP_MOVE | SWP_SIZE);




            ////////////
            //
            // ┬é┬« ┬ó├á┬Ñ┬¼├» ├á┬á┬í┬«├ó├½ ┬«├ó ┬í┬á├ó┬á├á┬Ñ┬¿
            // Title
            //
            ///////////

            y=y+iCY+yspace;
            x=x0+8;

            LangeGetString(BatteryLange, "While battery is discharging", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,ID_COMBO_DISCHARGING_TITLE), (char*)msg);

            WinSetWindowPos(WinWindowFromID(hwnd, ID_COMBO_DISCHARGING_TITLE),
                  HWND_TOP,
                  x,
                  y,
                  iCX, iCY,
                  SWP_MOVE | SWP_SIZE);


            ////////////
            //
            // ┬Å┬«┬¬┬á ┬í┬á├ó┬á├á┬Ñ├» ┬º┬á├á├»┬ª┬á┬Ñ├ó├í├»
            // Combo-box
            //
            ///////////

            y=y+iCY+yspace;
            x=x0+8;

            for(int a=0; a<=5; a++) {
                sprintf((char*)buf, "WhenCharging%02d", a);
                LangeGetString(BatteryLange, (char*)buf, (char*)msg, sizeof(msg));

                WinSendDlgItemMsg(hwnd,ID_COMBO_CHARGING, LM_INSERTITEM,
                  MPFROMSHORT(LIT_END),MPFROMP(  msg  ));
            }


            WinSetWindowPos(WinWindowFromID(hwnd, ID_COMBO_CHARGING),
                  HWND_TOP,
                  x,
                  y-120,
                  iCX, iCY+128,
                  SWP_MOVE | SWP_SIZE);


            ////////////
            //
            // ┬Å┬«┬¬┬á ┬í┬á├ó┬á├á┬Ñ├» ┬º┬á├á├»┬ª┬á┬Ñ├ó├í├»
            // Title
            //
            ///////////

            y=y+iCY+yspace;
            x=x0+8;

            LangeGetString(BatteryLange, "While battery is charging", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,ID_COMBO_CHARGING_TITLE), (char*)msg);

            WinSetWindowPos(WinWindowFromID(hwnd, ID_COMBO_CHARGING_TITLE),
                  HWND_TOP,
                  x,
                  y,
                  iCX, iCY,
                  SWP_MOVE | SWP_SIZE);


            ////////////
            //
            // ┬è┬«┬ú┬ñ┬á ┬º┬á├á├»┬ª┬Ñ┬¡┬á, ┬»┬¿├ó┬á┬¡┬¿┬Ñ ┬«├ó ├í┬Ñ├ó┬¿
            // Combo-box
            //
            ///////////

            y=y+iCY+yspace;
            x=x0+8;

            for(int a=0; a<=6; a++) {
                sprintf((char*)buf, "WhenCharged%02d", a);
                LangeGetString(BatteryLange, (char*)buf, (char*)msg, sizeof(msg));

                WinSendDlgItemMsg(hwnd,ID_COMBO_CHARGED, LM_INSERTITEM,
                  MPFROMSHORT(LIT_END),MPFROMP(  msg  ));
            }


            WinSetWindowPos(WinWindowFromID(hwnd, ID_COMBO_CHARGED),
                  HWND_TOP,
                  x,
                  y-120,
                  iCX, iCY+128,
                  SWP_MOVE | SWP_SIZE);


            ////////////
            //
            // ┬è┬«┬ú┬ñ┬á ┬º┬á├á├»┬ª┬Ñ┬¡┬á, ┬»┬¿├ó┬á┬¡┬¿┬Ñ ┬«├ó ├í┬Ñ├ó┬¿
            // Title
            //
            ///////////

            y=y+iCY+yspace;
            x=x0+8;

            LangeGetString(BatteryLange, "When battery is charged", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,ID_COMBO_CHARGED_TITLE), (char*)msg);

            WinSetWindowPos(WinWindowFromID(hwnd, ID_COMBO_CHARGED_TITLE),
                  HWND_TOP,
                  x,
                  y,
                  iCX, iCY,
                  SWP_MOVE | SWP_SIZE);




            ////////////////////////////////
*/
            y=iCY+18+4*yspace;;
            //y=y+iCY+2*yspace;
            x=x0+8;
            //x=x0+20;


            /*
            WinSetWindowPos(WinWindowFromID(hwnd, ENTRY_USERNAME),
                  HWND_TOP,
                  x,
                  y,
                  iCX, iCY,
                  SWP_MOVE | SWP_SIZE);
            WinSendMsg(WinWindowFromID(hwnd, ENTRY_USERNAME),EM_SETTEXTLIMIT,(MPARAM)64,(MPARAM)0);
            WinSetWindowText(WinWindowFromID(hwnd, ENTRY_USERNAME), pSetup->pszUserName);
                  */


            y=y+iCY+yspace;
            x=x0+8;

            LangeGetString(BatteryLange, "CRC working directory", (char*)msg, sizeof(msg));
            WinSetWindowText (WinWindowFromID(hwnd,ID_COMBO_CHARGING_TITLE), (char*)msg);

            WinSetWindowPos(WinWindowFromID(hwnd, ID_COMBO_CHARGING_TITLE),
                  HWND_TOP,
                  x,
                  y,
                  iCX, iCY,
                  SWP_MOVE | SWP_SIZE);


            y=y+iCY+2*yspace;
            x=x0+8;

/*#
            WinSetWindowPos(WinWindowFromID(hwnd, ENTRY_NEOWPS),
                  HWND_TOP,
                  x,
                  y,
                  iCX+20, iCY,      // Rousseau: increased length
                  SWP_MOVE | SWP_SIZE);
*/
            // Rousseau: changed from 64 to 72
            WinSendMsg(WinWindowFromID(hwnd, ENTRY_NEOWPS),EM_SETTEXTLIMIT,(MPARAM)72,(MPARAM)0);
//#         WinSendMsg(WinWindowFromID(hwnd, ENTRY_NEOWPS),EM_SETFIRSTCHAR,(MPARAM)1,(MPARAM)0);
            WinSetWindowText(WinWindowFromID(hwnd, ENTRY_NEOWPS), "NeoWPS: http://ecomstation.ru/neowps");


            y=y+iCY;
            x=x0+8;
/*#
            WinSetWindowPos(WinWindowFromID(hwnd, ENTRY_TITLE),
                  HWND_TOP,
                  x,
                  y,
                  iCX+20, iCY,      // Rousseau increased length
                  SWP_MOVE | SWP_SIZE);
*/


            // Rousseau: Moved widget-name-string to Dialogids.h (WIDGET_NAME)
            WinSendMsg(WinWindowFromID(hwnd, ENTRY_TITLE),EM_SETTEXTLIMIT,(MPARAM)72,(MPARAM)0);
//#         WinSendMsg(WinWindowFromID(hwnd, ENTRY_TITLE),EM_SETFIRSTCHAR,(MPARAM)10,(MPARAM)0);
//#         WinSetWindowText(WinWindowFromID(hwnd, ENTRY_TITLE), WIDGET_NAME" == TEST VERSIE #1 ==");


            height=y+iCY+yspace+cyTitle+2*cyBorder+16;


                // Set the version number
                WinSetDlgItemText(hwnd, ID_VERSION_TEXT, WIDGET_VERSION);


            ///////////////////////


            LangeGetString(BatteryLange, "USB widget Settings", (char*)msg, sizeof(msg));
            WinSetWindowText (hwnd, (char*)msg);


            //WinCheckButton(hwnd, ID_SHOW_DEBUG_INFO, pSetup->debugInfo);
            //WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGED, LM_SELECTITEM, MPARAM(pSetup->WhenCharged),MPARAM(1));
            //WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGING, LM_SELECTITEM, MPARAM(pSetup->WhenCharging),MPARAM(1));
            //WinSendDlgItemMsg(hwnd, ID_COMBO_DISCHARGING, LM_SELECTITEM, MPARAM(pSetup->WhenDischarging),MPARAM(1));


            //

            SWP swp;
             WinQueryWindowPos( hwnd, (PSWP)&swp );

            POINTL ptl;
            int direction, W, H;
            WinQueryPointerPos(HWND_DESKTOP, &ptl);

/*#
                if(ptl.y>=cyDesktop/2) {
                  if(ptl.x>=cxDesktop/2) {
                     direction=1;
                     // ├í┬»├á┬á┬ó┬á ┬ó┬ó┬Ñ├á├Ñ├ú
                  }
                  else {
                     direction=0;
                     // ├í┬½┬Ñ┬ó┬á ┬ó┬ó┬Ñ├á├Ñ├ú
                  }
                }
                else {
                  if(ptl.x>cxDesktop/2) {
                     direction=3;
                  }
                  else {
                     direction=2;
                  }
                }

                switch(direction) {
                  case 0: H=-height-20; W=0; break;
                  case 1: H=-height-20; W=-width; break;
                  case 2: H=0; W=0; break;
                  case 3: H=0; W=-width; break;
                }
*/

/*#
                WinSetWindowPos( hwnd, HWND_TOP, ptl.x+W, ptl.y+H, width, height,
                     SWP_SHOW | SWP_MOVE | SWP_SIZE | SWP_ZORDER);
*/


                // Rousseau: position dialog -- leaving width and height as defined in the rc-file and just adjusting the position
                {
                    ptl.x = (ptl.x+swp.cx > cxDesktop) ? ptl.x - swp.cx : ptl.x;
                    ptl.y = (ptl.y+swp.cy > cyDesktop) ? ptl.y - swp.cy : ptl.y;
                    WinSetWindowPos( hwnd, HWND_TOP, ptl.x, ptl.y, swp.cx, swp.cy, SWP_SHOW | SWP_MOVE | SWP_SIZE | SWP_ZORDER);
                }

            }
        break; }

        case WM_DESTROY:
        {

            if(pData)
            {

                    LONG    index;
                    XSTRING strSetup;
                    PMONITORSETUP pSetup=(PMONITORSETUP)pData->pUser;
                    // 'store' settings in pSetup
                    index = (long)WinSendDlgItemMsg(hwnd, ID_ICON_AND_TEXT,
                                                    BM_QUERYCHECKINDEX,
                                                    (MPARAM)0,
                                                    (MPARAM)0);

                    pSetup->displayIcon = (index == 0 || index == 1);
                    pSetup->displayText = (index == 0 || index == 2);

//                  sprintf(temp, "OK Pressed       index = %d, displayIcon=%d, displaytext=%d, displayPercentage=%d", index, pSetup->displayIcon, pSetup->displayText, pSetup->displayPercentage);
//                  __debugg(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

                    index = (long)WinSendDlgItemMsg(hwnd, ID_LIFE_PERCENTAGE,
                                                      BM_QUERYCHECKINDEX,
                                                      (MPARAM)0,
                                                      (MPARAM)0);
                    pSetup->displayPercentage = (index == 0);


                    //sprintf(temp, "got percentage index = %d, displayIcon=%d, displaytext=%d, displayPercentage=%d", index, pSetup->displayIcon, pSetup->displayText, pSetup->displayPercentage);
                    //__debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

//PS+++
                    if ( index == 2 )
                        {
                        pSetup->displayPercentage = 2;
                        }

                    index = (long)WinSendDlgItemMsg(hwnd, ID_LIFE_MINUTES,
                                                      BM_QUERYCHECKINDEX,
                                                      (MPARAM)0,
                                                      (MPARAM)0);
                    pSetup->displayMinutes = (index == 0);


                    pSetup->WhenCharged=(ULONG)WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGED, LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), (MPARAM)NULL);
                    pSetup->WhenCharging=(ULONG)WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGING, LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), (MPARAM)NULL);
                    pSetup->WhenDischarging=(ULONG)WinSendDlgItemMsg(hwnd, ID_COMBO_DISCHARGING, LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), (MPARAM)NULL);


                    //sprintf(temp, "got minutes        index = %d, displayIcon=%d, displaytext=%d, displayMinutes=%d", index, pSetup->displayIcon, pSetup->displayText, pSetup->displayMinutes);
                    //__debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

                    pSetup->debugInfo = WinQueryButtonCheckstate(hwnd, ID_SHOW_DEBUG_INFO);

                    WinQueryDlgItemText(hwnd, ID_TIME_DIVIDER, 4, (PSZ)szTemp);
                    if ((pSetup->timeDivider = atof(szTemp)) == 0)
                        pSetup->timeDivider = 1;

                    // something has changed:
                    MwgtSaveSetup(&strSetup, pSetup);
                    pData->pctrSetSetupString(pData->hSettings, strSetup.psz);
                    pxstrClear(&strSetup);


                if(pSetup)
                {

                    WgtClearSetup(pSetup);
                    free(pSetup);
                } // end if (pSetup)
             } // end if (pData)
        break; }


        case WM_CONTROL:
        {
            if(pData && SHORT2FROMMP(mp1)==BN_CLICKED)
            {
                switch (SHORT1FROMMP(mp1)) {
                    case ID_BEEPS_CONNECT: {
                        PMONITORSETUP pSetup=(PMONITORSETUP)pData->pUser;

                        enablebeeps = WinQueryButtonCheckstate(hwnd, ID_BEEPS_CONNECT);
                        pSetup->enablebeeps=enablebeeps;


                        sprintf((char*)buf, "Setup: enablebeeps=%d", enablebeeps);
                        __debug(__FUNCTION__, (char*)buf, DBG_MLE | DBG_AUX);

                        // disable groupbox+children
                        //WinEnableWindow(WinWindowFromID(hwnd, ID_LIFE_PERCENTAGE),    FALSE);
                        //WinEnableWindow(WinWindowFromID(hwnd, ID_LIFE_TIME),        FALSE);

                    }

                    case ID_MONITOR_ZIP: {
                        PMONITORSETUP pSetup=(PMONITORSETUP)pData->pUser;

                        monitorzip = WinQueryButtonCheckstate(hwnd, ID_MONITOR_ZIP);
                        pSetup->monitorzip=monitorzip;

                        sprintf((char*)buf, "Setup: monitorzip=%d", monitorzip);
                        __debug(__FUNCTION__, (char*)buf, DBG_MLE | DBG_AUX);

                    }

                }






                if(SHORT1FROMMP(mp1) == ID_BEEPS_CONNECT)               // Icon only
                {
                    PMONITORSETUP pSetup=(PMONITORSETUP)pData->pUser;

                    enablebeeps = WinQueryButtonCheckstate(hwnd, ID_BEEPS_CONNECT);
                    pSetup->enablebeeps=enablebeeps;


                    sprintf((char*)buf, "Setup: enablebeeps=%d", enablebeeps);
                    __debug(__FUNCTION__, (char*)buf, DBG_MLE | DBG_AUX);

                    // disable groupbox+children
                    //WinEnableWindow(WinWindowFromID(hwnd, ID_LIFE_PERCENTAGE),    FALSE);
                    //WinEnableWindow(WinWindowFromID(hwnd, ID_LIFE_TIME),        FALSE);
                }

                /*
                else if ((SHORT1FROMMP(mp1) == ID_TEXT_ONLY)        // Text only
                        || (SHORT1FROMMP(mp1) == ID_ICON_AND_TEXT)) // Icon and Text
                {
                    WinEnableWindow(WinWindowFromID(hwnd, ID_LIFE_PERCENTAGE),  TRUE);
                    WinEnableWindow(WinWindowFromID(hwnd, ID_LIFE_TIME),          TRUE);
                }
                */
            }
        break; }


        case WM_COMMAND:
        {


            switch(SHORT1FROMMP(mp1))
            {


                case UNDO_BUTTON: // ok-button
                {
                    // WinDismissDlg(hwnd, TRUE);

                    if(pData) {
                      PMONITORSETUP pSetup=(PMONITORSETUP)pData->pUser;

                      enablebeeps=prev_enablebeeps;
                      pSetup->enablebeeps=enablebeeps;
                      WinCheckButton(hwnd, ID_BEEPS_CONNECT, pSetup->enablebeeps);

                      //WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGED, LM_SELECTITEM, MPARAM(pSetup->WhenCharged),MPARAM(1));
                      //WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGING, LM_SELECTITEM, MPARAM(pSetup->WhenCharging),MPARAM(1));
                      //WinSendDlgItemMsg(hwnd, ID_COMBO_DISCHARGING, LM_SELECTITEM, MPARAM(pSetup->WhenDischarging),MPARAM(1));
                    }

                    break;
                }


                case DEFAULT_BUTTON: // ok-button
                {
                    if(pData) {
                      PMONITORSETUP pSetup=(PMONITORSETUP)pData->pUser;

                      pSetup->debugInfo=0;
                      pSetup->WhenCharged = 6;
                      pSetup->WhenCharging = 2;
                      pSetup->WhenDischarging = 5;


                      /*
                      WinCheckButton(hwnd, ID_SHOW_DEBUG_INFO, pSetup->debugInfo);
                      WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGED, LM_SELECTITEM, MPARAM(pSetup->WhenCharged),MPARAM(1));
                      WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGING, LM_SELECTITEM, MPARAM(pSetup->WhenCharging),MPARAM(1));
                      WinSendDlgItemMsg(hwnd, ID_COMBO_DISCHARGING, LM_SELECTITEM, MPARAM(pSetup->WhenDischarging),MPARAM(1));
                      */

                      // Beeps
                      pSetup->enablebeeps=1;
                      enablebeeps=1;
                      WinCheckButton(hwnd, ID_BEEPS_CONNECT, enablebeeps);

                    }

                    break;

                }

                case ID_FF_NEOWPS: {
                    //StartFireFoxAsProgram("http://en.ecomstation.ru/projects/neowps");
                    StartFireFoxAsSession("http://rousseaux.github.io/netlabs.xwpwidgets");
                    WinPostMsg(hwnd, WM_COMMAND, (MPARAM) CLOSE_BUTTON, (MPARAM) NULL);
                }
                break;

                case ID_FF_BENSBITS: {
                    //StartFireFoxAsProgram("http://www.bensbits.nl");
                    StartFireFoxAsSession("http://github.com/rousseaux/netlabs.xwpwidgets/releases");
                    WinPostMsg(hwnd, WM_COMMAND, (MPARAM) CLOSE_BUTTON, (MPARAM) NULL);
                }
                break;


                case CLOSE_BUTTON:
                    WinDismissDlg(hwnd, TRUE);
                    break;

/*
                // NOT USED
                case DID_OK: // ok-button
                {
                    LONG    index;
                    XSTRING strSetup;
                    PMONITORSETUP pSetup=(PMONITORSETUP)pData->pUser;
                    // 'store' settings in pSetup
                    index = (long)WinSendDlgItemMsg(hwnd, ID_ICON_AND_TEXT,
                                                    BM_QUERYCHECKINDEX,
                                                    (MPARAM)0,
                                                    (MPARAM)0);
                    pSetup->displayIcon = (index == 0 || index == 1);
                    pSetup->displayText = (index == 0 || index == 2);

                    index = (long)WinSendDlgItemMsg(hwnd, ID_LIFE_PERCENTAGE,
                                                      BM_QUERYCHECKINDEX,
                                                      (MPARAM)0,
                                                      (MPARAM)0);
                    pSetup->displayPercentage = (index == 0);

                    sprintf(temp, "got percentage   index = %d, displayIcon=%d, displaytext=%d, displayPercentage=%d", index, pSetup->displayIcon, pSetup->displayText, pSetup->displayPercentage);
//PS+++
                    if ( index == 2 )
                        {
                        pSetup->displayPercentage = 2;
                        }

                    index = (long)WinSendDlgItemMsg(hwnd, ID_LIFE_MINUTES,
                                                      BM_QUERYCHECKINDEX,
                                                      (MPARAM)0,
                                                      (MPARAM)0);
                    pSetup->displayMinutes = (index == 0);

                    pSetup->WhenCharged=(ULONG)WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGED, LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), (MPARAM)NULL);
                    pSetup->WhenCharging=(ULONG)WinSendDlgItemMsg(hwnd, ID_COMBO_CHARGING, LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), (MPARAM)NULL);
                    pSetup->WhenDischarging=(ULONG)WinSendDlgItemMsg(hwnd, ID_COMBO_DISCHARGING, LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), (MPARAM)NULL);

                    sprintf(temp, "got minutes      index = %d, displayIcon=%d, displaytext=%d, displayMinutes=%d", index, pSetup->displayIcon, pSetup->displayText, pSetup->displayMinutes);
                    cmnLog(__FILE__, __LINE__, __FUNCTION__, temp);

                    pSetup->debugInfo = WinQueryButtonCheckstate(hwnd, ID_SHOW_DEBUG_INFO);

                    WinQueryDlgItemText(hwnd, ID_TIME_DIVIDER, 4, (PSZ)szTemp);
                    if ((pSetup->timeDivider = atof(szTemp)) == 0)
                        pSetup->timeDivider = 1;
                    // something has changed:
                    MwgtSaveSetup(&strSetup, pSetup);
                    pData->pctrSetSetupString(pData->hSettings, strSetup.psz);
                    pxstrClear(&strSetup);

                    WinDismissDlg(hwnd, TRUE);

                break; }

                case DID_CANCEL: // Cancel button
                {
                    WinDismissDlg(hwnd, TRUE);
                break; }
*/

            }
        break; }

        default:
            mrc=WinDefDlgProc(hwnd, msg0, mp1, mp2);
        break;
    }

    return(mrc);
}



static void internal_GetStaticTextSize(HWND hwnd, ULONG ulID, int *piCX, int *piCY) {
    HPS     hps;
    char    achTemp[512];
    POINTL  aptl[TXTBOX_COUNT];

    WinQueryDlgItemText(hwnd, ulID, sizeof(achTemp), achTemp);

    hps = WinGetPS(WinWindowFromID(hwnd, ulID));

    GpiQueryTextBox(
        hps,
        strlen(achTemp),
        achTemp,
        TXTBOX_COUNT,
        aptl
    );

    WinReleasePS(hps);

    *piCX = aptl[TXTBOX_TOPRIGHT].x - aptl[TXTBOX_BOTTOMLEFT].x;
    *piCY = aptl[TXTBOX_TOPRIGHT].y - aptl[TXTBOX_BOTTOMLEFT].y;

    return;
}


//!: << WgtShowSettingsDialog >>
/**
 *@@ WwgtShowSettingsDlg:
 *    this displays the winlist widget's settings
 *    dialog.
 *
 *    This procedure's address is stored in
 *    XCENTERWIDGET so that the XCenter knows that
 *    we can do this.
 *
 *    When calling this function, the XCenter expects
 *    it to display a modal dialog and not return
 *    until the dialog is destroyed. While the dialog
 *    is displaying, it would be nice to have the
 *    widget dynamically update itself.
 */
VOID EXPENTRY WwgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData)
{
    HWND hwnd;

    /* Load the Settings Dialog */
    hwnd =  WinLoadDlg(
                HWND_DESKTOP,           // parent
                pData->hwndOwner,       // owner
                fnwpSettingsDlg,        // dlgproc
                hmodMe,                 // handle to module containing dialog
                ID_USB_WIDGET_DIALOG,   // id of dialog
                (PVOID)pData            // pass setup string with WM_INITDLG
            );

    /* Backup current value incase of undo */
    prev_enablebeeps = enablebeeps;

    /* Process the Settings Dialog and destroy it afterwards */
    if (hwnd) {
        //~ pcmnSetControlsFont(hwnd, 1, 10000);
        //~ pwinhCenterWindow(hwnd);         // invisibly
        WinProcessDlg(hwnd);
        WinDestroyWindow(hwnd);
    }
}


/* ******************************************************************
 *
 *  PM window class implementation
 *
 ********************************************************************/

/*
 *    This code has the actual PM window class.
 *
 */

//!: << MwgtCreate >> (The Widget is created here)
/**
 * This function creates the Widget.
 * It receives a window-handle and and a pointer to a widget-structure.
 * Note that the window is already created and the task of this function is
 * to create the widget itself.
 */
/**
 *@@ MwgtCreate:
 *    implementation for WM_CREATE.
 */
MRESULT MwgtCreate(HWND hwnd, PXCENTERWIDGET pWidget)
{
    MRESULT     mrc             = 0;            // continue window creation
    BOOL        brc             = FALSE;
    PSZ         p               = NULL;
    CHAR        szHelpPath[400] = "";
    CHAR        szHelpFile[400] = "";
    ULONG       pathLength      = 399;
    ULONG       rc              = 0;
    char        msg[256]        = "\0";
    APIRET      ulrc            = 0;
    ULONG       ulAction        = -1;
    ULONG       ulRead          = 0;
    ULONG       ulWrite         = 0;
    ULONG       ulReply         = 0;
    char        buf[256]        = "\0";




    /*
    g_myUSBWidget->startHandlingEvents();
    */

    /* Change the owner from HWND_DESKTOP to the widget-window */
    //brc = WinSetOwner(hdlgDebugDialog, hwnd);



    __debug(NULL, "MwgtCreate::START", DBG_LBOX);




    __debug(NULL, "Loading Resources...", DBG_LBOX);

    /*
    // Rousseau: Moved the language resource loading from WgtInitModule() to here.
    // Now delete widget and then recreate reloads the language resource.
    */
    if (DosScanEnv(OSDIR_VAR_NAME, (const char**)&pszOSDirectory))
        pszOSDirectory=NULL;

    /*
    // Load Language.
    */
    strcpy(szNlsPath, pszOSDirectory);
    strcat(szNlsPath, "\\LANG\\usbshold.nls");
    LangeLoadLanguage(szNlsPath, 0, &BatteryLange);

    PMONITORPRIVATE pPrivate = (MONITORPRIVATE *)malloc(sizeof(MONITORPRIVATE));
    memset(pPrivate, 0, sizeof(MONITORPRIVATE));

    /* link the two together */
    pWidget->pUser      = pPrivate;
    pPrivate->pWidget   = pWidget;
    MyWidget            = pWidget;
    hwndWidget          = hwnd;

    /* Get the dimensions of the Desktop */
    cxDesktop = WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN);
    cyDesktop = WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN);

    /* Get lang-strings */
    LangeGetString(BatteryLange, "Rediscover PRM", (char*)msg, sizeof(msg));
    //sprintf(msg, "%s\n%s",(char *)msg, "http://ecomstation.ru/neowps");

    /* Load the popup-menu */
    hwndPopup = WinLoadMenu(HWND_DESKTOP, hmodMe, ID_POP_MENU);

    /* Insert the rediscover-item */
    WinSetMenuItemText(hwndPopup, ID_MENU_REDISCOVER, msg);
    // PgmMsg[2]

    // get widget type (clock, memory, ...) from class setting;
    // this is lost after WM_CREATE
    if (pWidget->pWidgetClass)
        pPrivate->ulType = pWidget->pWidgetClass->ulExtra;

    pPrivate->cxCurrent = 10;         // we'll resize ourselves later
    pPrivate->cyCurrent = 10;

    // initialize binary setup structure from setup string
    MwgtScanSetup(pWidget->pcszSetupString, &pPrivate->Setup);

    // set window font (this affects all the cached presentation
    // spaces we use)
    pwinhSetWindowFont(hwnd,
                        (pPrivate->Setup.pszFont)
                        ? pPrivate->Setup.pszFont
                        // default font: use the same as in the rest of XWorkplace:
                        : pcmnQueryDefaultFont());

    pPrivate->ulMiniIconSize = WinQuerySysValue(HWND_DESKTOP, SV_CXICON) / 2;

    /* Load the icons for in the menu-items */
    hptrEject       = WinLoadPointer(HWND_DESKTOP, hmodMe, ID_ICO_EJECT);
    hptrCD          = WinLoadPointer(HWND_DESKTOP, hmodMe, ID_ICO_CD);
    hptrHDD         = WinLoadPointer(HWND_DESKTOP, hmodMe, ID_ICO_HDD);
    hptrHDDBAD      = WinLoadPointer(HWND_DESKTOP, hmodMe, ID_ICO_HDDBAD);
    hptrHDDPHANTOM  = WinLoadPointer(HWND_DESKTOP, hmodMe, ID_ICO_HDDPHANTOM);
    hptrZIP         = WinLoadPointer(HWND_DESKTOP, hmodMe, ID_ICO_ZIP);


    /*
    // Hier wordt het pictogram voor de widget-knop geladen.
    // Het formaat is .ICO
    */
    pPrivate->hptrNoBattery  = WinLoadPointer(HWND_DESKTOP,
                                                hmodMe,
                                                ID_ICO_USB_WIDGET);


    /* This registers the class for the notification windows */
    WinRegisterClass( pWidget->pGlobals->hab, szIndiClass, fnwpIndiClient, CS_SIZEREDRAW, 0 );

    ///
    //!: -------------------------------------- [ CREATE NOTIFICATION WINDOWS ]
    ///

    /**
     * The various Notification Windows are created here.
     */

    //!: ------------------------------------ [ Insertion Notification Window ]
    hwndIndiDevInsert =     WinCreateWindow(
                                HWND_DESKTOP,       // parent
                                szIndiClass,        // class
                                "USB widget indicator (device insert)", // text
                                NULL,               // styleflags
                                200,                // x-pos
                                200,                // y-pos
                                200,                // width
                                200,                // height
                                hwnd,               // owner
                                HWND_TOP,           // z-order
                                ID_INDI_DEV_INSERT, // id
                                NULL,               // control-data
                                NULL                // pres-params
                            );

    /* Set Presentation Parameters */
    WinSetPresParam(
        hwndIndiDevInsert,
        PP_FONTNAMESIZE,
        strlen("9.Warpsans") + 1,
        "9.Warpsans"
    );



    //!: ----------------------------------- [ Safe Eject Notification Window ]
    hwndIndiSafeEject =     WinCreateWindow(
                                HWND_DESKTOP,       // parent
                                szIndiClass,        // class
                                "USB widget indicator (safe eject)",    // text
                                NULL,               // styleflags
                                200,                // x-pos
                                200,                // y-pos
                                200,                // width
                                200,                // height
                                hwnd,               // owner
                                HWND_TOP,           // z-order
                                ID_INDI_DEV_INSERT, // id
                                NULL,               // control-data
                                NULL                // pres-params
                            );

    /* Set Presentation Parameters */
    WinSetPresParam(
        hwndIndiSafeEject,
        PP_FONTNAMESIZE,
        strlen("9.Warpsans") + 1,
        "9.Warpsans"
    );



    //!: ------------------------ [ Forced Device Removal Notification Window ]
    hwndIndiDevRemove =     WinCreateWindow(
                                HWND_DESKTOP,       // parent
                                szIndiClass,        // class
                                "USB widget indicator (force remove)",  // text
                                NULL,               // styleflags
                                200,                // x-pos
                                200,                // y-pos
                                200,                // width
                                200,                // height
                                hwnd,               // owner
                                HWND_TOP,           // z-order
                                ID_INDI_DEV_INSERT, // id
                                NULL,               // control-data
                                NULL                // pres-params
                            );

    /* Set Presentation Parameters */
    WinSetPresParam(
        hwndIndiDevRemove,
        PP_FONTNAMESIZE,
        strlen("9.Warpsans") + 1,
        "9.Warpsans"
    );


    //!: ------------------------------- [ Drives Changed Notification Window ]
    hwndIndiDrivesChanged = WinCreateWindow(
                                HWND_DESKTOP,       // parent
                                szIndiClass,        // class
                                "USB widget indicator (drives changed)",    // text
                                NULL,               // styleflags
                                200,                // x-pos
                                200,                // y-pos
                                200,                // width
                                200,                // height
                                hwnd,               // owner
                                HWND_TOP,           // z-order
                                ID_INDI_DEV_INSERT, // id
                                NULL,               // control-data
                                NULL                // pres-params
                            );

    /* Set Presentation Parameters */
    WinSetPresParam(
        hwndIndiDrivesChanged,
        PP_FONTNAMESIZE,
        strlen("9.Warpsans") + 1,
        "9.Warpsans"
    );

/*
      int xpos, ypos, width, height;
      xpos=100;
      ypos=100;


      width=128;
      height=128;

      WinSetWindowPos( hwndIndiDevInsert, HWND_TOP, xpos, ypos, width, height,
                     SWP_ZORDER | SWP_ACTIVATE  | SWP_SHOW |SWP_SIZE|SWP_MOVE);
*/


    //WinSetPresParam(hwndIndiDevInsert,PP_FONTNAMESIZE,strlen("18.Helv.Bold")+1,"18.Helv.Bold");








    //!: ----------------------------------------------------- [ Load Bitmaps ]
    HPS     hps = WinGetPS(hwnd);
    hbmUSBThumb=GpiLoadBitmap(hps, hmodMe, USB_THUMB, 128, 128);
    hbmIconInfo=GpiLoadBitmap(hps, hmodMe, ICON_INFO, 64, 64);
    hbmIconWarning=GpiLoadBitmap(hps, hmodMe, ICON_WARNING, 64, 64);
    WinReleasePS(hps);

    /* Load the png icons */


    // enable context menu help
//  pWidget->pcszHelpLibrary = pcmnQueryHelpLibrary();
//  rc = DosQueryCurrentDir(0, szHelpPath, &pathLength);
//  if (rc != NO_ERROR)
//  sprintf(szHelpFile, "\\%s\\%s", szHelpPath, "plugins\\xcenter\\Battery.HLP", 0);
//  pWidget->pcszHelpLibrary = szHelpFile;

//  pWidget->ulHelpPanelID = 1;

    // start update timer

    /*
    pPrivate->ulTimerID = ptmrStartXTimer((_XTIMERSET *)pWidget->pGlobals->pvXTimerSet,
                                          hwnd,
                                          1,
                                          2000);

    */
    //pPrivate->ulTimerID;

    //DosBeep(1500,1500);

    /*
    // Empty the list of current devices.
    */
    //~ __debug(__FUNCTION__, "CleanUSBDevicesList", DBG_LBOX || DBG_AUX);
    //~ CleanUSBDevicesList(pws->currentUSBDevicesList);

    /*
    // (Re)Create the list of current devices.
    // Note that some previously gathered data on the device, like the drivemap, gets lost here.
    // That data will be restored in the compare-function below if the device is still present.
    */
    __debug(__FUNCTION__, "QueryUSBList", DBG_LBOX);
    //~ QueryUSBList();

    __debug(__FUNCTION__, "CompareUSBDevicesList", DBG_LBOX);
    //~ CompareUSBDevicesList();

    __debug(NULL, "OK.", DBG_LBOX);




    //!------------------------------------------------------------------------
    //!
    //!     See if any drives are removed since the last Widget session
    //!
    //! The Widget periodically stores the current drivemap in the Widget
    //! Structure in Shared Memory allocated by the Daemon. It also stores the
    //! drivemap when terminating. On a restart of the Widget, the user could
    //! have ejected a device using the command line.
    //!
    //! In case the device is still in it's socket, the drive is unmounted.
    //! In that case we set the safe_eject flag on the device.
    //!
    //! The user can also have removed the device.
    //! Then we
    //!
    //!------------------------------------------------------------------------
    {
        /* Locals */
        ULONG   drives_before   = 0;
        ULONG   drives_after    = 0;
        ULONG   drives_changed  = 0;
        ULONG   drives_new      = 0;
        ULONG   drives_gone     = 0;
        ULONG   drivecount      = 0;

        /* Get drivemap from previous widget session */
        drives_before = pws->drivemap;

        /* Get the current drivemap */
        drives_after = drive_map_dqcd();

        /* Determine all changes */
        drive_changes(
            drives_before,
            drives_after,
            &drives_changed,
            &drives_new,
            &drives_gone
        );

        /* Set the safe_eject flag on the device if it is still present */
        if (drives_gone) {
            int i;          // Bit index, start at 3 ('D')
            int m = 4;      // Mask, start with 'D'
            USBDeviceReport* usbdev = null;

            /* Iterate over the drivemap */
            for (i=3; i<26; i++) {

                /* If the bit is set, get the device and set the safe_eject flag */
                if (drives_gone & m) {
                    usbdev = GetUsbDeviceByMap(pws->currentUSBDevicesList, drives_gone & m);
                    if (usbdev) usbdev->safe_eject = true;
                }

                /* Next drive-bit */
                m <<= 1;
            }
        }
    }



    //!: ------------------------------------------------ [ START THE THREADS ]

    /*
    // Start all the loop-threads.
    // Such threads test a loop-flag and if true continue the loop.
    // Most block on a semaphore with or without a timeout.
    // So, to end a thread is to clear the loop-flag and post it's semaphore.
    */
    StartThreads();
    __debug(NULL, "MwgtCreate::END", DBG_LBOX);

    return (mrc);
}

//!: << MwgtControl >>
/**
 *@@ MwgtControl:
 *    implementation for WM_CONTROL in fnwpSampleWidget.
 *
 *    The XCenter communicates with widgets thru
 *    WM_CONTROL messages. At the very least, the
 *    widget should respond to XN_QUERYSIZE because
 *    otherwise it will be given some dumb default
 *    size.
 *
 *@@added V0.9.7 (2000-12-14) [umoeller]
 */
BOOL MwgtControl(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    BOOL brc = FALSE;
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    UCHAR   msg[128];

    if (pWidget)
    {

      PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)pWidget->pUser;
      USHORT  usID       = SHORT1FROMMP(mp1);
      USHORT  usNotifyCode = SHORT2FROMMP(mp1);

      // is this from the XCenter client?
      if (usID == ID_XCENTER_CLIENT)
      {
          switch (usNotifyCode)
          {
              /*
                * XN_QUERYSIZE:
                *     XCenter wants to know our size.
                */

              case XN_QUERYSIZE:
              {
                  PSIZEL pszl = (PSIZEL)mp2;
                  pszl->cx = pPrivate->cxCurrent;
                  pszl->cy = pPrivate->cyCurrent;
                  brc = TRUE;
              break; }

              /*
                * XN_SETUPCHANGED:
                *     XCenter has a new setup string for
                *     us in mp2.
                */

              case XN_SETUPCHANGED:
              {
                  const char *pcszNewSetupString = (const char*)mp2;

                  // reinitialize the setup data
                  WgtClearSetup(&pPrivate->Setup);
                  MwgtScanSetup(pcszNewSetupString, &pPrivate->Setup);

                  pPrivate->cxCurrent = 10;       // reset the size
                  WinInvalidateRect(pWidget->hwndWidget, NULL, FALSE);
              break; }
          } // end switch (usNotifyCode)
        }
        else if (usID == ID_XCENTER_TOOLTIP)
        {
            if (usNotifyCode == TTN_NEEDTEXT)
            {
                PTOOLTIPTEXT    pttt = (PTOOLTIPTEXT)mp2;


                GETPOWERSTATUS  getpowerstatusAPM;
                CHAR            szTip[400]       = "";
                CHAR            szBatteryLife[100] = "";
                CHAR            szBatteryTime[100] = "";
                CHAR            szACStatus[50]   = "";
                CHAR            szDebugInfo[200]    = "";
                ULONG           lifeInMinutes     = 0;
                int          rc;

                LangeGetString(BatteryLange, "Yes, My Lord!", (char*)msg, sizeof(msg));
                sprintf(szTip, "%s\n%s",(char *)msg, "http://ecomstation.ru/neowps");

                pttt->pszText = szTip;

                pttt->ulFormat = TTFMT_PSZ;

            }
        }
    }

    return (brc);
}

//!: << MwgtPaint >>
/**
 * MwgtPaint:
 *    implementation for WM_PAINT.
 *
 *    The specified HPS is switched to RGB mode before
 *    painting.
 */
/// CALLED FROM THE WINDOW PROC !
VOID MwgtPaint(HWND hwnd,
                PMONITORPRIVATE pPrivate,
                HPS hps,
                BOOL fDrawFrame)
{
    RECTL       rclWin;
    ULONG       ulBorder = 1;
    CHAR        szPaint[400] = "";
    CHAR        szTemp[400]  = "";
    ULONG       ulPaintLen  = 0;
    LONG        lcolBackground;
    LONG        lcolForeground;
    POINTL      aptlText[TXTBOX_COUNT];
    ULONG       ulIconWidth = 0;
    ULONG       ulTextWidth = 0;
    HPOINTER    hptrCurrent = 0;
    ULONG       lifeInMinutes     = 0;
    LONG        currentwidth;
    USHORT      level;
    BOOL        charging=0;

    //GETPOWERSTATUS  getpowerstatusAPM;

    // country settings from XCenter globals
    // (what a pointer)
    PCOUNTRYSETTINGS pCountrySettings
        = (PCOUNTRYSETTINGS)pPrivate->pWidget->pGlobals->pCountrySettings;

    // now paint button frame
    WinQueryWindowRect(hwnd, &rclWin);
    pgpihSwitchToRGB(hps);

    if (pPrivate->pWidget->pGlobals->flDisplayStyle & XCS_SUNKBORDERS)
    {
        if (fDrawFrame)
        {
            RECTL rcl2;
            rcl2.xLeft  = rclWin.xLeft;
            rcl2.yBottom = rclWin.yBottom;
            rcl2.xRight  = rclWin.xRight - 1;
            rcl2.yTop   = rclWin.yTop - 1;
            pgpihDraw3DFrame(hps,
                             &rcl2,
                             ulBorder,
                             pPrivate->pWidget->pGlobals->lcol3DDark,
                             pPrivate->pWidget->pGlobals->lcol3DLight);
        }

        // now paint middle
        rclWin.xLeft    += ulBorder;
        rclWin.yBottom += ulBorder;
        rclWin.xRight  -= ulBorder;
        rclWin.yTop -= ulBorder;
    }

    lcolBackground = pPrivate->Setup.lcolBackground;
    lcolForeground = pPrivate->Setup.lcolForeground;


    WinFillRect(hps,
                    &rclWin,
                    lcolBackground);

    BOOL drawicon=1;
    hptrCurrent = pPrivate->hptrNoBattery;



    if(drawicon) {
        //if ( (charging) ) {
                        //!: ----------------------------- [ Draw Widget Icon ]
            WinDrawPointer(hps,
                            0,
                            ((rclWin.yTop - rclWin.yBottom) - pPrivate->ulMiniIconSize) / 2,
                            hptrCurrent,
                            DP_MINI);
        //}

        // add offset for painting text
        ulIconWidth = pPrivate->ulMiniIconSize;
    }
    else {
        ulIconWidth = 0;

    }


    ulTextWidth=0;


    /*
    if(drawtext) {
            ulPaintLen = strlen(szPaint);
            GpiQueryTextBox(hps,
                        ulPaintLen,
                        szPaint,
                        TXTBOX_COUNT,
                        aptlText);
            ulTextWidth = aptlText[TXTBOX_TOPRIGHT].x;
     }


    strcpy(szPaint, "USB!");
    */

    currentwidth=(ulTextWidth + ulIconWidth) + 4 - rclWin.xRight;
    if ( abs(currentwidth)  > 4 ) {
        // we need more space: tell XCenter client
        pPrivate->cxCurrent = (ulTextWidth + ulIconWidth + 1*ulBorder);
        WinPostMsg(WinQueryWindow(hwnd, QW_PARENT),
                    XCM_SETWIDGETSIZE,
                    (MPARAM)hwnd,
                    (MPARAM)pPrivate->cxCurrent
                  );
    }
    else
    {
        if (pPrivate->Setup.displayText)
        {
            // sufficient space:
            GpiSetBackMix(hps, BM_OVERPAINT);
            rclWin.xLeft += ulIconWidth;
            WinDrawText(hps,
                        ulPaintLen,
                        szPaint,
                        &rclWin,
                        lcolForeground,
                        lcolBackground,
                        DT_CENTER | DT_VCENTER);
            }
    }

    // enable update
    //WinEnableWindowUpdate(hwnd, TRUE);
    //WinShowWindow(hwnd, TRUE);
}

//!: << MwgtPresParamChanged >>
/**
 *@@ MwgtPresParamChanged:
 *    implementation for WM_PRESPARAMCHANGED.
 */
VOID MwgtPresParamChanged(HWND hwnd,
                          ULONG ulAttrChanged,
                          PXCENTERWIDGET pWidget)
{
    PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)pWidget->pUser;
    GETPOWERSTATUS  getpowerstatusAPM;

    if (pPrivate)
    {
        BOOL fInvalidate = TRUE;
        switch (ulAttrChanged)
        {
            /*
            case 0:  // layout palette thing dropped
            case PP_BACKGROUNDCOLOR:
            case PP_FOREGROUNDCOLOR:
                // Retrieve the power details
                BatteryStatus(&getpowerstatusAPM);

                if (getpowerstatusAPM.ucBatteryLife < 20)
                {
                    pPrivate->Setup.lcolBackgroundLow
                        = pwinhQueryPresColor(hwnd,
                                              PP_BACKGROUNDCOLOR,
                                              FALSE,
                                              SYSCLR_DIALOGBACKGROUND);
                }
                else
                {
                    pPrivate->Setup.lcolBackground
                        = pwinhQueryPresColor(hwnd,
                                              PP_BACKGROUNDCOLOR,
                                              FALSE,
                                              SYSCLR_DIALOGBACKGROUND);
                }
                if (getpowerstatusAPM.ucACStatus)
                {
                    pPrivate->Setup.lcolForegroundAC
                        = pwinhQueryPresColor(hwnd,
                                              PP_FOREGROUNDCOLOR,
                                              FALSE,
                                              SYSCLR_WINDOWSTATICTEXT);
                }
                else
                {
                    pPrivate->Setup.lcolForeground
                        = pwinhQueryPresColor(hwnd,
                                              PP_FOREGROUNDCOLOR,
                                              FALSE,
                                              SYSCLR_WINDOWSTATICTEXT);
                }
            break;
            */

            case PP_FONTNAMESIZE:
            {
                PSZ pszFont = 0;
                if (pPrivate->Setup.pszFont)
                {
                    free(pPrivate->Setup.pszFont);
                    pPrivate->Setup.pszFont = NULL;
                }

                pszFont = pwinhQueryWindowFont(hwnd);
                if (pszFont)
                {
                    // we must use local malloc() for the font
                    pPrivate->Setup.pszFont = strdup(pszFont);
                    pwinhFree(pszFont);
                }
            break; }

            default:
                fInvalidate = FALSE;
        }

        if (fInvalidate)
        {
            XSTRING strSetup;
            WinInvalidateRect(hwnd, NULL, FALSE);

            MwgtSaveSetup(&strSetup,
                          &pPrivate->Setup);
            if (strSetup.ulLength)
                WinSendMsg(pPrivate->pWidget->pGlobals->hwndClient,
                            XCM_SAVESETUP,
                            (MPARAM)hwnd,
                            (MPARAM)strSetup.psz);
            pxstrClear(&strSetup);
        }
    } // end if (pPrivate)
}


//!: o-----------------------------------------------------> [ BUTTON 1 CLICK ]
//!: << MwgtButton1Click >>
/**
 *@@ MwgtButton1DblClick:
 *    implementation for WM_BUTTON1DBLCLK.
 */
/// CALLED FROM THE WINDOW PROC !
VOID MwgtButton1Click(HWND hwnd,
                         PXCENTERWIDGET pWidget)
{

//PEJECTPRIVATE pPrivate;
RECTL        rcl;
POINTL      ptl;
SWP         swp = { 0 };

    PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)pWidget->pUser;

    //~ __debug("MwgtButton1Click", "At entry", DBG_MLE);

    if ( !pWidget ) return;
    if ( pPrivate == NULL ) return;

    //~ __debug("MwgtButton1Click", "Before FillMenu", DBG_MLE);

//!: o------------------------------------------------------> [ FILL THE MENU ]
    /*
    // Fill the menu on a left-click.
    */
    FillMenu(hwndPopup);

    //~ __debug("MwgtButton1Click", "After FillMenu", DBG_MLE);


    // Place popup according to xCenter position
    if ( pWidget->pGlobals->ulPosition == XCENTER_BOTTOM )
    {
        WinQueryWindowRect(hwnd, &rcl);
        ptl.y = rcl.yTop + 1;
        ptl.x = rcl.xLeft;
        WinMapWindowPoints(HWND_DESKTOP, hwnd, &ptl, 0);
    }
    else
    {
        WinQueryWindowRect( hwnd, &rcl );
        ptl.y = rcl.yBottom;
        ptl.x = rcl.xLeft;
        WinMapWindowPoints(HWND_DESKTOP, hwnd, &ptl, 0);

        swp.fl = SWP_SIZE;
        WinSendMsg(hwndPopup, WM_ADJUSTWINDOWPOS, MPFROMP(&swp), 0);
        ptl.y -= swp.cy;
    }

    //~ __debug("MwgtButton1Click", "Before WinPopupMenu", DBG_MLE);


//!: o-----------------------------------------------------> [ POPUP THE MENU ]
    /*
    // PopUp the Menu.
    */
    WinPopupMenu(hwnd, hwnd, hwndPopup, ptl.x, ptl.y+2, 0,                      // menu-offset, offsetting x only works if less than menu-width
                 PU_MOUSEBUTTON1|PU_HCONSTRAIN|PU_VCONSTRAIN|PU_KEYBOARD);

    //~ __debug("MwgtButton1Click", "After WinPopupMenu", DBG_MLE);


//  PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)pWidget->pUser;
//  if (pPrivate)
//  {

    // Oleg Zhukov!!!

    // 1. ┬»┬«┬¬┬á┬º┬á├ó├¼ popup-┬¼┬Ñ┬¡├«
    //

        /*
        const char *pcszID = NULL;
        HOBJECT hobj = NULLHANDLE;

        pcszID = "<WP_POWER>";

        if (pcszID)
            hobj = WinQueryObject((PSZ)pcszID);

        if (hobj)
        {
            WinOpenObject(hobj,
                          0, // OPEN_SETTINGS,
                          TRUE);
        }
        */

//  } // end if (pPrivate)

    //~ __debug("MwgtButton1Click", "Returning", DBG_MLE);
}


//!: << MeasureMenuItem >>
/**
 *@@ MeasureMenuItem:
 *      Calculates the height and width for a specified menu item.
 */
MRESULT MeasureMenuItem(PXCENTERWIDGET pWidget, POWNERITEM poi) {
    POINTL  aptl[TXTBOX_COUNT];
    SIZEL   szl = { 0 };
    int     i;

    for (i=0; i<26; i++) {
        if ( Drives[i] && strlen(Drives[i]) ) {
            GpiQueryTextBox(poi->hps,strlen(Drives[i]),Drives[i],TXTBOX_COUNT,aptl);
            szl.cy = max(szl.cy, aptl[TXTBOX_TOPRIGHT].y-aptl[TXTBOX_BOTTOMLEFT].y);
            szl.cx = max(szl.cx, aptl[TXTBOX_TOPRIGHT].x-aptl[TXTBOX_BOTTOMLEFT].x);
        }
    }

    poi->rclItem.yBottom    = 0;
    poi->rclItem.yTop       = max(szl.cy, (pWidget->pGlobals->cxMiniIcon)+4);   // Item hoogte
    poi->rclItem.xLeft      = 0;
    poi->rclItem.xRight     = poi->rclItem.yTop + szl.cx+5;

    return MRFROMLONG(poi->rclItem.yTop);
}

//!: << DrawMenuItem >>
/**
 *@@ DrawMenuItem:
 *      This is called each time an item is to be drawn.
 */
MRESULT DrawMenuItem(PXCENTERWIDGET pWidget, POWNERITEM poi) {
    POINTL      pos;
    HPOINTER    icoptr  = (HPOINTER)poi->hItem;
    RECTL       rect    = poi->rclItem;
    LONG        id      = poi->idItem-ID_MENU_REDISCOVER-1;
    BOOL        select  = (poi->fsAttribute & MIA_HILITED) ? TRUE : FALSE;

    WinFillRect(poi->hps, &rect, select ? SYSCLR_MENUHILITEBGND : SYSCLR_MENU);



    /*
    // Meerdere pictogrammen per entry kunnen worden getekend.
    // Dus drives per disk.
    */

    rect.xLeft += 8;    // used to be 2
    rect.yBottom += 2;
    WinDrawPointer(poi->hps, rect.xLeft, rect.yBottom, icoptr, DP_MINI);

    rect.xLeft += poi->rclItem.yTop - poi->rclItem.yBottom + 5;
    rect.yBottom = poi->rclItem.yBottom;

    WinDrawText(poi->hps, -1, Drives[id], &rect,
                select ? SYSCLR_MENUHILITE : SYSCLR_MENUTEXT, 0,
                DT_LEFT | DT_VCENTER);


    /*
    // Well well well... And now ;) we are going to fix The Great-Warp4-Menu-Bug.
    // Make to redraw erased parts of menu's window border.
    */

    /* vertical "light" line */
    pos.x = poi->rclItem.xLeft;
    pos.y = 1;
    GpiSetColor(poi->hps, SYSCLR_BUTTONLIGHT);
    GpiMove(poi->hps, &pos);
    pos.y = poi->rclItem.yTop;
    GpiLine(poi->hps, &pos);

    /* horizontal "dark" line */
    pos.x = 1;
    pos.y = 1;
    GpiSetColor(poi->hps, SYSCLR_BUTTONDARK);
    GpiMove(poi->hps, &pos);
    pos.x = poi->rclItem.xRight;
    GpiLine(poi->hps, &pos);


    poi->fsAttributeOld = (poi->fsAttribute &= ~MIA_HILITED);

    return MRFROMLONG(TRUE);
}


//!: << MwgtDestroy >>
/**
 *@@ MwgtDestroy:
 *
 *@@added V0.9.12 (2001-05-26) [umoeller]
 */
VOID MwgtDestroy(PXCENTERWIDGET pWidget)
{
    APIRET  ulrc=0;
    ULONG   ulReply = 0;
    BOOL    cleanup=TRUE;
    BOOL    brc = FALSE;


    ShowDebugDialog();

    /* Where are we ? */
    __debug(NULL, "MwgtDestroy", DBG_LBOX);


///
//!: ----------------------------------------------------- [ STOP THE THREADS ]
///

    __debug(NULL, "Stopping Threads", DBG_LBOX);
    if (cleanup) {
        StopThreads();
    }
    __debug(NULL, "Threads stopped ?", DBG_LBOX);

    //~ StopThreads();

    WinStopTimer(myhab, hwndIndiDevInsert,      TIMER_SHOW_BANNER_DEV_INSERT);
    WinStopTimer(myhab, hwndIndiSafeEject,      TIMER_SHOW_BANNER_SAFE_EJECT);
    WinStopTimer(myhab, hwndIndiDevRemove,      TIMER_SHOW_BANNER_DEV_REMOVE);
    WinStopTimer(myhab, hwndIndiDrivesChanged,  TIMER_SHOW_BANNER_DRIVES_CHANGED);

    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- myhab=%08X", myhab);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hwndIndiDevInsert=%08X", hwndIndiDevInsert);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hwndIndiSafeEject=%08X", hwndIndiSafeEject);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hwndIndiDevRemove=%08X", hwndIndiDevRemove);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hwndPopup=%08X", hwndPopup);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hptrCD=%08X", hptrCD);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hptrHDD=%08X", hptrHDD);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hptrHDDBAD=%08X", hptrHDDBAD);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hptrHDDPHANTOM=%08X", hptrHDDPHANTOM);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hptrEject=%08X", hptrEject);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hbmUSBThumb=%08X", hbmUSBThumb);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- hbmIconInfo=%08X", hbmIconInfo);

    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- tid_lvm=%08X", tid_lvm);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- tid_monitor=%08X", tid_monitor);
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "-- tid_eject=%08X", tid_eject);



    if(hptrCD) WinDestroyPointer(hptrCD);
    if(hptrHDD) WinDestroyPointer(hptrHDD);
    if(hptrHDD) WinDestroyPointer(hptrHDDBAD);
    if(hptrHDD) WinDestroyPointer(hptrHDDPHANTOM);
    if(hptrZIP) WinDestroyPointer(hptrZIP);
    if(hptrEject) WinDestroyPointer(hptrEject);

    if(hbmUSBThumb) GpiDeleteBitmap(hbmUSBThumb);
    if(hbmIconInfo) GpiDeleteBitmap(hbmIconInfo);
    if(hbmIconWarning) GpiDeleteBitmap(hbmIconWarning);


    LangeUnLoadLanguage(BatteryLange);



    PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
        /*
        if (pPrivate->ulTimerID)
            ptmrStopXTimer((_XTIMERSET *)pPrivate->pWidget->pGlobals->pvXTimerSet,
                            pWidget->hwndWidget,
                            pPrivate->ulTimerID);
        */

        if (pPrivate->hptrAC)
            WinDestroyPointer(pPrivate->hptrAC);
        if (pPrivate->hptrBattery)
            WinDestroyPointer(pPrivate->hptrBattery);
        if (pPrivate->hptrNoBattery)
            WinDestroyPointer(pPrivate->hptrNoBattery);

        free(pPrivate);
    } // end if (pPrivate)



    if (hwndPopup) {
        brc = WinDestroyWindow (hwndPopup);
        hwndPopup = NULL;
    }

    /*
    // Destroy the notification-windows.
    */
    if (hwndIndiDevInsert) {
        brc = WinDestroyWindow(hwndIndiDevInsert);
        hwndIndiDevInsert = NULL;
    }
    if (hwndIndiSafeEject) {
        brc = WinDestroyWindow(hwndIndiSafeEject);
        hwndIndiSafeEject = NULL;
    }
    if (hwndIndiDevRemove) {
        brc = WinDestroyWindow(hwndIndiDevRemove);
        hwndIndiDevRemove = NULL;
    }
    if (hwndIndiDrivesChanged) {
        brc = WinDestroyWindow(hwndIndiDrivesChanged);
        hwndIndiDrivesChanged = NULL;
    }
    if (hdlgNotification) {
        brc = WinDestroyWindow(hdlgNotification);
        hdlgNotification = NULL;
    }

    if (hdlgProblemFixerDialog) {
        brc = WinDestroyWindow(hdlgProblemFixerDialog);
        hdlgProblemFixerDialog = NULL;
    }


    /*
    if (hdlgDebugDialog) {
        brc = WinDestroyWindow(hdlgDebugDialog);
        hdlgDebugDialog = NULL;
    }
    */

    /* Change the owner from widget-window to HWND_DESKTOP */
    //~ if (hdlgDebugDialog) {
        //~ brc = WinSetOwner(hdlgDebugDialog, HWND_DESKTOP);
    //~ }

    /* Capture current drive map */
    DosQueryCurrentDisk(NULL, &pws->drivemap);

    /* Process */
    //ulReply = WinProcessDlg(hdlgDebugDialog);
    __debug(NULL, "Processed Destroy", DBG_LBOX);

    __debug(NULL, "About to destroy Debug Dialog", DBG_LBOX);

    // Returns immediately ??
    //~ if (hdlgDebugDialog)
    WinProcessDlg(hdlgDebugDialog);
    WinProcessDlg(hdlgDebugDialog);
    WinProcessDlg(hdlgDebugDialog);
    __debug(NULL, "After WinProcessDlg", DBG_LBOX);

    //~ DestroyDebugDialog();
}


//!: << fnwpSampleWidget >> (Window Procedure for the Widget)
/**
 *@@ fnwpSampleWidget:
 *    window procedure for the winlist widget class.
 *
 *    There are a few rules which widget window procs
 *    must follow. See ctrDefWidgetProc in src\shared\center.c
 *    for details.
 *
 *    Other than that, this is a regular window procedure
 *    which follows the basic rules for a PM window class.
 */
MRESULT EXPENTRY fnwpSampleWidget(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    MRESULT         mrc = 0;
    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET  pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    // this ptr is valid after WM_CREATE

    CARDINAL32  rc;
    char        drive[2] = { 0,0 };
    short       event = SHORT1FROMMP(mp1);
    char        temp[256];
    char        buf[256];
    char        msgbuf[256];
    APIRET      ulrc=0;


    //~ sprintf(buf, "  >>> DLGPROC: msg:%08lX, mp1:%08lX, mp2:%08lX", msg, mp1, mp2);
    //~ __debug("Widget Window", buf, DBG_AUX);

    switch (msg) {
        /*
        * WM_CREATE:
        *     as with all widgets, we receive a pointer to the
        *     XCENTERWIDGET in mp1, which was created for us.
        *
        *     The first thing the widget MUST do on WM_CREATE
        *     is to store the XCENTERWIDGET pointer (from mp1)
        *     in the QWL_USER window word by calling:
        *
        *         WinSetWindowPtr(hwnd, QWL_USER, mp1);
        *
        *     We use XCENTERWIDGET.pUser for allocating
        *     MONITORPRIVATE for our own stuff.
        */

        case WM_CREATE: {
            WinSetWindowPtr(hwnd, QWL_USER, mp1);
            pWidget = (PXCENTERWIDGET)mp1;
            if ((pWidget) && (pWidget->pfnwpDefWidgetProc))
                mrc = MwgtCreate(hwnd, pWidget);
            else
                // stop window creation
                mrc = (MPARAM)TRUE;
            break;
        }


/*
        case WM_ERASEBACKGROUND:
            return (MRESULT) TRUE;
        break;
*/


         /*
         * WM_CONTROL:
         *    process notifications/queries from the XCenter.
         */

        case WM_CONTROL: {
            mrc = 0;
            //mrc = (MPARAM)MwgtControl(hwnd, mp1, mp2);


            break;
        }


        case WM_COMMAND: {
            mrc = 0;
          //sprintf(temp, "WM_COMMAND: %d", event);
          //__debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);


            pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);


            /*
            // Do the specific action corresponding to the choosen menu-item.
            */
            switch (event) {

                case ID_SUBMENU_DEBUG_CONNECT_PIPE: {
                    mrc = 0;
                    OpenPipe();
                    //MessageBox("DEBUG", "CONNECT_PIPE");
                    break;
                }

                case ID_SUBMENU_DEBUG_DISCONNECT_PIPE: {
                    mrc = 0;
                    ClosePipe();
                    //MessageBox("DEBUG", "DISCONNECT_PIPE");
                    break;
                }


                case ID_SHOW_INSERT_NOTIFICATION: {
                    mrc = 0;
                    WinSetWindowPos(hwndIndiDevInsert, HWND_TOP, 0, 0, 0, 0, SWP_SHOW | SWP_ZORDER);
                    //WinShowWindow(hwndIndiDevInsert, TRUE);
                    break;
                }

                case ID_HIDE_INSERT_NOTIFICATION: {
                    mrc = 0;
                    WinSetWindowPos(hwndIndiDevInsert, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDE | SWP_ZORDER);
                    //WinShowWindow(hwndIndiDevInsert, FALSE);
                    break;
                }

                case ID_SHOW_EJECT_NOTIFICATION: {
                    mrc = 0;
                    WinSetWindowPos(hwndIndiSafeEject, HWND_TOP, 0, 0, 0, 0, SWP_SHOW | SWP_ZORDER);
                    //WinShowWindow(hwndIndiSafeEject, TRUE);
                    break;
                }

                case ID_HIDE_EJECT_NOTIFICATION: {
                    WinSetWindowPos(hwndIndiSafeEject, HWND_BOTTOM, 0, 0, 0, 0, SWP_HIDE | SWP_ZORDER);
                    //WinShowWindow(hwndIndiSafeEject, FALSE);
                    break;
                }

                case ID_SUBMENU_DEBUG_SHOW_DRIVE_MAP: {
                    mrc = 0;
                    ShowDriveMap();
                    return 0;
                    break;
                }

                case ID_SUBMENU_DEBUG_SHOW_DRIVE_MAP2: {
                    mrc = 0;
                    ShowDriveMap2();
                    return 0;
                    break;
                }

                case ID_SUBMENU_DEBUG_SHOW_VOLUMES: {
                    mrc = 0;
                    ShowVolumes();
                    return 0;
                    break;
                }

                case ID_SUBMENU_DEBUG_SHOW_VOLUMES2: {
                    mrc = 0;
                    ShowVolumes2();
                    return 0;
                    break;
                }

                case ID_SUBMENU_DEBUG_SHOW_DIALOGS: {
                    mrc = 0;
                    ShowDialogs();
                    return 0;
                    break;
                }

                case ID_SUBMENU_DEBUG_HIDE_DIALOGS: {
                    mrc = 0;
                    HideDialogs();
                    return 0;
                    break;
                }

                case ID_SUBMENU_DEBUG_SHOW_WINDOWS: {
                    mrc = 0;
                    ShowWindows();
                    return 0;
                    break;
                }

                case ID_SUBMENU_DEBUG_HIDE_WINDOWS: {
                    mrc = 0;
                    HideWindows();
                    return 0;
                    break;
                }

                //! WidgetSettingsDialog
                case ID_SUBMENU_WIDGET_SETTINGS_SHOW_NOTEBOOK: {
                    BOOL    brc = FALSE;
                    ULONG   ulReply = NULL;
                    HWND    hwndWidgetSettingsDialog = NULL;
                    WidgetSettingsDialog* lwsd = NULL;

                    /* Test new WidgetSettingsDialog */
                    lwsd = new WidgetSettingsDialog;        // Default Dialog
                    //~ lwsd = new WidgetSettingsDialogEx;      // Extended Dialog to test overrides
                    if (lwsd) {
                        lwsd->create();
                        lwsd->show();
                        lwsd->process();
                        lwsd->hide();
                        lwsd->destroy();
                        delete lwsd;
                        lwsd = NULL;
                    }

                    mrc = 0;
                    break;
                }

                //! Debug Dialog
                case ID_SUBMENU_DEBUG_SHOW_DEBUG_DIALOG: {
                    BOOL    brc = FALSE;
                    ULONG   ulReply = NULL;
                    HWND    hwndDialog = NULL;
                    //MessageBox("Testing","Testing");
                    mrc = 0;

                    //~ WinShowWindow(hdlgDebugDialog, TRUE);
                    ShowDebugDialog();
                    return 0;
                    break;
                }

                /*
                // Rediscover the LVM-state.
                */
                case ID_MENU_REDISCOVER: {
                    mrc = 0;
                    //Rediscover_PRMs(&rc);

                    sprintf(temp, "MENU: Refresh LVM");
                    //__debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

                    //!: Rousseau: Hier zit de culprit...
                    ulrc=DosPostEventSem(hevPushLVMthread);

                    /* Wait half a sec */
                    //~ DosBlock(500);
                    MyDosSleep(500);


                    if(enablebeeps) {
                        RefreshBeep();
                    }
                    // Rousseau: added
                    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, "DosPostEventSem--> hevPushLVMthread=%08X, ulrc=%ld", hevPushLVMthread, ulrc);
                    // Rousseau: hevPushLVMthread is invalid handle on second time... (after first crash)
                    //          So, no event-semaphore is posted and no threads are started.

                    //RefreshLVM();
                    break;
                }

                /*
                // Any unhandled casses endup here.
                */
                default: {
                    mrc = 0;
                    /* All EJECT-items indexes have the same base; see if the menu-item id falls in this range. */
                    if ( (event >= (ID_MENU_REDISCOVER+1)) && (event <= (ID_MENU_REDISCOVER+26)) ){ // "Eject" command
                        drive[0] = event-ID_MENU_REDISCOVER+'A'-1;

                        foreign_eject = FALSE;
                        //EjectRemovableDrive(drive);                                       // Werkt wel in widget niet in logpipe
                        //ejectdrive=drive[0];

                        //!: ---------------------------- [ Start EjectThread ]
                        /**
                         * The EjectThread is a one-shot thread.
                         */
                        tid_eject = _beginthread(EjectThread, NULL, 40000, (void*)drive[0]);  // Points to "?\0", no colon
                        mrc = 0;
                    }
                    else {
                        /* Nope, they don't -- delegate to default handling. */
                        //return 0;
                        mrc = pWidget->pfnwpDefWidgetProc(hwnd, msg, mp1, mp2);
                    }
                    break;
                }

            } // switch (event)
            break;
        } // WM_COMMAND

        /*
         * WM_PAINT:
         *
         */

        case WM_PAINT: {
            mrc = 0;
            HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
            pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

            if (hps)
            {
                // get widget data and its button data from QWL_USER
                PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)pWidget->pUser;
                if (pPrivate)
                {
                    MwgtPaint(hwnd,
                              pPrivate,
                              hps,
                              TRUE);        // draw everything
                } // end if (pPrivate)

                WinEndPaint(hps);
            }
            break;
        } // WM_PAINT


        /*
         * WM_TIMER:
         *    clock timer --> repaint.
         */

        case WM_TIMER: {
/*
            HPS hps = WinGetPS(hwnd);
            if (hps)
            {
                // get widget data and its button data from QWL_USER
                PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)pWidget->pUser;
                if (pPrivate)
                {
                    MwgtPaint(hwnd,
                              pPrivate,
                              hps,
                              FALSE);   // text only
                } // end if (pPrivate)

                WinReleasePS(hps);
            }
*/          break;
        } // WM_TIMER


        /*
         *@@ WM_BUTTON1DBLCLK:
         *    on double-click on clock, open
         *    system clock settings.
         */

        case WM_MEASUREITEM: {
            return MeasureMenuItem(pWidget, (POWNERITEM)mp2);
            break;
        } // WM_MEASUREITEM

        //---------------------------------------------------------------------
        // WM_DRAWITEM: draw the menu item
        //---------------------------------------------------------------------
        case WM_DRAWITEM: {
            return DrawMenuItem(pWidget, (POWNERITEM)mp2);
            break;
        } // EM_DRAWITEM

        case WM_BUTTON1CLICK: {
            MwgtButton1Click(hwnd, pWidget);
            mrc = (MPARAM)TRUE;  // message processed
            break;
        } // WM_BUTTON1CLICK

        /*
         * WM_PRESPARAMCHANGED:
         *
         */

        case WM_PRESPARAMCHANGED: {
            if (pWidget)
                // this gets sent before this is set!
                MwgtPresParamChanged(hwnd, (ULONG)mp1, pWidget);
            break;
        } // WM_PRESPARAMSCHANGED

        /*
         * WM_DESTROY:
         *    clean up. This _must_ be passed on to
         *    ctrDefWidgetProc.
         */
        case WM_DESTROY: {
            MwgtDestroy(pWidget);
            mrc = pWidget->pfnwpDefWidgetProc(hwnd, msg, mp1, mp2);
            break;
        } // WM_DESTROY

        default: {
            if (pWidget)
                mrc = pWidget->pfnwpDefWidgetProc(hwnd, msg, mp1, mp2);
            else
                mrc = WinDefWindowProc(hwnd, msg, mp1, mp2);
            break;
        } // default
    } // switch(msg)

    //__debug(__FUNCTION__, "leaving\n\n", DBG_MLE | DBG_AUX);
    return (mrc);
}




//!: << fnwpIndiClient >> (Window Procedure for Notification Windows)
MRESULT EXPENTRY fnwpIndiClient( HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2 )
{
    HBITMAP     hbm;
    HPS         hps;
    RECTL       rec;
    int         framenumber;
    POINTL      lbPos;
    POINTL      ptl;
    int         dnum;
    char        buf[256];
    char        buf2[256];
    int         action;
    POINTL      aptlPoints[3];
    FONTMETRICS fm;
    FATTRS      fat;
    MRESULT     mrc = 0;
    USHORT      id = 0;

    //~ sprintf(buf, "  >>> DLGPROC: msg:%08lX, mp1:%08lX, mp2:%08lX", msg, mp1, mp2);
    //~ __debug("Notification Windows", buf, DBG_AUX);

    //!: CHECKME: Aanpassen ! not global, use window storage !!
    USBDeviceReport*    usbdev = (USBDeviceReport*) g_usbdev;


    variables.devinsert_banner_width        = 256+64;
    variables.devinsert_banner_height       = 96;
    variables.safeeject_banner_width        = 256+64;
    variables.safeeject_banner_height       = 96;
    variables.devremove_banner_width        = 360+64;
    variables.devremove_banner_height       = 112;

    variables.driveschanged_banner_width    = 192+64;
    variables.driveschanged_banner_height   = 96;

    switch (msg) {
        /*
        case WM_BUTTON1CLICK:
            HideINDI();
        break;
        */

        /*
        case WM_SIZE:
        break;
        */

        case WM_ERASEBACKGROUND: {
            mrc = (MRESULT) TRUE;
            break;
        }

        case WM_DEVINFO: {
            mrc = (MRESULT) TRUE;
            g_usbdev = (USBDeviceReport*) mp1;
            break;
        }

        case WM_TIMER: {
            switch ((int)mp1) {
                case TIMER_SHOW_BANNER_DEV_INSERT:
                    WinStopTimer(myhab, hwndIndiDevInsert, TIMER_SHOW_BANNER_DEV_INSERT);
                    WinShowWindow(hwndIndiDevInsert, FALSE);
                    break;

                case TIMER_SHOW_BANNER_SAFE_EJECT:
                    WinStopTimer(myhab, hwndIndiSafeEject, TIMER_SHOW_BANNER_SAFE_EJECT);
                    WinShowWindow(hwndIndiSafeEject, FALSE);
                    break;

                case TIMER_SHOW_BANNER_DEV_REMOVE:
                    WinStopTimer(myhab, hwndIndiDevRemove, TIMER_SHOW_BANNER_DEV_REMOVE);
                    WinShowWindow(hwndIndiDevRemove, FALSE);
                    break;

                case TIMER_SHOW_BANNER_DRIVES_CHANGED:
                    WinStopTimer(myhab, hwndIndiDrivesChanged, TIMER_SHOW_BANNER_DRIVES_CHANGED);
                    WinShowWindow(hwndIndiDrivesChanged, FALSE);
                    break;
            }
            break;
        }


        case WM_PAINT: {
            mrc = 0;

            hps=WinBeginPaint(hwnd,NULLHANDLE,&rec);
            WinFillRect(hps,&rec, CLR_WHITE);

            /*
            // Sort of a jump-table / case-statement because switch on vars cannot be done.
            // It uses a goto to avoid ever indenting if-then-else constructs.
            */
            if(hwnd == hwndIndiDevInsert)       { action = SHOW_DEV_INSERT;     goto select_on_action; }
            if(hwnd == hwndIndiSafeEject)       { action = SHOW_SAFE_EJECT;     goto select_on_action; }
            if(hwnd == hwndIndiDevRemove)       { action = SHOW_DEV_REMOVE;     goto select_on_action; }
            if(hwnd == hwndIndiDrivesChanged)   { action = SHOW_DRIVES_CHANGED; goto select_on_action; }

            /* Early break, the hwnd is not one of the notification-windows, so we return immediately */
            break;

            /* We arrive here from the jump-table above */
            select_on_action:

            /*
            // Show the corresponding window based on window-handle.
            */
            switch (action) {

                /*
                // NEW DEVICE INSERTED.
                */
                case SHOW_DEV_INSERT: {
                    WinFillRect(hps,&rec, CLR_WHITE);

                    DrawRect(0, 0,
                                variables.devinsert_banner_width-1,
                                variables.devinsert_banner_height-1,
                                CLR_BLACK);

                    lbPos.x=8; lbPos.y=28;
                    //WinDrawBitmap(hps, hbmIconInfo, NULL, &lbPos,0,0, DBM_NORMAL);


                    /*
                    // Experimenting with embedded PNG's.
                    */
                    // FL_STRIPALPHA        // Ignore the aplha channel
                    // FL_PROCESSALPHA      // Process alpha by mixing image with specified background color
                    // FL_COPYALPHA         // Copy alpha channel when blitting                                 (*)
                    // FL_USEBKGD           // Use bKGD PNG chunk to determine background color                 (*)
                    {
                        BOOL        brc = FALSE;
                        APIRET      ulrc = -1;
                        ULONG       pngsize = 0;
                        BYTE*       pngimage = NULL;
                        HBITMAP2    hbm2 = NULL;
                        //POINTL        lbPos;
                        POINTL      aptlPoints[3];

                        CHAR        txtbuf[1024] = "\0";

                        /*
                        ulrc = DosQueryResourceSize(hmodMe, RT_PNG, ID_PNG_TRANSPORTATION, &pngsize);
                        sprintf(txtbuf,"ulrc=%d, pngsize=%d%s", ulrc, pngsize, "\n");
                        WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) txtbuf, (MPARAM) NULL);
                        //pngimage=new BYTE[pngsize];

                        ulrc = DosGetResource(hmodMe, RT_PNG, ID_PNG_TRANSPORTATION, (PPVOID) &pngimage);
                        sprintf(txtbuf,"ulrc=%d, pngimage=%08X%s", ulrc, (PULONG) *pngimage, "\n");
                        WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) txtbuf, (MPARAM) NULL);

                        hbm2 = Gpi2ReadPng((PBYTE) pngimage, 0, FL_STRIPALPHA);
                        */

                        hbm2 = Gpi2LoadPng(hmodMe, ID_PNG_USBDEV2, 0x808080, FL_COPYALPHA);
                        sprintf(txtbuf,"ulrc=%d, hbm2=%08X", ulrc, hbm2);
                        __debug("Gpi2LoadPng", txtbuf, DBG_MLE);

                        aptlPoints[0].x = lbPos.x;
                        aptlPoints[0].y = lbPos.y-6;
                        //brc = WinDrawBitmap(hps, hbm2, NULL, &lbPos, 0, 0, DBM_NORMAL);
                        ulrc = Gpi2DrawBitmap(hps, hbm2, 1, aptlPoints);
                        sprintf(txtbuf,"ulrc=%d, brc=%d", ulrc, brc);
                        __debug("Gpi2DrawBitmap", txtbuf, DBG_MLE);

                        Gpi2DeleteBitmap(hbm2);

                        /*
                        ulrc = DosFreeResource(pngimage);
                        sprintf(txtbuf,"ulrc=%d%s", ulrc, "\n");
                        WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) txtbuf, (MPARAM) NULL);
                        */
                    }


                    /*
                    // Button Testing.
                    */
                    {
                        HWND        hwndButton  = NULL;
                        BTNCDATA    btncd;

                        btncd.cb            = sizeof(BTNCDATA);
                        btncd.fsCheckState  = TRUE;
                        btncd.fsHiliteState = TRUE;
                        btncd.hImage        = NULL;


                        //!: -------------- [ Create Button on N otify Window ]
                        /*
                        // Preferred notation.
                        */
                        hwndButton = WinCreateWindow(
                            hwnd,                       // parent
                            WC_BUTTON,                  // class
                            "Bliep",                    // text
                            NULL,                       // visibility   WS_VISIBLE
                            10,                         // x-pos lower-left
                            10,                         // y-pos lower-left
                            50,                         // width
                            25,                         // height
                            hwnd,                       // owner
                            HWND_TOP,                   // z-order
                            33,                         // id
                            NULL,                       // control-data
                            NULL                        //
                        );
                    }





                    GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &fm);

                    memset(&fat, 0, sizeof(fat));
                    fat.usRecordLength  = sizeof(FATTRS);
                    fat.lMatch        = fm.lMatch;

                    strcpy(fat.szFacename, fm.szFacename);
                    fat.fsSelection  = FATTR_SEL_BOLD;
                    GpiCreateLogFont(hps, 0, 1L, &fat);

//                  rcl=((POWNERITEM)mp2)->rclItem;
//                  rcl.xLeft+=CX_ICON+10;

                    /* Bold */
                    GpiSetCharSet(hps, 1L);

                    ptl.x=8+64+8; ptl.y=64;
                    //LangeGetString(BatteryLange, "New Device %c:", (char*)buf, sizeof(buf));
                    LangeGetString(BatteryLange, "New USB Device", (char*)buf, sizeof(buf));
                    sprintf(buf2, buf, drvletter_safeeject);
                    GpiCharStringAt(hps, &ptl, strlen(buf2), (PSZ)buf2);

                    /* Normal*/
                    GpiSetCharSet(hps, 0L);

                    if (usbdev) {
                        sprintf(buf, "\"%s\"", usbdev->devicename);
                    }


                    //LangeGetString(BatteryLange, "A new device has been inserted.", (char*)buf, sizeof(buf));
                    //sprintf(buf2, " [%c:]", drvletter_safeeject);
                    //strcat(buf, buf2);

                    //strcat(buf, "\n");

                    //strcat(buf, buf2);


                    POINTL aptl[TXTBOX_COUNT];
                    double w, h;
                    POINTL ptl;


                    if(buf) GpiQueryTextBox(hps, strlen(buf), buf, TXTBOX_COUNT, aptl);
                    w=aptl[2].x;
                    h=aptl[2].y;

                    //DrawRect(DrawAt->x, DrawAt->y, w, h, PaperColor);

                    //ptl.x=8+64+8;
                    //ptl.y=20;

                    GpiSetColor(hps, CLR_BLACK);
                    //GpiSetColor(hps, CLR_BLUE);
                    //if(buf) GpiCharStringAt(hps, &ptl, strlen(buf), buf);

                    RECTL rtl;

                    rtl.xLeft=8+64+8;
                    rtl.xRight=256+64;
                    rtl.yBottom=0;
                    rtl.yTop=64-8;

                    LONG    hText;
                    LONG    hTotalDrawn=0;
                    LONG    hDrawn=0;
                    LONG    CharHeight;                 // set character height
                    char*   pszText=buf;
                    //LONG hText;

                    hText = (LONG)strlen(pszText);      // get length of string
                    CharHeight = 15;                    // set character height

                    /* until all chars drawn */
                    for (hTotalDrawn = 0;  hTotalDrawn !=  hText;
                                        rtl.yTop -= CharHeight) {

                        /* draw the text */
                        hDrawn = WinDrawText(hps,                       // presentation-space handle
                                                hText -  hTotalDrawn,   // length of text to draw
                                                pszText +  hTotalDrawn, // address of the text
                                                &rtl,                   // rectangle to draw in
                                                0L,                     // foreground color
                                                0L,                     // background color
                                                DT_WORDBREAK | DT_TOP | DT_LEFT | DT_TEXTATTRS);

                        if (hDrawn)
                            hTotalDrawn +=  hDrawn;
                        else
                            break;
                    }
                    break;
                } // SHOW_DEV_INSERT




                /*
                // DEVICE SAFE EJECTED.
                */
                case SHOW_SAFE_EJECT: {
                    WinFillRect(hps,&rec, CLR_WHITE);


                    DrawRect(0,0,
                                variables.safeeject_banner_width-1,
                                variables.safeeject_banner_height-1,
                                CLR_BLACK);

                    lbPos.x=8; lbPos.y=28;

                    //WinDrawBitmap(hps, hbmIconInfo, NULL, &lbPos,0,0, DBM_NORMAL);


                    /*
                    // Experimenting with embedded PNG's.
                    */
                    // FL_STRIPALPHA        // Ignore the aplha channel
                    // FL_PROCESSALPHA      // Process alpha by mixing image with specified background color
                    // FL_COPYALPHA         // Copy alpha channel when blitting                                 (*)
                    // FL_USEBKGD           // Use bKGD PNG chunk to determine background color                 (*)
                    {
                        BOOL        brc = FALSE;
                        APIRET      ulrc = -1;
                        ULONG       pngsize = 0;
                        BYTE*       pngimage = NULL;
                        HBITMAP2    hbm2 = NULL;
                        //POINTL        lbPos;
                        POINTL      aptlPoints[3];
                        CHAR        txtbuf[1024] = "\0";

                        hbm2 = Gpi2LoadPng(hmodMe, ID_PNG_INFORMATION, 0x808080, FL_COPYALPHA);
                        aptlPoints[0].x = lbPos.x;
                        aptlPoints[0].y = lbPos.y;
                        ulrc = Gpi2DrawBitmap(hps, hbm2, 1, aptlPoints);
                        Gpi2DeleteBitmap(hbm2);
                    }

                    GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &fm);
                    memset(&fat, 0, sizeof(fat));
                    fat.usRecordLength  = sizeof(FATTRS);
                    fat.lMatch        = fm.lMatch;

                    strcpy(fat.szFacename, fm.szFacename);
                    fat.fsSelection  = FATTR_SEL_BOLD;
                    GpiCreateLogFont(hps, 0, 1L, &fat);

//                  rcl=((POWNERITEM)mp2)->rclItem;
//                  rcl.xLeft+=CX_ICON+10;

                    GpiSetCharSet(hps, 1L);

                    //~: Safe Eject %c:
                    ptl.x=8+64+8; ptl.y=64;

                    if (!foreign_eject) {

                        LangeGetString(BatteryLange, "Safe Eject %c:", (char*)buf, sizeof(buf));

                        //~ sprintf(buf2, buf, usbdev->dletter);
                        sprintf(buf2, buf, drvletter_safeeject);


                        //!: FIXME: Wrong driveletters and garbage after string

                        /*
                        // Append sibling drives if present.
                        */
                        {
                            ULONG   drivemap_ejd = drive_letter_to_map(drvletter_safeeject);

                            char    buf7[256] = "\0";
                            char    buf8[256] = "\0";
                            char    buf9[256] = "\0";
                            if (drive_count(drivemap_safeeject) > 1) {
                                //drivemap_safeeject ^= drivemap_ejd;
                                LangeGetString(BatteryLange, " and also %s", (char*)buf7, sizeof(buf7));
                                sprintf(buf9, buf7, drive_letters(buf8, drivemap_safeeject^drivemap_ejd));
                            }
                            strcat(buf2, buf9);

                            {
                                char    b[100] = "\0";
                                sprintf(b, "MASK-SE: %08lX, MASK-EJ: %08lX", drivemap_safeeject, drivemap_ejd);
                                __debug("MASKS", b, DBG_LBOX | DBG_AUX);
                                __debug("DRIVES EJECTED", buf2, DBG_LBOX | DBG_AUX);
                            }
                        }
                    }
                    else {
                        char    b[100] = "\0";
                        sprintf(b, "GONE-MASK: %08lX, MASK-EJ: %08lX", g_drives_gone);
                        __debug("MASKS", b, DBG_LBOX | DBG_AUX);
                        __debug("DRIVES EJECTED", drive_letters(buf, g_drives_gone), DBG_LBOX | DBG_AUX);


                        if (drive_count(g_drives_gone) == 1) {
                            sprintf(buf2, "Drive %ssafely ejected", drive_letters(buf, g_drives_gone));
                        }
                        else {
                            sprintf(buf2, "Drives %ssafely ejected", drive_letters(buf, g_drives_gone));
                        }
                    }


                    foreign_eject = TRUE;


                    GpiCharStringAt(hps, &ptl, strlen(buf2), (PSZ)buf2);

                    GpiSetCharSet(hps, 0L);

                    LangeGetString(BatteryLange, "You can now safely remove", (char*)buf, sizeof(buf));
                    strcat(buf,"\n");

                    if (usbdev) {
                        sprintf(buf2,"\"%s\"", usbdev->devicename);
                    }
                    else {
                        char buf3[256] = "\0";
                        LangeGetString(BatteryLange, "Mass Storage Device", (char*)buf3, sizeof(buf3));
                        sprintf(buf2,"\"%s\"", buf3);
                        //sprintf(buf2,"\"%s\"", "Mass Storage Device");
                    }


                    strcat(buf,buf2);


                    POINTL aptl[TXTBOX_COUNT];
                    double w, h;
                    POINTL ptl;


                    if(buf) GpiQueryTextBox(hps, strlen(buf), buf, TXTBOX_COUNT, aptl);
                    w=aptl[2].x;
                    h=aptl[2].y;

                    //DrawRect(DrawAt->x, DrawAt->y, w, h, PaperColor);

                    //ptl.x=8+64+8;
                    //ptl.y=20;

                    GpiSetColor(hps, CLR_BLACK);
                    //if(buf) GpiCharStringAt(hps, &ptl, strlen(buf), buf);

                    RECTL rtl;

                    rtl.xLeft=8+64+8;
                    rtl.xRight=256+64;
                    rtl.yBottom=0;
                    rtl.yTop=64-8;




                    LONG    hText;
                    LONG    hTotalDrawn=0;
                    LONG    hDrawn=0;
                    LONG    CharHeight;                                 // set character height
                    char*   pszText=buf;
                    //LONG hText;

                    hText = (LONG)strlen(pszText);                      // get length of string
                    CharHeight = 15;                                    // set character height

                    /* until all chars drawn */
                    for (hTotalDrawn = 0;  hTotalDrawn !=  hText;
                                        rtl.yTop -= CharHeight) {

                        /* draw the text */
                        hDrawn = WinDrawText(hps,                       // presentation-space handle
                                                hText -  hTotalDrawn,   // length of text to draw
                                                pszText +  hTotalDrawn, // address of the text
                                                &rtl,                   // rectangle to draw in
                                                0L,                     // foreground color
                                                0L,                     // background color
                                                DT_WORDBREAK | DT_TOP | DT_LEFT | DT_TEXTATTRS);

                        if (hDrawn)
                            hTotalDrawn +=  hDrawn;
                        else
                            break;
                    }
                    break;
                } // SHOW_SAFE_EJECT




                /*
                // DEVICE REMOVED.
                */
                case SHOW_DEV_REMOVE: {
                    WinFillRect(hps,&rec, CLR_YELLOW);

                    DrawRect(0,0,
                                variables.devremove_banner_width-1,
                                variables.devremove_banner_height-1,
                                CLR_RED);

                    lbPos.x=8; lbPos.y=28;

                    //WinDrawBitmap(hps, hbmIconWarning, NULL, &lbPos,0,0, DBM_NORMAL);



                    /*
                    // Experimenting with embedded PNG's.
                    */
                    // FL_STRIPALPHA        // Ignore the aplha channel
                    // FL_PROCESSALPHA      // Process alpha by mixing image with specified background color
                    // FL_COPYALPHA         // Copy alpha channel when blitting                                 (*)
                    // FL_USEBKGD           // Use bKGD PNG chunk to determine background color                 (*)
                    {
                        BOOL        brc = FALSE;
                        APIRET      ulrc = -1;
                        ULONG       pngsize = 0;
                        BYTE*       pngimage = NULL;
                        HBITMAP2    hbm2 = NULL;
                        //POINTL        lbPos;
                        POINTL      aptlPoints[3];
                        CHAR        txtbuf[1024] = "\0";

                        hbm2 = Gpi2LoadPng(hmodMe, ID_PNG_WARNING, 0x808080, FL_COPYALPHA);
                        aptlPoints[0].x = lbPos.x;
                        aptlPoints[0].y = lbPos.y+15;
                        ulrc = Gpi2DrawBitmap(hps, hbm2, 1, aptlPoints);
                        Gpi2DeleteBitmap(hbm2);
                    }


                    GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &fm);
                    memset(&fat, 0, sizeof(fat));
                    fat.usRecordLength  = sizeof(FATTRS);
                    fat.lMatch        = fm.lMatch;

                    strcpy(fat.szFacename, fm.szFacename);
                    fat.fsSelection  = FATTR_SEL_BOLD;
                    GpiCreateLogFont(hps, 0, 1L, &fat);

//                  rcl=((POWNERITEM)mp2)->rclItem;
//                  rcl.xLeft+=CX_ICON+10;

                    GpiSetCharSet(hps, 1L);

                    ptl.x=8+64+8; ptl.y=96-16;
                    //LangeGetString(BatteryLange, "Force Remove %c:", (char*)buf, sizeof(buf));
                    LangeGetString(BatteryLange, "Forced Removal -- Possible Data Loss !", (char*)buf, sizeof(buf));
                    sprintf(buf2, buf, drvletter_safeeject);
                    GpiCharStringAt(hps, &ptl, strlen(buf2), (PSZ)buf2);

                    GpiSetCharSet(hps, 0L);

                    LangeGetString(BatteryLange,"A device has been removed without proper eject !", (char*)buf2, sizeof(buf2));
                    //strcat(buf,buf2);
                    //strcat(buf,"\n");
                    ptl.x=8+64+8; ptl.y=96-16-8-16;
                    GpiCharStringAt(hps, &ptl, strlen(buf2), (PSZ)buf2);


                    LangeGetString(BatteryLange,"Please reinsert it and do a proper eject to prevent", (char*)buf2, sizeof(buf2));
                    //strcat(buf,buf2);
                    //strcat(buf,"\n");
                    ptl.x=8+64+8; ptl.y=96-16-8-16-16;
                    GpiCharStringAt(hps, &ptl, strlen(buf2), (PSZ)buf2);

                    LangeGetString(BatteryLange,"the system from becoming unstable.", (char*)buf2, sizeof(buf2));
//                  strcat(buf,buf2);
                    ptl.x=8+64+8; ptl.y=96-16-8-16-16-16;
                    GpiCharStringAt(hps, &ptl, strlen(buf2), (PSZ)buf2);



                    POINTL aptl[TXTBOX_COUNT];
                    double w, h;
                    POINTL ptl;


                    if(buf) GpiQueryTextBox(hps, strlen(buf), buf, TXTBOX_COUNT, aptl);
                    w=aptl[2].x;
                    h=aptl[2].y;

                    //DrawRect(DrawAt->x, DrawAt->y, w, h, PaperColor);

                    //ptl.x=8+64+8;
                    //ptl.y=20;

                    GpiSetColor(hps, CLR_RED);
                    //if(buf) GpiCharStringAt(hps, &ptl, strlen(buf), buf);

                    RECTL rtl;

                    rtl.xLeft=8+64+8;
                    rtl.xRight=360+64;
                    rtl.yBottom=0;
                    rtl.yTop=96-8-16;

                    LONG    hText;
                    LONG    hTotalDrawn=0;
                    LONG    hDrawn=0;
                    LONG    CharHeight;                                 // set character height
                    char*   pszText=buf;
                    //LONG hText;

                    hText = (LONG)strlen(pszText);                      // get length of string
                    CharHeight = 15;                                    // set character height

                    // Nu even niet
                    hText=0;

                    /* until all chars drawn */
                    for (hTotalDrawn = 0;  hTotalDrawn !=  hText;
                                        rtl.yTop -= CharHeight) {

                        /* draw the text */
                        hDrawn = WinDrawText(hps,                       // presentation-space handle
                                                hText -  hTotalDrawn,   // length of text to draw
                                                pszText +  hTotalDrawn, // address of the text
                                                &rtl,                   // rectangle to draw in
                                                0L,                     // foreground color
                                                0L,                     // background color
                                                DT_WORDBREAK | DT_TOP | DT_LEFT | DT_TEXTATTRS);

                        if (hDrawn)
                            hTotalDrawn +=  hDrawn;
                        else
                            break;
                    }
                    break;
                } // SHOW_DEV_REMOVE


                /*
                // DRIVES CHANGED.
                */
                //!: --------------------------------------- [ DRIVES CHANGED ]
                case SHOW_DRIVES_CHANGED: {
                    CHAR*   txt_removed     = "Drives Removed";
                    CHAR*   txt_added       = "Drives Added";
                    CHAR*   txt_phantoms    = "Phantoms Recovered";

                    WinFillRect(hps,&rec, CLR_WHITE);

                    DrawRect(0,0,
                                variables.driveschanged_banner_width-1,
                                variables.driveschanged_banner_height-1,
                                CLR_BLACK);

                    lbPos.x=8; lbPos.y=28;

                    //WinDrawBitmap(hps, hbmIconWarning, NULL, &lbPos,0,0, DBM_NORMAL);



                    /*
                    // Experimenting with embedded PNG's.
                    */
                    // FL_STRIPALPHA        // Ignore the aplha channel
                    // FL_PROCESSALPHA      // Process alpha by mixing image with specified background color
                    // FL_COPYALPHA         // Copy alpha channel when blitting                                 (*)
                    // FL_USEBKGD           // Use bKGD PNG chunk to determine background color                 (*)
                    {
                        BOOL        brc = FALSE;
                        APIRET      ulrc = -1;
                        ULONG       pngsize = 0;
                        BYTE*       pngimage = NULL;
                        HBITMAP2    hbm2 = NULL;
                        //POINTL        lbPos;
                        POINTL      aptlPoints[3];
                        CHAR        txtbuf[1024] = "\0";

                        hbm2 = Gpi2LoadPng(hmodMe, ID_PNG_INFORMATION, 0x808080, FL_COPYALPHA);
                        aptlPoints[0].x = lbPos.x;
                        aptlPoints[0].y = lbPos.y;
                        ulrc = Gpi2DrawBitmap(hps, hbm2, 1, aptlPoints);
                        Gpi2DeleteBitmap(hbm2);
                    }


                    GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &fm);

                    //fm.lEmHeight +=10;

                    memset(&fat, 0, sizeof(fat));
                    fat.usRecordLength  = sizeof(FATTRS);
                    fat.lMatch        = fm.lMatch;

                    strcpy(fat.szFacename, fm.szFacename);
                    fat.fsSelection  = FATTR_SEL_BOLD;
                    GpiCreateLogFont(hps, 0, 1L, &fat);

//                  rcl=((POWNERITEM)mp2)->rclItem;
//                  rcl.xLeft+=CX_ICON+10;

                    GpiSetCharSet(hps, 1L);

                    ptl.x=8+64+8; ptl.y=96-16-10;   // Postition 1st line
                    //LangeGetString(BatteryLange, "Dev Remove %c:", (char*)buf, sizeof(buf));
                    if (usbdev) {
                        if (usbdev->iamnew) {
                            LangeGetString(BatteryLange, txt_added, (char*)buf, sizeof(buf));
                        }
                        else {
                            LangeGetString(BatteryLange, txt_removed, (char*)buf, sizeof(buf));
                        }
                        //!: THE DEVICE WAS A PHANTOM
                        if (usbdev->was_phantom) {
                            LangeGetString(BatteryLange, txt_phantoms, (char*)buf, sizeof(buf));
                        //  //usbdev->was_phantom = FALSE;
                        }

                    }


                    sprintf(buf2, buf, drvletter_safeeject);
                    GpiCharStringAt(hps, &ptl, strlen(buf2), (PSZ)buf2);

                    //GpiSetCharSet(hps, 0L);

                    /*
                    LangeGetString(
                        BatteryLange,
                        "Drives Changed.",
                        (char*)buf, sizeof(buf));
                    */

                    if (usbdev) {
                        drive_letters(buf, usbdev->drivemap);   // drivemap
                    }

                    //strcat(buf, " -- ");
                    //strcat(buf, fm.szFamilyname);


                    //strcat(buf, "\n");
                    //sprintf(buf2, "new=%d, gone=%d, safe_eject=%d", usbdev->iamnew, usbdev->removed, usbdev->safe_eject);
                    //sprintf(buf2, "notifications=%d", WinIsWindowVisible(hwndIndiDevInsert));
                    //strcat(buf, buf2);


                    POINTL aptl[TXTBOX_COUNT];
                    double w, h;
                    POINTL ptl;


                    if(buf) GpiQueryTextBox(hps, strlen(buf), buf, TXTBOX_COUNT, aptl);
                    w=aptl[2].x;
                    h=aptl[2].y;

                    //DrawRect(DrawAt->x, DrawAt->y, w, h, PaperColor);

                    //ptl.x=8+64+8;
                    //ptl.y=20;

                    GpiSetColor(hps, CLR_BLUE);
                    //if(buf) GpiCharStringAt(hps, &ptl, strlen(buf), buf);

                    RECTL rtl;

                    rtl.xLeft=8+64+8+20;
                    rtl.xRight=384+64;
                    rtl.yBottom=0;
                    rtl.yTop=96-8-16-10;                                // Position 2nd line

                    LONG    hText;
                    LONG    hTotalDrawn=0;
                    LONG    hDrawn=0;
                    LONG    CharHeight;                                 // set character height
                    char*   pszText=buf;
                    //LONG hText;

                    hText = (LONG)strlen(pszText);                      // get length of string
                    CharHeight = 15;                                    // set character height

                    /* until all chars drawn */
                    for (hTotalDrawn = 0;  hTotalDrawn !=  hText;
                                        rtl.yTop -= CharHeight) {

                        /* draw the text */
                        hDrawn = WinDrawText(hps,                       // presentation-space handle
                                                hText -  hTotalDrawn,   // length of text to draw
                                                pszText +  hTotalDrawn, // address of the text
                                                &rtl,                   // rectangle to draw in
                                                0L,                     // foreground color
                                                0L,                     // background color
                                                DT_WORDBREAK | DT_TOP | DT_LEFT | DT_TEXTATTRS);

                        if (hDrawn)
                            hTotalDrawn +=  hDrawn;
                        else
                            break;
                    }
                    break;
                } // SHOW_DRIVES_CHANGED





            } // switch (action)
            WinEndPaint(hps);
            break;
        } // WM_PAINT

        /*
        // Hide window when clicked on.
        */
        case WM_CONTROL:
        case WM_BUTTON1DOWN:
        case WM_COMMAND: {
            WinShowWindow(hwnd, FALSE);
            break;
        }


    } // switch (msg)


    return WinDefWindowProc( hwnd, msg, mp1, mp2 );
}


//!: << PopUpChangedDevices >>
/*
// ############################################################################
// # PopUpChangedDevices
// ############################################################################
*/
int PopUpChangedDevices(int action, USBDeviceReport* usbdev) {

    RECTL       rcl;
    POINTL      ptl;
    SWP         swp = { 0 };

    int         xpos,ypos, width, height, eCenterH;
    HWND        hwnd, hWndTemp;
    char        buf[256];
    ULONG       timerid;
    CHAR*       nodev = "NO DEVICENAME";
    CHAR*       devname = NULL;
    BOOL        notifications = FALSE;

    if (usbdev)
        devname = usbdev->devicename;
    else
        devname = nodev;

    if (!MyWidget)
        return 1;

    PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)MyWidget->pUser;


    if (pPrivate == NULL)
        return 1;

    //sprintf(buf, "PopUpChangedDevices");
    //__debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

    hwnd=MyWidget->hwndWidget;
    //hwndWidget;



    /*
    // Setup the notification-window for
    // the corresponding action.
    */
    switch(action) {

        case SHOW_DEV_INSERT: {
            hWndTemp    = hwndIndiDevInsert;
            timerid     = TIMER_SHOW_BANNER_DEV_INSERT;
            width       = variables.devinsert_banner_width;
            height      = variables.devinsert_banner_height;
            sprintf(buf, "PopUpChangedDevices: hwndIndiDevInsert=%08X, w=%d, h=%d, devname=%s",
                hwndIndiDevInsert,
                width,
                height,
                devname);

            __debug(NULL, buf, DBG_LBOX);
            /* Start the timer for this window */
            WinStartTimer(myhab, hwndIndiDevInsert, timerid, TIMEOUT_BANNER_DEV_INSERT);
            break;
        }

        case SHOW_SAFE_EJECT: {
            hWndTemp    = hwndIndiSafeEject;
            timerid     = TIMER_SHOW_BANNER_SAFE_EJECT;
            width       = variables.safeeject_banner_width;
            height      = variables.safeeject_banner_height;
            sprintf(buf, "PopUpChangedDevices: hwndIndiSafeEject=%08X, w=%d, h=%d",
                hwndIndiSafeEject,
                width,
                height);
            __debug(NULL, buf, DBG_LBOX);
            /* Start the timer for this window */
            WinStartTimer(myhab, hwndIndiSafeEject, timerid, TIMEOUT_BANNER_SAFE_EJECT);
            break;
        }

        case SHOW_DEV_REMOVE: {
            hWndTemp    = hwndIndiDevRemove;
            timerid     = TIMER_SHOW_BANNER_DEV_REMOVE;
            width       = variables.devremove_banner_width;
            height      = variables.devremove_banner_height;
            sprintf(buf, "PopUpChangedDevices: hwndIndiDevRemove=%08X, w=%d, h=%d",
                hwndIndiDevRemove,
                width,
                height);
            __debug(NULL, buf, DBG_LBOX);
            /* Start the timer for this window */
            WinStartTimer(myhab, hwndIndiDevRemove, timerid, TIMEOUT_BANNER_DEV_REMOVE);
            break;
        }

        case SHOW_DRIVES_CHANGED: {
            hWndTemp    = hwndIndiDrivesChanged;
            timerid     = TIMER_SHOW_BANNER_DRIVES_CHANGED;
            width       = variables.driveschanged_banner_width;
            height      = variables.driveschanged_banner_height;
            sprintf(buf, "PopUpChangedDevices: hwndIndiDrivesChanged=%08X, w=%d, h=%d",
                hwndIndiDrivesChanged,
                width,
                height);
            __debug(NULL, buf, DBG_LBOX);
            /* Start the timer for this window */
            WinStartTimer(myhab, hwndIndiDrivesChanged, timerid, TIMEOUT_BANNER_DRIVES_CHANGED);
            break;
        }
    }

    /* Query widget-window */
    WinQueryWindowRect( hwnd, &rcl );
    ptl.y = rcl.yBottom;
    ptl.x = rcl.xLeft;

    eCenterH=rcl.yTop-rcl.yBottom;

    //sprintf(buf, "Top: rect: %d, %d, %d, %d", rcl.xLeft, rcl.yBottom, rcl.xRight, rcl.yTop);
    //__debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

    WinMapWindowPoints(hwnd, HWND_DESKTOP, &ptl, 1);


    notifications = WinIsWindowVisible(hwndIndiDevInsert) ||
                    WinIsWindowVisible(hwndIndiDevRemove) ||
                    WinIsWindowVisible(hwndIndiSafeEject);

    /* Position the notification-window */
    if(ptl.x<cxDesktop/2) {
        xpos=ptl.x;
        if (hWndTemp == hwndIndiDrivesChanged) {
            xpos += 0;
        }
    }
    else {
        xpos=ptl.x-width;
        if (hWndTemp == hwndIndiDrivesChanged) {
            xpos -= 0;
        }
    }

    //WinQueryWindowPos(hwnd,&swp);


    // Place popup according to xCenter position
    if ( MyWidget->pGlobals->ulPosition == XCENTER_BOTTOM ) {
        ypos=ptl.y+eCenterH+8;                                          // 8-pixels offset
        if ((hWndTemp == hwndIndiDrivesChanged) && notifications) {
            ypos += (96+2);
        }

    }
    else {
        ypos=ptl.y-height-8; // -eCenterH                               // 8-pixels offset
        if ((hWndTemp == hwndIndiDrivesChanged) && notifications) {
            ypos -= (96+2);
        }
    }

    //swp.fl = SWP_SIZE;
    //WinSendMsg(hwndPopup, WM_ADJUSTWINDOWPOS, MPFROMP(&swp), 0);
    //ptl.y -= swp.cy;


    //sprintf(buf, "Top: before: %d, %d, after: %d, %d", ptl.x, ptl.y, xpos, ypos);
    //__debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);



    /*
    // Position and show
    // the notification-window.
    */
    WinSetWindowPos(hWndTemp, HWND_TOP, xpos, ypos, width, height,
                    SWP_ZORDER | SWP_ACTIVATE  | SWP_SHOW |SWP_SIZE|SWP_MOVE);


    /*
    // Pass the usb-device to the notification window.
    // This uses a global var !! veranderen !!
    // Local window data gebruiken !!
    */
    WinSendMsg(hWndTemp, WM_DEVINFO, (MPARAM) usbdev, (MPARAM) NULL);

    /*
    // Force a repaint of the window so the info get's updated.
    */
    WinInvalidateRect(hWndTemp, NULL, FALSE);


    WinShowWindow(hWndTemp, TRUE);

    return 0;

}

//!: EjectThread was here






///
//!: -------------------------------------------------- [ Exported procedures ]
///

//!: << WgtInitModule >> (DLL MODULE INITIALIZATION)
/**
 *@@ WgtInitModule:
 *    required export with ordinal 1, which must tell
 *    the XCenter how many widgets this DLL provides,
 *    and give the XCenter an array of XCENTERWIDGETCLASS
 *    structures describing the widgets.
 *
 *    With this call, you are given the module handle of
 *    XFLDR.DLL. For convenience, you may resolve imports
 *    for some useful functions which are exported thru
 *    src\shared\xwp.def. See the code below.
 *
 *    This function must also register the PM window classes
 *    which are specified in the XCENTERWIDGETCLASS array
 *    entries. For this, you are given a HAB which you
 *    should pass to WinRegisterClass. For the window
 *    class style (4th param to WinRegisterClass),
 *    you should specify
 *
 +        CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT
 *
 *    Your widget window _will_ be resized, even if you're
 *    not planning it to be.
 *
 *    This function only gets called _once_ when the widget
 *    DLL has been successfully loaded by the XCenter. If
 *    there are several instances of a widget running (in
 *    the same or in several XCenters), this function does
 *    not get called again. However, since the XCenter unloads
 *    the widget DLLs again if they are no longer referenced
 *    by any XCenter, this might get called again when the
 *    DLL is re-loaded.
 *
 *    There will ever be only one load occurence of the DLL.
 *    The XCenter manages sharing the DLL between several
 *    XCenters. As a result, it doesn't matter if the DLL
 *    has INITINSTANCE etc. set or not.
 *
 *    If this returns 0, this is considered an error, and the
 *    DLL will be unloaded again immediately.
 *
 *    If this returns any value > 0, *ppaClasses must be
 *    set to a static array (best placed in the DLL's
 *    global data) of XCENTERWIDGETCLASS structures,
 *    which must have as many entries as the return value.
 */
ULONG EXPENTRY WgtInitModule(HAB hab,                            // XCenter's anchor block
                                HMODULE hmodPlugin,                 // module handle of the widget DLL
                                HMODULE hmodXFLDR,                  // XFLDR.DLL module handle
                                PXCENTERWIDGETCLASS *ppaClasses,
                                PSZ pszErrorMsg) {                  // if 0 is returned, 500 bytes of error msg

    APIRET          ulrc            = -1;
    ULONG           ul              = 0;
    BOOL            fImportsFailed  = FALSE;
    char            temp[128];
    ULONG           ulAction;
    ULONG           ulRead;
    ULONG           ulWrite=0;
    ULONG           ulReply;
    char            buf[256]        = "\0";
    char            buf2[256]       = "\0";
    int             i;
    BOOL            brc;
    HEV             wait4daemon     = NULL;
    PMASTERMEMPOOL  pmmp            = NULL;
    POOLTYPE        pt              = PT_USE_NAMED;

    /* Assign these to globals */
    myhab   = hab;
    hmodMe  = hmodPlugin;

    //!: Init Beep
    //~ DosBeep(4000, 20);

    //~ MessageBox("usbimpl", "WgtInitModule");


#if     DEBUG

    /* Create Debug Dialog */
    CreateDebugDialog();

    /* Show it */
    ShowDebugDialog();

    __debug("WidgetInitModule", "START", DBG_LBOX);

    //~ __debug("WidgetInitModule", "  >> Create Debug Dialog", DBG_AUX);
    //~ __debug("WidgetInitModule", "  >> Position Debug Dialog", DBG_AUX);

    //~ MessageBox("Message", "Debug Mode !");
#else
    hdlgDebugDialog = NULL;
    //~ MessageBox("No Debug !", "Message");
#endif

    //!: Start Beep
    //~ DosBeep(7000, 20);
    //~ __debug("After Debug Dialog Creation", NULL, DBG_POPUP);

    __debug("WidgetInitModule", "  >> Create hevNeverPosted Event Semaphore", DBG_LBOX);

    ulrc = DosCreateEventSem(NULL, &hevNeverPosted, DC_SEM_SHARED, FALSE);
    sprintf(buf, "ulrc:%d", ulrc);
    __debug("DosCreateEventSem(hevNeverPosted)", buf, DBG_LBOX);

    /*
    // Let's go OO if g_use_new_implementation is set.
    */
    //~ if (g_use_new_implementation) {
        //~ g_myUsbWidget = new UsbWidget("Hello from UsbWidget Class");
        //~ g_myUsbWidget->init();
        //~ g_myUsbWidget->startHandlingEvents();
        //~ g_myUsbWidget->stopHandlingEvents();
        //~ g_myUsbWidget->uninit();
        //~ delete g_myUsbWidget;
    //~ }

    __debug("WidgetInitModule", "  >> Query Module Name", DBG_LBOX);

    /* Get the full pathname of the widget dll */
    ulrc = DosQueryModuleName(hmodMe, sizeof(buf2), buf2);
    sprintf(buf, "DosQueryModuleName : %s, ulrc: %d", buf2, ulrc);
    __debug(NULL, buf, DBG_MLE);

    __debug("WidgetInitModule", "  >> Construct DFSVOS2.BIN name", DBG_AUX);

    /* Construct DFSVOS2.BIN name */
    {
        char*   p = 0;
        int     s = 0;
        strcpy(g_dfsvos2, buf2);
        s = strlen(g_dfsvos2);
        p = g_dfsvos2 + s;
        p -= 12;
        *p = '\0';
        strcat(p, "DFSVOS2.BIN");
    }

    __debug("WidgetInitModule", "  >> See if DFSVOS2.BIN exists", DBG_AUX);

    /* See if the DFSVOS2.BIN version exists */
    g_dfsvos2_exists = file_exists(g_dfsvos2);

    /* If not, compose the name of DFSVOS2.EXE in ?:\eCS\Install */
    if (!g_dfsvos2_exists) {
        sprintf(g_dfsvos2, "%c:\\eCS\\Install\\DFSVOS2.EXE", QueryBootDrive());
    }

    __debug("WidgetInitModule", "  >> See if DFSVOS2.EXE exists", DBG_AUX);

    /* See if the DFSVOS2.EXE version exists */
    g_dfsvos2_exists = file_exists(g_dfsvos2);

    strcat(g_dfsvos2, g_dfsvos2_flags);
    sprintf(buf, "%s -- exists:%d", g_dfsvos2, g_dfsvos2_exists);
    __debug("Name of DFSVOS2    ", buf, DBG_MLE);
    //~ MessageBox("Module Name", buf2);

    __debug("WidgetInitModule", "  >> Construct DAEMON name", DBG_LBOX);

    /* Construct DAEMON name */
    {
        char*   p = 0;
        int     s = 0;
        s = strlen(buf2);
        p = buf2 + s;
        p -= 7;
        *p = '\0';
        strcat(p,"LPR.DMN");
    }

    sprintf(buf, "%s", buf2);
    __debug("Name of DAEMON     ", buf, DBG_LBOX);

    __debug("WidgetInitModule", "  >> Create Named wait4daemon Event Semaphore", DBG_LBOX);

    /*
    // Create a named shared semaphore and clear it.
    // The daemon will post it when mem is available.
    // Will return "DUPLICATE_NAME" if already exists.
    */
    ulrc = DosCreateEventSem(EVSEM_WAIT4DAEMON, &wait4daemon, DC_SEM_SHARED, FALSE);
    sprintf(buf, "ulrc:%d", ulrc);
    __debug("DosCreateEventSem(wait4daemon)", buf, DBG_LBOX);

    __debug("WidgetInitModule", "  >> Try connect to Named Shared Memory", DBG_LBOX);

    /* Try to connect to shared memory from daemon */
    ulrc = DosGetNamedSharedMem(&masterpool, SHMEM_NAME, PAG_READ | PAG_WRITE);
    sprintf(buf, "masterpool: %08lX, ulrc: %d", masterpool, ulrc);
    __debug("DosGetNamedSharedMem", buf, DBG_MLE);


    if (ulrc) {     // ulrc != 0 -> shmem not found

        __debug("WidgetInitModule", "  >> Start DAEMON", DBG_LBOX);

        //!: Start the Daemon
        ulrc = StartDaemon(buf2);
        //~ ulrc = StartDaemonAsSession(buf2);
        sprintf(buf, "ulrc: %d", ulrc);
        __debug("StartDaemon", buf, DBG_LBOX);

        if (ulrc) {
            // ERROR, COULD NOT START DAEMON !
            __debug("WidgetInitModule", "  >> **ERROR** Could not start DAEMON", DBG_LBOX);
            /*
            // Cleanup the resources allocated until now.
            */
            {
                if (hevNeverPosted) {
                    DosPostEventSem(hevNeverPosted);
                    DosCloseEventSem(hevNeverPosted);
                }

                if (wait4daemon) {
                    DosPostEventSem(wait4daemon);
                    DosCloseEventSem(wait4daemon);
                }

                if (hdlgDebugDialog) {
                    WinDestroyWindow(hdlgDebugDialog);
                        hdlgDebugDialog = NULL;
                }

                MessageBox(
                    "USB Widget",
                    "The Widget Daemon could not be started.\n\n"
                    "Please ensure usbshlpr.dmn is in the same directory\n"
                    "as usbshold.dll\n\n"
                    "After fixing this you need to restart eCenter and use\n"
                    "'Create new widget' from it's Properties Menu to add the USB Widget.\n\n"
                    "** The USB Widget is not available at this time **"
                );


                /* Return indicating error */
                return 0;
            }

            //!: Allocating from local memory does not work yet !!

            /* Allocate from local memory */
            //~ masterpool = allocate_master_pool(PT_USE_OS2);
            //~ __debug("AFTER allocate_master_pool(PT_USE_OS2)", NULL, DBG_POPUP);
        }
        else { // DAEMON STARTED

            __debug("WidgetInitModule", "  >> Wait 2 secs on wait4daemon Event Semaphore", DBG_LBOX);

            /* Wait 2 secs for the daemon */
            ulrc = DosWaitEventSem(wait4daemon, 2000);

            __debug("WidgetInitModule", "  >> Get Shared Memory now DAEMON is started", DBG_LBOX);

             //~ MessageBox("Master Pool :: Daemon Started", buf);
            //!: DosSleep weer unresolved external...
            //~ MyDosSleep(2000);
            /* Again try to connect to shared memory from daemon */
            ulrc = DosGetNamedSharedMem(&masterpool, SHMEM_NAME, PAG_READ | PAG_WRITE);
            sprintf(buf, "masterpool: %08lX, poolsize: %d, ulrc: %d", masterpool, ((PMASTERMEMPOOL)masterpool)->poolsize, ulrc);
            __debug("DosGetNamedSharedMem", buf, DBG_LBOX);

            if (ulrc) { // ulrc !=0 -> shmem not found
                __debug("WidgetInitModule", "  >> DAEMON possibly stared but no SHMEM", DBG_LBOX);
                ulrc = -1;  // force failure for now
                // ERROR, DAEMON STARTED BUT NO NAMED SHARED MEM
                __debug("DAEMON but no SHMEM", NULL, DBG_LBOX);
            }
            else {
                __debug("WidgetInitModule", "  >> Using SHMEM from DAEMON", DBG_LBOX);
                // USING NAMED SHARED MEM FROM DAEMON
                pmmp = (PMASTERMEMPOOL) masterpool;
                sprintf(buf, "SHARED MEMORY FOUND AFTER STARTING DAEMON: Signature: %08lX", pmmp->signature);
                __debug("DosGetNamedSharedMem", buf, DBG_LBOX);
            }
        }
    }
    else {
        __debug("WidgetInitModule", "  >> SHMEM found at first try", DBG_LBOX);
        // SHMEM FOUND RIGHT AWAY
        pmmp = (PMASTERMEMPOOL) masterpool;
        //~ MessageBox("Master Pool II", buf);
        sprintf(buf, "EXISTING SHARED MEMORY FOUND AT FIRST TRY: Signature: %08lX", pmmp->signature);
        __debug("DosGetNamedSharedMem", buf, DBG_LBOX);

        //~ MessageBox("Master Pool III", buf);
    }


    //~ sprintf(buf, "pmmp:%08lX, pmmp->pwidgetstate:%08lX, pws:%08lX, ulrc:%d",
        //~ pmmp, pmmp->pwidgetstate, pws, ulrc);
    //~ __debug("XX:After memfind/allocate", buf, DBG_POPUP);

    /* If connected to named shared memory we have to attach to the submempool */
    if (pmmp->pooltype == PT_USE_NAMED) {
        __debug("WidgetInitModule", "  >> Attaching to SubMemPool", DBG_LBOX);
        //~ __debug("XXa", "Attaching", DBG_POPUP);
        ulrc = DosSubSetMem(pmmp->submempool, 0, SHMEM_SIZE+sizeof(SUBMEMPOOL));
        sprintf(buf, "pmmp->submempool: %08lX, pooltype: %d, widgetstate: %08lX, ulrc: %d",
            pmmp->submempool, pmmp->pooltype, pmmp->pwidgetstate, ulrc);
        __debug("DosSubSetMem(attach)", buf, DBG_LBOX);
    }
    /* Otherwise it was initialized by allocate_master_pool */
    else {
        //~ __debug("XXb", "Already allocated", DBG_POPUP);
        //~ sprintf(buf, "Using Local Memory: pmmp->submempool: %08lX, pooltype: %d, widgetstate: %08lX, ulrc: %d",
            //~ pmmp->submempool, pmmp->pooltype, pmmp->pwidgetstate, ulrc);
        //~ __debug(NULL, buf, DBG_LBOX);
    }

    //~ sprintf(buf, "pmmp:%08lX, pmmp->pwidgetstate:%08lX, pws:%08lX, ulrc:%d",
        //~ pmmp, pmmp->pwidgetstate, pws, ulrc);
    //~ __debug("YY:Before allocation Widget State", buf, DBG_POPUP);

    /* Allocate widget state if not allocated yet */
    //!: TEMP DISABLE RETAIN STATE !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if (!pmmp->pwidgetstate) {
        __debug("WidgetInitModule", "  >> Allocating Widget State", DBG_LBOX);
        __debug(NULL, "Allocating Widget State", DBG_LBOX);
        pmmp->pwidgetstate = (PWIDGETSTATE) malloc(sizeof(WIDGETSTATE));
        //!: CHECKME: Check for failure here
        if (pws)
            free(pws);
        pws = (PWIDGETSTATE) pmmp->pwidgetstate;


        //!: ---------------------------- [ Initialize Widget State structure ]
        /**
         * Must be done *only* when *no signature* is found.
         * Should be moved to widget creation because widget remove/add *does*
         * retain the state because the DLL does not get unloaded.
         * The DLL only gets unloaded on a WPS restart.
         */

        /* Set the signature to indicate the structure is valid */
        pws->signature = WGTST_SIGNATURE;

        pws->wait4daemon = NULL;

        /* First entry is not used */
        pws->pdisks[0].usbid = -1;

        __debug("WidgetInitModule", "  >> Initializing Disk Array", DBG_LBOX);

        /*
        // Clear the array of physical disks.
        // This array contains the relations between usb dasd-devices and physical disks.
        // Note that this array persists between widget delete and adds.
        */
        //~ for (i=0; i<=sizeof(pws->pdisks)/sizeof(PHYSICAL_DISK); i++) {
        //!: ------------------------------------------- [ Clear pdisks array ]
        for (i=0; i<sizeof(pws->pdisks)/sizeof(PHYSICAL_DISK); i++) {
            pws->pdisks[i].drivemap = 0;
            pws->pdisks[i].present = FALSE;
            pws->pdisks[i].safe_eject = FALSE;
            pws->pdisks[i].usbid = 0;
            pws->pdisks[i].zipdrive = FALSE;
        }

        //!: ----------------------------------------- [ Clear phantoms array ]
        for (i=0; i<sizeof(pws->phantoms); i++) {
            pws->phantoms[i] = FALSE;
        }

        //~ MessageBox("Master Pool VII (pdisks initialized)", buf);


        //!: ------------------------------------ [ Allocate USB-device Lists ]
        /**
         * Allocate USB Devices Lists.
         * When using shared memory the new operator is overloaded to allocate
         * from the shared memory instead of the local heap.
         */

        __debug("WidgetInitModule", "  >> Creating New Device Lists", DBG_LBOX);
        pws->currentUSBDevicesList  = new CList();  // List of current devices
        pws->previousUSBDevicesList = new CList();  // List of previous devices
        pws->removedUSBDevicesList  = new CList();  // List of removed devices
    }
    else {
        __debug(__FUNCTION__, "WARM START", DBG_LBOX | DBG_AUX);
        warm_start = TRUE;
    }

    pws = (PWIDGETSTATE) pmmp->pwidgetstate;


    sprintf(buf, "Widget State -- ulrc: %d, address: %08lX, pooltype: %d", ulrc, pws, ((PMASTERMEMPOOL)masterpool)->pooltype);
    __debug(NULL, buf, DBG_LBOX);


    //ulrc = DosGetNamedSharedMem(&shmem, SHMEM_NAME, PAG_READ | PAG_WRITE);
    //~ sprintf(buf, "DosGetNamedSharedMem -- ulrc: %d, address: %08lX", ulrc, shmem);

    //~ MessageBox("Master Pool VIII (CLists initialized)", buf);

    /* Process */
    //ulReply = WinProcessDlg(hdlgDebugDialog);


    //~ sprintf(buf, "sizeof(pws->pdisks): %d,%d", sizeof(pws->pdisks),sizeof(PHYSICAL_DISK));
    //~ __debug(NULL, buf, DBG_MLE);


    //!: CHECKME: sizeof()
    //~ sprintf(buf, "Processed Init (%d)", sizeof(pws->pdisks));

    //~ OpenPipe();

    __debug("WidgetInitModule", "  >> Resolving Imports", DBG_LBOX);
    __debug(NULL, "Resolving Imports...", DBG_LBOX);

    //!: -------------------------------------------------- [ Resolve Imports ]
    /**
     * resolve imports from XFLDR.DLL (this is basically
     * a copy of the doshResolveImports code, but we can't
     * use that before resolving...)
    */
    for (ul = 0;
         ul < sizeof(G_aImports) / sizeof(G_aImports[0]);
         ul++) {

        if (DosQueryProcAddr(hmodXFLDR,
                             0,                                                 // ordinal, ignored
                             (PSZ)G_aImports[ul].pcszFunctionName,
                             G_aImports[ul].ppFuncAddress)
                    != NO_ERROR) {
            sprintf(pszErrorMsg, "Import %s failed.", G_aImports[ul].pcszFunctionName);
            fImportsFailed = TRUE;
            break;
        }
    }

    if (!fImportsFailed) {

        __debug("WidgetInitModule", "  >> Registering Window Class", DBG_LBOX);
        /*
        // Register the widget class.
        */
        if (!WinRegisterClass(hab,
                                WNDCLASS_WIDGET_USB,
                                fnwpSampleWidget,
                                CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT,
                                sizeof(PMONITORPRIVATE))) {     // extra memory to reserve for QWL_USER

            strcpy(pszErrorMsg, "WinRegisterClass failed.");
        }
        else {
            // no error:
            // return classes
            *ppaClasses = G_WidgetClasses;

            // return no. of classes in this DLL (one here):
            ulrc = sizeof(G_WidgetClasses) / sizeof(G_WidgetClasses[0]);


/*
            if (DosScanEnv(OSDIR_VAR_NAME, (const char**)&pszOSDirectory))
                pszOSDirectory=NULL;


            strcpy(szNlsPath, pszOSDirectory);
            strcat(szNlsPath, "\\LANG\\usbshold.nls");

            LangeLoadLanguage(szNlsPath, 0, &BatteryLange);

            sprintf(temp, "\n\nSTART\n");
            __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

            sprintf(temp, "Load LANGE: {%p} {%s}\n", pszOSDirectory, szNlsPath);
            __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);
*/
        }
    }

    __debug("WidgetInitModule", "  >> Logging Device Lists", DBG_LBOX);

    //~ pws->currentUSBDevicesList  = &currentUSBDevicesList;
    sprintf(buf, "address: %08lX", pws->currentUSBDevicesList);
    __debug("pws->currentUSBDevicesList ", buf, DBG_LBOX);


    //~ pws->previousUSBDevicesList  = &previousUSBDevicesList;
    sprintf(buf, "address: %08lX", pws->previousUSBDevicesList);
    __debug("pws->previousUSBDevicesList" , buf, DBG_LBOX);


    //~ pws->removedUSBDevicesList  = &removedUSBDevicesList;
    sprintf(buf, "address: %08lX", pws->removedUSBDevicesList);
    __debug("pws->removedUSBDevicesList " , buf, DBG_LBOX);

    __debug(NULL, "WgtInitModule::END", DBG_LBOX);
    __debug("WidgetInitModule", "END", DBG_AUX);

    /* Return number of Widget Classes handled by this DLL */
    return sizeof(G_WidgetClasses) / sizeof(XCENTERWIDGETCLASS);
}

//!: << WgtUnInitModule >> (DLL MODULE UNINIT)
/*
 *@@ WgtUnInitModule:
 *    optional export with ordinal 2, which can clean
 *    up global widget class data.
 *
 *    This gets called by the XCenter right before
 *    a widget DLL gets unloaded. Note that this
 *    gets called even if the "init module" export
 *    returned 0 (meaning an error) and the DLL
 *    gets unloaded right away.
 */
VOID EXPENTRY WgtUnInitModule()
{
    APIRET  ulrc        = -1;
    APIRET  ulrc2       = -2;
    ULONG   ulReply     = 0;
    BOOL    cleanup     = TRUE;
    BOOL    brc         = TRUE;
    CHAR    buf[256]    = "\0";

    //~ MessageBox("usbimpl", "WgtUnInitModule");

    ShowDebugDialog();

    __debug("WidgetUninitModule", "START", DBG_LBOX);

    // Detach from submempool and unlink from masterpool
    if (masterpool) {
        ulrc = DosSubUnsetMem(((PMASTERMEMPOOL) masterpool)->submempool);
        ulrc2 = DosFreeMem(masterpool);
    }


#if     DEBUG
    /* Show it */
    ulReply = WinProcessDlg(hdlgDebugDialog);
    ulReply = WinProcessDlg(hdlgDebugDialog);
    ulReply = WinProcessDlg(hdlgDebugDialog);

    /* Where are we ? */
    sprintf(buf, "masterpool:%08lX, ulrc:%d, ulrc2:%d, ulReply:%d", masterpool, ulrc, ulrc2, ulReply);
    __debug(NULL, buf, DBG_LBOX);
    __debug(NULL, "WgtUnInitModule", DBG_LBOX);
    __debug(NULL, "Processed UnInit", DBG_LBOX);

#endif

    /* Process */
    //ulReply = WinProcessDlg(hdlgDebugDialog);


///
//!: ----------------------------------------------------- [ STOP THE THREADS ]
///
    if (cleanup) {
        StopThreads();
    }

    //~ ClosePipe();



#if     DEBUG
    //~: Send WM_QUIT to debug dialog
    /// was post
    WinSendMsg(hdlgDebugDialog, WM_QUIT, NULL, NULL);

    /*
    // Redirection naar doshSleep niet gevonden.
    // Heeft te maken met setup.h van xwp.
    // Zit een redirectie in dosh.h
    // Dus maar een eigen wrapper gemaakt.
    // Zit in APIHelpers.cpv.
    */
    MyDosSleep(2000);
    //~ DosBlock(10000);

    __debug("WidgetUninitModule", "END", DBG_LBOX);

    if (hevNeverPosted)
        DosCloseEventSem(hevNeverPosted);

    DestroyDebugDialog();

#endif  // DEBUG

    /* Destroy the global WidgetSettingsDialog */
    //~ if (g_WidgetSettingsDialog) {
        //~ delete g_WidgetSettingsDialog;
        //~ g_WidgetSettingsDialog = NULL;
    //~ }

}


//!: << WgtQueryVersion >> (This called first on widget creation)
/*
 *@@ WgtQueryVersion:
 *    this new export with ordinal 3 can return the
 *    XWorkplace version number which is required
 *    for this widget to run. For example, if this
 *    returns 0.9.10, this widget will not run on
 *    earlier XWorkplace versions.
 *
 *    NOTE: This export was mainly added because the
 *    prototype for the "Init" export was changed
 *    with V0.9.9. If this returns 0.9.9, it is
 *    assumed that the INIT export understands
 *    the new FNWGTINITMODULE_099 format (see center.h).
 *
 *@@added V0.9.9 (2001-02-06) [umoeller]
 */
VOID EXPENTRY WgtQueryVersion(PULONG pulMajor,
                              PULONG pulMinor,
                              PULONG pulRevision) {
    char    header[] = \
    "\n\n\n"\
    "::::::::::::::::::::::::::::::::::::::\n"\
    ":: NeoWPS * USB Widget -- Debug Log ::\n"\
    "::::::::::::::::::::::::::::::::::::::"\
    ;


    //~ MessageBox2("usbimpl", "WgtQueryVersion");


    __debug(NULL, header, DBG_AUX);
    __debug(NULL, bldlevel, DBG_AUX);
    __debug(NULL, NULL, DBG_AUX);

    //~ __debug("WidgetQueryVersion", "START", DBG_AUX);

    // report 0.9.9
    *pulMajor = 0;
    *pulMinor = 9;
    *pulRevision = 9;

    __debug("WidgetQueryVersion", "END", DBG_AUX);
}



#ifdef __cplusplus
        }
#endif
