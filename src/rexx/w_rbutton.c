
/*
 *@@sourcefile w_rbutton.c:
 *      XCenter "Rexx button" widget.
 *
 *      This is an example of an XCenter widget plugin.
 *      This widget resides in RBUTTON.DLL, which (as
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
 *      A Rexx button widget recognize the following setup strings:
 *
 *      -- BGNDCOL:  the background color (in rrggbb format).
 *      -- CANDROP:  0 if the script does not support drop, a positive
 *                   number specifying the kind of allowed drop
 *                   otherwise (1, 2, 3, or 4 so far).
 *      -- FONT:     the default font to use (in "8.Helv" format).
 *      -- ICONFILE: the path to the button's icon.
 *      -- SCRIPT:   the encoded script to be run.
 *      -- SEPARATE: 1 if the script is to be run in a separate
 *                   thread.
 *      -- TEXTCOL:  the foreground color (in rrggbb format).
 *      -- TITLE:    the encoded button title.
 *
 *      The makefile in src\widgets compiles widgets
 *      with the VAC subsystem library. As a result,
 *      multiple threads are not supported.
 *
 *      This is all new with V0.1.0.
 *
 *@@added V0.1.0 (2001-01-22) [lafaix]
 *@@header "shared\center.h"
 */

/*
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
#define INCL_WINRECTANGLES
#define INCL_WINPOINTERS
#define INCL_WINSYS
#define INCL_WINBUTTONS
#define INCL_WINMLE
#define INCL_WINSTDDRAG
#define INCL_WINMESSAGEMGR
#define INCL_WINSTATICS
#define INCL_WINLISTBOXES
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

// generic headers
#include "setup.h"                      // code generation and debugging options

// headers in /helpers
#ifdef DOSH_STANDARDWRAPPERS
    #undef DOSH_STANDARDWRAPPERS
#endif
#include "helpers\dosh.h"               // Control Program helper routines
#include "helpers\gpih.h"               // GPI helper routines
#include "helpers\linklist.h"           // linked list helper routines
#include "helpers\prfh.h"               // INI file helper routines;
                                        // this include is required for some
                                        // of the structures in shared\center.h
#ifdef WINH_STANDARDWRAPPERS
    #undef WINH_STANDARDWRAPPERS
#endif
#include "helpers\winh.h"               // PM helper routines
#include "helpers\xstring.h"            // extended string helpers
#include "helpers\comctl.h"             // common controls (window procs)
#include "helpers\except.h"             // exception handling
#include "helpers\threads.h"            // thread helpers

// XWorkplace implementation headers
#include "shared\center.h"              // public XCenter interfaces
#include "shared\common.h"              // the majestic XWorkplace include file

#include "rexx\w_rbutton.h"             // private rbutton definitions

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

#define WNDCLASS_WIDGET_RBUTTON "XWPCenterRexxButtonWidget"

static XCENTERWIDGETCLASS G_WidgetClasses[]
    = {
        WNDCLASS_WIDGET_RBUTTON,    // PM window class name
        0,                          // additional flag, not used here
        "RexxButton",               // internal widget class name
        NULL,                       // widget class name displayed to user
                                    // is set by RwgtInitModule (NLS)
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

PLSTAPPENDITEM plstAppendItem = NULL;
PLSTCLEAR plstClear = NULL;
PLSTCOUNTITEMS plstCountItems = NULL;
PLSTCREATE plstCreate = NULL;
PLSTFREE plstFree = NULL;
PLSTQUERYFIRSTNODE plstQueryFirstNode = NULL;
PLSTSTRDUP plstStrDup = NULL;

PWINHFREE pwinhFree = NULL;
PWINHQUERYPRESCOLOR pwinhQueryPresColor = NULL;
PWINHQUERYWINDOWFONT pwinhQueryWindowFont = NULL;
PWINHSETWINDOWFONT pwinhSetWindowFont = NULL;
PWINHCENTERWINDOW pwinhCenterWindow = NULL;

PXSTRCAT pxstrcat = NULL;
PXSTRCLEAR pxstrClear = NULL;
PXSTRINIT pxstrInit = NULL;

PTHRCREATE pthrCreate = NULL;
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

        "lstAppendItem", (PFN*)&plstAppendItem,
        "lstClear", (PFN*)&plstClear,
        "lstCountItems", (PFN*)&plstCountItems,
        "lstCreate", (PFN*)&plstCreate,
        "lstFree", (PFN*)&plstFree,
        "lstQueryFirstNode", (PFN*)&plstQueryFirstNode,
        "lstStrDup", (PFN*)&plstStrDup,

        "winhFree", (PFN*)&pwinhFree,
        "winhQueryPresColor", (PFN*)&pwinhQueryPresColor,
        "winhQueryWindowFont", (PFN*)&pwinhQueryWindowFont,
        "winhSetWindowFont", (PFN*)&pwinhSetWindowFont,
        "winhCenterWindow", (PFN*)&pwinhCenterWindow,

        "xstrcat", (PFN*)&pxstrcat,
        "xstrClear", (PFN*)&pxstrClear,
        "xstrInit", (PFN*)&pxstrInit,

        "thrCreate", (PFN*)&pthrCreate,
        "excHandlerLoud", (PFN*)&pexcHandlerLoud
    };

/* ******************************************************************
 *
 *   Private widget instance data
 *
 ********************************************************************/

/*
 *@@ RBUTTONSETUP:
 *      instance data to which setup strings correspond.
 *      This is also a member of RBUTTONPRIVATE.
 *
 *      Putting these settings into a separate structure
 *      is no requirement, but comes in handy if you
 *      want to use the same setup string routines on
 *      both the open widget window and a settings dialog.
 */

typedef struct _RBUTTONSETUP
{
    LONG        lcolBackground,         // background color
                lcolForeground;         // foreground color (for text)

    PSZ         pszFont;
            // if != NULL, non-default font (in "8.Helv" format);
            // this has been allocated using local malloc()!

    PSZ         pszIconFile;
    HPOINTER    hIcon;
            // this REXX widget uses an icon; we must put those fields
            // here and in this order if we want to use the common
            // icon static control code

    PSZ         pszScript;
    BOOL        fSeparateThread;
    USHORT      fCanDrop;

    PSZ         pszTitle;
} RBUTTONSETUP, *PRBUTTONSETUP;

/*
 *@@ RBUTTONPRIVATE:
 *      more window data for the widget.
 *
 *      An instance of this is created on WM_CREATE in
 *      fnwpRButtonWidget and stored in XCENTERWIDGET.pUser.
 */

typedef struct _RBUTTONPRIVATE
{
    PXCENTERWIDGET pWidget;
            // reverse ptr to general widget data ptr; we need
            // that all the time and don't want to pass it on
            // the stack with each function call

    RBUTTONSETUP   Setup;
            // widget settings that correspond to a setup string

    BOOL           fButtonSunk;
            // is button depressed ?

    CHAR           achX[16];
    CHAR           achY[16];
    CHAR           achModifiers[16];
    CHAR           achHWND[16];
            // placeholders for running script arguments

    PLINKLIST      pllQueue;
            // the (possibly empty) list of elements to add to
            // DRAGITEM stem

    BYTE           abUserData[USERDATASIZE];
            // user data area
    USHORT         usUserDataLength;
            // user data area length

    HPOINTER       hUserIcon;
            // currently shown icon (if not null)

    CHAR           achTooltip[250];
            // the current tooltip for the button (if null, the default
            // title is used)

    LONG           lcolBackground;
            // the current background color (if -1, the default color
            // is used)

} RBUTTONPRIVATE, *PRBUTTONPRIVATE;


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

VOID RwgtClearSetup(PRBUTTONSETUP pSetup)
{
    if (pSetup)
    {
        free(pSetup->pszFont);
        pSetup->pszFont = NULL;

        free(pSetup->pszScript);
        pSetup->pszScript = NULL;

        free(pSetup->pszTitle);
        pSetup->pszTitle = NULL;

        free(pSetup->pszIconFile);
        pSetup->pszIconFile = NULL;

        if (pSetup->hIcon)
        {
            WinFreeFileIcon(pSetup->hIcon);
            pSetup->hIcon = NULLHANDLE;
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
                   PRBUTTONSETUP pSetup)
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

    // iconfile:
    p = pctrScanSetupString(pcszSetupString,
                            "ICONFILE");
    if (p)
    {
        pSetup->pszIconFile = strdup(p);
        pctrFreeSetupValue(p);
        pSetup->hIcon = WinLoadFileIcon(pSetup->pszIconFile, FALSE);
    }
    else
        pSetup->pszIconFile = strdup("");

    // separate thread:
    p = pctrScanSetupString(pcszSetupString,
                            "SEPARATE");
    if (p)
    {
        pSetup->fSeparateThread = (strcmp(p, "1") == 0);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->fSeparateThread = 0;

    // drop support:
    p = pctrScanSetupString(pcszSetupString,
                            "CANDROP");
    if (p)
    {
        pSetup->fCanDrop = atoi(p);
        pctrFreeSetupValue(p);
    }
    else
        pSetup->fCanDrop = 0;
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
    CHAR    szTemp[CCHMAXSCRIPT*3+8];
            // 3 times the length of an unencoded script plus length of "SCRIPT="
    PSZ     psz = 0;
    PRBUTTONSETUP pSetup = (PRBUTTONSETUP)pvSetup;

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

    if (pSetup->pszIconFile)
    {
        if (strcmp(pSetup->pszIconFile, "") != 0)
        {
            sprintf(szTemp, "ICONFILE=%s;",
                    pSetup->pszIconFile);
            pxstrcat(pstrSetup, szTemp, 0);
        }
    }

    if (pSetup->fSeparateThread)
    {
        pxstrcat(pstrSetup, "SEPARATE=1;", 0);
    }

    if (pSetup->fCanDrop)
    {
        sprintf(szTemp, "CANDROP=%d;",
                pSetup->fCanDrop);
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
                  PRBUTTONSETUP pSetup)
{
    HWND hwndDrop = WinWindowFromID(hwnd, ID_CRDI_SCRIPT_CANDROP);

    WinSendDlgItemMsg(hwnd, ID_CRDI_SCRIPT_ICON, SM_SETHANDLE, (MPARAM)(pSetup->hIcon), (MPARAM)0);
    WinSetDlgItemText(hwnd, ID_CRDI_SCRIPT_TITLE, pSetup->pszTitle);
    WinSetDlgItemText(hwnd, ID_CRDI_SCRIPT_TEXT, pSetup->pszScript);

    WinCheckButton(hwnd, ID_CRDI_SCRIPT_SEPARATETHREAD, pSetup->fSeparateThread);

    WinCheckButton(hwnd, ID_CRDI_SCRIPT_CANDROP, pSetup->fCanDrop);

    WinSendDlgItemMsg(hwnd, ID_CRDI_SCRIPT_TITLE, MLM_SETTEXTLIMIT, MPFROMLONG(CCHMAXTITLE), (MPARAM)0);
    WinSendDlgItemMsg(hwnd, ID_CRDI_SCRIPT_TEXT, MLM_SETTEXTLIMIT, MPFROMLONG(CCHMAXSCRIPT), (MPARAM)0);

    WinInsertLboxItem(hwndDrop, LIT_END, pszNoDrop);
    WinInsertLboxItem(hwndDrop, LIT_END, pszDropFile);
    WinInsertLboxItem(hwndDrop, LIT_END, pszDropFiles);
    WinInsertLboxItem(hwndDrop, LIT_END, pszDropObject);
    WinInsertLboxItem(hwndDrop, LIT_END, pszDropObjects);
    WinSendMsg(hwndDrop, LM_SELECTITEM,
                         MPFROMSHORT(pSetup->fCanDrop),
                         MPFROMSHORT(TRUE));
}

/*
 *@@ fnwpSettingsDlg:
 *      dialog proc for the rbutton settings dialog.
 *
 *@@changed V0.5.2 (2001-06-19) [lafaix]: added Apply/Reset support
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
            PRBUTTONSETUP pSetup = calloc(1, sizeof(RBUTTONSETUP));

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
                PRBUTTONSETUP pSetup = ((PSTORAGE)pData->pUser)->pSetup;
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
                            RwgtSaveSetup(&strSetup,
                                          pSetup);
                            pctrSetSetupString(pData->hSettings,
                                               strSetup.psz);
                            pxstrClear(&strSetup);
                            WinDismissDlg(hwnd, DID_OK);
                        break; }

                        /*
                         * DID_APPLY:
                         *      recompose settings, and apply the changes.
                         */

                        case DID_APPLY:
                        {
                            XSTRING strSetup;
                            PSTORAGE pStorage = (PSTORAGE)pData->pUser;

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
                        break; }

                        /*
                         * DID_RESET:
                         *       reset the uncommited changes
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

                        case DID_HELP:
                            pctrDisplayHelp(pData->pGlobals,
                                            RwgtQueryHelpLibrary(),
                                            ID_CRH_RBUTTON_SETTINGS);
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
                PRBUTTONSETUP pSetup = ((PSTORAGE)pData->pUser)->pSetup;
                if (pSetup)
                {
                    USHORT usItemID = SHORT1FROMMP(mp1),
                           usNotifyCode = SHORT2FROMMP(mp1);
                    switch (usItemID)
                    {
                        // script:
                        case ID_CRDI_SCRIPT_TEXT:
                            if (usNotifyCode == MLN_CHANGE)
                            {
                                CHAR szTemp[CCHMAXSCRIPT+1];

                                WinQueryDlgItemText(hwnd, ID_CRDI_SCRIPT_TEXT, sizeof(szTemp), (PSZ)szTemp);
                                free(pSetup->pszScript);
                                pSetup->pszScript = strdup(szTemp);
                            }
                            else
                            if (usNotifyCode == MLN_SETFOCUS)
                            {
                                ((PSTORAGE)pData->pUser)->ulCurrent = 0;
                                WinStartTimer(G_habThis, hwnd, 1, 100);
                            }
                            else
                            if (usNotifyCode == MLN_KILLFOCUS)
                            {
                                WinStopTimer(G_habThis, hwnd, 1);
                                WinSetDlgItemText(hwnd, ID_CRDI_SCRIPT_STATUS, "");
                                ((PSTORAGE)pData->pUser)->ulCurrent = 0;
                            }
                        break;

                        // button title (used by tooltip)
                        case ID_CRDI_SCRIPT_TITLE:
                            if (usNotifyCode == MLN_CHANGE)
                            {
                                CHAR szTemp[251];

                                WinQueryDlgItemText(hwnd, ID_CRDI_SCRIPT_TITLE, sizeof(szTemp), (PSZ)szTemp);
                                free(pSetup->pszTitle);
                                pSetup->pszTitle = strdup(szTemp);
                            }
                        break;

                        // separate thread support:
                        case ID_CRDI_SCRIPT_SEPARATETHREAD:
                            if (usNotifyCode == BN_CLICKED)
                                pSetup->fSeparateThread = WinQueryButtonCheckstate(hwnd, ID_CRDI_SCRIPT_SEPARATETHREAD);
                        break;

                        // drop type supported:
                        case ID_CRDI_SCRIPT_CANDROP:
                            if (usNotifyCode == LN_SELECT)
                                pSetup->fCanDrop = SHORT1FROMMR(WinSendMsg((HWND)mp2,
                                                              LM_QUERYSELECTION,
                                                              MPFROMSHORT(LIT_FIRST),
                                                              (MPARAM)0));
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

            mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
        break; }

        /*
         * WM_TIMER:
         *      update the specified MLE status area if the cursor
         *      position has changed
         */

        case WM_TIMER:
            if (SHORT1FROMMP(mp1) == 1)
            {
                PWIDGETSETTINGSDLGDATA pData = (PWIDGETSETTINGSDLGDATA)WinQueryWindowPtr(hwnd, QWL_USER);
                if (pData)
                {
                    PSTORAGE pStorage = (PSTORAGE)pData->pUser;
                    if (pStorage)
                    {
                        ULONG ulTotal = (ULONG)WinSendDlgItemMsg(hwnd,
                                                          ID_CRDI_SCRIPT_TEXT,
                                                          MLM_QUERYLINECOUNT,
                                                          0,
                                                          0);
                        ULONG ulCurrent = 1 + (ULONG)WinSendDlgItemMsg(hwnd,
                                                            ID_CRDI_SCRIPT_TEXT,
                                                            MLM_LINEFROMCHAR,
                                                            MPFROMLONG(-1L),
                                                            0);
                        ULONG ulColumn = 1 + (ULONG)WinSendDlgItemMsg(hwnd,
                                                        ID_CRDI_SCRIPT_TEXT,
                                                        MLM_QUERYSEL,
                                                        MPFROMSHORT(MLFQS_CURSORSEL),
                                                        0)
                                       - (ULONG)WinSendDlgItemMsg(hwnd,
                                                        ID_CRDI_SCRIPT_TEXT,
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
                            WinSetDlgItemText(hwnd, ID_CRDI_SCRIPT_STATUS, achBuffer);
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
 *@@added V0.3.0 (2001-02-18) [lafaix]
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
        case ID_CRH_RBUTTON_SCRIPTERROR:
        case ID_CRH_RBUTTON_ALREADYRUNNING:
        case ID_CRH_RBUTTON_THREADCREATION:
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
 *      this displays the rbutton widget's settings
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
                           ID_CRD_RBUTTONWGT_SETTINGS,
                           // pass original setup string with WM_INITDLG
                           (PVOID)pData);
    if (hwnd)
    {
        // subclass the static control to handle icon drop
        HWND hwndIcon = WinWindowFromID(hwnd, ID_CRDI_SCRIPT_ICONFILE);
        G_pfnwpOldIconFile = WinSubclassWindow(hwndIcon, fnwpIconFile);

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
 *@@ fntRunScript:
 *      transient thread started by RwgtButton1Click
 *      to run the script possibly in its own thread.
 *
 *      As this module is linked with the VAC subsystem
 *      libraries and this function may run in its own
 *      thread, we cannot use memory allocation functions
 *      (calloc, malloc, realloc or free).
 *
 *      This thread is created with a msg queue.
 *
 *@@added V0.1.0 (2001-01-25) [lafaix]
 *@@changed V0.3.0 (2001-03-06) [lafaix]: G_hwnd was always garbage
 *@@changed V0.7.0 (2001-07-16) [lafaix]: dropped element list now cleared in RwgtInitializeStem
 */

void _Optlink fntRunScript(PTHREADINFO pti)
{
    PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)(pti->ulData);
    RXSTRING params[4];
    RXSTRING instore[2];
    RXSTRING retstr;
    SHORT    src;
    LONG     rc;
    CHAR     achBuffer[250];

    #define excHandlerLoud pexcHandlerLoud
            // TRY_LOUD expects excHandlerLoud, not pexcHandlerLoud

    MAKERXSTRING(instore[0], pPrivate->Setup.pszScript, strlen(pPrivate->Setup.pszScript));
    MAKERXSTRING(instore[1], NULL, 0);

    MAKERXSTRING(params[0], pPrivate->achX, strlen(pPrivate->achX));
    MAKERXSTRING(params[1], pPrivate->achY, strlen(pPrivate->achY));
    MAKERXSTRING(params[2], pPrivate->achModifiers, strlen(pPrivate->achModifiers));
    MAKERXSTRING(params[3], pPrivate->achHWND, strlen(pPrivate->achHWND));
    MAKERXSTRING(retstr, achBuffer, sizeof(achBuffer));
    achBuffer[0] = 0;

    TRY_LOUD(excpt2)
    {
        rc = RexxStart(4,
                       &params[0],
                       "RexxButton",
                       &instore[0],
                       "CMD",
                       RXCOMMAND,
                       G_exit_list,
                       &src,
                       &retstr);

        if (rc)
            _Pmpf(("RexxStart returns %ld %d {%s}", rc, src, achBuffer));
    }
    CATCH(excpt2) {}  END_CATCH();

    if (rc < 0)
    {
        // a REXX interpreter error occured; tell so
        CHAR szBuf[500];
        HWND hwnd;

        sprintf(szBuf, pszScriptError, -rc);

        sscanf(pPrivate->achHWND, "%lX", &hwnd);
        G_hwnd = hwnd;

        WinMessageBox(HWND_DESKTOP,
                      NULLHANDLE,
                      szBuf,
                      pPrivate->Setup.pszTitle,
                      ID_CRH_RBUTTON_SCRIPTERROR,
                      MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);

        // more than one thread may be running; G_hwnd may have changed
        if (G_hwnd == hwnd)
            G_hwnd = NULLHANDLE;
    }

    // not running anymore
    pPrivate->achHWND[0] = 0;

    // release the tokenized code
    if (RXSTRPTR(instore[1]))
        DosFreeMem(RXSTRPTR(instore[1]));

    // release the returned value if needed
    if (RXSTRPTR(retstr) != achBuffer)
        DosFreeMem(RXSTRPTR(retstr));
}

/*
 *@@ RwgtInitializeStem
 *      initialize the DRAGITEM. stem with the dropped elements
 *      references and the BUTTON. stem with the user data.
 *
 *      This function is reentrant, and should not allocate heap
 *      memory.  It gets its parameter from the (about to run)
 *      script, using RexxVariablePool.
 *
 *@@added V0.3.0 (2001-02-12) [lafaix]
 *@@changed V0.5.1 (2001-06-06) [lafaix]: added BUTTON.USER
 *@@changed V0.5.2 (2001-06-13) [lafaix]: fixed incorrect DRAGITEM filling
 *@@changed V0.7.0 (2001-07-16) [lafaix]: clearing dropped elements here now
 */

LONG EXPENTRY RwgtInitializeStem(LONG exitno,
                                 LONG subfunc,
                                 PUCHAR parmblock)
{
    SHVBLOCK block;
    CHAR szHwnd[16];
            // placeholder for our hwnd query
    CHAR szStem[16];
            // placeholder for stem name (DRAGITEM.x)
    HWND hwnd = 0;
    APIRET rc = 0;

    // we need to query the hwnd handle from the script (arg(4))
    block.shvnext = 0;
    block.shvcode = RXSHV_PRIV;
    block.shvret  = 0;

    MAKERXSTRING(block.shvname, "PARM.4", 6);
    MAKERXSTRING(block.shvvalue, szHwnd, 16);

    rc = RexxVariablePool(&block);
    _Pmpf(("RexxVariablePool returns %d", rc));

    // sscanf needs a null-terminated string, and we have enough space
    szHwnd[block.shvvaluelen] = 0;

    sscanf(szHwnd, "%lX", &hwnd);

    if (hwnd)
    {
        PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

        if (pWidget)
        {
            PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)pWidget->pUser;

            if (pPrivate)
            {
                block.shvcode = RXSHV_SET;

                // user data
                MAKERXSTRING(block.shvname, "BUTTON.USER", 11);
                MAKERXSTRING(block.shvvalue, pPrivate->abUserData, pPrivate->usUserDataLength);

                RexxVariablePool(&block);

                if (pPrivate->Setup.fCanDrop)
                {
                    PLISTNODE pNode = plstQueryFirstNode(pPrivate->pllQueue);
                    USHORT usCount = 0;

                    // dropped elements
                    while (pNode)
                    {
                        PSZ pszElement = (PSZ)pNode->pItemData;

                        _Pmpf(("Adding element %s", pszElement));

                        sprintf(szStem, "DRAGITEM.%d", ++usCount);
                        MAKERXSTRING(block.shvname, szStem, strlen(szStem));
                        MAKERXSTRING(block.shvvalue, pszElement, strlen(pszElement));

                        RexxVariablePool(&block);

                        pNode = pNode->pNext;
                    }

                    // sets DRAGITEM.0
                    MAKERXSTRING(block.shvname, "DRAGITEM.0", 10);
                    sprintf(szStem, "%d", usCount);
                    MAKERXSTRING(block.shvvalue, szStem, strlen(szStem));

                    RexxVariablePool(&block);

                    // clear the queue (elements are automatically freed)
                    plstClear(pPrivate->pllQueue);
                }
            }
        }
    }

    return 0;
}

/*
 *@@ RwgtExtractStem
 *      extract the BUTTON.USER element.
 *
 *      This function is reentrant, and should not allocate heap
 *      memory.  It gets its parameter from the (about to run)
 *      script, using RexxVariablePool.
 *
 *@@added V0.5.1 (2001-06-06) [lafaix]
 */

LONG EXPENTRY RwgtExtractStem(LONG exitno,
                              LONG subfunc,
                              PUCHAR parmblock)
{
    SHVBLOCK block;
    CHAR szHwnd[16];
            // placeholder for our hwnd query
    CHAR szData[250];
            // placeholder for our BUTTON. queries
    HWND hwnd = 0;
    APIRET rc = 0;

    // we need to query the hwnd handle from the script (arg(4))
    block.shvnext = 0;
    block.shvcode = RXSHV_PRIV;
    block.shvret  = 0;

    MAKERXSTRING(block.shvname, "PARM.4", 6);
    MAKERXSTRING(block.shvvalue, szHwnd, 16);

    rc = RexxVariablePool(&block);
    _Pmpf(("RexxVariablePool returns %d", rc));

    // sscanf needs a null-terminated string, and we have enough space
    szHwnd[block.shvvaluelen] = 0;

    sscanf(szHwnd, "%lX", &hwnd);

    if (hwnd)
    {
        PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

        if (pWidget)
        {
            PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)pWidget->pUser;

            if (pPrivate)
            {
                block.shvcode = RXSHV_FETCH;

                // user data
                MAKERXSTRING(block.shvname, "BUTTON.USER", 11);
                MAKERXSTRING(block.shvvalue, szData, USERDATASIZE);
                RexxVariablePool(&block);
                if (block.shvret == RXSHV_OK)
                {
                    memcpy(pPrivate->abUserData, szData, block.shvvaluelen);
                    pPrivate->usUserDataLength = block.shvvaluelen;
                }

                // user icon
                MAKERXSTRING(block.shvname, "BUTTON.ICON", 11);
                MAKERXSTRING(block.shvvalue, szData, 250-1);
                RexxVariablePool(&block);
                if (block.shvret == RXSHV_OK)
                {
                    // an icon has been specified (it may be invalid, but
                    // that's not our problem); so free the previous one
                    if (pPrivate->hUserIcon)
                        WinFreeFileIcon(pPrivate->hUserIcon);

                    szData[block.shvvaluelen] = 0;
                    pPrivate->hUserIcon = WinLoadFileIcon(szData, FALSE);
                }
                else
                {
                    // use the default icon; we must free the previous
                    // one if appropriate
                    if (pPrivate->hUserIcon)
                        WinFreeFileIcon(pPrivate->hUserIcon);

                    pPrivate->hUserIcon = NULLHANDLE;
                }

                // tooltip
                MAKERXSTRING(block.shvname, "BUTTON.TOOLTIP", 14);
                MAKERXSTRING(block.shvvalue, pPrivate->achTooltip, 250-1);
                RexxVariablePool(&block);
                if (block.shvret == RXSHV_OK)
                    pPrivate->achTooltip[block.shvvaluelen] = 0;
                else
                    pPrivate->achTooltip[0] = 0;

                // color
                MAKERXSTRING(block.shvname, "BUTTON.BACKGROUND", 17);
                MAKERXSTRING(block.shvvalue, szData, 250-1);
                RexxVariablePool(&block);
                if (block.shvret == RXSHV_OK)
                {
                    szData[block.shvvaluelen] = 0;
                    pPrivate->lcolBackground = pctrParseColorString(szData);
                }
                else
                    pPrivate->lcolBackground = -1L;

                // things may have changed, update display
                WinInvalidateRect(pWidget->hwndWidget, NULL, FALSE);
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
    PRBUTTONPRIVATE pPrivate = calloc(1, sizeof(RBUTTONPRIVATE));

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

    // if you want the context menu help to be enabled,
    // add your help library here; if these fields are
    // left NULL, the "Help" context menu item is disabled

    pWidget->pcszHelpLibrary = RwgtQueryHelpLibrary();
    pWidget->ulHelpPanelID = ID_CRH_RBUTTON_MAIN;

    // create the (so far empty) list of queued elements
    pPrivate->pllQueue = plstCreate(TRUE);

    // -1L means use the default background color (0 being a valid color,
    // namely, black)
    pPrivate->lcolBackground = -1L;

    return (mrc);
}

/*
 *@@ RwgtButton1Click:
 *      Prepare parameters and run the script.
 *
 *@@added V0.1.0 (2001-01-22) [lafaix]
 *@@changed V0.5.2 (2001-06-13) [lafaix]: fixed thrCreate prototype change
 */

void RwgtButton1Click(HWND hwnd,
                      MPARAM mp1,
                      MPARAM mp2)
{
    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

    if (pWidget)
    {
        PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)pWidget->pUser;

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
                              ID_CRH_RBUTTON_ALREADYRUNNING,
                              MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);

                // more than one thread may be running; G_hwnd may have changed
                if (G_hwnd == hwnd)
                    G_hwnd = NULLHANDLE;
            }
            else
            {
                RECTL rclWin;

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

                if (pPrivate->Setup.fSeparateThread)
                {
                    // Run the script in its own thread (with a message queue)
                    if (pthrCreate(NULL,
                                   fntRunScript,
                                   NULL, // running flag
                                   "RexxButtonThread",
                                   THRF_PMMSGQUEUE | THRF_TRANSIENT,
                                   (ULONG)pPrivate) == 0)
                    {
                        // creation failed, release lock
                        pPrivate->achHWND[0] = 0;

                        G_hwnd = hwnd;

                        WinMessageBox(HWND_DESKTOP,
                                      hwnd,
                                      pszThreadCreationFailed,
                                      pPrivate->Setup.pszTitle,
                                      ID_CRH_RBUTTON_THREADCREATION,
                                      MB_OK|MB_HELP|MB_INFORMATION|MB_MOVEABLE);

                        // more than one thread may be running;
                        // G_hwnd may have changed
                        if (G_hwnd == hwnd)
                            G_hwnd = NULLHANDLE;
                    }
                }
                else
                {
                    // Run the script immediately
                    THREADINFO ti;

                    ti.ulData = (ULONG)pPrivate;

                    fntRunScript(&ti);
                }
            } // end if (pPrivate->achHWND[0])
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
 *@@added V0.1.0 (2001-01-22) [lafaix]
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
        PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)pWidget->pUser;
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
                        pszl->cx = pWidget->pGlobals->cxMiniIcon
                                   + 2;    // 2*1 spacing between icon and border
                        if ((pWidget->pGlobals->flDisplayStyle & XCS_FLATBUTTONS)
                                        == 0)
                            pszl->cx += 4;     // 2*2 for button borders

                        // we wanna be square
                        pszl->cy = pszl->cx;

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
                        RwgtClearSetup(&pPrivate->Setup);
                        RwgtScanSetup(pcszNewSetupString, &pPrivate->Setup);

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

                            if (pPrivate->achTooltip[0])
                                pttt->pszText = pPrivate->achTooltip;
                            else
                                pttt->pszText = pPrivate->Setup.pszTitle;

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
 *      This really does nothing, except painting a
 *      3D rectangle and printing either an icon or a question mark.
 */

VOID RwgtPaint(HWND hwnd,
               PXCENTERWIDGET pWidget)
{
    HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
    if (hps)
    {
        PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)pWidget->pUser;
        if (pPrivate)
        {
            RECTL       rclWin;
            ULONG       ulBorder = 0,
                        cx,
                        cy,
                        cxMiniIcon = pWidget->pGlobals->cxMiniIcon,
                        ulOfs = 0;
            LONG        lLeft,
                        lRight;
            HPOINTER    hptr = NULLHANDLE;

            if ((pWidget->pGlobals->flDisplayStyle & XCS_FLATBUTTONS) == 0)
                ulBorder = 2;

            WinQueryWindowRect(hwnd, &rclWin);        // exclusive
            pgpihSwitchToRGB(hps);

            if (pPrivate->fButtonSunk)
            {
                // paint button "down":
                lLeft = pWidget->pGlobals->lcol3DDark;
                lRight = pWidget->pGlobals->lcol3DLight;
                // add offset for icon painting at the bottom
                ulOfs += 1;
                if (ulBorder == 0)
                    ulBorder = 1;
            }
            else
            {
                lLeft = pWidget->pGlobals->lcol3DLight;
                lRight = pWidget->pGlobals->lcol3DDark;
            }

            if (ulBorder)
            {
                // button border:

                // now paint button frame
                rclWin.xRight--;
                rclWin.yTop--;
                pgpihDraw3DFrame(hps,
                                 &rclWin,        // inclusive
                                 ulBorder,
                                 lLeft,
                                 lRight);

                // now paint button middle
                rclWin.xLeft += ulBorder;
                rclWin.yBottom += ulBorder;
                rclWin.xRight -= ulBorder - 1;  // make exclusive again
                rclWin.yTop -= ulBorder - 1;    // make exclusive again
            }

            WinFillRect(hps,
                        &rclWin,                // exclusive
                        (pPrivate->lcolBackground == -1L) ? // V0.5.1 (2001-06-07) [lafaix]
                            pPrivate->Setup.lcolBackground :
                            pPrivate->lcolBackground);

            // If a user icon exists, use it.  Otherwise, use the
            // default icon (if any).  V0.5.1 (2001-06-06) [lafaix]
            if (pPrivate->hUserIcon != NULLHANDLE)
            {
                cx = rclWin.xRight - rclWin.xLeft;
                cy = rclWin.yTop - rclWin.yBottom;
                GpiIntersectClipRectangle(hps, &rclWin);    // exclusive!
                WinDrawPointer(hps,
                               // center this in remaining rectl
                               rclWin.xLeft + ((cx - cxMiniIcon) / 2) + ulOfs,
                               rclWin.yBottom + ((cy - cxMiniIcon) / 2) - ulOfs,
                               pPrivate->hUserIcon, // hptr,
                               DP_MINI);
            }
            else
            if (pPrivate->Setup.hIcon != NULLHANDLE)
            {
                cx = rclWin.xRight - rclWin.xLeft;
                cy = rclWin.yTop - rclWin.yBottom;
                GpiIntersectClipRectangle(hps, &rclWin);    // exclusive!
                WinDrawPointer(hps,
                               // center this in remaining rectl
                               rclWin.xLeft + ((cx - cxMiniIcon) / 2) + ulOfs,
                               rclWin.yBottom + ((cy - cxMiniIcon) / 2) - ulOfs,
                               pPrivate->Setup.hIcon, // hptr,
                               DP_MINI);
            }
            else
            {
                // print question mark if no icon
                WinDrawText(hps,
                            1,
                            "?",
                            &rclWin,                // exclusive
                            pPrivate->Setup.lcolForeground,
                            pPrivate->Setup.lcolBackground,
                            DT_CENTER | DT_VCENTER);
            }
        }
        WinEndPaint(hps);
    }
}

/*
 *@@ DrawTargetEmphasis:
 *      paint a black rectangle around the icon if fStatus is TRUE
 *      or a dialog-background--colored one otherwise.
 */

VOID DrawTargetEmphasis(HWND hwnd,
                        HPS hps,
                        BOOL fStatus)
{
    RECTL rclWin;
    LONG lColor;

    WinQueryWindowRect(hwnd, &rclWin);
    pgpihSwitchToRGB(hps);

    rclWin.xLeft += 1;
//    rclWin.xRight -= 1;
    rclWin.yBottom += 1;
//    rclWin.yTop -= 1;

    if (fStatus)
       lColor = CLR_BLACK;
    else
       lColor = pwinhQueryPresColor(hwnd,
                                    PP_BACKGROUNDCOLOR,
                                    FALSE,
                                    SYSCLR_DIALOGBACKGROUND);

    pgpihDraw3DFrame(hps, &rclWin, 1, lColor, lColor);
}

/*
 *@@ RwgtDragOverDrop:
 *      implementation for DM_DRAGOVER and DM_DROP
 *
 *@@added V0.3.0 (2001-02-09) [lafaix]
 */

MRESULT RwgtDragOverDrop(HWND hwnd,
                         MPARAM mp1,
                         MPARAM mp2,
                         BOOL fDrop)
{
    MRESULT mrc = 0;

    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    if (pWidget)
    {
        // get private data from that widget data
        PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)pWidget->pUser;
        if (pPrivate && pPrivate->Setup.fCanDrop)
        {
            PDRAGINFO dragInfo = (PDRAGINFO)mp1;
            PDRAGITEM dragItem;
            USHORT usOp, usIndicator;
            ULONG ulCount;

            DrgAccessDraginfo(dragInfo);
            switch (dragInfo->usOperation)
            {
                case DO_UNKNOWN:
                  DrgFreeDraginfo(dragInfo);
                  return (MRFROM2SHORT(DOR_NODROPOP, 0));
                case DO_DEFAULT:
                  usOp = DO_COPY;
                  break;
                case DO_LINK:
                case DO_COPY:
                case DO_MOVE:
                  usOp = dragInfo->usOperation;
                  break;
            }

            usIndicator = DOR_NEVERDROP;
            ulCount = DrgQueryDragitemCount(dragInfo);

            if (  (pPrivate->Setup.fCanDrop == CANDROP_FILES)
               || (pPrivate->Setup.fCanDrop == CANDROP_OBJECTS)
               || (ulCount == 1))
            {
                int i;
                PSZ pszMech = ((pPrivate->Setup.fCanDrop == CANDROP_ONEOBJECT) ||
                               (pPrivate->Setup.fCanDrop == CANDROP_OBJECTS)) ?
                               "DRM_OBJECT" : "DRM_OS2FILE";

                usIndicator = DOR_DROP;

                // we must check all elements to ensure they are acceptable
                for (i = 0; i < ulCount && usIndicator == DOR_DROP; i++)
                {
                    dragItem = DrgQueryDragitemPtr(dragInfo, i);

                    if (((dragItem->fsSupportedOps & DO_COPYABLE) &&
                         (usOp == (USHORT)DO_COPY))               ||
                        ((dragItem->fsSupportedOps & DO_MOVEABLE) &&
                         (usOp == (USHORT)DO_MOVE))               ||
                        ((dragItem->fsSupportedOps & DO_LINKABLE) &&
                         (usOp == (USHORT)DO_LINK)))
                    {
                        if (DrgVerifyRMF(dragItem, pszMech, NULL))
                            usIndicator = DOR_DROP;
                        else
                            usIndicator = DOR_NEVERDROP;
                    }
                    else
                        usIndicator = DOR_NODROPOP;
                }

                // OK, drop allowed
                if (usIndicator == DOR_DROP)
                {
                    HPS hps = DrgGetPS(hwnd);

                    DrawTargetEmphasis(hwnd, hps, !fDrop);

                    DrgReleasePS(hps);

                    if (fDrop)
                    {
                        CHAR szTemp[CCHMAXPATH];
                        POINTL ptl;
                        USHORT usModifiers = KC_NONE;

                        plstClear(pPrivate->pllQueue);

                        // for each dropped item
                        while (ulCount--)
                        {
                            dragItem = DrgQueryDragitemPtr(dragInfo, ulCount);
                            DrgQueryStrName(dragItem->hstrContainerName, CCHMAXPATH, szTemp);
                            DrgQueryStrName(dragItem->hstrSourceName, CCHMAXPATH, szTemp+strlen(szTemp));
                            plstAppendItem(pPrivate->pllQueue, plstStrDup(szTemp));
                        }

                        // the drop coordinates are relative to the desktop,
                        // so we must convert them
                        ptl.x = dragInfo->xDrop;
                        ptl.y = dragInfo->yDrop;
                        WinMapWindowPoints(HWND_DESKTOP, hwnd, &ptl, 1);

                        // rebuild the modifiers from dragInfo->usOperation
                        switch (dragInfo->usOperation)
                        {
                            case DO_COPY:
                                usModifiers = KC_CTRL;
                                break;
                            case DO_MOVE:
                                usModifiers = KC_SHIFT;
                                break;
                            case DO_LINK:
                                usModifiers = KC_SHIFT | KC_CTRL;
                                break;
                        }

                        WinPostMsg(hwnd,
                                   WM_BUTTON1CLICK,
                                   MPFROM2SHORT(ptl.x, ptl.y),
                                   MPFROM2SHORT(0, usModifiers));
                    } // end if (fDrop)
                } // end if (usIndicator == DOR_DROP)
            }

            DrgFreeDraginfo(dragInfo);
            mrc = MRFROM2SHORT(usIndicator, usOp);
        }
    }

    return (mrc);
}

/*
 *@@ RwgtDragLeave:
 *      implementation for DM_DRAGLEAVE
 *
 *@@added V0.3.0 (2001-02-09) [lafaix]
 */

MRESULT RwgtDragLeave(HWND hwnd,
                      MPARAM mp1,
                      MPARAM mp2)
{
    MRESULT mrc = 0;
    HPS hps = DrgGetPS(hwnd);

    DrawTargetEmphasis(hwnd, hps, FALSE);

    DrgReleasePS(hps);

    return (mrc);
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
    PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
        RwgtClearSetup(&pPrivate->Setup);

        plstFree(&(pPrivate->pllQueue));

        if (pPrivate->hUserIcon)
            WinFreeFileIcon(pPrivate->hUserIcon);

        free(pPrivate);
            // pWidget is cleaned up by DestroyWidgets
    }
}

/*
 *@@ fnwpRButtonWidget:
 *      window procedure for the rexx button widget class.
 *
 *      There are a few rules which widget window procs
 *      must follow. See ctrDefWidgetProc in src\shared\center.c
 *      for details.
 *
 *      Other than that, this is a regular window procedure
 *      which follows the basic rules for a PM window class.
 */

MRESULT EXPENTRY fnwpRButtonWidget(HWND hwnd,
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
         *      RBUTTONPRIVATE for our own stuff.
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
         * WM_BUTTON1CLICK:
         *
         */

        case WM_BUTTON1CLICK:
            RwgtButton1Click(hwnd, mp1, mp2);

            mrc = (MPARAM)TRUE;
        break;

        case WM_BUTTON1UP:
        case WM_BUTTON1DOWN:
            {
            PRBUTTONPRIVATE pPrivate = (PRBUTTONPRIVATE)pWidget->pUser;

            WinSetCapture(HWND_DESKTOP,
                          (msg == WM_BUTTON1DOWN) ? hwnd : NULLHANDLE);
            pPrivate->fButtonSunk = (msg == WM_BUTTON1DOWN);

            // repaint sunk button state
            WinInvalidateRect(hwnd, NULL, FALSE);
            }
        break;

        /*
         * DM_DRAGOVER
         *
         */

        case DM_DRAGOVER:
            mrc = RwgtDragOverDrop(hwnd, mp1, mp2, FALSE);
        break;

        /*
         * DM_DRAGLEAVE
         *
         */

        case DM_DRAGLEAVE:
            mrc = RwgtDragLeave(hwnd, mp1, mp2);
        break;

        /*
         * DM_DROP
         *
         */

        case DM_DROP:
            mrc = RwgtDragOverDrop(hwnd, mp1, mp2, TRUE);
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
                              WNDCLASS_WIDGET_RBUTTON,
                              fnwpRButtonWidget,
                              CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT,
                              sizeof(PRBUTTONPRIVATE))
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

            // load NLS strings
            LOADSTRING(ID_CRSI_NAME, pszName);
            LOADSTRING(ID_CRSI_NODROP, pszNoDrop);
            LOADSTRING(ID_CRSI_DROPFILE, pszDropFile);
            LOADSTRING(ID_CRSI_DROPFILES, pszDropFiles);
            LOADSTRING(ID_CRSI_DROPOBJECT, pszDropObject);
            LOADSTRING(ID_CRSI_DROPOBJECTS, pszDropObjects);
            LOADSTRING(ID_CRSI_ALREADYRUNNING, pszAlreadyRunning);
            LOADSTRING(ID_CRSI_THREADINGREQUIRED, pszThreadingRequired);
            LOADSTRING(ID_CRSI_SCRIPTERROR, pszScriptError);
            LOADSTRING(ID_CRSI_THREADCREATIONFAILED, pszThreadCreationFailed);
            LOADSTRING(ID_CRSI_SETTINGSSTATUS, pszSettingsStatus);

            G_WidgetClasses[0].pcszClassTitle = pszName;

            // register our REXX exit (stem initializer)
            rc = RexxRegisterExitDll("BUTTONINIT", // REXX exit name
                                G_szThis,                // module name
                                "RwgtInitializeStem",    // function name
                                NULL,                    // No user area
                                RXEXIT_NONDROP);         // local drop only

            _Pmpf(("RexxRegisterExitDll returns %d", rc));

            // register our REXX exit (stem extracter)
            rc = RexxRegisterExitDll("BUTTONTERM", // REXX exit name
                                G_szThis,                // module name
                                "RwgtExtractStem",       // function name
                                NULL,                    // No user area
                                RXEXIT_NONDROP);         // local drop only

            _Pmpf(("RexxRegisterExitDll returns %d", rc));

            // initialize our global REXX exit list structure
            G_exit_list[0].sysexit_name = "BUTTONINIT";
            G_exit_list[0].sysexit_code = RXINI;
            G_exit_list[1].sysexit_name = "BUTTONTERM";
            G_exit_list[1].sysexit_code = RXTER;
            G_exit_list[2].sysexit_code = RXENDLST;

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
    RexxDeregisterExit("BUTTONINIT",       // REXX exit name
                       G_szThis);          // module name
    RexxDeregisterExit("BUTTONTERM",       // REXX exit name
                       G_szThis);          // module name

    free(pszName);
    free(pszNoDrop);
    free(pszDropFile);
    free(pszDropFiles);
    free(pszDropObject);
    free(pszDropObjects);
    free(pszAlreadyRunning);
    free(pszThreadingRequired);
    free(pszScriptError);

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
        *pulRevision = 12;
}

#include "common.c"
