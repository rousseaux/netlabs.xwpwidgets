
/*
 *@@sourcefile w_spacer.c:
 *      XCenter "spacer" widget.
 *
 *      This is an example of an XCenter widget plugin.
 *      This widget resides in SPACER.DLL, which (as
 *      with all widget plugins) must be put into the
 *      plugins/xcenter directory of the XWorkplace
 *      installation directory.
 *
 *      Any XCenter widget plugin DLL must export the
 *      following procedures by ordinal:
 *
 *      -- Ordinal 1 (SwgtInitModule): this must
 *         return the widgets which this DLL provides.
 *
 *      -- Ordinal 2 (SwgtUnInitModule): this must
 *         clean up global DLL data.
 *
 *      A spacer widget recognize the following setup strings:
 *
 *      -- BGNDCOL: the background color (in rrggbb format).
 *      -- HEIGHT:  -1 if the height is greedy, the height in pixels
 *                  otherwise.
 *      -- WIDTH:   -1 if the width is greedy, the width in pixels
 *                  otherwise.
 *
 *      The makefile in src\widgets compiles widgets
 *      with the VAC subsystem library. As a result,
 *      multiple threads are not supported.
 *
 *      This is all new with V0.9.9.
 *
 *@@added V0.9.9 (2001-02-01) [lafaix]
 *@@header "shared\center.h"
 */

/*
 *      Copyright (C) 2000 Ulrich M”ller.
 *      Copyright (C) 2001 Martin Lafaix.
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

#define INCL_WINWINDOWMGR
#define INCL_WINDIALOGS
#define INCL_WINRECTANGLES
#define INCL_WINSYS
#define INCL_WINENTRYFIELDS
#define INCL_WINBUTTONS

#define INCL_GPILOGCOLORTABLE
#include <os2.h>

// C library headers
#include <stdio.h>

// generic headers
#include "setup.h"                      // code generation and debugging options

// headers in /helpers
#include "helpers\dosh.h"               // Control Program helper routines
#include "helpers\gpih.h"               // GPI helper routines
#include "helpers\prfh.h"               // INI file helper routines;
                                        // this include is required for some
                                        // of the structures in shared\center.h
#include "helpers\winh.h"               // PM helper routines
#include "helpers\xstring.h"            // extended string helpers

// XWorkplace implementation headers
#include "shared\center.h"              // public XCenter interfaces
#include "shared\common.h"              // the majestic XWorkplace include file

#include "spacer\w_spacer.h"            // private spacer definitions

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

#define WNDCLASS_WIDGET_SPACER "XWPCenterSpacerWidget"

static XCENTERWIDGETCLASS G_WidgetClasses[]
    = {
        WNDCLASS_WIDGET_SPACER,     // PM window class name
        0,                          // additional flag, not used here
        "Spacer",                   // internal widget class name
        NULL,                       // widget class name displayed to user
                                    // is set by SwgtInitModule (NLS)
        0,                          // widget class flags
        SwgtShowSettingsDlg         // settings dialog
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
 *      The actual imports are then made by SwgtInitModule.
 */

// resolved function pointers from XFLDR.DLL
PCMNQUERYDEFAULTFONT pcmnQueryDefaultFont = NULL;
PCMNSETCONTROLSFONT pcmnSetControlsFont = NULL;

PCTRDISPLAYHELP pctrDisplayHelp = NULL;
PCTRFREESETUPVALUE pctrFreeSetupValue = NULL;
PCTRPARSECOLORSTRING pctrParseColorString = NULL;
PCTRSCANSETUPSTRING pctrScanSetupString = NULL;
PCTRSETSETUPSTRING pctrSetSetupString = NULL;

PGPIHSWITCHTORGB pgpihSwitchToRGB = NULL;

PWINHQUERYPRESCOLOR pwinhQueryPresColor = NULL;
PWINHCENTERWINDOW pwinhCenterWindow = NULL;

PXSTRCAT pxstrcat = NULL;
PXSTRCLEAR pxstrClear = NULL;
PXSTRINIT pxstrInit = NULL;

RESOLVEFUNCTION G_aImports[] =
    {
        "cmnQueryDefaultFont", (PFN*)&pcmnQueryDefaultFont,
        "cmnSetControlsFont", (PFN*)&pcmnSetControlsFont,

        "ctrDisplayHelp", (PFN*)&pctrDisplayHelp,
        "ctrFreeSetupValue", (PFN*)&pctrFreeSetupValue,
        "ctrParseColorString", (PFN*)&pctrParseColorString,
        "ctrScanSetupString", (PFN*)&pctrScanSetupString,
        "ctrSetSetupString", (PFN*)&pctrSetSetupString,

        "gpihSwitchToRGB", (PFN*)&pgpihSwitchToRGB,

        "winhQueryPresColor", (PFN*)&pwinhQueryPresColor,
        "winhCenterWindow", (PFN*)&pwinhCenterWindow,

        "xstrcat", (PFN*)&pxstrcat,
        "xstrClear", (PFN*)&pxstrClear,
        "xstrInit", (PFN*)&pxstrInit,
    };

/* ******************************************************************
 *
 *   Private widget instance data
 *
 ********************************************************************/

/*
 *@@ SPACERSETUP:
 *      instance data to which setup strings correspond.
 *      This is also a member of SPACERPRIVATE.
 *
 *      Putting these settings into a separate structure
 *      is no requirement, but comes in handy if you
 *      want to use the same setup string routines on
 *      both the open widget window and a settings dialog.
 */

typedef struct _SPACERSETUP
{
    LONG        lcolBackground;         // background color

    ULONG       ulWidth,
                ulHeight,
                ulStyle;
} SPACERSETUP, *PSPACERSETUP;

/*
 *@@ SPACERPRIVATE:
 *      more window data for the widget.
 *
 *      An instance of this is created on WM_CREATE in
 *      fnwpSpacerWidget and stored in XCENTERWIDGET.pUser.
 */

typedef struct _SPACERPRIVATE
{
    PXCENTERWIDGET pWidget;
            // reverse ptr to general widget data ptr; we need
            // that all the time and don't want to pass it on
            // the stack with each function call

    SPACERSETUP Setup;
            // widget settings that correspond to a setup string
} SPACERPRIVATE, *PSPACERPRIVATE;

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
 *@@ SwgtClearSetup:
 *      cleans up the data in the specified setup
 *      structure, but does not free the structure
 *      itself.
 */

VOID SwgtClearSetup(PSPACERSETUP pSetup)
{
}

/*
 *@@ SwgtScanSetup:
 *      scans the given setup string and translates
 *      its data into the specified binary setup
 *      structure.
 *
 *      NOTE: It is assumed that pSetup is zeroed
 *      out. We do not clean up previous data here.
 */

VOID SwgtScanSetup(const char *pcszSetupString,
                   PSPACERSETUP pSetup)
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

    // width:
    pSetup->ulStyle &= ~SWS_GREEDYX;
    p = pctrScanSetupString(pcszSetupString,
                            "WIDTH");
    if (p)
    {
        int width = atoi(p);
        if (width == -1)
            pSetup->ulStyle |= SWS_GREEDYX;
        else
            pSetup->ulWidth = width;
        
        if (pSetup->ulWidth == 0) pSetup->ulWidth = 5; // KLUDGE to be removed when settings usable from notebook
    }
    else
        pSetup->ulWidth = 5;
        
    // height:
    pSetup->ulStyle &= ~SWS_GREEDYY;
    p = pctrScanSetupString(pcszSetupString,
                            "HEIGHT");
    if (p)
    {
        int height = atoi(p);
        if (height == -1)
            pSetup->ulStyle |= SWS_GREEDYY;
        else
            pSetup->ulHeight = height;
        if (pSetup->ulHeight == 0) pSetup->ulHeight = 5; // KLUDGE to be removed when settings usable from notebook
    }
    else
        pSetup->ulHeight = 5;

}

/*
 *@@ SwgtSaveSetup:
 *      composes a new setup string.
 *      The caller must invoke xstrClear on the
 *      string after use.
 */

VOID SwgtSaveSetup(PXSTRING pstrSetup,       // out: setup string (is cleared first)
                   PSPACERSETUP pSetup)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
{
    CHAR    szTemp[250];
    pxstrInit(pstrSetup, 500);

    sprintf(szTemp, "BGNDCOL=%06lX;",
            pSetup->lcolBackground);
    pxstrcat(pstrSetup, szTemp, 0);

    if (pSetup->ulStyle & SWS_GREEDYX)
    {
        pxstrcat(pstrSetup, "WIDTH=-1;", 9);
    }
    else
    {
        sprintf(szTemp, "WIDTH=%d;",
                pSetup->ulWidth);
        pxstrcat(pstrSetup, szTemp, 0);
    }

    if (pSetup->ulStyle & SWS_GREEDYY)
    {
        pxstrcat(pstrSetup, "HEIGHT=-1;", 10);
    }
    else
    {
        sprintf(szTemp, "HEIGHT=%d;",
                pSetup->ulHeight);
        pxstrcat(pstrSetup, szTemp, 0);
    }
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
                  PSPACERSETUP pSetup)
{
    BOOL fGreedyX = ((pSetup->ulStyle & SWS_GREEDYX) == SWS_GREEDYX),
         fGreedyY = ((pSetup->ulStyle & SWS_GREEDYY) == SWS_GREEDYY);
         
    WinSetDlgItemShort(hwnd, ID_CRDI_SPACER_WIDTH, pSetup->ulWidth, FALSE);
    WinSetDlgItemShort(hwnd, ID_CRDI_SPACER_HEIGHT, pSetup->ulHeight, FALSE);

    WinEnableControl(hwnd, ID_CRDI_SPACER_WIDTH, !fGreedyX);
    WinEnableControl(hwnd, ID_CRDI_SPACER_HEIGHT, !fGreedyY);

    WinCheckButton(hwnd, ID_CRDI_SPACER_GREEDYX, fGreedyX);
    WinCheckButton(hwnd, ID_CRDI_SPACER_USERX, !fGreedyX);

    WinCheckButton(hwnd, ID_CRDI_SPACER_GREEDYY, fGreedyY);
    WinCheckButton(hwnd, ID_CRDI_SPACER_USERY, !fGreedyY);
}

/*@@ SwgtQueryHelpLibrary:
 *      returns PSZ of full help library path (in the same directory as
 *      the widget, with an extension of HLP).
 *
 *@@added V0.9.9 (2001-02-04) [lafaix]
 */
 
char G_szLibraryName[CCHMAXPATH] = {0};
 
const char *SwgtQueryHelpLibrary(VOID)
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
 *      dialog proc for the spacer settings dialog.
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
            PSPACERSETUP pSetup = malloc(sizeof(SPACERSETUP));
            WinSetWindowPtr(hwnd, QWL_USER, pData);
            if (pSetup)
            {
                memset(pSetup, 0, sizeof(*pSetup));
                // store this in WIDGETSETTINGSDLGDATA
                pData->pUser = pSetup;

                SwgtScanSetup(pData->pcszSetupString, pSetup);

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
                PSPACERSETUP pSetup = (PSPACERSETUP)pData->pUser;
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
                            SwgtSaveSetup(&strSetup,
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
                                            SwgtQueryHelpLibrary(),
                                            ID_XSH_WIDGET_SPACER_SETTINGS);
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
                PSPACERSETUP pSetup = (PSPACERSETUP)pData->pUser;
                if (pSetup)
                {
                    USHORT usItemID = SHORT1FROMMP(mp1),
                           usNotifyCode = SHORT2FROMMP(mp1);
                    switch (usItemID)
                    {
                        // greedy x-wise:
                        case ID_CRDI_SPACER_GREEDYX:
                        case ID_CRDI_SPACER_USERX:
                            if (usNotifyCode == BN_CLICKED)
                            {
                                SHORT sWidth;
                                
                                // saving width so that it is not zeroed
                                if (WinQueryDlgItemShort(hwnd, ID_CRDI_SPACER_WIDTH, &sWidth, FALSE))
                                    pSetup->ulWidth = (USHORT)sWidth;

                                if (WinQueryButtonCheckstate(hwnd, ID_CRDI_SPACER_GREEDYX))
                                    pSetup->ulStyle |= SWS_GREEDYX;
                                else
                                    pSetup->ulStyle &= ~SWS_GREEDYX;

                                WinEnableControl(hwnd,
                                                 ID_CRDI_SPACER_WIDTH,
                                                 !(pSetup->ulStyle & SWS_GREEDYX));
                            }
                        break;

                        // greedy y-wise:
                        case ID_CRDI_SPACER_GREEDYY:
                        case ID_CRDI_SPACER_USERY:
                            if (usNotifyCode == BN_CLICKED)
                            {
                                SHORT sHeight;

                                // saving height so that it is not zeroed
                                if (WinQueryDlgItemShort(hwnd, ID_CRDI_SPACER_HEIGHT, &sHeight, FALSE))
                                    pSetup->ulWidth = (USHORT)sHeight;

                                if (WinQueryButtonCheckstate(hwnd, ID_CRDI_SPACER_GREEDYY))
                                    pSetup->ulStyle |= SWS_GREEDYY;
                                else
                                    pSetup->ulStyle &= ~SWS_GREEDYY;

                                WinEnableControl(hwnd,
                                                 ID_CRDI_SPACER_HEIGHT,
                                                 !(pSetup->ulStyle & SWS_GREEDYY));
                            }
                        break;

                        // width or height:
                        case ID_CRDI_SPACER_WIDTH:
                        case ID_CRDI_SPACER_HEIGHT:
                            if (usNotifyCode == EN_CHANGE)
                            {
                                SHORT sValue;
                                
                                if (WinQueryDlgItemShort(hwnd, usItemID, &sValue, FALSE))
                                    if (usItemID == ID_CRDI_SPACER_WIDTH)
                                        pSetup->ulWidth = (USHORT)sValue;
                                    else
                                        pSetup->ulHeight = (USHORT)sValue;
                            }
                        break;
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
                PSPACERSETUP pSetup = (PSPACERSETUP)pData->pUser;
                if (pSetup)
                {
                    SwgtClearSetup(pSetup);
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
 *@@ SwgtShowSettingsDlg:
 *      this displays the spacer widget's settings
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

VOID EXPENTRY SwgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData)
{
    HWND hwnd = WinLoadDlg(HWND_DESKTOP,         // parent
                           pData->hwndOwner,
                           fnwpSettingsDlg,
                           G_hmodThis,           // this module
                           ID_CRD_SPACER_SETTINGS,
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
 *@@ SwgtCreate:
 *      implementation for WM_CREATE.
 */

MRESULT SwgtCreate(HWND hwnd,
                   PXCENTERWIDGET pWidget)
{
    MRESULT mrc = 0;
    PSZ p;
    PSPACERPRIVATE pPrivate = malloc(sizeof(SPACERPRIVATE));
    memset(pPrivate, 0, sizeof(SPACERPRIVATE));
    // link the two together
    pWidget->pUser = pPrivate;
    pPrivate->pWidget = pWidget;

    // initialize binary setup structure from setup string
    SwgtScanSetup(pWidget->pcszSetupString,
                  &pPrivate->Setup);

    // if you want the context menu help to be enabled,
    // add your help library here; if these fields are
    // left NULL, the "Help" context menu item is disabled

    pWidget->pcszHelpLibrary = SwgtQueryHelpLibrary();
    pWidget->ulHelpPanelID = ID_XSH_WIDGET_SPACER_MAIN;

    return (mrc);
}

/*
 *@@ SwgtControl:
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

BOOL SwgtControl(HWND hwnd,
                 MPARAM mp1,
                 MPARAM mp2)
{
    BOOL frc = FALSE;

    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    if (pWidget)
    {
        // get private data from that widget data
        PSPACERPRIVATE pPrivate = (PSPACERPRIVATE)pWidget->pUser;
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
                        if (pPrivate->Setup.ulStyle & SWS_GREEDYX)
                            pszl->cx = -1;

                        pszl->cy = pPrivate->Setup.ulHeight;
// not supported yet by XCenter
//                        if (pPrivate->Setup.ulStyle & SWS_GREEDYY)
//                            pszl->cy = -1;

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

                        // reinitialize the setup data
                        SwgtClearSetup(&pPrivate->Setup);
                        SwgtScanSetup(pcszNewSetupString, &pPrivate->Setup);

                        // asks the xcenter to relayout
                        WinPostMsg(pWidget->pGlobals->hwndClient,
                                   XCM_REFORMAT,
                                   (MPARAM)(XFMF_GETWIDGETSIZES|XFMF_REPOSITIONWIDGETS),
                                   (MPARAM)0);
                    break; }
                }
            }
        } // end if (pPrivate)
    } // end if (pWidget)

    return (frc);
}

/*
 *@@ SwgtPaint:
 *      implementation for WM_PAINT.
 *
 *      This really does nothing, except painting the background.
 */

VOID SwgtPaint(HWND hwnd,
               PXCENTERWIDGET pWidget)
{
    HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
    if (hps)
    {
        PSPACERPRIVATE pPrivate = (PSPACERPRIVATE)pWidget->pUser;
        if (pPrivate)
        {
            RECTL       rclWin;

            WinQueryWindowRect(hwnd, &rclWin);        // exclusive
            pgpihSwitchToRGB(hps);

            WinFillRect(hps,
                        &rclWin,                // exclusive
                        pPrivate->Setup.lcolBackground);
        }
        WinEndPaint(hps);
    }
}

/*
 *@@ SwgtPresParamChanged:
 *      implementation for WM_PRESPARAMCHANGED.
 *
 *      While this isn't exactly required, it's a nice
 *      thing for a widget to react to colordropped on it.
 *      While we're at it, we also save this color in
 *      our setup string data.
 */

VOID SwgtPresParamChanged(HWND hwnd,
                          ULONG ulAttrChanged,
                          PXCENTERWIDGET pWidget)
{
    PSPACERPRIVATE pPrivate = (PSPACERPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
        BOOL fInvalidate = TRUE;
        switch (ulAttrChanged)
        {
            case 0:     // layout palette thing dropped
            case PP_BACKGROUNDCOLOR:    // background color (no ctrl pressed)
                // update our setup data; the presparam has already
                // been changed, so we can just query it
                pPrivate->Setup.lcolBackground
                    = pwinhQueryPresColor(hwnd,
                                          PP_BACKGROUNDCOLOR,
                                          FALSE,
                                          SYSCLR_DIALOGBACKGROUND);
            break;

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
            SwgtSaveSetup(&strSetup,
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
 *@@ SwgtDestroy:
 *      implementation for WM_DESTROY.
 *
 *      This must clean up all allocated resources.
 */

VOID SwgtDestroy(HWND hwnd,
                 PXCENTERWIDGET pWidget)
{
    PSPACERPRIVATE pPrivate = (PSPACERPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
        SwgtClearSetup(&pPrivate->Setup);

        free(pPrivate);
            // pWidget is cleaned up by DestroyWidgets
    }
}

/*
 *@@ fnwpSpacerWidget:
 *      window procedure for the spacer widget class.
 *
 *      There are a few rules which widget window procs
 *      must follow. See ctrDefWidgetProc in src\shared\center.c
 *      for details.
 *
 *      Other than that, this is a regular window procedure
 *      which follows the basic rules for a PM window class.
 */

MRESULT EXPENTRY fnwpSpacerWidget(HWND hwnd,
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
         *      SPACERPRIVATE for our own stuff.
         */

        case WM_CREATE:
            WinSetWindowPtr(hwnd, QWL_USER, mp1);
            pWidget = (PXCENTERWIDGET)mp1;
            if ((pWidget) && (pWidget->pfnwpDefWidgetProc))
                mrc = SwgtCreate(hwnd, pWidget);
            else
                // stop window creation!!
                mrc = (MPARAM)TRUE;
        break;

        /*
         * WM_CONTROL:
         *      process notifications/queries from the XCenter.
         */

        case WM_CONTROL:
            mrc = (MPARAM)SwgtControl(hwnd, mp1, mp2);
        break;

        /*
         * WM_PAINT:
         *
         */

        case WM_PAINT:
            SwgtPaint(hwnd, pWidget);
        break;

        /*
         * WM_PRESPARAMCHANGED:
         *
         */

        case WM_PRESPARAMCHANGED:
            if (pWidget)
                // this gets sent before this is set!
                SwgtPresParamChanged(hwnd, (ULONG)mp1, pWidget);
        break;

        /*
         * WM_DESTROY:
         *      clean up. This _must_ be passed on to
         *      ctrDefWidgetProc.
         */

        case WM_DESTROY:
            SwgtDestroy(hwnd, pWidget);
            // we _MUST_ pass this on, or the default widget proc
            // cannot clean up.
            mrc = pWidget->pfnwpDefWidgetProc(hwnd, msg, mp1, mp2);
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
 *@@ SwgtInitModule:
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

ULONG EXPENTRY SwgtInitModule(HAB hab,         // XCenter's anchor block
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
                                    sprintf(szBuf, "SwgtInitModule error: string resource %d not found in module %s", id, G_szThis); \
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
                              WNDCLASS_WIDGET_SPACER,
                              fnwpSpacerWidget,
                              CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT,
                              sizeof(PSPACERPRIVATE))
                                    // extra memory to reserve for QWL_USER
                             )
            strcpy(pszErrorMsg, "WinRegisterClass failed.");
        else
        {
            CHAR szBuf[500] = "";

            // no error:
            // return widget classes
            *ppaClasses = G_WidgetClasses;

            // load NLS strings
            LOADSTRING(ID_CRSI_NAME, pszName);

            G_WidgetClasses[0].pcszClassTitle = pszName;

            // return no. of classes in this DLL (one here):
            ulrc = sizeof(G_WidgetClasses) / sizeof(G_WidgetClasses[0]);
        }
    }

    return (ulrc);
}

/*
 *@@ SwgtUnInitModule:
 *      optional export with ordinal 2, which can clean
 *      up global widget class data.
 *
 *      This gets called by the XCenter right before
 *      a widget DLL gets unloaded. Note that this
 *      gets called even if the "init module" export
 *      returned 0 (meaning an error) and the DLL
 *      gets unloaded right away.
 */

VOID EXPENTRY SwgtUnInitModule(VOID)
{
    free(pszName);
}

/*@@ _DLL_InitTerm: 
 *      defining this is the easiest way to get our module handle.
 *      We need it to load our settings dialog.
 *
 *@@added V0.9.9 (2001-02-04) [lafaix]
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

