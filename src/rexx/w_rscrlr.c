
/*
 *@@sourcefile w_rscrlr.c:
 *      XCenter "Rexx scroller" widget.
 *
 *      This is an example of an XCenter widget plugin.
 *      This widget resides in RSCRLR.DLL, which (as
 *      with all widget plugins) must be put into the
 *      plugins/xcenter directory of the XWorkplace
 *      installation directory.
 *
 *      Any XCenter widget plugin DLL must export the
 *      following procedures by ordinal:
 *
 *      -- Ordinal 1 (RwgtInitModule): this must
 *         return the widgets which this DLL provides.
 *
 *      -- Ordinal 2 (RwgtUnInitModule): this must
 *         clean up global DLL data.
 *
 *      -- Ordinal 3 (RwgtQueryVersion): this must
 *         return the widgets minimal XWorkplace version level
 *         required.
 *
 *      A Rexx scroller widget recognize the following setup strings:
 *
 *      -- BGNDCOL:  the background color (in rrggbb format).
 *      -- COLOR1:   the color for the first part of the scroller (in
 *                   rrggbb format).
 *      -- COLOR2:   the color for the second part of the scroller (in
 *                   rrggbb format).
 *      -- COLOR3:   the color for the third part of the scroller (in
 *                   rrggbb format).
 *      -- DBLCLK:   the encoded script to be run whenever
 *                   a double click occurs.
 *      -- FONT:     the font used for the widget's text (in "8.Helv"
 *                   format).
 *      -- HEIGHT:   the height in pixels.
 *      -- REFRESH:  the interval to use between refreshes, in 1/1000
 *                   of a seconds (it must be greater or equal than
 *                   500).
 *      -- SCRIPT:   the script to be run, encoded.
 *      -- SIZEABLE: 1 if the widget is user-sizeable.
 *      -- FILLED:   1 if the graph should be filled.
 *      -- AVERAGE:  1 if the widget should average values to
 *                   produce a smoother curve.
 *      -- TEXTCOL:  the color of the widget's text (in rrggbb
 *                   format).
 *      -- TITLE:    the default title of the scroller, encoded.
 *      -- WIDTH:    the width in pixels.
 *
 *      The makefile in src\rexx compiles widgets
 *      with the VAC subsystem library. As a result,
 *      multiple threads are not supported.
 *
 *      This is all new with V0.7.0.
 *
 *@@added V0.7.0 (2002-06-04) [criguada]
 *@@header "shared\center.h"
 */

/*
 *      Copyright (C) 2002 Cristiano Guadagnino.
 *      Copyright (C) 2001 Martin Lafaix.
 *      Copyright (C) 2000 Ulrich M”ller.
 *      This file is part of the XWorkplace Widget Library source package.
 *      XWorkplace is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published
 *      by the Free Software Foundation, in version 2 as it comes in the
 *      "COPYING" file of the XWorkplace main distribution.
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 */

/*
 *  Suggested #include order:
 *  1)  os2.h
 *  2)  C library headers
 *  3)  setup.h (code generation and debugging options)
 *  4)  headers in helpers\
 *  5)  dlgids.h, headers in shared\ (as needed)
 *  6)  headers in implementation dirs (e.g. filesys\, as needed)
 *  7)  #pragma hdrstop and then more SOM headers which crash with precompiled headers
 */

#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#define INCL_DOSPROCESS
#define INCL_DOSEXCEPTIONS

#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_WINDIALOGS
#define INCL_WININPUT
#define INCL_WINSWITCHLIST
#define INCL_WINRECTANGLES
#define INCL_WINPOINTERS
#define INCL_WINSYS
#define INCL_WINENTRYFIELDS
#define INCL_WINBUTTONS
#define INCL_WINMLE
#define INCL_WINSTDDRAG
#define INCL_WINMESSAGEMGR
#define INCL_WINSTATICS
#define INCL_WINHOOKS
#define INCL_WINTIMER

#define INCL_GPIPRIMITIVES
#define INCL_GPILOGCOLORTABLE
#define INCL_GPIREGIONS
#include <os2.h>

#define INCL_REXXSAA
#include <rexxsaa.h>

// C library headers
#include <stdio.h>
#include <setjmp.h>             // needed for except.h
#include <assert.h>             // needed for except.h
#include <memory.h>
#include <math.h>

// generic headers
#include "setup.h"                      // code generation and debugging options

// headers in /helpers
#ifdef DOSH_STANDARDWRAPPERS
    #undef DOSH_STANDARDWRAPPERS
#endif
#include "helpers\dosh.h"               // Control Program helper routines
#include "helpers\gpih.h"               // GPI helper routines
#include "helpers\prfh.h"               // INI file helper routines;
                                        // this include is required for some
                                        // of the structures in shared\center.h
#ifdef WINH_STANDARDWRAPPERS
    #undef WINH_STANDARDWRAPPERS
#endif
#include "helpers\winh.h"               // PM helper routines
#include "helpers\timer.h"              // replacement PM timers
#include "helpers\xstring.h"            // extended string helpers
#include "helpers\comctl.h"             // common controls (window procs)
#include "helpers\except.h"             // exception handling

// XWorkplace implementation headers
#include "shared\center.h"              // public XCenter interfaces
#include "shared\common.h"              // the majestic XWorkplace include file

#include "rexx\w_rscrlr.h"              // private rscrlr definitions

#define INCL_NOICON
#include "rexx\common.h"                // common stuff for REXX widgets

#pragma hdrstop                     // VAC++ keeps crashing otherwise


/* ******************************************************************
 *
 *   XCenter widget class definition
 *
 ********************************************************************/

/*
 *      This contains the name of the PM window class and
 *      the XCENTERWIDGETCLASS definition(s) for the widget
 *      class(es) in this DLL.
 */

#define WNDCLASS_WIDGET_RSCRLR "XWPCenterRexxScrlrWidget"

static XCENTERWIDGETCLASS G_WidgetClasses[]
    = {
        WNDCLASS_WIDGET_RSCRLR,     // PM window class name
        0,                          // additional flag, not used here
        "RexxScrlr",                // internal widget class name
        NULL,                       // widget class name displayed to user;
                                    // is set in RwgtInitModule (NLS)
        WGTF_TOOLTIP | WGTF_TRAYABLE,
                                    // widget class flags
        RwgtShowSettingsDlg         // settings dialog
      };

/* ******************************************************************
 *
 *   Function imports from XFLDR.DLL
 *
 ********************************************************************/

/*
 *      To reduce the size of the widget DLL, it can
 *      be compiled with the VAC subsystem libraries.
 *      In addition, instead of linking frequently
 *      used helpers against the DLL again, you can
 *      import them from XFLDR.DLL, whose module handle
 *      is given to you in the INITMODULE export.
 *
 *      Note that importing functions from XFLDR.DLL
 *      is _not_ a requirement. We only do this to
 *      avoid duplicate code.
 *
 *      For each funtion that you need, add a global
 *      function pointer variable and an entry to
 *      the G_aImports array. These better match.
 *
 *      The actual imports are then made by RwgtInitModule.
 */

// resolved function pointers from XFLDR.DLL
PCMNQUERYDEFAULTFONT pcmnQueryDefaultFont = NULL;
PCMNSETCONTROLSFONT pcmnSetControlsFont = NULL;

PCTRDISPLAYHELP pctrDisplayHelp = NULL;
PCTRFREESETUPVALUE pctrFreeSetupValue = NULL;
PCTRPARSECOLORSTRING pctrParseColorString = NULL;
PCTRSCANSETUPSTRING pctrScanSetupString = NULL;
PCTRSETSETUPSTRING pctrSetSetupString = NULL;

PGPIHDRAW3DFRAME pgpihDraw3DFrame = NULL;
PGPIHSWITCHTORGB pgpihSwitchToRGB = NULL;
PGPIHCREATEXBITMAP pgpihCreateXBitmap = NULL;
PGPIHBOX pgpihBox = NULL;
PGPIHDESTROYXBITMAP pgpihDestroyXBitmap = NULL;
/*PGPIHLOADBITMAPFILE pgpihLoadBitmapFile = NULL;*/

PTMRSTARTXTIMER ptmrStartXTimer = NULL;
PTMRSTOPXTIMER ptmrStopXTimer = NULL;

PWINHFREE pwinhFree = NULL;
PWINHQUERYPRESCOLOR pwinhQueryPresColor = NULL;
PWINHQUERYWINDOWFONT pwinhQueryWindowFont = NULL;
PWINHSETWINDOWFONT pwinhSetWindowFont = NULL;
PWINHCENTERWINDOW pwinhCenterWindow = NULL;

PXSTRCAT pxstrcat = NULL;
PXSTRCLEAR pxstrClear = NULL;
PXSTRINIT pxstrInit = NULL;
PXSTRENCODE pxstrEncode = NULL;
PXSTRDECODE pxstrDecode = NULL;

PEXCHANDLERLOUD pexcHandlerLoud = NULL;

RESOLVEFUNCTION G_aImports[] =
    {
        "cmnQueryDefaultFont", (PFN*)&pcmnQueryDefaultFont,
        "cmnSetControlsFont", (PFN*)&pcmnSetControlsFont,

        "ctrDisplayHelp", (PFN*)&pctrDisplayHelp,
        "ctrFreeSetupValue", (PFN*)&pctrFreeSetupValue,
        "ctrParseColorString", (PFN*)&pctrParseColorString,
        "ctrScanSetupString", (PFN*)&pctrScanSetupString,
        "ctrSetSetupString", (PFN*)&pctrSetSetupString,

        "gpihDraw3DFrame", (PFN*)&pgpihDraw3DFrame,
        "gpihSwitchToRGB", (PFN*)&pgpihSwitchToRGB,
        "gpihCreateXBitmap", (PFN*)&pgpihCreateXBitmap,
        "gpihBox", (PFN*)&pgpihBox,
        "gpihDestroyXBitmap", (PFN*)&pgpihDestroyXBitmap,
/*        "gpihLoadBitmapFile", (PFN*)&pgpihLoadBitmapFile,*/

        "tmrStartXTimer", (PFN*)&ptmrStartXTimer,
        "tmrStopXTimer", (PFN*)&ptmrStopXTimer,

        "winhFree", (PFN*)&pwinhFree,
        "winhQueryPresColor", (PFN*)&pwinhQueryPresColor,
        "winhQueryWindowFont", (PFN*)&pwinhQueryWindowFont,
        "winhSetWindowFont", (PFN*)&pwinhSetWindowFont,
        "winhCenterWindow", (PFN*)&pwinhCenterWindow,

        "xstrcat", (PFN*)&pxstrcat,
        "xstrClear", (PFN*)&pxstrClear,
        "xstrInit", (PFN*)&pxstrInit,
        "xstrEncode", (PFN*)&pxstrEncode,
        "xstrDecode", (PFN*)&pxstrDecode,

        "excHandlerLoud", (PFN*)&pexcHandlerLoud
    };

/* ******************************************************************
 *
 *   Private widget instance data
 *
 ********************************************************************/

/*
 *@@ RSCRLRSETUP:
 *      instance data to which setup strings correspond.
 *      This is also a member of RSCRLRPRIVATE.
 *
 *      Putting these settings into a separate structure
 *      is no requirement, but comes in handy if you
 *      want to use the same setup string routines on
 *      both the open widget window and a settings dialog.
 */

typedef struct _RSCRLRSETUP
{
    LONG        lcolBackground,         // background color
                lcolForeground;         // foreground color (for text)

    PSZ         pszFont;
            // if != NULL, non-default font (in "8.Helv" format);
            // this has been allocated using local malloc()!

    PSZ         pszScript;
    PSZ         pszTitle;

    ULONG       ulScriptLength;
            // strlen(pszScript), for speed.

    LONG        lcol1,
                lcol2,
                lcol3;

    ULONG       ulHeight,
                ulWidth;

    ULONG       ulTimerDelay;

    PSZ         pszDblClkScript;
    BOOL        fSizeable;

    BOOL        fFilled;
    BOOL        fAverage;

    BOOL        fBLMode;
            // if TRUE, the widget goes into "baseline" mode

} RSCRLRSETUP, *PRSCRLRSETUP;

/*
 *@@ RSCRLRPRIVATE:
 *      more window data for the widget.
 *
 *      An instance of this is created on WM_CREATE in
 *      fnwpRScrlrWidget and stored in XCENTERWIDGET.pUser.
 *
 */

typedef struct _RSCRLRPRIVATE
{
    PXCENTERWIDGET pWidget;
            // reverse ptr to general widget data ptr; we need
            // that all the time and don't want to pass it on
            // the stack with each function call

    RSCRLRSETUP    Setup;
            // widget settings that correspond to a setup string

    CHAR           achX[16];
    CHAR           achY[16];
    CHAR           achModifiers[16];
    CHAR           achHWND[16];
            // placeholders for scripts arguments

    LONG           lVal1,
                   lVal2,
                   lVal3;
            // the returned values for the scroller

    BOOL           fBusy;
            // TRUE if a script is currently running, false otherwise

    BOOL           fTooltipShowing;
            // TRUE when tooltip shown over the scroller

    CHAR           achText[100];
            // the text to be used for the scroller

    CHAR           achTooltip[250];
            // the current tooltip for the scroller (if null, the default
            // title is used)

    ULONG          ulTimerID;
            // the running timer ID

    PSZ            pszInstore;
    LONG           lInstoreSize;
            // timer script precompiled store

    BYTE           abUserData[USERDATASIZE];
            // user data area
    USHORT         usUserDataLength;
            // user data area length

    LONG           lcolForeground,
                   lcolBackground,
                   lcol1,
                   lcol2,
                   lcol3;
            // runtime colors (if -1L, the defaults apply)

    PXBITMAP       pBitmap;
            // bitmap for graph; this contains only
            // the "client" (without the 3D frame)

    HBITMAP        hbmBgBmp;
            // background bitmap for the widget

    CHAR           achBgBmp[250];
            // path to the bitmap file

    LONG           cxBgBmp,
                   cyBgBmp;
            // bitmap size

    BOOL           fUpdateGraph;
            // if TRUE, PwgtPaint recreates the entire bitmap

    ULONG          cValues;
    PLONG          palValues1[2];
    PLONG          palValues2[2];
    PLONG          palValues3[2];
            // ptrs to arrays of LONGs containing previous values;
            // array[0] is the normal values; array[1] is the averaged
            // values

    LONG           lMaxValue;
            // holds the peak value among all the three arrays
} RSCRLRPRIVATE, *PRSCRLRPRIVATE;



/* ******************************************************************/
#include "loadbmp.c"                // needed because Ulrich doesn't export
                                    // this in xfldr.dll (why?)
                                    // FIXME


/* ******************************************************************
 *
 *   Widget setup management
 *
 ********************************************************************/

/*
 *      This section contains shared code to manage the
 *      widget's settings. This can translate a widget
 *      setup string into the fields of a binary setup
 *      structure and vice versa. This code is used by
 *      an open widget window, but could be shared with
 *      a settings dialog, if you implement one.
 */

/*
 *@@ RwgtClearSetup:
 *      cleans up the data in the specified setup
 *      structure, but does not free the structure
 *      itself.
 */

VOID RwgtClearSetup(PVOID pvSetup)
{
    PRSCRLRSETUP pSetup = (PRSCRLRSETUP)pvSetup;

    if (pSetup)
    {
        free(pSetup->pszFont);
        pSetup->pszFont = NULL;

        free(pSetup->pszScript);
        pSetup->pszScript = NULL;
        pSetup->ulScriptLength = 0;

        free(pSetup->pszDblClkScript);
        pSetup->pszDblClkScript = NULL;

        free(pSetup->pszTitle);
        pSetup->pszTitle = NULL;
    }
}

/*
 *@@ RwgtScanSetup:
 *      scans the given setup string and translates
 *      its data into the specified binary setup
 *      structure.
 *
 *      NOTE: It is assumed that pSetup is zeroed
 *      out. We do not clean up previous data here.
 */

VOID RwgtScanSetup(const char *pcszSetupString,
                   PRSCRLRSETUP pSetup)
{
    PSZ p;

    // background color
    p = pctrScanSetupString(pcszSetupString,
                            "BGNDCOL");
    if (p)
    {
        pSetup->lcolBackground = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else
        // default color:
        pSetup->lcolBackground = WinQuerySysColor(HWND_DESKTOP, SYSCLR_BUTTONMIDDLE, 0);

    // text color:
    p = pctrScanSetupString(pcszSetupString,
                            "TEXTCOL");
    if (p)
    {
        pSetup->lcolForeground = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->lcolForeground = WinQuerySysColor(HWND_DESKTOP, SYSCLR_WINDOWSTATICTEXT, 0);

    // font:
    // we set the font presparam, which automatically
    // affects the cached presentation spaces
    p = pctrScanSetupString(pcszSetupString,
                            "FONT");
    if (p)
    {
        pSetup->pszFont = strdup(p);
        pctrFreeSetupValue(p);
    }
    // else: leave this field null

    // script:
    p = pctrScanSetupString(pcszSetupString,
                            "SCRIPT");
    if (p)
    {
        // p is an encoded string;  we must decode it
        PSZ pszDecoded = RwgtDecodeString(p);

        pSetup->pszScript = strdup(pszDecoded);
        pSetup->ulScriptLength = strlen(pszDecoded);
        pctrFreeSetupValue(p);
        RwgtFree(pszDecoded);
    }
    else
    {
        pSetup->pszScript = strdup("");
        pSetup->ulScriptLength = 0;
    }

    // double click script:
    p = pctrScanSetupString(pcszSetupString,
                            "DBLCLK");
    if (p)
    {
        // p is an encoded string;  we must decode it
        PSZ pszDecoded = RwgtDecodeString(p);

        pSetup->pszDblClkScript = strdup(pszDecoded);
        pctrFreeSetupValue(p);
        RwgtFree(pszDecoded);
    }
    else
        pSetup->pszDblClkScript = strdup("");

    // title:
    p = pctrScanSetupString(pcszSetupString,
                            "TITLE");
    if (p)
    {
        // p is an encoded string;  we must decode it
        PSZ pszDecoded = RwgtDecodeString(p);

        pSetup->pszTitle = strdup(pszDecoded);
        pctrFreeSetupValue(p);
        RwgtFree(pszDecoded);
    }
    else
        pSetup->pszTitle = strdup("");

    // color 1:
    p = pctrScanSetupString(pcszSetupString,
                            "COLOR1");
    if (p)
    {
        pSetup->lcol1 = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->lcol1 = RGB_GREEN;

    // color 2:
    p = pctrScanSetupString(pcszSetupString,
                            "COLOR2");
    if (p)
    {
        pSetup->lcol2 = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->lcol2 = RGB_YELLOW;

    // color 3:
    p = pctrScanSetupString(pcszSetupString,
                            "COLOR3");
    if (p)
    {
        pSetup->lcol3 = pctrParseColorString(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->lcol3 = RGB_RED;

    // width:
    p = pctrScanSetupString(pcszSetupString,
                            "WIDTH");
    if (p)
    {
        int width = atoi(p);
        pSetup->ulWidth = width;
    }
    else
        pSetup->ulWidth = 100;

    // sizeable:
    p = pctrScanSetupString(pcszSetupString,
                            "SIZEABLE");
    if (p)
    {
        pSetup->fSizeable = (strcmp(p, "1") == 0);
    }
    else
        pSetup->fSizeable = 0;

    // filled:
    p = pctrScanSetupString(pcszSetupString,
                            "FILLED");
    if (p)
    {
        pSetup->fFilled = (strcmp(p, "1") == 0);
    }
    else
        pSetup->fFilled = 1;

    // average:
    p = pctrScanSetupString(pcszSetupString,
                            "AVERAGE");
    if (p)
    {
        pSetup->fAverage = (strcmp(p, "1") == 0);
    }
    else
        pSetup->fAverage = 0;

    // baseline mode:
    p = pctrScanSetupString(pcszSetupString,
                            "BLMODE");
    if (p)
    {
        pSetup->fBLMode = (strcmp(p, "1") == 0);
    }
    else
        pSetup->fBLMode = 0;

    // height:
    p = pctrScanSetupString(pcszSetupString,
                            "HEIGHT");
    if (p)
    {
        int height = atoi(p);
        pSetup->ulHeight = height;
    }
    else
        pSetup->ulHeight = 10;

    // timer delay:
    p = pctrScanSetupString(pcszSetupString,
                            "REFRESH");
    if (p)
    {
        pSetup->ulTimerDelay = atol(p);

        // delay must be at least .5 second
        if (pSetup->ulTimerDelay < 500)
            pSetup->ulTimerDelay = 500;
    }
    else
        // default to 5 seconds
        pSetup->ulTimerDelay = 5000;
}

/*
 *@@ RwgtSaveSetup:
 *      composes a new setup string.
 *      The caller must invoke xstrClear on the
 *      string after use.
 */

VOID RwgtSaveSetup(PXSTRING pstrSetup,       // out: setup string (is cleared first)
                   PVOID pvSetup)
{
    CHAR         szTemp[CCHMAXSCRIPT*3+8];
            // 3 times the length of an unencoded script plus length of "SCRIPT="
    PSZ          psz = 0;
    PRSCRLRSETUP pSetup = (PRSCRLRSETUP)pvSetup;

    pxstrInit(pstrSetup, 500);

    sprintf(szTemp, "BGNDCOL=%06lX;",
            pSetup->lcolBackground);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "TEXTCOL=%06lX;",
            pSetup->lcolForeground);
    pxstrcat(pstrSetup, szTemp, 0);

    if (pSetup->pszFont)
    {
        // non-default font:
        sprintf(szTemp, "FONT=%s;",
                pSetup->pszFont);
        pxstrcat(pstrSetup, szTemp, 0);
    }

    sprintf(szTemp, "COLOR1=%06lX;",
            pSetup->lcol1);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "COLOR2=%06lX;",
            pSetup->lcol2);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "COLOR3=%06lX;",
            pSetup->lcol3);
    pxstrcat(pstrSetup, szTemp, 0);

    if (pSetup->pszScript)
    {
        if (strcmp(pSetup->pszScript, "") != 0)
        {
            // pszScript has to be encoded for safety
            PSZ pszEncoded = RwgtEncodeString(pSetup->pszScript);

            sprintf(szTemp, "SCRIPT=%s;",
                    pszEncoded);
            pxstrcat(pstrSetup, szTemp, 0);
            RwgtFree(pszEncoded);
        }
    }

    if (pSetup->pszDblClkScript)
    {
        if (strcmp(pSetup->pszDblClkScript, "") != 0)
        {
            // pszDblClkScript has to be encoded for safety
            PSZ pszEncoded = RwgtEncodeString(pSetup->pszDblClkScript);

            sprintf(szTemp, "DBLCLK=%s;",
                    pszEncoded);
            pxstrcat(pstrSetup, szTemp, 0);
            RwgtFree(pszEncoded);
        }
    }

    if (pSetup->pszTitle)
    {
        if (strcmp(pSetup->pszTitle, "") != 0)
        {
            // pszTitle has to be encoded for safety
            PSZ pszEncoded = RwgtEncodeString(pSetup->pszTitle);

            sprintf(szTemp, "TITLE=%s;",
                    pszEncoded);
            pxstrcat(pstrSetup, szTemp, 0);
            RwgtFree(pszEncoded);
        }
    }

    if (pSetup->fSizeable)
        pxstrcat(pstrSetup, "SIZEABLE=1;", 11);

    if (pSetup->fFilled)
        pxstrcat(pstrSetup, "FILLED=1;", 9);
    else
        pxstrcat(pstrSetup, "FILLED=0;", 9);

    if (pSetup->fAverage)
        pxstrcat(pstrSetup, "AVERAGE=1;", 10);

    if (pSetup->fBLMode)
        pxstrcat(pstrSetup, "BLMODE=1;", 9);

    sprintf(szTemp, "HEIGHT=%d;",
            pSetup->ulHeight);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "WIDTH=%d;",
            pSetup->ulWidth);
    pxstrcat(pstrSetup, szTemp, 0);

    sprintf(szTemp, "REFRESH=%d;",
            pSetup->ulTimerDelay);
    pxstrcat(pstrSetup, szTemp, 0);

}

/* ******************************************************************
 *
 *   Widget settings dialog
 *
 ********************************************************************/

/*
 *@@ Settings2Dlg:
 *      sets up the dialog according to the current settings.
 */

VOID Settings2Dlg(HWND hwnd,
                  PRSCRLRSETUP pSetup)
{
    WinSetDlgItemText(hwnd, ID_CRDI_RSCRLR_TITLE, pSetup->pszTitle);
    WinSetDlgItemText(hwnd, ID_CRDI_RSCRLR_SCRIPT, pSetup->pszScript);
    WinSetDlgItemText(hwnd, ID_CRDI_RSCRLR_DBLCLK, pSetup->pszDblClkScript);

    WinSetDlgItemShort(hwnd, ID_CRDI_RSCRLR_REFRESH, pSetup->ulTimerDelay, FALSE);

    // adjusting colors
    WinSetPresParam(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR1),
                    PP_BACKGROUNDCOLOR,
                    sizeof(ULONG),
                    &pSetup->lcol1);
    WinSetPresParam(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR2),
                    PP_BACKGROUNDCOLOR,
                    sizeof(ULONG),
                    &pSetup->lcol2);
    WinSetPresParam(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR3),
                    PP_BACKGROUNDCOLOR,
                    sizeof(ULONG),
                    &pSetup->lcol3);

    WinSendDlgItemMsg(hwnd, ID_CRDI_RSCRLR_TITLE, MLM_SETTEXTLIMIT, MPFROMLONG(CCHMAXTITLE), (MPARAM)0);
    WinSendDlgItemMsg(hwnd, ID_CRDI_RSCRLR_SCRIPT, MLM_SETTEXTLIMIT, MPFROMLONG(CCHMAXSCRIPT), (MPARAM)0);
    WinSendDlgItemMsg(hwnd, ID_CRDI_RSCRLR_DBLCLK, MLM_SETTEXTLIMIT, MPFROMLONG(CCHMAXSCRIPT), (MPARAM)0);

    WinSetDlgItemShort(hwnd, ID_CRDI_RSCRLR_WIDTH, pSetup->ulWidth, FALSE);
    WinCheckButton(hwnd, ID_CRDI_RSCRLR_RESIZEABLE, pSetup->fSizeable);
    WinCheckButton(hwnd, ID_CRDI_RSCRLR_FIXEDWIDTH, !pSetup->fSizeable);

    WinCheckButton(hwnd, ID_CRDI_RSCRLR_FILLGRAPH, pSetup->fFilled);
    WinCheckButton(hwnd, ID_CRDI_RSCRLR_LINEGRAPH, !pSetup->fFilled);
    WinCheckButton(hwnd, ID_CRDI_RSCRLR_AVERAGE, pSetup->fAverage);
    WinCheckButton(hwnd, ID_CRDI_RSCRLR_BLMODE, pSetup->fBLMode);
}

/*
 *@@ fnwpSettingsDlg:
 *      dialog proc for the rscrlr settings dialog.
 *
 */

MRESULT EXPENTRY fnwpSettingsDlg(HWND hwnd,
                                 ULONG msg,
                                 MPARAM mp1,
                                 MPARAM mp2)
{
    MRESULT mrc = 0;

    switch (msg)
    {
        /*
         * WM_INITDLG:
         *      fill the control. We get the
         *      WIDGETSETTINGSDLGDATA in mp2.
         */

        case WM_INITDLG:
        {
            PWIDGETSETTINGSDLGDATA pData = (PWIDGETSETTINGSDLGDATA)mp2;
            PSTORAGE pStorage = calloc(1, sizeof(STORAGE));
            PRSCRLRSETUP pSetup = calloc(1, sizeof(RSCRLRSETUP));

            WinSetWindowPtr(hwnd, QWL_USER, pData);
            if (    (pSetup)
                 && (pStorage)
               )
            {
                // pcszSetupString can be null
                if (pData->pcszSetupString)
                    pStorage->pszInitialSetup = strdup(pData->pcszSetupString);

                pStorage->pSetup = pSetup;

                // store this in WIDGETSETTINGSDLGDATA
                pData->pUser = pStorage;

                RwgtScanSetup(pData->pcszSetupString, pSetup);

                Settings2Dlg(hwnd, pSetup);
            }
            mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
        break; }

        /*
         * WM_COMMAND:
         *
         */

        case WM_COMMAND:
        {
            PWIDGETSETTINGSDLGDATA pData = (PWIDGETSETTINGSDLGDATA)WinQueryWindowPtr(hwnd, QWL_USER);
            if (pData)
            {
                PRSCRLRSETUP pSetup = (PRSCRLRSETUP)((PSTORAGE)pData->pUser)->pSetup;
                PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)(PSTORAGE)pData->pUser;
                if (pSetup)
                {
                    USHORT usCmd = (USHORT)mp1;

                    switch (usCmd)
                    {
                        /*
                         * DID_OK:
                         *      OK button -> recompose settings
                         *      and get outta here.
                         */

                        case DID_OK:
                        {
                            XSTRING strSetup;
                            CHAR szTemp[CCHMAXSCRIPT+1];

                            // saving colors
                            pSetup->lcol1 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR1),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_GREEN);
                            pSetup->lcol2 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR2),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_YELLOW);
                            pSetup->lcol3 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR3),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_RED);

                            WinQueryDlgItemText(hwnd, ID_CRDI_RSCRLR_SCRIPT, sizeof(szTemp), (PSZ)szTemp);
                            free(pSetup->pszScript);
                            pSetup->pszScript = strdup(szTemp);
                            pSetup->ulScriptLength = strlen(szTemp);

                            WinQueryDlgItemText(hwnd, ID_CRDI_RSCRLR_DBLCLK, sizeof(szTemp), (PSZ)szTemp);
                            free(pSetup->pszDblClkScript);
                            pSetup->pszDblClkScript = strdup(szTemp);

                            WinQueryDlgItemText(hwnd, ID_CRDI_RSCRLR_TITLE, sizeof(szTemp), (PSZ)szTemp);
                            free(pSetup->pszTitle);
                            pSetup->pszTitle = strdup(szTemp);

                            RwgtSaveSetup(&strSetup,
                                          pSetup);

                            pctrSetSetupString(pData->hSettings,
                                               strSetup.psz);
                            pxstrClear(&strSetup);
                            WinDismissDlg(hwnd, DID_OK);
                        break; }

                        /*
                         * DID_APPLY:
                         *      apply the changes (without closing the dialog)
                         */

                        case DID_APPLY:
                        {
                            XSTRING strSetup;
                            PSTORAGE pStorage = (PSTORAGE)pData->pUser;
                            CHAR szTemp[CCHMAXSCRIPT+1];

                            // saving colors
                            pSetup->lcol1 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR1),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_GREEN);
                            pSetup->lcol2 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR2),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_YELLOW);
                            pSetup->lcol3 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RSCRLR_COLOR3),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_RED);

                            WinQueryDlgItemText(hwnd, ID_CRDI_RSCRLR_SCRIPT, sizeof(szTemp), (PSZ)szTemp);
                            free(pSetup->pszScript);
                            pSetup->pszScript = strdup(szTemp);
                            pSetup->ulScriptLength = strlen(szTemp);

                            WinQueryDlgItemText(hwnd, ID_CRDI_RSCRLR_DBLCLK, sizeof(szTemp), (PSZ)szTemp);
                            free(pSetup->pszDblClkScript);
                            pSetup->pszDblClkScript = strdup(szTemp);

                            WinQueryDlgItemText(hwnd, ID_CRDI_RSCRLR_TITLE, sizeof(szTemp), (PSZ)szTemp);
                            free(pSetup->pszTitle);
                            pSetup->pszTitle = strdup(szTemp);

                            RwgtSaveSetup(&strSetup,
                                          pSetup);

                            pctrSetSetupString(pData->hSettings,
                                               strSetup.psz);

                            // update the last committed changes, so that
                            // DID_RESET can revert the new ones
                            free(pStorage->pszLastCommitted);

                            pStorage->pszLastCommitted = malloc(strSetup.ulLength+1);
                            if (pStorage->pszLastCommitted)
                            {
                                memcpy(pStorage->pszLastCommitted,
                                       strSetup.psz,
                                       strSetup.ulLength+1);
                            }

                            pxstrClear(&strSetup);
                        break;}

                        /*
                         * DID_RESET:
                         *      reset the uncommitted changes
                         */

                        case DID_RESET:
                        {
                            PSTORAGE pStorage = (PSTORAGE)pData->pUser;

                            if (pSetup)
                            {
                                RwgtClearSetup(pSetup);

                                if (pStorage->pszLastCommitted)
                                    RwgtScanSetup(pStorage->pszLastCommitted, pSetup);
                                else
                                    RwgtScanSetup(pData->pcszSetupString, pSetup);

                                Settings2Dlg(hwnd, pSetup);
                            }
                        break;}

                        /*
                         * DID_CANCEL:
                         *      cancel button; must reset the widget's settings
                         *      if changes have been applied
                         */

                        case DID_CANCEL:
                            if (((PSTORAGE)pData->pUser)->pszLastCommitted)
                            {
                                // changes have been committed, we must revert
                                // them
                                pctrSetSetupString(pData->hSettings,
                                                   ((PSTORAGE)pData->pUser)->pszInitialSetup);
                            }
                            WinDismissDlg(hwnd, DID_CANCEL);
                        break;

                        /*
                         * DID_HELP:
                         *
                         */

                        case DID_HELP:
                            pctrDisplayHelp(pData->pGlobals,
                                            RwgtQueryHelpLibrary(),
                                            ID_CRH_RSCRLR_SETTINGS);
                        break;
                    } // end switch (usCmd)
                } // end if (pSetup)
            } // end if (pData)
        break; } // WM_COMMAND

        /*
         * WM_CONTROL:
         *
         */

        case WM_CONTROL:
        {
            PWIDGETSETTINGSDLGDATA pData = (PWIDGETSETTINGSDLGDATA)WinQueryWindowPtr(hwnd, QWL_USER);
            if (pData)
            {
                PRSCRLRSETUP pSetup = (PRSCRLRSETUP)((PSTORAGE)pData->pUser)->pSetup;
                if (pSetup)
                {
                    USHORT usItemID = SHORT1FROMMP(mp1),
                           usNotifyCode = SHORT2FROMMP(mp1);
                    switch (usItemID)
                    {
                        // scripts: status area handling
                        case ID_CRDI_RSCRLR_SCRIPT:
                            if (usNotifyCode == MLN_SETFOCUS)
                            {
                                ((PSTORAGE)pData->pUser)->ulCurrent = 0;
                                WinStartTimer(G_habThis, hwnd, 1, 100);
                            }
                            else
                            if (usNotifyCode == MLN_KILLFOCUS)
                            {
                                WinStopTimer(G_habThis, hwnd, 1);
                                WinSetDlgItemText(hwnd, ID_CRDI_RSCRLR_STATUS, "");
                                ((PSTORAGE)pData->pUser)->ulCurrent = 0;
                            }
                        break;

                        case ID_CRDI_RSCRLR_DBLCLK:
                            if (usNotifyCode == MLN_SETFOCUS)
                            {
                                ((PSTORAGE)pData->pUser)->ulCurrent = 0;
                                WinStartTimer(G_habThis, hwnd, 2, 100);
                            }
                            else
                            if (usNotifyCode == MLN_KILLFOCUS)
                            {
                                WinStopTimer(G_habThis, hwnd, 2);
                                WinSetDlgItemText(hwnd, ID_CRDI_RSCRLR_STATUS2, "");
                                ((PSTORAGE)pData->pUser)->ulCurrent = 0;
                            }
                        break;

                        // refresh rate:
                        case ID_CRDI_RSCRLR_REFRESH:
                            if (usNotifyCode == EN_CHANGE)
                            {
                                SHORT sValue;

                                if (WinQueryDlgItemShort(hwnd, usItemID, &sValue, FALSE))
                                    pSetup->ulTimerDelay = (USHORT)sValue;
                            }
                        break;

                        // sizeable:
                        case ID_CRDI_RSCRLR_RESIZEABLE:
                        case ID_CRDI_RSCRLR_FIXEDWIDTH:
                            if (usNotifyCode == BN_CLICKED)
                            {
                                if (WinQueryButtonCheckstate(hwnd, ID_CRDI_RSCRLR_RESIZEABLE))
                                    pSetup->fSizeable = TRUE;
                                else
                                    pSetup->fSizeable = FALSE;
                            }
                        break;

                        // graph type:
                        case ID_CRDI_RSCRLR_FILLGRAPH:
                        case ID_CRDI_RSCRLR_LINEGRAPH:
                            if (usNotifyCode == BN_CLICKED)
                            {
                                if (WinQueryButtonCheckstate(hwnd, ID_CRDI_RSCRLR_FILLGRAPH))
                                    pSetup->fFilled = TRUE;
                                else
                                    pSetup->fFilled = FALSE;
                            }
                        break;

                        // average values:
                        case ID_CRDI_RSCRLR_AVERAGE:
                            if (usNotifyCode = BN_CLICKED)
                            {
                                if (WinQueryButtonCheckstate(hwnd, ID_CRDI_RSCRLR_AVERAGE))
                                    pSetup->fAverage = TRUE;
                                else
                                    pSetup->fAverage = FALSE;
                            }
                        break;

                        // baseline mode:
                        case ID_CRDI_RSCRLR_BLMODE:
                            if (usNotifyCode = BN_CLICKED)
                            {
                                if (WinQueryButtonCheckstate(hwnd, ID_CRDI_RSCRLR_BLMODE))
                                    pSetup->fBLMode = TRUE;
                                else
                                    pSetup->fBLMode = FALSE;
                            }
                        break;

                        // width:
                        case ID_CRDI_RSCRLR_WIDTH:
                            if (usNotifyCode == EN_CHANGE)
                            {
                                SHORT sValue;

                                if (WinQueryDlgItemShort(hwnd, usItemID, &sValue, FALSE))
                                    pSetup->ulWidth = (USHORT)sValue;
                            }
                        break;

                        // color changes are not tracked here, but in
                        // WM_CONTROL instead
                    }
                }
            }
        break; }

        /*
         * WM_DESTROY:
         *
         */

        case WM_DESTROY:
        {
            PWIDGETSETTINGSDLGDATA pData = (PWIDGETSETTINGSDLGDATA)WinQueryWindowPtr(hwnd, QWL_USER);
            if (pData)
            {
                PSTORAGE pStorage = (PSTORAGE)pData->pUser;
                if (pStorage)
                {
                    if (pStorage->pSetup)
                    {
                        RwgtClearSetup(pStorage->pSetup);
                        free(pStorage->pSetup);
                    }

                    free(pStorage->pszInitialSetup);

                    free(pStorage);
                } // end if (pStorage)
            } // end if (pData)

            WinStopTimer(G_habThis, hwnd, 1);
            WinStopTimer(G_habThis, hwnd, 2);

            mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
        break; }

        /*
         * WM_TIMER:
         *      update the specified MLE status area if the cursor
         *      position has changed
         */

        case WM_TIMER:
            if (    (SHORT1FROMMP(mp1) == 1)
                 || (SHORT1FROMMP(mp1) == 2)
               )
            {
                PWIDGETSETTINGSDLGDATA pData = (PWIDGETSETTINGSDLGDATA)WinQueryWindowPtr(hwnd, QWL_USER);
                if (pData)
                {
                    PSTORAGE pStorage = (PSTORAGE)pData->pUser;
                    if (pStorage)
                    {
                        USHORT usMLE = (SHORT1FROMMP(mp1) == 1)
                                           ? ID_CRDI_RSCRLR_SCRIPT
                                           : ID_CRDI_RSCRLR_DBLCLK,
                               usEF = (SHORT1FROMMP(mp1) == 1)
                                          ? ID_CRDI_RSCRLR_STATUS
                                          : ID_CRDI_RSCRLR_STATUS2;

                        ULONG ulTotal = (ULONG)WinSendDlgItemMsg(hwnd,
                                                          usMLE,
                                                          MLM_QUERYLINECOUNT,
                                                          0,
                                                          0);
                        ULONG ulCurrent = 1 + (ULONG)WinSendDlgItemMsg(hwnd,
                                                            usMLE,
                                                            MLM_LINEFROMCHAR,
                                                            MPFROMLONG(-1L),
                                                            0);
                        ULONG ulColumn = 1 + (ULONG)WinSendDlgItemMsg(hwnd,
                                                        usMLE,
                                                        MLM_QUERYSEL,
                                                        MPFROMSHORT(MLFQS_CURSORSEL),
                                                        0)
                                       - (ULONG)WinSendDlgItemMsg(hwnd,
                                                        usMLE,
                                                        MLM_CHARFROMLINE,
                                                        MPFROMLONG(-1L),
                                                        0);
                        CHAR achBuffer[100];

                        if (    (pStorage->ulTotal != ulTotal)
                             || (pStorage->ulCurrent != ulCurrent)
                             || (pStorage->ulColumn != ulColumn)
                           )
                        {
                            sprintf(achBuffer, pszSettingsStatus,
                                               ulCurrent,
                                               ulTotal,
                                               ulColumn);
                            pStorage->ulTotal = ulTotal;
                            pStorage->ulCurrent = ulCurrent;
                            pStorage->ulColumn = ulColumn;
                            WinSetDlgItemText(hwnd, usEF, achBuffer);
                        }
                    }
                }
            }
            else
                mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
        break;

        default:
            mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
    }

    return (mrc);
}

/* ******************************************************************
 *
 *   Callbacks stored in XCENTERWIDGETCLASS
 *
 ********************************************************************/

/*
 *@@ RwgtHelpHook:
 *      this handles the help requests from the message boxes.
 *
 */

BOOL EXPENTRY RwgtHelpHook(HAB hab,
                           ULONG usMode,
                           ULONG idTopic,
                           ULONG idSubTopic,
                           PRECTL prcPosition)
{
    BOOL rc = FALSE;

    switch ((USHORT)idTopic)
    {
        case ID_CRH_RSCRLR_DBLCLKERROR1:
        case ID_CRH_RSCRLR_DBLCLKERROR2:
        case ID_CRH_RSCRLR_TIMERERROR:
            // the hook is _global_, so we must process this message
            // only if it comes from one of our windows.  G_hwnd is
            // used for this purpose, as there is no other way to
            // know if the help request came from one of our message
            // box.
            if (G_hwnd)
            {
                PWIDGETSETTINGSDLGDATA pData = (PWIDGETSETTINGSDLGDATA)WinQueryWindowPtr(G_hwnd, QWL_USER);

                if (pData)
                    pctrDisplayHelp(pData->pGlobals,
                                    RwgtQueryHelpLibrary(),
                                    (USHORT)idTopic);
                rc = TRUE;
            }
        break;
    }

    return (rc);
}

/*
 *@@ RwgtShowSettingsDlg:
 *      this displays the rscrlr widget's settings
 *      dialog.
 *
 *      This procedure's address is stored in
 *      XCENTERWIDGET so that the XCenter knows that
 *      we can do this.
 *
 *      When calling this function, the XCenter expects
 *      it to display a modal dialog and not return
 *      until the dialog is destroyed. While the dialog
 *      is displaying, it would be nice to have the
 *      widget dynamically update itself.
 */

VOID EXPENTRY RwgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData)
{
    HWND hwnd = WinLoadDlg(HWND_DESKTOP,         // parent
                           pData->hwndOwner,
                           fnwpSettingsDlg,
                           G_hmodThis,
                           ID_CRD_RSCRLR_SETTINGS,
                           // pass original setup string with WM_INITDLG
                           (PVOID)pData);
    if (hwnd)
    {
        pcmnSetControlsFont(hwnd,
                            1,
                            10000);

        pwinhCenterWindow(hwnd);         // invisibly

        // go!!
        WinProcessDlg(hwnd);

        WinDestroyWindow(hwnd);
    }
}

/*
 *@@ LoadBitmap:
 *
 */

VOID LoadBitmap(HWND hwnd,
                PRSCRLRPRIVATE pPrivate)
{
    HPS hps = WinGetPS(hwnd);
    APIRET arc;

    // free old bmp, if there's one
    if (pPrivate->hbmBgBmp)
    {
        GpiDeleteBitmap(pPrivate->hbmBgBmp);
        pPrivate->hbmBgBmp = NULLHANDLE;
    }

    // load user bitmap
    if (pPrivate->achBgBmp[0] != 0)
        if (arc = gpihLoadBitmapFile(&pPrivate->hbmBgBmp,
                                     hps,
                                     pPrivate->achBgBmp))
            _Pmpf(("Error %d loading background bitmap file \"%s\".",
                    arc,
                    pPrivate->achBgBmp));

    if (    (!pPrivate->hbmBgBmp)
         && (!(pPrivate->hbmBgBmp = GpiLoadBitmap(hps,
                                                  G_hmodThis,
                                                  ID_BGBMP,
                                                  0,
                                                  0)))
       )
        _Pmpf(("Cannot load background bitmap"));

    if (pPrivate->hbmBgBmp)
    {
        BITMAPINFOHEADER2 bmih2;
        bmih2.cbFix = sizeof(bmih2);
        GpiQueryBitmapInfoHeader(pPrivate->hbmBgBmp,
                                 &bmih2);
        pPrivate->cxBgBmp = bmih2.cx;
        pPrivate->cyBgBmp = bmih2.cy;
    }
    WinReleasePS(hps);
}

/*
 *@@ RwgtInitializeScrlrStem
 *      Initialize the SCRLR.USER stem value.
 *
 *      This function is reentrant, and should not allocate heap
 *      memory.  It gets its parameter from the (just completed)
 *      script, using RexxVariablePool.
 *
 */

LONG EXPENTRY RwgtInitializeScrlrStem(LONG exitno,
                                      LONG subfunc,
                                      PUCHAR parmblock)
{
    SHVBLOCK block;
    CHAR     szData[250];
            // placeholder for our queries
    HWND     hwnd = 0;
    USHORT   usKind;

    block.shvnext = 0;
    block.shvcode = RXSHV_PRIV;
    block.shvret  = 0;

    // there's either 1 or 4 params
    MAKERXSTRING(block.shvname, "PARM", 4);
    MAKERXSTRING(block.shvvalue, szData, 250-1);

    RexxVariablePool(&block);

    szData[block.shvvaluelen] = 0;

    usKind = atoi(szData);

    // we need to query the hwnd handle from the script (arg(1) or arg(4))
    MAKERXSTRING(block.shvname, (usKind == 1) ? "PARM.1" : "PARM.4", 6);
    MAKERXSTRING(block.shvvalue, szData, 250-1);

    RexxVariablePool(&block);

    // sscanf needs a null-terminated string, and we have enough space
    szData[block.shvvaluelen] = 0;
    sscanf(szData, "%lX", &hwnd);

    if (hwnd)
    {
        PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

        if (pWidget)
        {
            PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;

            if (pPrivate)
            {
                block.shvcode = RXSHV_SET;

                // user data
                MAKERXSTRING(block.shvname, "SCRLR.USER", 10);
                MAKERXSTRING(block.shvvalue, pPrivate->abUserData, pPrivate->usUserDataLength);

                RexxVariablePool(&block);
            }
        }
    }

    return 0;
}

/*
 *@@ RwgtExtractScrlrStem
 *      Extract the SCRLR. stem values.
 *
 *      This function is reentrant, and should not allocate heap
 *      memory.  It gets its parameter from the (just completed)
 *      script, using RexxVariablePool.
 *
 */

LONG EXPENTRY RwgtExtractScrlrStem(LONG exitno,
                                   LONG subfunc,
                                   PUCHAR parmblock)
{
    SHVBLOCK block;
    CHAR     szData[250];
            // placeholder for our queries
    HWND     hwnd = 0;
    USHORT   usKind;

    block.shvnext = 0;
    block.shvcode = RXSHV_PRIV;
    block.shvret  = 0;

    // the temporary storage area is used for most of our stem queries
    MAKERXSTRING(block.shvvalue, szData, 250-1);

    // there's either 1 or 4 params; we must check here
    MAKERXSTRING(block.shvname, "PARM", 4);
    RexxVariablePool(&block);

    szData[block.shvvaluelen] = 0;
    usKind = atoi(szData);

    // we need to query the hwnd handle from the script (arg(1))
    MAKERXSTRING(block.shvname, (usKind == 1) ? "PARM.1" : "PARM.4", 6);
    RexxVariablePool(&block);

    // sscanf needs a null-terminated string, and we have enough space
    szData[block.shvvaluelen] = 0;
    sscanf(szData, "%lX", &hwnd);

    if (hwnd)
    {
        PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

        if (pWidget)
        {
            PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;

            if (pPrivate)
            {
                block.shvcode = RXSHV_FETCH;

                if (usKind == 1)
                {
                    CHAR    achSaveBmpName[250];

                    // this is the timer script, so we must collect
                    // all possible changes

                    // lVal1
                    MAKERXSTRING(block.shvname, "SCRLR.1", 7);
                    RexxVariablePool(&block);
                    if (block.shvret == RXSHV_OK)
                    {
                        szData[block.shvvaluelen] = 0;
                        pPrivate->lVal1 = atoi(szData);

                        if (pPrivate->lVal1 != 0)
                        {
                            MAKERXSTRING(block.shvname, "SCRLR.COLOR1", 12);
                            RexxVariablePool(&block);
                            if (block.shvret == RXSHV_OK)
                            {
                                szData[block.shvvaluelen] = 0;
                                pPrivate->lcol1 = pctrParseColorString(szData);
                            }
                            else
                                pPrivate->lcol1 = -1L;
                        }
                    }
                    else
                        pPrivate->lVal1 = 0;

                    // lVal2
                    MAKERXSTRING(block.shvname, "SCRLR.2", 7);
                    RexxVariablePool(&block);
                    if (block.shvret == RXSHV_OK)
                    {
                        szData[block.shvvaluelen] = 0;
                        pPrivate->lVal2 = atoi(szData);

                        if (pPrivate->lVal2 != 0)
                        {
                            MAKERXSTRING(block.shvname, "SCRLR.COLOR2", 12);
                            RexxVariablePool(&block);
                            if (block.shvret == RXSHV_OK)
                            {
                                szData[block.shvvaluelen] = 0;
                                pPrivate->lcol2 = pctrParseColorString(szData);
                            }
                            else
                                pPrivate->lcol2 = -1L;
                        }
                    }
                    else
                        pPrivate->lVal2 = 0;

                    // lVal3
                    MAKERXSTRING(block.shvname, "SCRLR.3", 7);
                    RexxVariablePool(&block);
                    if (block.shvret == RXSHV_OK)
                    {
                        szData[block.shvvaluelen] = 0;
                        pPrivate->lVal3 = atoi(szData);

                        if (pPrivate->lVal3 != 0)
                        {
                            MAKERXSTRING(block.shvname, "SCRLR.COLOR3", 12);
                            RexxVariablePool(&block);
                            if (block.shvret == RXSHV_OK)
                            {
                                szData[block.shvvaluelen] = 0;
                                pPrivate->lcol3 = pctrParseColorString(szData);
                            }
                            else
                                pPrivate->lcol3 = -1L;
                        }
                    }
                    else
                        pPrivate->lVal3 = 0;

                    // background color
                    MAKERXSTRING(block.shvname, "SCRLR.BACKGROUND", 16);
                    RexxVariablePool(&block);
                    if (block.shvret == RXSHV_OK)
                    {
                        szData[block.shvvaluelen] = 0;
                        pPrivate->lcolBackground = pctrParseColorString(szData);
                    }
                    else
                        pPrivate->lcolBackground = -1L;

                    // text
                    MAKERXSTRING(block.shvname, "SCRLR.TEXT", 10);
                    MAKERXSTRING(block.shvvalue, pPrivate->achText, 100-1);
                    RexxVariablePool(&block);
                    if (block.shvret == RXSHV_OK)
                    {
                        pPrivate->achText[block.shvvaluelen] = 0;

                        if (block.shvvaluelen)
                        {
                            MAKERXSTRING(block.shvname, "SCRLR.FOREGROUND", 16);
                            MAKERXSTRING(block.shvvalue, szData, 250-1);
                            RexxVariablePool(&block);
                            if (block.shvret == RXSHV_OK)
                            {
                                szData[block.shvvaluelen] = 0;
                                pPrivate->lcolForeground = pctrParseColorString(szData);
                            }
                            else
                                pPrivate->lcolForeground = -1L;
                        }
                    }
                    else
                        pPrivate->achText[0] = 0;

                    // tooltip
                    MAKERXSTRING(block.shvname, "SCRLR.TOOLTIP", 13);
                    MAKERXSTRING(block.shvvalue, pPrivate->achTooltip, 250-1);
                    RexxVariablePool(&block);
                    if (block.shvret == RXSHV_OK)
                        pPrivate->achTooltip[block.shvvaluelen] = 0;
                    else
                        pPrivate->achTooltip[0] = 0;

                    // background bitmap
                    strcpy(achSaveBmpName, pPrivate->achBgBmp);   // remember old name
                    MAKERXSTRING(block.shvname, "SCRLR.BITMAP", 12);
                    MAKERXSTRING(block.shvvalue, pPrivate->achBgBmp, 250-1);
                    RexxVariablePool(&block);
                    if (block.shvret == RXSHV_OK)
                        pPrivate->achBgBmp[block.shvvaluelen] = 0;
                    else
                        pPrivate->achBgBmp[0] = 0;
                    if (strcmp(pPrivate->achBgBmp, achSaveBmpName) != 0)
                        LoadBitmap(hwnd, pPrivate);     // bitmap has changed: (re)load
                } // end if (usKind == 1)

                // for all scripts, we collect the user data area

                MAKERXSTRING(block.shvname, "SCRLR.USER", 10);
                MAKERXSTRING(block.shvvalue, szData, USERDATASIZE);
                RexxVariablePool(&block);
                if (block.shvret == RXSHV_OK)
                {
                    memcpy(pPrivate->abUserData, szData, block.shvvaluelen);
                    pPrivate->usUserDataLength = block.shvvaluelen;
                }
            } // end if (pPrivate)
        } // end if (pWidget)
    } // end if (hwnd)

    return 0;
}

/* ******************************************************************
 *
 *   PM window class implementation
 *
 ********************************************************************/

/*
 *      This code has the actual PM window class.
 *
 */

/*
 *@@ RwgtCreate:
 *      implementation for WM_CREATE.
 *
 *@added V0.7.1 (2003-01-19) [lafaix]: do not start script if VK_CTRL down
 */

MRESULT RwgtCreate(HWND hwnd,
                   PXCENTERWIDGET pWidget)
{
    MRESULT mrc = 0;
    PSZ p;
    PRSCRLRPRIVATE pPrivate = calloc(1, sizeof(RSCRLRPRIVATE));

    // link the two together
    pWidget->pUser = pPrivate;
    pPrivate->pWidget = pWidget;

    // initialize binary setup structure from setup string
    RwgtScanSetup(pWidget->pcszSetupString,
                  &pPrivate->Setup);

    // set window font (this affects all the cached presentation
    // spaces we use)
    pwinhSetWindowFont(hwnd,
                       (pPrivate->Setup.pszFont)
                        ? pPrivate->Setup.pszFont
                        // default font: use the same as in the rest of XWorkplace:
                        : pcmnQueryDefaultFont());

    // ensure instore is really null
    /* already handled by calloc(...) above.
    pPrivate->pszInstore = NULL;
    pPrivate->lInstoreSize = 0;
    */

    // sets resizeable property
    pWidget->fSizeable = pPrivate->Setup.fSizeable;

    // if you want the context menu help to be enabled,
    // add your help library here; if these fields are
    // left NULL, the "Help" context menu item is disabled

    pWidget->pcszHelpLibrary = RwgtQueryHelpLibrary();
    pWidget->ulHelpPanelID = ID_CRH_RSCRLR_MAIN;

    // specifying default runtime colors
    pPrivate->lcolForeground =
    pPrivate->lcolBackground =
    pPrivate->lcol1          =
    pPrivate->lcol2          =
    pPrivate->lcol3          = -1L;

    // specify graph update
    pPrivate->fUpdateGraph = TRUE;

    // computing hex representation for hwnd handle (passed to scripts)
    sprintf(pPrivate->achHWND, "%08lX", (LONG)hwnd);

    // run the script once, so that the display is correct on
    // startup (the timer send ticks _after_ its expiration delay)
    // V0.7.1 (2003-01-19) [lafaix]: VK_CTRL check added
    if ((WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000) == 0)
        if (RwgtTimer(hwnd, pWidget))
        {
            // start update timer, as the script looks OK (there is no
            // need starting the timer if the script is invalid)
            pPrivate->ulTimerID = ptmrStartXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                                                  hwnd,
                                                  1,
                                                  pPrivate->Setup.ulTimerDelay);
        }

    return (mrc);
}

/*
 *@@ RwgtButton1DblClk:
 *      Prepare parameters and run the script.
 *
 */

void RwgtButton1DblClk(HWND hwnd,
                      MPARAM mp1,
                      MPARAM mp2)
{
    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

    if (pWidget)
    {
        PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;

        if (pPrivate)
        {
            // our structure is not multiple-instances--aware yet so
            // we better check it out beforehand (even if it is for
            // safety's sake, as this normally cannot happen).
            if (pPrivate->fBusy)
            {
                G_hwnd = hwnd;
                WinMessageBox(HWND_DESKTOP,
                              hwnd,
                              pszAlreadyRunning,
                              pPrivate->Setup.pszTitle,
                              ID_CRH_RSCRLR_DBLCLKERROR1,
                              MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);
                G_hwnd = NULLHANDLE;
            }
            else
            if (    (pPrivate->Setup.pszDblClkScript)
                 && (strlen(pPrivate->Setup.pszDblClkScript) > 4)
                 && (pPrivate->Setup.pszDblClkScript[0] = '/')
                 && (pPrivate->Setup.pszDblClkScript[1] = '*')
               )
            {
                // its a REXX script; run it immediately
                RECTL    rclWin;
                RXSTRING params[4];
                RXSTRING instore[2];
                RXSTRING retstr;
                SHORT    src;
                LONG     rc;
                CHAR     achBuffer[250];

                pPrivate->fBusy = TRUE;

                WinQueryWindowRect(hwnd, &rclWin);

                // Calling the script with 5 args:
                // 1- X normalized position ([0..100])
                // 2- Y normalized position ([0..100])
                // 3- modifiers
                // 4- current peak value
                // 5- hwnd (in hex)
                sprintf(pPrivate->achX, "%d", SHORT1FROMMP(mp1) * 100 / (rclWin.xRight-rclWin.xLeft));
                sprintf(pPrivate->achY, "%d", SHORT2FROMMP(mp1) * 100 / (rclWin.yTop-rclWin.yBottom));
                sprintf(pPrivate->achModifiers, "%d", SHORT2FROMMP(mp2));

                MAKERXSTRING(instore[0], pPrivate->Setup.pszDblClkScript, strlen(pPrivate->Setup.pszDblClkScript));
                MAKERXSTRING(instore[1], NULL, 0);

                MAKERXSTRING(params[0], pPrivate->achX, strlen(pPrivate->achX));
                MAKERXSTRING(params[1], pPrivate->achY, strlen(pPrivate->achY));
                MAKERXSTRING(params[2], pPrivate->achModifiers, strlen(pPrivate->achModifiers));
                MAKERXSTRING(params[3], pPrivate->achHWND, 8);
                MAKERXSTRING(retstr, achBuffer, sizeof(achBuffer));

                #define excHandlerLoud pexcHandlerLoud
                        // TRY_LOUD expects excHandlerLoud, not pexcHandlerLoud

                TRY_LOUD(excpt1)
                {
                    rc = RexxStart(4,
                                   &params[0],
                                   "RexxScrlrDblClk",
                                   &instore[0],
                                   "CMD",
                                   RXCOMMAND,
                                   G_exit_list,
                                   &src,
                                   &retstr);

                    if (rc)
                        _Pmpf(("RexxStart returns %ld", rc));
                }
                CATCH(excpt1) {}  END_CATCH();

                pPrivate->fBusy = 0; // not running anymore

                if (rc < 0)
                {
                    // a REXX interpreter error occured; tell so
                    CHAR     szBuf[500];

                    sprintf(szBuf, pszInterpreterErrorDblClk, -rc);

                    G_hwnd = hwnd;
                    WinMessageBox(HWND_DESKTOP,
                                  hwnd,
                                  szBuf,
                                  pPrivate->Setup.pszTitle,
                                  ID_CRH_RSCRLR_DBLCLKERROR2,
                                  MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);

                    if (G_hwnd == hwnd)
                        G_hwnd = NULLHANDLE;
                }

                // release the tokenized code
                if (RXSTRPTR(instore[1]))
                    DosFreeMem(RXSTRPTR(instore[1]));

                // release the returned value if needed
                if (RXSTRPTR(retstr) != achBuffer)
                    DosFreeMem(RXSTRPTR(retstr));
            }
        } // end if (pPrivate)
    } // end if (pWidget)
}

/*
 *@@ RwgtControl:
 *      implementation for WM_CONTROL.
 *
 *      The XCenter communicates with widgets thru
 *      WM_CONTROL messages. At the very least, the
 *      widget should respond to XN_QUERYSIZE because
 *      otherwise it will be given some dumb default
 *      size.
 *
 */

BOOL RwgtControl(HWND hwnd,
                 MPARAM mp1,
                 MPARAM mp2)
{
    BOOL frc = FALSE;

    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    if (pWidget)
    {
        // get private data from that widget data
        PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;
        if (pPrivate)
        {
            USHORT  usID = SHORT1FROMMP(mp1),
                    usNotifyCode = SHORT2FROMMP(mp1);

            // is this from the XCenter client?
            if (usID == ID_XCENTER_CLIENT)
            {
                // yes:

                switch (usNotifyCode)
                {
                    /*
                     * XN_QUERYSIZE:
                     *      XCenter wants to know our size.
                     */

                    case XN_QUERYSIZE:
                    {
                        PSIZEL pszl = (PSIZEL)mp2;
                        pszl->cx = pPrivate->Setup.ulWidth;
                        pszl->cy = pPrivate->Setup.ulHeight;

                        frc = TRUE;
                    break; }

                    /*
                     * XN_SETUPCHANGED:
                     *      XCenter has a new setup string for
                     *      us in mp2.
                     */

                    case XN_SETUPCHANGED:
                    {
                        const char *pcszNewSetupString = (const char*)mp2;

                        // stop timer
                        if (pPrivate->ulTimerID)
                            ptmrStopXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                                           hwnd,
                                           pPrivate->ulTimerID);

                        // reinitialize the setup data
                        RwgtClearSetup(&pPrivate->Setup);
                        RwgtScanSetup(pcszNewSetupString, &pPrivate->Setup);

                        // sets resizeable property
                        pWidget->fSizeable = pPrivate->Setup.fSizeable;

                        // asks the xcenter to relayout
                        WinPostMsg(pWidget->pGlobals->hwndClient,
                                   XCM_REFORMAT,
                                   (MPARAM)(XFMF_GETWIDGETSIZES|XFMF_REPOSITIONWIDGETS),
                                   (MPARAM)0);

                        // reinitialize instore
                        if (pPrivate->pszInstore)
                            DosFreeMem(pPrivate->pszInstore);
                        pPrivate->pszInstore = NULL;
                        pPrivate->lInstoreSize = 0;

                        // re-run scroller script, so that the display
                        // is up to date
                        if (RwgtTimer(hwnd, pWidget))
                        {
                            // reinitialize timer if first run is successful
                            pPrivate->ulTimerID = ptmrStartXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                                                                  hwnd,
                                                                  1,
                                                                  pPrivate->Setup.ulTimerDelay);
                        }
                    break; }
                }
            }
            else
            {
                if (usID == ID_XCENTER_TOOLTIP)
                {
                    switch (usNotifyCode)
                    {

                       /*
                        * TTN_NEEDTEXT:
                        *      XCenter wants to know our tooltip text.
                        */

                        case TTN_NEEDTEXT:
                        {
                            PTOOLTIPTEXT pttt = (PTOOLTIPTEXT)mp2;

                            if (pPrivate->achTooltip[0] == 0)
                                pttt->pszText = pPrivate->Setup.pszTitle;
                            else
                                pttt->pszText = pPrivate->achTooltip;

                            pttt->ulFormat = TTFMT_PSZ;
                        break; }

                        case TTN_SHOW:
                            pPrivate->fTooltipShowing = TRUE;
                        break;

                        case TTN_POP:
                            pPrivate->fTooltipShowing = FALSE;
                        break;
                    }
                }
            }
        } // end if (pPrivate)
    } // end if (pWidget)

    return (frc);
}

/*
 *@@ RwgtUpdateGraph:
 *      updates the graph bitmap. This does not paint
 *      on the screen yet.
 *
 *      Preconditions:
 *
 *      --  pPrivate->pBitmap must be ready.
 *
 */

VOID RwgtUpdateGraph(HWND hwnd,
                     PRSCRLRPRIVATE pPrivate)
{
    PXCENTERWIDGET pWidget = pPrivate->pWidget;

    RECTL   rclBmp;
    POINTL  ptl;
    ULONG   ul = 0;
    BOOL    fAvg = pPrivate->Setup.fAverage;
    LONG    lBL = 0;    // baseline position

    // size for bitmap: same as widget, except
    // for the border
    WinQueryWindowRect(hwnd, &rclBmp);

    rclBmp.xRight -= 2;
    rclBmp.yTop -= 2;

    if (!pPrivate->pBitmap) {
        pPrivate->pBitmap = pgpihCreateXBitmap(pWidget->habWidget,
                                              rclBmp.xRight,
                                              rclBmp.yTop);
    }

    if (pPrivate->pBitmap)
    {
        HPS     hpsMem = pPrivate->pBitmap->hpsMem;

        if (!pPrivate->cValues)
        {
            // fill the bitmap rectangle
            if (pPrivate->hbmBgBmp != NULLHANDLE)
            {
                WinDrawBitmap(hpsMem,
                                pPrivate->hbmBgBmp,
                                NULL,
                                (PPOINTL)&rclBmp,
                                0,
                                0,
                                DBM_STRETCH);
            }
            else
            {
                GpiSetColor(hpsMem,
                            (pPrivate->lcolBackground == -1L)
                                ? pPrivate->Setup.lcolBackground
                                : pPrivate->lcolBackground);

                pgpihBox(hpsMem,
                        DRO_FILL,
                        &rclBmp);
            }
        }
        else
        {
            // find the max value first
            LONG lMaxValue = 0;
            for (ul = 0;
                 ((ul < pPrivate->cValues) && (ul < rclBmp.xRight));
                 ul++)
            {
                LONG lThis;

                lThis = pPrivate->palValues1[fAvg][ul];
                lThis = labs(lThis);
                if (lThis > lMaxValue)
                    lMaxValue = lThis;

                lThis = pPrivate->palValues2[fAvg][ul];
                lThis = labs(lThis);
                if (lThis > lMaxValue)
                    lMaxValue = lThis;

                lThis = pPrivate->palValues3[fAvg][ul];
                lThis = labs(lThis);
                if (lThis > lMaxValue)
                    lMaxValue = lThis;
            }

            // Set the baseline
            lBL = (rclBmp.yTop / 2) * (pPrivate->Setup.fBLMode);
                // if fBLMode is FALSE, this always returns 0

            // fill the bitmap rectangle
            if (pPrivate->hbmBgBmp != NULLHANDLE)
            {
                WinDrawBitmap(hpsMem,
                                pPrivate->hbmBgBmp,
                                NULL,
                                (PPOINTL)&rclBmp,
                                0,
                                0,
                                DBM_STRETCH);
            }
            else
            {
                GpiSetColor(hpsMem,
                            (pPrivate->lcolBackground == -1L)
                                ? pPrivate->Setup.lcolBackground
                                : pPrivate->lcolBackground);

                pgpihBox(hpsMem,
                        DRO_FILL,
                        &rclBmp);
            }

            if (lMaxValue)  // avoid division by zero
            {
                // go thru all values in the third LONG array
                ptl.x = 0;
                ptl.y = lBL;
                GpiMove(hpsMem, &ptl);

                for (ptl.x = 0;
                     ((ptl.x < pPrivate->cValues) && (ptl.x < rclBmp.xRight));
                     ptl.x++)
                {
                    // part 3
                    ptl.y = lBL;
                    if (pPrivate->palValues3[fAvg])
                    {
                        LONG lThis = pPrivate->palValues3[fAvg][ptl.x];
                        if ((lThis < 0) && !pPrivate->Setup.fBLMode) lThis = labs(lThis);

                        GpiSetColor(hpsMem,
                                    (pPrivate->lcol3 == -1L)
                                        ? pPrivate->Setup.lcol3
                                        : pPrivate->lcol3);

                        if (pPrivate->Setup.fFilled) GpiMove(hpsMem, &ptl);
                        ptl.y = lBL + ((lThis * (rclBmp.yTop - lBL)) / lMaxValue);
                        if (ptl.y != lBL)
                            GpiLine(hpsMem, &ptl);
                        else {
                            POINTL tmp;
                            BOOL rc;

                            rc = GpiQueryCurrentPosition(hpsMem, &tmp);
                            if ((rc) && (tmp.y == lBL))
                                GpiMove(hpsMem, &ptl);
                            else {
                                GpiLine(hpsMem, &ptl);
                            }
                        }
                    }
                } // end for


                // go thru all values in the second LONG array
                ptl.x = 0;
                ptl.y = lBL;
                GpiMove(hpsMem, &ptl);

                for (ptl.x = 0;
                     ((ptl.x < pPrivate->cValues) && (ptl.x < rclBmp.xRight));
                     ptl.x++)
                {
                    // part 2
                    ptl.y = lBL;
                    if (pPrivate->palValues2[fAvg])
                    {
                        LONG lThis = pPrivate->palValues2[fAvg][ptl.x];
                        if ((lThis < 0) && !pPrivate->Setup.fBLMode) lThis = labs(lThis);

                        GpiSetColor(hpsMem,
                                    (pPrivate->lcol2 == -1L)
                                        ? pPrivate->Setup.lcol2
                                        : pPrivate->lcol2);

                        if (pPrivate->Setup.fFilled) GpiMove(hpsMem, &ptl);
                        ptl.y = lBL + ((lThis * (rclBmp.yTop - lBL)) / lMaxValue);
                        if (ptl.y != lBL)
                            GpiLine(hpsMem, &ptl);
                        else {
                            POINTL tmp;
                            BOOL rc;

                            rc = GpiQueryCurrentPosition(hpsMem, &tmp);
                            if ((rc) && (tmp.y == lBL))
                                GpiMove(hpsMem, &ptl);
                            else
                                GpiLine(hpsMem, &ptl);
                        }
                    }
                } // end for

                // go thru all values in the first LONG array
                ptl.x = 0;
                ptl.y = lBL;
                GpiMove(hpsMem, &ptl);

                for (ptl.x = 0;
                     ((ptl.x < pPrivate->cValues) && (ptl.x < rclBmp.xRight));
                     ptl.x++)
                {
                    // part 1
                    ptl.y = lBL;
                    if (pPrivate->palValues1[fAvg])
                    {
                        LONG lThis = pPrivate->palValues1[fAvg][ptl.x];
                        if ((lThis < 0) && !pPrivate->Setup.fBLMode) lThis = labs(lThis);

                        GpiSetColor(hpsMem,
                                    (pPrivate->lcol1 == -1L)
                                        ? pPrivate->Setup.lcol1
                                        : pPrivate->lcol1);

                        if (pPrivate->Setup.fFilled) GpiMove(hpsMem, &ptl);
                        ptl.y = lBL + ((lThis * (rclBmp.yTop - lBL)) / lMaxValue);
                        if (ptl.y != lBL)
                            GpiLine(hpsMem, &ptl);
                        else {
                            POINTL tmp;
                            BOOL rc;

                            rc = GpiQueryCurrentPosition(hpsMem, &tmp);
                            if ((rc) && (tmp.y == lBL))
                                GpiMove(hpsMem, &ptl);
                            else
                                GpiLine(hpsMem, &ptl);
                        }
                    }
                } // end for
            }

            // Draw the baseline
            if (lBL > 0)
            {
                ptl.x = 0;
                ptl.y = lBL;

                GpiMove(hpsMem, &ptl);
                GpiSetColor(hpsMem, pWidget->pGlobals->lcol3DDark);
                ptl.x = pPrivate->cValues - 1;
                ptl.y = lBL;
                GpiLine(hpsMem, &ptl);
            }
        }
    }
    pPrivate->fUpdateGraph = FALSE;
}

/*
 * RwgtPaint2:
 *      this does the actual painting of the bitmap.
 *
 *      Gets called by RwgtPaint.
 *
 *      The specified HPS is switched to RGB mode before
 *      painting.
 *
 */

VOID RwgtPaint2(HWND hwnd,
                HPS hps,
                PRSCRLRPRIVATE pPrivate)
{
    PXCENTERWIDGET pWidget = pPrivate->pWidget;
    RECTL       rclWin;
    ULONG       ulBorder = 1;
    POINTL      ptlBmpDest;

    WinQueryWindowRect(hwnd, &rclWin);
        // exclusive

    pgpihSwitchToRGB(hps);

    rclWin.xRight--;
    rclWin.yTop--;
        // rclWin is now inclusive

    if (pPrivate->fUpdateGraph)
        // graph bitmap needs to be updated:
        RwgtUpdateGraph(hwnd, pPrivate);

    ptlBmpDest.x = rclWin.xLeft + ulBorder;
    ptlBmpDest.y = rclWin.yBottom + ulBorder;

    // now paint graph from bitmap
    WinDrawBitmap(hps,
                  pPrivate->pBitmap->hbm,
                  NULL,     // entire bitmap
                  &ptlBmpDest,
                  0, 0,
                  DBM_NORMAL);
}


/*
 *@@ RwgtPaint:
 *      implementation for WM_PAINT.
 *
 *      This really does nothing, except painting the background,
 *      and calling RwgtPaint2.
 *
 *      The scroller values are not computed here.  This is done
 *      in RwgtTimer.
 *
 */

VOID RwgtPaint(HWND hwnd,
               PXCENTERWIDGET pWidget)
{
    HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);

    if (hps)
    {
        PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;
        if (pPrivate)
        {
            RECTL       rclWin;
            LONG lDark, lLight;
            ULONG ulTextLength;
            ULONG ulBorder = 1;

            WinQueryWindowRect(hwnd, &rclWin);        // exclusive
            pgpihSwitchToRGB(hps);

            rclWin.xRight--;
            rclWin.yTop--;
                // rclWin now inclusive

            if (pPrivate->pWidget->pGlobals->flDisplayStyle & XCS_SUNKBORDERS)
            {
                lDark = pWidget->pGlobals->lcol3DDark;
                lLight = pWidget->pGlobals->lcol3DLight;
            }
            else
            {
                lDark =
                lLight = (pPrivate->lcolBackground == -1L)
                             ? pPrivate->Setup.lcolBackground
                             : pPrivate->lcolBackground;
            }

            pgpihDraw3DFrame(hps,
                             &rclWin,        // inclusive
                             ulBorder,
                             lDark,
                             lLight);

            rclWin.xLeft++;
            rclWin.yBottom++;
                // rclWin now exclusive and shrunk

            RwgtPaint2(hwnd, hps, pPrivate);

            // draw the text, centered
            ulTextLength = strlen(pPrivate->achText);
            if (ulTextLength)
                WinDrawText(hps,
                            ulTextLength,
                            pPrivate->achText,
                            &rclWin,
                            (pPrivate->lcolForeground == -1L)
                                ? pPrivate->Setup.lcolForeground
                                : pPrivate->lcolForeground,
                            0,      // background, ignored anyway
                            DT_CENTER | DT_VCENTER);
        }
        WinEndPaint(hps);
    }
}

/*
 *@@ RwgtWindowPosChanged:
 *      implementation for WM_WINDOWPOSCHANGED.
 *
 */

VOID RwgtWindowPosChanged(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    if (pWidget)
    {
        PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;
        if (pPrivate)
        {
            PSWP pswpNew = (PSWP)mp1,
                 pswpOld = pswpNew + 1;
            if (pswpNew->fl & SWP_SIZE)
            {
                // window was resized:

                // destroy the bitmap because we need a new one
                if (pPrivate->pBitmap)
                    pgpihDestroyXBitmap(&pPrivate->pBitmap);

                if (pswpNew->cx != pswpOld->cx)
                {
                    XSTRING strSetup;
                    // width changed:

                    if (pPrivate->palValues1[0])
                    {
                        // we also need a new array of past values
                        // since the array is cx items wide...
                        // so reallocate the array, but keep past
                        // values
                        ULONG ulNewClientCX = pswpNew->cx - 2;
                        PLONG palNewValues = (PLONG)malloc(sizeof(LONG) * ulNewClientCX);
                        PLONG palNewValues1 = (PLONG)malloc(sizeof(LONG) * ulNewClientCX);

                        if (ulNewClientCX > pPrivate->cValues)
                        {
                            // window has become wider:
                            // fill the front with zeroes
                            memset(palNewValues,
                                   0,
                                   (ulNewClientCX - pPrivate->cValues) * sizeof(LONG));
                            memset(palNewValues1,
                                   0,
                                   (ulNewClientCX - pPrivate->cValues) * sizeof(LONG));
                            // and copy old values after that
                            memcpy(&palNewValues[(ulNewClientCX - pPrivate->cValues)],
                                   pPrivate->palValues1[0],
                                   pPrivate->cValues * sizeof(LONG));
                            memcpy(&palNewValues1[(ulNewClientCX - pPrivate->cValues)],
                                   pPrivate->palValues1[1],
                                   pPrivate->cValues * sizeof(LONG));
                        }
                        else
                        {
                            // window has become smaller:
                            // e.g. ulnewClientCX = 100
                            //      pPrivate->cValues = 200
                            // drop the first items
                            memcpy(palNewValues,
                                   &pPrivate->palValues1[0][pPrivate->cValues - ulNewClientCX],
                                   ulNewClientCX * sizeof(LONG));
                            memcpy(palNewValues1,
                                   &pPrivate->palValues1[1][pPrivate->cValues - ulNewClientCX],
                                   ulNewClientCX * sizeof(LONG));
                        }

                        free(pPrivate->palValues1[0]);
                        pPrivate->palValues1[0] = palNewValues;
                        free(pPrivate->palValues1[1]);
                        pPrivate->palValues1[1] = palNewValues1;

                        // do the same for the second array
                        if (pPrivate->palValues2[0])
                        {
                            PLONG palNewValues2 = (PLONG)malloc(sizeof(LONG) * ulNewClientCX);
                            PLONG palNewValues21 = (PLONG)malloc(sizeof(LONG) * ulNewClientCX);

                            if (ulNewClientCX > pPrivate->cValues)
                            {
                                // window has become wider:
                                // fill the front with zeroes
                                memset(palNewValues2,
                                       0,
                                       (ulNewClientCX - pPrivate->cValues) * sizeof(LONG));
                                memset(palNewValues21,
                                       0,
                                       (ulNewClientCX - pPrivate->cValues) * sizeof(LONG));
                                // and copy old values after that
                                memcpy(&palNewValues2[(ulNewClientCX - pPrivate->cValues)],
                                       pPrivate->palValues2[0],
                                       pPrivate->cValues * sizeof(LONG));
                                memcpy(&palNewValues21[(ulNewClientCX - pPrivate->cValues)],
                                       pPrivate->palValues2[1],
                                       pPrivate->cValues * sizeof(LONG));
                            }
                            else
                            {
                                // window has become smaller:
                                // drop the first items
                                memcpy(palNewValues2,
                                       &pPrivate->palValues2[0][pPrivate->cValues - ulNewClientCX],
                                       ulNewClientCX * sizeof(LONG));
                                memcpy(palNewValues21,
                                       &pPrivate->palValues2[1][pPrivate->cValues - ulNewClientCX],
                                       ulNewClientCX * sizeof(LONG));
                            }

                            free(pPrivate->palValues2[0]);
                            pPrivate->palValues2[0] = palNewValues2;
                            free(pPrivate->palValues2[1]);
                            pPrivate->palValues2[1] = palNewValues21;
                        }

                        // do the same for the third array
                        if (pPrivate->palValues3[0])
                        {
                            PLONG palNewValues3 = (PLONG)malloc(sizeof(LONG) * ulNewClientCX);
                            PLONG palNewValues31 = (PLONG)malloc(sizeof(LONG) * ulNewClientCX);

                            if (ulNewClientCX > pPrivate->cValues)
                            {
                                // window has become wider:
                                // fill the front with zeroes
                                memset(palNewValues3,
                                       0,
                                       (ulNewClientCX - pPrivate->cValues) * sizeof(LONG));
                                memset(palNewValues31,
                                       0,
                                       (ulNewClientCX - pPrivate->cValues) * sizeof(LONG));
                                // and copy old values after that
                                memcpy(&palNewValues3[(ulNewClientCX - pPrivate->cValues)],
                                       pPrivate->palValues3[0],
                                       pPrivate->cValues * sizeof(LONG));
                                memcpy(&palNewValues31[(ulNewClientCX - pPrivate->cValues)],
                                       pPrivate->palValues3[1],
                                       pPrivate->cValues * sizeof(LONG));
                            }
                            else
                            {
                                // window has become smaller:
                                // drop the first items
                                memcpy(palNewValues3,
                                       &pPrivate->palValues3[0][pPrivate->cValues - ulNewClientCX],
                                       ulNewClientCX * sizeof(LONG));
                                memcpy(palNewValues31,
                                       &pPrivate->palValues3[1][pPrivate->cValues - ulNewClientCX],
                                       ulNewClientCX * sizeof(LONG));
                            }

                            free(pPrivate->palValues3[0]);
                            pPrivate->palValues3[0] = palNewValues3;
                            free(pPrivate->palValues3[1]);
                            pPrivate->palValues3[1] = palNewValues31;
                        }

                        pPrivate->cValues = ulNewClientCX;

                    } // end if (pPrivate->palValues1[0])

                    pPrivate->Setup.ulWidth = pswpNew->cx;
                    RwgtSaveSetup(&strSetup,
                                  &pPrivate->Setup);
                    if (strSetup.ulLength)
                        WinSendMsg(pWidget->pGlobals->hwndClient,
                                   XCM_SAVESETUP,
                                   (MPARAM)hwnd,
                                   (MPARAM)strSetup.psz);
                    pxstrClear(&strSetup);
                } // end if (pswpNew->cx != pswpOld->cx)

                // force recreation of bitmap
                pPrivate->fUpdateGraph = TRUE;
                WinInvalidateRect(hwnd, NULL, FALSE);

            } // end if (pswpNew->fl & SWP_SIZE)
        } // end if (pPrivate)
    } // end if (pWidget)
}

/*
 *@@ RwgtTimer:
 *      updates the scroller values by running the script, updates the
 *      window.
 *
 *      pWidget must not be NULL.
 *
 *      Returns TRUE is the script ran without errors, FALSE otherwise.
 */

BOOL RwgtTimer(HWND hwnd, PXCENTERWIDGET pWidget)
{
    BOOL bRC = TRUE;
    PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;

    if (    (pPrivate)
         && (pPrivate->fBusy == 0)           // script not already running
         && (pPrivate->Setup.pszScript)      // a script exists
         && (pPrivate->Setup.ulScriptLength > 4)
         && (pPrivate->Setup.pszScript[0] = '/')
         && (pPrivate->Setup.pszScript[1] = '*')
                                             // the script starts with a
                                             // comment
       )
    {
        // run script
        RXSTRING params[2],
                 instore[2],
                 retstr;
        SHORT    src;
        LONG     rc, i;
        CHAR     achBuffer[250];

        pPrivate->fBusy = TRUE;

        // calling the script with one arg (our hwnd)
        MAKERXSTRING(params[0], pPrivate->achHWND, 8);

        MAKERXSTRING(instore[0], pPrivate->Setup.pszScript, pPrivate->Setup.ulScriptLength);
        MAKERXSTRING(instore[1], pPrivate->pszInstore, pPrivate->lInstoreSize);

        MAKERXSTRING(retstr, achBuffer, sizeof(achBuffer));

        TRY_LOUD(excpt1)
        {
            rc = RexxStart(1,
                           &params[0],
                           "RexxScrlrTimer",
                           &instore[0],
                           "CMD",
                           RXCOMMAND,
                           G_exit_list,
                           &src,
                           &retstr);

            if (rc)
                _Pmpf(("RexxStart returns %ld", rc));
        }
        CATCH(excpt1) {}  END_CATCH();

        // we don't have to care about the previous instore
        // content in pPrivate; it's either NULL or the
        // same values
        pPrivate->pszInstore = RXSTRPTR(instore[1]);
        pPrivate->lInstoreSize = RXSTRLEN(instore[1]);

        if (rc < 0)
        {
            // a REXX interpreter error occured; stop the timer
            // and tell so

            // !!! I would like to get the errant line, but I don't
            // !!! know how to get it.  SIGL returns garbage.

            CHAR szBuf[500];

            // stop timer
            if (pPrivate->ulTimerID)
                ptmrStopXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                               hwnd,
                               pPrivate->ulTimerID);
            pPrivate->ulTimerID = 0;

            sprintf(szBuf, pszInterpreterErrorTimer, -rc);
            G_hwnd = hwnd;

            WinMessageBox(HWND_DESKTOP,
                          hwnd,
                          szBuf,
                          pPrivate->Setup.pszTitle,
                          ID_CRH_RSCRLR_TIMERERROR,
                          MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);

            if (G_hwnd == hwnd)
                G_hwnd = NULLHANDLE;

            bRC = FALSE;
        }

        pPrivate->fBusy = 0; // not running anymore

        // release the returned value if needed
        if (RXSTRPTR(retstr) != achBuffer)
            DosFreeMem(RXSTRPTR(retstr));

        if (TRUE)   // dummy for clearness
        {
            RECTL rclClient;
            ULONG ulGraphCX;

            WinQueryWindowRect(hwnd, &rclClient);
            ulGraphCX = rclClient.xRight - 2;       // minus border

            // allocate the values arrays if needed
            if (pPrivate->palValues1[0] == NULL)
            {
                // create array 1
                pPrivate->cValues = ulGraphCX;
                pPrivate->palValues1[0] = (PLONG)calloc((size_t)pPrivate->cValues, sizeof(LONG));
                pPrivate->palValues1[1] = (PLONG)calloc((size_t)pPrivate->cValues, sizeof(LONG));
            }

            if (pPrivate->palValues2[0] == NULL)
            {
                // create array 2
                pPrivate->cValues = ulGraphCX;
                pPrivate->palValues2[0] = (PLONG)calloc((size_t)pPrivate->cValues, sizeof(LONG));
                pPrivate->palValues2[1] = (PLONG)calloc((size_t)pPrivate->cValues, sizeof(LONG));
            }

            if (pPrivate->palValues3[0] == NULL)
            {
                // create array 3
                pPrivate->cValues = ulGraphCX;
                pPrivate->palValues3[0] = (PLONG)calloc((size_t)pPrivate->cValues, sizeof(LONG));
                pPrivate->palValues3[1] = (PLONG)calloc((size_t)pPrivate->cValues, sizeof(LONG));
            }
        }

        // update the values arrays; move each entry one
        // to the front; drop the oldest entry
        // 1
        memcpy(&pPrivate->palValues1[0][0],
               &pPrivate->palValues1[0][1],
               sizeof(LONG) * (pPrivate->cValues - 1));

        memcpy(&pPrivate->palValues1[1][0],
               &pPrivate->palValues1[1][1],
               sizeof(LONG) * (pPrivate->cValues - 1));

        // 2
        memcpy(&pPrivate->palValues2[0][0],
               &pPrivate->palValues2[0][1],
               sizeof(LONG) * (pPrivate->cValues - 1));

        memcpy(&pPrivate->palValues2[1][0],
               &pPrivate->palValues2[1][1],
               sizeof(LONG) * (pPrivate->cValues - 1));

        // 3
        memcpy(&pPrivate->palValues3[0][0],
               &pPrivate->palValues3[0][1],
               sizeof(LONG) * (pPrivate->cValues - 1));

        memcpy(&pPrivate->palValues3[1][0],
               &pPrivate->palValues3[1][1],
               sizeof(LONG) * (pPrivate->cValues - 1));

        // ...and update the last entry with the current value
        // 1
        pPrivate->palValues1[1][pPrivate->cValues - 1]
            = (pPrivate->lVal1 + pPrivate->palValues1[0][pPrivate->cValues - 1]) / 2;
        pPrivate->palValues1[0][pPrivate->cValues - 1]
            = pPrivate->lVal1;

        // 2
        pPrivate->palValues2[1][pPrivate->cValues - 1]
            = (pPrivate->lVal2 + pPrivate->palValues2[0][pPrivate->cValues - 1]) / 2;
        pPrivate->palValues2[0][pPrivate->cValues - 1]
            = pPrivate->lVal2;

        // 3
        pPrivate->palValues3[1][pPrivate->cValues - 1]
            = (pPrivate->lVal3 + pPrivate->palValues3[0][pPrivate->cValues - 1]) / 2;
        pPrivate->palValues3[0][pPrivate->cValues - 1]
            = pPrivate->lVal3;

        // tooltip management
        if (pPrivate->fTooltipShowing)
            // tooltip currently showing:
            // refresh its display
            WinSendMsg(pWidget->pGlobals->hwndTooltip,
                       TTM_UPDATETIPTEXT,
                       (pPrivate->achTooltip[0] == 0)
                           ? (MPARAM)pPrivate->Setup.pszTitle
                           : (MPARAM)pPrivate->achTooltip,
                       0);

        // invalidate window, to cause a refresh
        pPrivate->fUpdateGraph = TRUE;
        WinInvalidateRect(hwnd, NULL, FALSE);

    } // end if (pPrivate)

    return (bRC);
}

/*
 *@@ RwgtDestroy:
 *      implementation for WM_DESTROY.
 *
 *      This must clean up all allocated resources.
 */

VOID RwgtDestroy(HWND hwnd,
                 PXCENTERWIDGET pWidget)
{
    PRSCRLRPRIVATE pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
        RwgtClearSetup(&pPrivate->Setup);

        if (pPrivate->pBitmap)
            pgpihDestroyXBitmap(&pPrivate->pBitmap);

        if (pPrivate->hbmBgBmp)
            GpiDeleteBitmap(pPrivate->hbmBgBmp);

        if (pPrivate->ulTimerID)
            ptmrStopXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                           hwnd,
                           pPrivate->ulTimerID);

        if (pPrivate->pszInstore)
            DosFreeMem(pPrivate->pszInstore);

        free(pPrivate);
            // pWidget is cleaned up by DestroyWidgets
    }
}

/*
 *@@ fnwpRScrlrWidget:
 *      window procedure for the rexx scroller widget class.
 *
 *      There are a few rules which widget window procs
 *      must follow. See ctrDefWidgetProc in src\shared\center.c
 *      for details.
 *
 *      Other than that, this is a regular window procedure
 *      which follows the basic rules for a PM window class.
 */

MRESULT EXPENTRY fnwpRScrlrWidget(HWND hwnd,
                                  ULONG msg,
                                  MPARAM mp1,
                                  MPARAM mp2)
{
    MRESULT mrc = 0;
    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
                    // this ptr is valid after WM_CREATE

    PRSCRLRPRIVATE pPrivate;

    switch (msg)
    {
        /*
         * WM_CREATE:
         *      as with all widgets, we receive a pointer to the
         *      XCENTERWIDGET in mp1, which was created for us.
         *
         *      The first thing the widget MUST do on WM_CREATE
         *      is to store the XCENTERWIDGET pointer (from mp1)
         *      in the QWL_USER window word by calling:
         *
         *          WinSetWindowPtr(hwnd, QWL_USER, mp1);
         *
         *      We use XCENTERWIDGET.pUser for allocating
         *      RSCRLRPRIVATE for our own stuff.
         */

        case WM_CREATE:
            WinSetWindowPtr(hwnd, QWL_USER, mp1);
            pWidget = (PXCENTERWIDGET)mp1;
            if ((pWidget) && (pWidget->pfnwpDefWidgetProc))
                mrc = RwgtCreate(hwnd, pWidget);
            else
                // stop window creation!!
                mrc = (MPARAM)TRUE;

            pPrivate = (PRSCRLRPRIVATE)pWidget->pUser;
        break;

        /*
         * WM_CONTROL:
         *      process notifications/queries from the XCenter.
         */

        case WM_CONTROL:
            mrc = (MPARAM)RwgtControl(hwnd, mp1, mp2);
        break;

        /*
         * WM_PAINT:
         *
         */

        case WM_PAINT:
            RwgtPaint(hwnd, pWidget);
        break;

        /*
         * WM_WINDOWPOSCHANGED:
         *      on window resize, remember new width.
         */

        case WM_WINDOWPOSCHANGED:
            RwgtWindowPosChanged(hwnd, mp1, mp2);
        break;

        /*
         * WM_PRESPARAMCHANGED:
         *
         */

        case WM_PRESPARAMCHANGED:
            if (pWidget)
            {
                // this gets sent before this is set!
                RwgtPresParamChanged(hwnd, (ULONG)mp1, pWidget);
                if (  ((ULONG)mp1 == 0)
                    ||((ULONG)mp1 == PP_BACKGROUNDCOLOR)
                    ||((ULONG)mp1 == PP_FOREGROUNDCOLOR)
                    ||((ULONG)mp1 == PP_FONTNAMESIZE))
                {
                    pPrivate->fUpdateGraph = TRUE;
                }
            }
        break;

        /*
         * WM_DESTROY:
         *      clean up. This _must_ be passed on to
         *      ctrDefWidgetProc.
         */

        case WM_DESTROY:
            RwgtDestroy(hwnd, pWidget);
            // we _MUST_ pass this on, or the default widget proc
            // cannot clean up.
            mrc = pWidget->pfnwpDefWidgetProc(hwnd, msg, mp1, mp2);
        break;

        /*
         * WM_BUTTON1DBLCLK:
         *
         */
        case WM_BUTTON1DBLCLK:
            RwgtButton1DblClk(hwnd, mp1, mp2);
            mrc = (MPARAM)TRUE;
        break;

        /*
         * WM_TIMER:
         *      clock timer --> reexecute and repaint.
         */

        case WM_TIMER:
            if (pWidget)
                // pWidget must not be null for RwgtTimer
                RwgtTimer(hwnd, pWidget);
        break;

        default:
            mrc = pWidget->pfnwpDefWidgetProc(hwnd, msg, mp1, mp2);
    } // end switch(msg)

    return (mrc);
}

/* ******************************************************************
 *
 *   Exported procedures
 *
 ********************************************************************/

/*
 *@@ RwgtInitModule:
 *      required export with ordinal 1, which must tell
 *      the XCenter how many widgets this DLL provides,
 *      and give the XCenter an array of XCENTERWIDGETCLASS
 *      structures describing the widgets.
 *
 *      With this call, you are given the module handle of
 *      XFLDR.DLL. For convenience, you may resolve imports
 *      for some useful functions which are exported thru
 *      src\shared\xwp.def. See the code below.
 *
 *      This function must also register the PM window classes
 *      which are specified in the XCENTERWIDGETCLASS array
 *      entries. For this, you are given a HAB which you
 *      should pass to WinRegisterClass. For the window
 *      class style (4th param to WinRegisterClass),
 *      you should specify
 *
 +          CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT
 *
 *      Your widget window _will_ be resized, even if you're
 *      not planning it to be.
 *
 *      This function only gets called _once_ when the widget
 *      DLL has been successfully loaded by the XCenter. If
 *      there are several instances of a widget running (in
 *      the same or in several XCenters), this function does
 *      not get called again. However, since the XCenter unloads
 *      the widget DLLs again if they are no longer referenced
 *      by any XCenter, this might get called again when the
 *      DLL is re-loaded.
 *
 *      There will ever be only one load occurence of the DLL.
 *      The XCenter manages sharing the DLL between several
 *      XCenters. As a result, it doesn't matter if the DLL
 *      has INITINSTANCE etc. set or not.
 *
 *      If this returns 0, this is considered an error, and the
 *      DLL will be unloaded again immediately.
 *
 *      If this returns any value > 0, *ppaClasses must be
 *      set to a static array (best placed in the DLL's
 *      global data) of XCENTERWIDGETCLASS structures,
 *      which must have as many entries as the return value.
 */

ULONG EXPENTRY RwgtInitModule(HAB hab,         // XCenter's anchor block
                              HMODULE hmodSelf,     // plugin module handle
                              HMODULE hmodXFLDR,    // XFLDR.DLL module handle
                              PXCENTERWIDGETCLASS *ppaClasses,
                              PSZ pszErrorMsg)  // if 0 is returned, 500 bytes of error msg
{
    ULONG   ulrc = 0,
            ul = 0;
    BOOL    fImportsFailed = FALSE;

    // save our module handle
    G_hmodThis = hmodSelf;

    DosQueryModuleName(G_hmodThis, CCHMAXPATH, G_szThis);

    // resolve imports from XFLDR.DLL (this is basically
    // a copy of the doshResolveImports code, but we can't
    // use that before resolving...)
    for (ul = 0;
         ul < sizeof(G_aImports) / sizeof(G_aImports[0]);
         ul++)
    {
        if (DosQueryProcAddr(hmodXFLDR,
                             0,               // ordinal, ignored
                             (PSZ)G_aImports[ul].pcszFunctionName,
                             G_aImports[ul].ppFuncAddress)
                    != NO_ERROR)
        {
            sprintf(pszErrorMsg,
                    "Import %s failed.",
                    G_aImports[ul].pcszFunctionName);
            fImportsFailed = TRUE;
            break;
        }
    }

    if (!fImportsFailed)
    {
        // all imports OK:
        // register our PM window class
        if (!WinRegisterClass(hab,
                              WNDCLASS_WIDGET_RSCRLR,
                              fnwpRScrlrWidget,
                              CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT,
                              sizeof(PRSCRLRPRIVATE))
                                    // extra memory to reserve for QWL_USER
                             )
            strcpy(pszErrorMsg, "WinRegisterClass failed.");
        else
        {
            APIRET rc = 0;
            CHAR szBuf[500] = "";

            // no error:
            // return widget classes
            *ppaClasses = G_WidgetClasses;

            // register our REXX exit (stem initializer)
            rc = RexxRegisterExitDll("SCRLRINIT",        // REXX exit name
                                G_szThis,                // module name
                                "RwgtInitializeScrlrStem",
                                                         // function name
                                NULL,                    // No user area
                                RXEXIT_NONDROP);         // local drop only

            _Pmpf(("RexxRegisterExitDll SCRLRINIT returns %d", rc));

            // register our REXX exit (stem dereferencer)
            rc = RexxRegisterExitDll("SCRLRSTEM",        // REXX exit name
                                G_szThis,                // module name
                                "RwgtExtractScrlrStem",  // function name
                                NULL,                    // No user area
                                RXEXIT_NONDROP);         // local drop only

            _Pmpf(("RexxRegisterExitDll SCRLRSTEM returns %d", rc));

            // initialize our global REXX exit list structure
            G_exit_list[0].sysexit_name = "SCRLRINIT";
            G_exit_list[0].sysexit_code = RXINI;
            G_exit_list[1].sysexit_name = "SCRLRSTEM";
            G_exit_list[1].sysexit_code = RXTER;
            G_exit_list[2].sysexit_code = RXENDLST;

            // load NLS strings
            LOADSTRING(ID_CRSI_NAME, pszName);
            LOADSTRING(ID_CRSI_INTERPRETER_DBLCLK, pszInterpreterErrorDblClk);
            LOADSTRING(ID_CRSI_INTERPRETER_TIMER, pszInterpreterErrorTimer);
            LOADSTRING(ID_CRSI_ALREADYRUNNING, pszAlreadyRunning);
            LOADSTRING(ID_CRSI_SETTINGSSTATUS, pszSettingsStatus);

            G_WidgetClasses[0].pcszClassTitle = pszName;

            // register help hook for message boxes
            WinSetHook(hab,
                       NULLHANDLE,  // global queue
                       HK_HELP,
                       (PFN)RwgtHelpHook,
                       G_hmodThis);
            G_habThis = hab;

            // return no. of classes in this DLL (one here):
            ulrc = sizeof(G_WidgetClasses) / sizeof(G_WidgetClasses[0]);
        }

        G_pfnExcHookError = NULL;
    }

    return (ulrc);
}

/*
 *@@ RwgtUnInitModule:
 *      optional export with ordinal 2, which can clean
 *      up global widget class data.
 *
 *      This gets called by the XCenter right before
 *      a widget DLL gets unloaded. Note that this
 *      gets called even if the "init module" export
 *      returned 0 (meaning an error) and the DLL
 *      gets unloaded right away.
 */

VOID EXPENTRY RwgtUnInitModule(VOID)
{
    RexxDeregisterExit("SCRLRINIT",      // REXX exit name
                       G_szThis);        // module name
    RexxDeregisterExit("SCRLRSTEM",      // REXX exit name
                       G_szThis);        // module name

    free(pszName);
    free(pszInterpreterErrorDblClk);
    free(pszInterpreterErrorTimer);
    free(pszAlreadyRunning);

    WinReleaseHook(G_habThis,
                   NULLHANDLE,
                   HK_HELP,
                   (PFN)RwgtHelpHook,
                   G_hmodThis);
}

/*
 *@@ RwgtQueryVersion:
 *      optional export with ordinal 3, which can requires
 *      a specific XWorkplace revision level.
 */

VOID EXPENTRY RwgtQueryVersion(PULONG pulMajor,
                               PULONG pulMinor,
                               PULONG pulRevision)
{
    if (pulMajor)
        *pulMajor = 0;
    if (pulMinor)
        *pulMinor = 9;
    if (pulRevision)
        *pulRevision = 19;
}

#include "common.c"

