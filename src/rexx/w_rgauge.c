
/*
 *@@sourcefile w_rgauge.c:
 *      XCenter "Rexx gauge" widget.
 *
 *      This is an example of an XCenter widget plugin.
 *      This widget resides in RGAUGE.DLL, which (as
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
 *      A Rexx gauge widget recognize the following setup strings:
 *
 *      -- BGNDCOL:  the background color (in rrggbb format).
 *      -- COLOR1:   the color for the first part of the gauge (in
 *                   rrggbb format).
 *      -- COLOR2:   the color for the second part of the gauge (in
 *                   rrggbb format).
 *      -- COLOR3:   the color for the third part of the gauge (in
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
 *      -- TEXTCOL:  the color of the widget's text (in rrggbb
 *                   format).
 *      -- TITLE:    the default title of the gauge, encoded.
 *      -- WIDTH:    the width in pixels.
 *
 *      The makefile in src\widgets compiles widgets
 *      with the VAC subsystem library. As a result,
 *      multiple threads are not supported.
 *
 *      This is all new with V0.9.9.
 *
 *@@added V0.9.9 (2001-01-22) [lafaix]
 *@@header "shared\center.h"
 */

/*
 *      Copyright (C) 2001 Martin Lafaix.
 *      Copyright (C) 2000 Ulrich M�ller.
 *      This file is part of the XWorkplace source package.
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
 *  5)  at least one SOM implementation header (*.ih)
 *  6)  dlgids.h, headers in shared\ (as needed)
 *  7)  headers in implementation dirs (e.g. filesys\, as needed)
 *  8)  #pragma hdrstop and then more SOM headers which crash with precompiled headers
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

// generic headers
#include "setup.h"                      // code generation and debugging options

// headers in /helpers
#include "helpers\dosh.h"               // Control Program helper routines
#include "helpers\gpih.h"               // GPI helper routines
#include "helpers\prfh.h"               // INI file helper routines;
                                        // this include is required for some
                                        // of the structures in shared\center.h
#include "helpers\winh.h"               // PM helper routines
#include "helpers\timer.h"              // replacement PM timers
#include "helpers\xstring.h"            // extended string helpers
#include "helpers\comctl.h"             // common controls (window procs)
#include "helpers\except.h"             // exception handling

// XWorkplace implementation headers
#include "shared\center.h"              // public XCenter interfaces
#include "shared\common.h"              // the majestic XWorkplace include file

#include "rexx\w_rgauge.h"              // private rgauge definitions

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

#define WNDCLASS_WIDGET_RGAUGE "XWPCenterRexxGaugeWidget"

static XCENTERWIDGETCLASS G_WidgetClasses[]
    = {
        WNDCLASS_WIDGET_RGAUGE,     // PM window class name
        0,                          // additional flag, not used here
        "RexxGauge",                // internal widget class name
        NULL,                       // widget class name displayed to user;
                                    // is set in RwgtInitModule (NLS)
        WGTF_TOOLTIP,               // widget class flags
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

        // first optional import
        "excHandlerLoud", (PFN*)&pexcHandlerLoud
    };

/* ******************************************************************
 *
 *   Private widget instance data
 *
 ********************************************************************/

/*
 *@@ RGAUGESETUP:
 *      instance data to which setup strings correspond.
 *      This is also a member of RGAUGEPRIVATE.
 *
 *      Putting these settings into a separate structure
 *      is no requirement, but comes in handy if you
 *      want to use the same setup string routines on
 *      both the open widget window and a settings dialog.
 */

typedef struct _RGAUGESETUP
{
    LONG        lcolBackground,         // background color
                lcolForeground;         // foreground color (for text)

    PSZ         pszFont;
            // if != NULL, non-default font (in "8.Helv" format);
            // this has been allocated using local malloc()!

    PSZ         pszScript;
    PSZ         pszTitle;

    LONG        lcol1,
                lcol2,
                lcol3;

    ULONG       ulHeight,
                ulWidth;

    ULONG       ulTimerDelay;

    PSZ         pszDblClkScript;

    BOOL        fSizeable;
} RGAUGESETUP, *PRGAUGESETUP;

/*
 *@@ RGAUGEPRIVATE:
 *      more window data for the widget.
 *
 *      An instance of this is created on WM_CREATE in
 *      fnwpRGaugeWidget and stored in XCENTERWIDGET.pUser.
 */

typedef struct _RGAUGEPRIVATE
{
    PXCENTERWIDGET pWidget;
            // reverse ptr to general widget data ptr; we need
            // that all the time and don't want to pass it on
            // the stack with each function call

    RGAUGESETUP Setup;
            // widget settings that correspond to a setup string

    CHAR achX[16];
    CHAR achY[16];
    CHAR achModifiers[16];
    CHAR achHWND[16];
            // placeholders for dblclk script arguments

    CHAR achHWNDTimer[16];
            // placeholder for timer script argument

    ULONG ulVal1,
          ulVal2,
          ulVal3;
            // the returned values for the gauge

    CHAR achText[100];
            // the text to be used for the gauge

    CHAR achTooltip[250];
            // the current tooltip for the gauge (if null, the default
            // title is used)

    ULONG ulTimerID;
            // the running timer ID

    PSZ pszInstore;
    LONG lInstoreSize;
            // timer script precompiled store
} RGAUGEPRIVATE, *PRGAUGEPRIVATE;

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
 *@@ RwgtEncodeString:
 *      encode a string so that it can safely be included in a
 *      setup string.  ASCII codes between 0 and 31 as well as
 *      ';', '=', and '%' are encoded as '%xx' where xx is the
 *      hexadecimal value of the character in upper case.
 *
 *      Use RwgtFree to release the returned value.
 *
 *@@added V0.9.9 (2001-01-27) [lafaix]
 */

int iscntrl(int c) { return ((c >= 0) && (c <= 31)); }
int isxdigit(int c) { return ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')); }
int isdigit(int c) { return ((c >= '0') && (c <= '9')); }

PSZ RwgtEncodeString(PSZ pszSource) // in: pszSource must not be NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
{
    PSZ pszDest = (PSZ) malloc(sizeof(CHAR)*strlen(pszSource)*3+1);
            // in the worst case, the encoded string is 3 time longer
    PSZ pszSourceCurr = pszSource,
        pszDestCurr = pszDest;
    CHAR ch;

    if (pszDest)
    {
        while ((ch = *pszSourceCurr++))
        {
            if (   (iscntrl(ch))
                || (ch == ';')
                || (ch == '=')
                || (ch == '%'))
            {
                sprintf(pszDestCurr, "%%%02X", ch);
                pszDestCurr += 3;
            }
            else
                *pszDestCurr++ = ch;
        }

        *pszDestCurr = 0;
    }

    return (pszDest);
}

/*
 *@@ RwgtDecodeString:
 *      decodes a string encoded by RwgtEncodeString.
 *
 *      Use RwgtFree to release the returned value.
 *
 *@@added V0.9.9 (2001-01-27) [lafaix]
 */

PSZ RwgtDecodeString(PSZ pszEncodedSource) // in: pszEncodedSource must not be NULL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
{
    PSZ pszDest = (PSZ) malloc(strlen(pszEncodedSource)+1);
            // decoded string cannot be longer that source
    PSZ pszSourceCurr = pszEncodedSource,
        pszDestCurr = pszDest;
    CHAR ch, ch2, ch3;

    if (pszDest)
    {
        while ((ch = *pszSourceCurr++))
        {
            if (ch == '%')
            {
                // check if it looks like an encoded symbol
                if (*pszSourceCurr)
                {
                    if ((isxdigit((ch2 = *pszSourceCurr++))))
                    {
                        if (*pszSourceCurr)
                        {
                            if (isxdigit((ch3 = *pszSourceCurr++)))
                            {
                                // it does looks like a valid encoded symbol
                                *pszDestCurr++ = ((isdigit(ch2) ? ch2 - '0' : 10 + (ch2 - 'A')) << 4) |
                                                 (isdigit(ch3) ? ch3 - '0' : 10 + (ch3 - 'A'));
                            }
                            else
                            {
                                // the second symbol following % is not an hex digit
                                *pszDestCurr++ = ch;
                                *pszDestCurr++ = ch2;
                                *pszDestCurr++ = ch3;
                            }
                        }
                        else
                        {
                            // the second symbol following % is nul
                            *pszDestCurr++ = ch;
                            *pszDestCurr++ = ch2;
                        }
                    }
                    else
                    {
                        // the first symbol following % is not an hex digit
                        *pszDestCurr++ = ch;
                        *pszDestCurr++ = ch2;
                    }
                }
                else
                    // the first symbol following % is nul
                    *pszDestCurr++ = ch;
            }
            else
                *pszDestCurr++ = ch;
        }
        *pszDestCurr = 0;
    }

    return (pszDest);
}

/*
 *@@ RwgtFree:
 *      free a string returned by either RwgtEncodeString or
 *      RwgtDecodeString.
 *
 *@@added V0.9.9 (2001-01-27) [lafaix]
 */

VOID RwgtFree(PSZ psz)
{
    free(psz);
}

/*
 *@@ RwgtClearSetup:
 *      cleans up the data in the specified setup
 *      structure, but does not free the structure
 *      itself.
 */

VOID RwgtClearSetup(PRGAUGESETUP pSetup)
{
    if (pSetup)
    {
        if (pSetup->pszFont)
        {
            free(pSetup->pszFont);
            pSetup->pszFont = NULL;
        }

        if (pSetup->pszScript)
        {
            free(pSetup->pszScript);
            pSetup->pszScript = NULL;
        }

        if (pSetup->pszDblClkScript)
        {
            free(pSetup->pszDblClkScript);
            pSetup->pszDblClkScript = NULL;
        }

        if (pSetup->pszTitle)
        {
            free(pSetup->pszTitle);
            pSetup->pszTitle = NULL;
        }

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
                   PRGAUGESETUP pSetup)
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
        pctrFreeSetupValue(p);
        RwgtFree(pszDecoded);
    }
    else
        pSetup->pszScript = strdup("");

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
        pSetup->pszScript = strdup("");

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

        if (pSetup->ulWidth == 0) pSetup->ulWidth = 5; // KLUDGE to be removed when settings usable from notebook
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

    // height:
    p = pctrScanSetupString(pcszSetupString,
                            "HEIGHT");
    if (p)
    {
        int height = atoi(p);
        pSetup->ulHeight = height;
        if (pSetup->ulHeight == 0) pSetup->ulHeight = 5; // KLUDGE to be removed when settings usable from notebook
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
                   PRGAUGESETUP pSetup)                                                                                                                                                                                                                                          
{
    CHAR    szTemp[CCHMAXSCRIPT*3+8];
            // 3 times the length of an unencoded script plus length of "SCRIPT="
    PSZ     psz = 0;
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
                  PRGAUGESETUP pSetup)
{
    WinSetDlgItemText(hwnd, ID_CRDI_RGAUGE_TITLE, pSetup->pszTitle);
    WinSetDlgItemText(hwnd, ID_CRDI_RGAUGE_SCRIPT, pSetup->pszScript);
    WinSetDlgItemText(hwnd, ID_CRDI_RGAUGE_DBLCLK, pSetup->pszDblClkScript);

    WinSetDlgItemShort(hwnd, ID_CRDI_RGAUGE_REFRESH, pSetup->ulTimerDelay, FALSE);

    // adjusting colors
    WinSetPresParam(WinWindowFromID(hwnd, ID_CRDI_RGAUGE_COLOR1),
                    PP_BACKGROUNDCOLOR,
                    sizeof(ULONG),
                    &pSetup->lcol1);
    WinSetPresParam(WinWindowFromID(hwnd, ID_CRDI_RGAUGE_COLOR2),
                    PP_BACKGROUNDCOLOR,
                    sizeof(ULONG),
                    &pSetup->lcol2);
    WinSetPresParam(WinWindowFromID(hwnd, ID_CRDI_RGAUGE_COLOR3),
                    PP_BACKGROUNDCOLOR,
                    sizeof(ULONG),
                    &pSetup->lcol3);

    WinSendDlgItemMsg(hwnd, ID_CRDI_RGAUGE_TITLE, MLM_SETTEXTLIMIT, MPFROMLONG(CCHMAXTITLE), (MPARAM)0);
    WinSendDlgItemMsg(hwnd, ID_CRDI_RGAUGE_SCRIPT, MLM_SETTEXTLIMIT, MPFROMLONG(CCHMAXSCRIPT), (MPARAM)0);
    WinSendDlgItemMsg(hwnd, ID_CRDI_RGAUGE_DBLCLK, MLM_SETTEXTLIMIT, MPFROMLONG(CCHMAXSCRIPT), (MPARAM)0);

    WinSetDlgItemShort(hwnd, ID_CRDI_RGAUGE_WIDTH, pSetup->ulWidth, FALSE);
    WinCheckButton(hwnd, ID_CRDI_RGAUGE_RESIZEABLE, pSetup->fSizeable);
    WinCheckButton(hwnd, ID_CRDI_RGAUGE_FIXEDWIDTH, !pSetup->fSizeable);
}

/*@@ RwgtQueryHelpLibrary:
 *      returns PSZ of full help library path (in the same directory as
 *      the widget, with an extension of HLP).
 *
 *@@added V0.9.9 (2001-02-08) [lafaix]
 */

char G_szLibraryName[CCHMAXPATH] = {0};

const char *RwgtQueryHelpLibrary(VOID)
{
    if (G_szLibraryName[0] == 0)
    {
        ULONG ulLength;

        // help library name not already known
        DosQueryModuleName(G_hmodThis,
                           CCHMAXPATH,
                           G_szLibraryName);
        ulLength = strlen(G_szLibraryName);
        G_szLibraryName[ulLength-1] = 'P';
        G_szLibraryName[ulLength-3] = 'H';
    }

    return (G_szLibraryName);
}

/*
 *@@ fnwpSettingsDlg:
 *      dialog proc for the rgauge settings dialog.
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
            PRGAUGESETUP pSetup = malloc(sizeof(RGAUGESETUP));

            WinSetWindowPtr(hwnd, QWL_USER, pData);
            if (pSetup)
            {
                memset(pSetup, 0, sizeof(*pSetup));
                // store this in WIDGETSETTINGSDLGDATA
                pData->pUser = pSetup;

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
                PRGAUGESETUP pSetup = (PRGAUGESETUP)pData->pUser;
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

                            // saving colors
                            pSetup->lcol1 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RGAUGE_COLOR1),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_GREEN);
                            pSetup->lcol2 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RGAUGE_COLOR2),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_YELLOW);
                            pSetup->lcol3 = pwinhQueryPresColor(WinWindowFromID(hwnd, ID_CRDI_RGAUGE_COLOR3),
                                                                PP_BACKGROUNDCOLOR,
                                                                FALSE,
                                                                RGB_RED);

                            RwgtSaveSetup(&strSetup,
                                          pSetup);
                            pctrSetSetupString(pData->hSettings,
                                               strSetup.psz);
                            pxstrClear(&strSetup);
                            WinDismissDlg(hwnd, DID_OK);
                        break; }

                        /*
                         * DID_CANCEL:
                         *      cancel button...
                         */

                        case DID_CANCEL:
                            WinDismissDlg(hwnd, DID_CANCEL);
                        break;

                        case DID_HELP:
                            pctrDisplayHelp(pData->pGlobals,
                                            RwgtQueryHelpLibrary(),
                                            ID_CRH_RGAUGE_SETTINGS);
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
                PRGAUGESETUP pSetup = (PRGAUGESETUP)pData->pUser;
                if (pSetup)
                {
                    USHORT usItemID = SHORT1FROMMP(mp1),
                           usNotifyCode = SHORT2FROMMP(mp1);
                    switch (usItemID)
                    {
                        // script:
                        case ID_CRDI_RGAUGE_SCRIPT:
                        case ID_CRDI_RGAUGE_DBLCLK:
                            if (usNotifyCode == MLN_CHANGE)
                            {
                                CHAR szTemp[CCHMAXSCRIPT+1];

                                WinQueryDlgItemText(hwnd, usItemID, sizeof(szTemp), (PSZ)szTemp);
                                if (usItemID == ID_CRDI_RGAUGE_SCRIPT)
                                {
                                    if (pSetup->pszScript)
                                        free(pSetup->pszScript);
                                    pSetup->pszScript = strdup(szTemp);
                                }
                                else
                                {
                                    if (pSetup->pszDblClkScript)
                                        free(pSetup->pszDblClkScript);
                                    pSetup->pszDblClkScript = strdup(szTemp);
                                }
                            }
                        break;

                        // gauge title (used by tooltip)
                        case ID_CRDI_RGAUGE_TITLE:
                            if (usNotifyCode == MLN_CHANGE)
                            {
                                CHAR szTemp[251];

                                WinQueryDlgItemText(hwnd, ID_CRDI_RGAUGE_TITLE, sizeof(szTemp), (PSZ)szTemp);
                                if (pSetup->pszTitle)
                                    free(pSetup->pszTitle);
                                pSetup->pszTitle = strdup(szTemp);
                            }
                        break;

                        // refresh rate:
                        case ID_CRDI_RGAUGE_REFRESH:
                            if (usNotifyCode == EN_CHANGE)
                            {
                                SHORT sValue;

                                if (WinQueryDlgItemShort(hwnd, usItemID, &sValue, FALSE))
                                    pSetup->ulTimerDelay = (USHORT)sValue;
                            }
                        break;

                        // sizeable:
                        case ID_CRDI_RGAUGE_RESIZEABLE:
                        case ID_CRDI_RGAUGE_FIXEDWIDTH:
                            if (usNotifyCode == BN_CLICKED)
                            {
                                if (WinQueryButtonCheckstate(hwnd, ID_CRDI_RGAUGE_RESIZEABLE))
                                    pSetup->fSizeable = TRUE;
                                else
                                    pSetup->fSizeable = FALSE;
                            }
                        break;

                        // width:
                        case ID_CRDI_RGAUGE_WIDTH:
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
                PRGAUGESETUP pSetup = (PRGAUGESETUP)pData->pUser;
                if (pSetup)
                {
                    RwgtClearSetup(pSetup);
                    free(pSetup);
                } // end if (pSetup)
            } // end if (pData)

            mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
        break; }

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
 *@@added V0.9.9 (2001-02-18) [lafaix]
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
        case ID_CRH_RGAUGE_DBLCLKERROR1:
        case ID_CRH_RGAUGE_DBLCLKERROR2:
        case ID_CRH_RGAUGE_TIMERERROR:
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
 *      this displays the rgauge widget's settings
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
                           ID_CRD_RGAUGE_SETTINGS,
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
 *@@ RwgtExtractGaugeStem
 *      Extract the GAUGE. stem values.
 *
 *      This function is reentrant, and should not allocate heap
 *      memory.  It gets its parameter from the (just completed)
 *      script, using RexxVariablePool.
 *
 *@@added V0.9.9 (2001-02-19) [lafaix]
 */

LONG EXPENTRY RwgtExtractGaugeStem(LONG exitno,
                                   LONG subfunc,
                                   PUCHAR parmblock)
{
    SHVBLOCK block;
    CHAR szHwnd[16];
            // placeholder for our hwnd query
    CHAR szData[250];
            // placeholder for our GAUGE. queries
    HWND hwnd = 0;
    APIRET rc = 0;

    // we need to query the hwnd handle from the script (arg(1))
    block.shvnext = 0;
    block.shvcode = RXSHV_PRIV;
    block.shvret  = 0;

    MAKERXSTRING(block.shvname, "PARM.1", 6);
    MAKERXSTRING(block.shvvalue, szHwnd, 16);
    block.shvvaluelen = 16;

    rc = RexxVariablePool(&block);

    // sscanf needs a null-terminated string, and we have enough space
    szHwnd[block.shvvalue.strlength] = 0;

    sscanf(szHwnd, "%lX", &hwnd);

    if (hwnd)
    {
        PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

        if (pWidget)
        {
            PRGAUGEPRIVATE pPrivate = (PRGAUGEPRIVATE)pWidget->pUser;

            if (pPrivate)
            {
                block.shvcode = RXSHV_FETCH;

                // ulVal1
                MAKERXSTRING(block.shvname, "GAUGE.1", 7);
                MAKERXSTRING(block.shvvalue, szData, 250-1);
                block.shvvaluelen = 250-1;
                RexxVariablePool(&block);
                szData[RXSTRLEN(block.shvvalue)] = 0;
                if (block.shvret == RXSHV_OK)
                    pPrivate->ulVal1 = atoi(szData);
                else
                    pPrivate->ulVal1 = 0;

                // ulVal2
                MAKERXSTRING(block.shvname, "GAUGE.2", 7);
                MAKERXSTRING(block.shvvalue, szData, 250-1);
                block.shvvaluelen = 250-1;
                RexxVariablePool(&block);
                szData[RXSTRLEN(block.shvvalue)] = 0;
                if (block.shvret == RXSHV_OK)
                    pPrivate->ulVal2 = atoi(szData);
                else
                    pPrivate->ulVal2 = 0;

                // ulVal3
                MAKERXSTRING(block.shvname, "GAUGE.3", 7);
                MAKERXSTRING(block.shvvalue, szData, 250-1);
                block.shvvaluelen = 250-1;
                RexxVariablePool(&block);
                szData[RXSTRLEN(block.shvvalue)] = 0;
                if (block.shvret == RXSHV_OK)
                    pPrivate->ulVal3 = atoi(szData);
                else
                    pPrivate->ulVal3 = 0;

                // text
                MAKERXSTRING(block.shvname, "GAUGE.TEXT", 10);
                MAKERXSTRING(block.shvvalue, pPrivate->achText, 100-1);
                block.shvvaluelen = 100-1;
                RexxVariablePool(&block);
                if (block.shvret == RXSHV_OK)
                    pPrivate->achText[RXSTRLEN(block.shvvalue)] = 0;
                else
                    pPrivate->achText[0] = 0;

                // tooltip
                MAKERXSTRING(block.shvname, "GAUGE.TOOLTIP", 13);
                MAKERXSTRING(block.shvvalue, pPrivate->achTooltip, 250-1);
                block.shvvaluelen = 250-1;
                RexxVariablePool(&block);
                if (block.shvret == RXSHV_OK)
                    pPrivate->achTooltip[RXSTRLEN(block.shvvalue)] = 0;
                else
                    pPrivate->achTooltip[0] = 0;
            }
        }
    }

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
 */

MRESULT RwgtCreate(HWND hwnd,
                   PXCENTERWIDGET pWidget)
{
    MRESULT mrc = 0;
    PSZ p;
    PRGAUGEPRIVATE pPrivate = malloc(sizeof(RGAUGEPRIVATE));
    memset(pPrivate, 0, sizeof(RGAUGEPRIVATE));
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
    pPrivate->pszInstore = NULL;
    pPrivate->lInstoreSize = 0;

    // sets resizeable property
    pWidget->fSizeable = pPrivate->Setup.fSizeable;

    // if you want the context menu help to be enabled,
    // add your help library here; if these fields are
    // left NULL, the "Help" context menu item is disabled

    pWidget->pcszHelpLibrary = RwgtQueryHelpLibrary();
    pWidget->ulHelpPanelID = ID_CRH_RGAUGE_MAIN;

    // start update timer
    pPrivate->ulTimerID = ptmrStartXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                                          hwnd,
                                          1,
                                          pPrivate->Setup.ulTimerDelay);

    return (mrc);
}

/*
 *@@ RwgtButton1DblClk:
 *      Prepare parameters and run the script.
 *
 *@@added V0.9.9 (2001-02-08) [lafaix]
 */

void RwgtButton1DblClk(HWND hwnd,
                      MPARAM mp1,
                      MPARAM mp2)
{
    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

    if (pWidget)
    {
        PRGAUGEPRIVATE pPrivate = (PRGAUGEPRIVATE)pWidget->pUser;

        if (pPrivate)
        {
            // our structure is not multiple-instances--aware yet so
            // we better check it out beforehand
            if (pPrivate->achHWND[0])
            {
                G_hwnd = hwnd;
                WinMessageBox(HWND_DESKTOP,
                              hwnd,
                              pszAlreadyRunning,
                              pPrivate->Setup.pszTitle,
                              ID_CRH_RGAUGE_DBLCLKERROR1,
                              MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);
                G_hwnd = NULLHANDLE;
            }
            else
            if (  (pPrivate->Setup.pszDblClkScript)
               && (strlen(pPrivate->Setup.pszDblClkScript) > 4)
               && (pPrivate->Setup.pszDblClkScript[0] = '/')
               && (pPrivate->Setup.pszDblClkScript[1] = '*'))
            {
                // its a REXX script; run it immediately
                RECTL rclWin;
                RXSTRING params[4];
                RXSTRING instore[2];
                RXSTRING retstr;
                SHORT    src;
                LONG     rc;
                CHAR     achBuffer[250];

                WinQueryWindowRect(hwnd, &rclWin);

                // Calling the script with 4 args:
                // 1- X normalized position ([0..100])
                // 2- Y normalized position ([0..100])
                // 3- modifiers
                // 4- hwnd (in hex)
                sprintf(pPrivate->achX, "%d", SHORT1FROMMP(mp1) * 100 / (rclWin.xRight-rclWin.xLeft));
                sprintf(pPrivate->achY, "%d", SHORT2FROMMP(mp1) * 100 / (rclWin.yTop-rclWin.yBottom));
                sprintf(pPrivate->achModifiers, "%d", SHORT2FROMMP(mp2));
                sprintf(pPrivate->achHWND, "%lX", (LONG)hwnd);
            
                MAKERXSTRING(instore[0], pPrivate->Setup.pszDblClkScript, strlen(pPrivate->Setup.pszDblClkScript));
                MAKERXSTRING(instore[1], NULL, 0);

                MAKERXSTRING(params[0], pPrivate->achX, strlen(pPrivate->achX));
                MAKERXSTRING(params[1], pPrivate->achY, strlen(pPrivate->achY));
                MAKERXSTRING(params[2], pPrivate->achModifiers, strlen(pPrivate->achModifiers));
                MAKERXSTRING(params[3], pPrivate->achHWND, strlen(pPrivate->achHWND));
                MAKERXSTRING(retstr, achBuffer, sizeof(achBuffer));

                #define excHandlerLoud pexcHandlerLoud
                        // TRY_LOUD expects excHandlerLoud, not pexcHandlerLoud

                TRY_LOUD(excpt1)
                {
                    rc = RexxStart(4,
                                   &params[0],
                                   "RexxGaugeDblClk",
                                   &instore[0],
                                   "CMD",
                                   RXCOMMAND,
                                   NULL,
                                   &src,
                                   &retstr);

                    if (rc)
                        _Pmpf(("RexxStart returns %ld", rc));
                }
                CATCH(excpt1) {}  END_CATCH();

                pPrivate->achHWND[0] = 0; // not running anymore

                if (rc < 0)
                {
                    // a REXX interpreter error occured; tell so
                    CHAR szBuf[500];

                    sprintf(szBuf, pszInterpreterErrorDblClk, -rc);
                    G_hwnd = hwnd;

                    WinMessageBox(HWND_DESKTOP,
                                  hwnd,
                                  szBuf,
                                  pPrivate->Setup.pszTitle,
                                  ID_CRH_RGAUGE_DBLCLKERROR2,
                                  MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);

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
 *@@added V0.9.9 (2001-01-22) [lafaix]
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
        PRGAUGEPRIVATE pPrivate = (PRGAUGEPRIVATE)pWidget->pUser;
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

                        // reinitialize timer
                        pPrivate->ulTimerID = ptmrStartXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                                                              hwnd,
                                                              1,
                                                              pPrivate->Setup.ulTimerDelay);

                        WinInvalidateRect(pWidget->hwndWidget, NULL, FALSE);
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
                    }
                }
            }
        } // end if (pPrivate)
    } // end if (pWidget)

    return (frc);
}

/*
 *@@ RwgtPaint:
 *      implementation for WM_PAINT.
 *
 *      This really does nothing, except painting the background,
 *      up to 3 rectangles and possibly a centered text.
 *
 *      No double-buffering is used.
 *
 *      The gauge values are not computed here.  This is done
 *      in RwgtTimer.
 *
 *@@changed V0.3.2 (2001-03-16) [lafaix]: clip gauge values at 100
 */

VOID RwgtPaint(HWND hwnd,
               PXCENTERWIDGET pWidget)
{
    HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
    if (hps)
    {
        PRGAUGEPRIVATE pPrivate = (PRGAUGEPRIVATE)pWidget->pUser;
        if (pPrivate)
        {
            RECTL       rclWin;
            LONG lDark, lLight;
            LONG lLeft, lRight;
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
                lLight = pPrivate->Setup.lcolBackground;
            }

            pgpihDraw3DFrame(hps,
                             &rclWin,        // inclusive
                             ulBorder,
                             lDark,
                             lLight);

            rclWin.xLeft++;
            rclWin.yBottom++;
                // rclWin now exclusive and shrunk

            // draw the gauge
            lLeft = rclWin.xLeft;
            lRight = rclWin.xRight;

            // part 1
            if (pPrivate->ulVal1 > 0)
            {
                rclWin.xRight = lLeft + (lRight - lLeft) * min(pPrivate->ulVal1, 100) / 100;
                WinFillRect(hps,
                            &rclWin,
                            pPrivate->Setup.lcol1);
                rclWin.xLeft = rclWin.xRight;
            }

            // part 2
            if (    (pPrivate->ulVal2 > pPrivate->ulVal1)
                 && (pPrivate->ulVal2 > 0))
            {
                rclWin.xRight = lLeft + (lRight - lLeft) * min(pPrivate->ulVal2, 100) / 100;
                WinFillRect(hps,
                            &rclWin,
                            pPrivate->Setup.lcol2);
                rclWin.xLeft = rclWin.xRight;
            }

            // part 3
            if (    (pPrivate->ulVal3 > pPrivate->ulVal2)
                 && (pPrivate->ulVal3 > pPrivate->ulVal1)
                 && (pPrivate->ulVal3 > 0))
            {
                rclWin.xRight = lLeft + (lRight - lLeft) * min(pPrivate->ulVal3, 100) / 100;
                WinFillRect(hps,
                            &rclWin,
                            pPrivate->Setup.lcol3);
                rclWin.xLeft = rclWin.xRight;
            }
                
            // the background, if any
            rclWin.xRight = lRight;
            if (rclWin.xLeft != rclWin.xRight)
                WinFillRect(hps,
                            &rclWin,                // exclusive
                            pPrivate->Setup.lcolBackground);

            // draw the text, centered
            rclWin.xLeft = lLeft;
            ulTextLength = strlen(pPrivate->achText);
            if (ulTextLength)
                WinDrawText(hps,
                            ulTextLength,
                            pPrivate->achText,
                            &rclWin,
                            0,      // background, ignored anyway
                            pPrivate->Setup.lcolForeground,
                            DT_CENTER | DT_VCENTER);
        }
        WinEndPaint(hps);
    }
}

/*
 *@@ RwgtPresParamChanged:
 *      implementation for WM_PRESPARAMCHANGED.
 *
 *      While this isn't exactly required, it's a nice
 *      thing for a widget to react to colors and fonts
 *      dropped on it. While we're at it, we also save
 *      these colors and fonts in our setup string data.
 */

VOID RwgtPresParamChanged(HWND hwnd,
                          ULONG ulAttrChanged,
                          PXCENTERWIDGET pWidget)
{
    PRGAUGEPRIVATE pPrivate = (PRGAUGEPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
        BOOL fInvalidate = TRUE;
        switch (ulAttrChanged)
        {
            case 0:     // layout palette thing dropped
            case PP_BACKGROUNDCOLOR:    // background color (no ctrl pressed)
            case PP_FOREGROUNDCOLOR:    // foreground color (ctrl pressed)
                // update our setup data; the presparam has already
                // been changed, so we can just query it
                pPrivate->Setup.lcolBackground
                    = pwinhQueryPresColor(hwnd,
                                          PP_BACKGROUNDCOLOR,
                                          FALSE,
                                          SYSCLR_DIALOGBACKGROUND);
                pPrivate->Setup.lcolForeground
                    = pwinhQueryPresColor(hwnd,
                                          PP_FOREGROUNDCOLOR,
                                          FALSE,
                                          SYSCLR_WINDOWSTATICTEXT);
            break;

            case PP_FONTNAMESIZE:       // font dropped:
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
                    // we must use local malloc() for the font;
                    // the winh* code uses a different C runtime
                    pPrivate->Setup.pszFont = strdup(pszFont);
                    pwinhFree(pszFont);
                }
            break; }

            default:
                fInvalidate = FALSE;
        }

        if (fInvalidate)
        {
            // something has changed:
            XSTRING strSetup;

            // repaint
            WinInvalidateRect(hwnd, NULL, FALSE);

            // recompose our setup string
            RwgtSaveSetup(&strSetup,
                          &pPrivate->Setup);
            if (strSetup.ulLength)
                // we have a setup string:
                // tell the XCenter to save it with the XCenter data
                WinSendMsg(pPrivate->pWidget->pGlobals->hwndClient,
                           XCM_SAVESETUP,
                           (MPARAM)hwnd,
                           (MPARAM)strSetup.psz);
            pxstrClear(&strSetup);
        }
    } // end if (pPrivate)
}

/*
 *@@ RwgtWindowPosChanged:
 *      implementation for WM_WINDOWPOSCHANGED.
 *
 *@@added V0.9.9 (2001-02-20) [lafaix]
 */

VOID RwgtWindowPosChanged(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    if (pWidget)
    {
        PRGAUGEPRIVATE pPrivate = (PRGAUGEPRIVATE)pWidget->pUser;
        if (pPrivate)
        {
            PSWP pswpNew = (PSWP)mp1,
                 pswpOld = pswpNew + 1;
            if (pswpNew->fl & SWP_SIZE)
            {
                // window was resized:

                if (pswpNew->cx != pswpOld->cx)
                {
                    XSTRING strSetup;
                    // width changed:

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

            } // end if (pswpNew->fl & SWP_SIZE)
        } // end if (pPrivate)
    } // end if (pWidget)
}

/*
 *@@ RwgtTimer:
 *      updates the gauge values by running the script, updates the
 *      the window.
 */

VOID RwgtTimer(HWND hwnd)
{
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    if (pWidget)
    {
        PRGAUGEPRIVATE pPrivate = (PRGAUGEPRIVATE)pWidget->pUser;

        if (  (pPrivate && pPrivate->achHWNDTimer[0] == 0)
           && (pPrivate->Setup.pszScript)
           && (strlen(pPrivate->Setup.pszScript) > 4)
           && (pPrivate->Setup.pszScript[0] = '/')
           && (pPrivate->Setup.pszScript[1] = '*'))
        {
            // run script
            RXSTRING params[1];
            RXSTRING instore[2];
            RXSTRING retstr;
            SHORT    src;
            LONG     rc;
            CHAR     achBuffer[250];
            RXSYSEXIT exit_list[2];

            // calling the script with one arg (our hwnd)
            sprintf(pPrivate->achHWNDTimer, "%lX", (LONG)hwnd);
            MAKERXSTRING(params[0], pPrivate->achHWNDTimer, strlen(pPrivate->achHWNDTimer));

            MAKERXSTRING(instore[0], pPrivate->Setup.pszScript, strlen(pPrivate->Setup.pszScript));
            MAKERXSTRING(instore[1], pPrivate->pszInstore, pPrivate->lInstoreSize);

            MAKERXSTRING(retstr, achBuffer, sizeof(achBuffer));

            exit_list[0].sysexit_name = "GAUGESTEM";
            exit_list[0].sysexit_code = RXTER;
            exit_list[1].sysexit_code = RXENDLST;

            TRY_LOUD(excpt1)
            {
                rc = RexxStart(1,
                               &params[0],
                               "RexxGaugeTimer",
                               &instore[0],
                               "CMD",
                               RXCOMMAND,
                               exit_list,
                               &src,
                               &retstr);

                if (rc)
                    _Pmpf(("RexxStart returns %ld", rc));
            }
            CATCH(excpt1) {}  END_CATCH();

            pPrivate->achHWNDTimer[0] = 0; // not running anymore

            if (rc < 0)
            {
                // a REXX interpreter error occured; stop the timer
                // and tell so
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
                              ID_CRH_RGAUGE_TIMERERROR,
                              MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);

                G_hwnd = NULLHANDLE;
            }

            // we don't have to care about the previous instore
            // content in pPrivate; it's either NULL or the
            // same values
            pPrivate->pszInstore = RXSTRPTR(instore[1]);
            pPrivate->lInstoreSize = RXSTRLEN(instore[1]);

            // release the returned value if needed
            if (RXSTRPTR(retstr) != achBuffer)
                DosFreeMem(RXSTRPTR(retstr));

            // invalidate window
            WinInvalidateRect(hwnd, NULL, FALSE);
        } // end if (pPrivate)
    } // end if (pWidget)
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
    PRGAUGEPRIVATE pPrivate = (PRGAUGEPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
        RwgtClearSetup(&pPrivate->Setup);

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
 *@@ fnwpRGaugeWidget:
 *      window procedure for the rexx gauge widget class.
 *
 *      There are a few rules which widget window procs
 *      must follow. See ctrDefWidgetProc in src\shared\center.c
 *      for details.
 *
 *      Other than that, this is a regular window procedure
 *      which follows the basic rules for a PM window class.
 */

MRESULT EXPENTRY fnwpRGaugeWidget(HWND hwnd,
                                  ULONG msg,
                                  MPARAM mp1,
                                  MPARAM mp2)
{
    MRESULT mrc = 0;
    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
                    // this ptr is valid after WM_CREATE

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
         *      RGAUGEPRIVATE for our own stuff.
         */

        case WM_CREATE:
            WinSetWindowPtr(hwnd, QWL_USER, mp1);
            pWidget = (PXCENTERWIDGET)mp1;
            if ((pWidget) && (pWidget->pfnwpDefWidgetProc))
                mrc = RwgtCreate(hwnd, pWidget);
            else
                // stop window creation!!
                mrc = (MPARAM)TRUE;
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
                // this gets sent before this is set!
                RwgtPresParamChanged(hwnd, (ULONG)mp1, pWidget);
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
            RwgtTimer(hwnd);
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
                              HMODULE hmodXFLDR,    // XFLDR.DLL module handle
                              PXCENTERWIDGETCLASS *ppaClasses,
                              PSZ pszErrorMsg)  // if 0 is returned, 500 bytes of error msg                                                                                                                                                      
{
    ULONG   ulrc = 0,
            ul = 0;
    BOOL    fImportsFailed = FALSE;

    #define LOADSTRING(id, str) if (!WinLoadString(hab, \
                                                   G_hmodThis, \
                                                   id, \
                                                   sizeof(szBuf), \
                                                   szBuf)) \
                                    sprintf(szBuf, "RwgtInitModule error: string resource %d not found in module %s", id, G_szThis); \
                                str = strdup(szBuf)

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
            if (strcmp(G_aImports[ul].pcszFunctionName, "excHandlerLoud") == 0)
            {
                // first optional import not found
                G_aImports[ul].ppFuncAddress = NULL;
            }
            else
            {
                sprintf(pszErrorMsg,
                        "Import %s failed.",
                        G_aImports[ul].pcszFunctionName);
                fImportsFailed = TRUE;
            }
            break;
        }
    }

    if (!fImportsFailed)
    {
        // all imports OK:
        // register our PM window class
        if (!WinRegisterClass(hab,
                              WNDCLASS_WIDGET_RGAUGE,
                              fnwpRGaugeWidget,
                              CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT,
                              sizeof(PRGAUGEPRIVATE))
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

            // register our REXX exit (stem dereferencer)
            rc = RexxRegisterExitDll("GAUGESTEM",        // REXX exit name
                                G_szThis,                // module name
                                "RwgtExtractGaugeStem",  // function name
                                NULL,                    // No user area
                                RXEXIT_NONDROP);         // local drop only

            _Pmpf(("RexxRegisterExitDll returns %d", rc));

            // load NLS strings
            LOADSTRING(ID_CRSI_NAME, pszName);
            LOADSTRING(ID_CRSI_INTERPRETER_DBLCLK, pszInterpreterErrorDblClk);
            LOADSTRING(ID_CRSI_INTERPRETER_TIMER, pszInterpreterErrorTimer);
            LOADSTRING(ID_CRSI_ALREADYRUNNING, pszAlreadyRunning);

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
    RexxDeregisterExit("GAUGESTEM",      // REXX exit name
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

/*@@ _DLL_InitTerm:
 *      defining this is the easiest way to get our module handle.
 *      We need it to load our settings dialog.
 *
 *@@added V0.9.9 (2001-02-05) [lafaix]
 */

unsigned long _System _DLL_InitTerm(unsigned long hModule,
                                    unsigned long ulFlag)
{
    if (ulFlag == 0) // DLL being loaded
    {
        G_hmodThis = hModule;

        DosQueryModuleName(G_hmodThis,
                           CCHMAXPATH,
                           G_szThis);

        // now initialize the subsystem environment before we
        // call any runtime functions
        if (_rmem_init() == -1)
           return (0);  // error
    }
    else
    if (ulFlag == 1) // DLL being unloaded
    {
        // DLL being freed: cleanup runtime
        _rmem_term();
    }

    return (1);
}
