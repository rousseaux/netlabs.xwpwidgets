
/*
 *@@sourcefile popper.c:
 *      XCenter "popper" widget.
 *
 *      This is an example of an XCenter widget plugin.
 *      This widget resides in POPPER.DLL, which (as
 *      with all widget plugins) must be put into the
 *      plugins/xcenter directory of the XWorkplace
 *      installation directory.
 *
 *      Any XCenter widget plugin DLL must export the
 *      following procedures by ordinal:
 *
 *      -- Ordinal 1 (WgtInitModule): this must
 *         return the widgets which this DLL provides.
 *
 *      -- Ordinal 2 (WgtUnInitModule): this must
 *         clean up global DLL data.
 *
 *      -- Ordinal 3 (WgtQueryVersion): this must
 *         return the widgets minimal XWorkplace version level
 *         required.
 *
 *      A POP3Checker widget recognize the following setup strings:
 *
 *      -- BGNDCOL:
 *      -- FONT:
 *      -- NAME:
 *      -- PASS:
 *      -- PORT:
 *      -- SAVEPASS:
 *      -- SERVER:
 *      -- TEXTCOL:
 *      -- TIMER:
 *      -- USER:
 *
 *      The makefile in src\popper compiles widgets
 *      with the VAC subsystem library. As a result,
 *      multiple threads are not supported.
 *
 *      This is all new with V0.7.0.
 *
 *@@header "shared\center.h"
 */

/*
 *      Copyright (C) 2000 fonz.
 *      Copyright (C) 2002 Martin Lafaix.
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

#define INCL_DOS
#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS

#define INCL_WIN
#define INCL_WINWINDOWMGR
#define INCL_WINFRAMEMGR
#define INCL_WINDIALOGS
#define INCL_WININPUT
#define INCL_WINSWITCHLIST
#define INCL_WINRECTANGLES
#define INCL_WINPOINTERS
#define INCL_WINSYS
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS

#define INCL_GPIPRIMITIVES
#define INCL_GPILOGCOLORTABLE
#include <os2.h>

// C library headers
#include <stdio.h>

// generic headers
#define DONT_REPLACE_FOR_DBCS           // do not replace strchr with DBCS version
#include "setup.h"                      // code generation and debugging options

// headers in /helpers
#include "helpers\dosh.h"               // Control Program helper routines
#include "helpers\gpih.h"               // GPI helper routines
#include "helpers\timer.h"
#include "helpers\prfh.h"               // INI file helper routines;
                                        // this include is required for some
                                        // of the structures in shared\center.h
#ifdef WINH_STANDARDWRAPPERS
    #undef WINH_STANDARDWRAPPERS
#endif
#include "helpers\winh.h"               // PM helper routines
#include "helpers\xstring.h"            // extended string helpers
#include "helpers\threads.h"            // thread helpers

// XWorkplace implementation headers
#include "dlgids.h"
#include "shared\center.h"              // public XCenter interfaces
#include "shared\common.h"              // the majestic XWorkplace include file

#include "popper\socket.h"

#pragma hdrstop                     // VAC++ keeps crashing otherwise


void EXPENTRY WgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData);
MRESULT EXPENTRY fnwpSettingsDlg(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
/* ******************************************************************
 *
 *   Private definitions
 *
 ********************************************************************/

// None currently.

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
#define WMXINT_SETUP        WM_USER+1805
#define WMXINT_NEWMAIL      WM_USER+1806
#define WMXINT_NOMAILS      WM_USER+1807

#define WNDCLASS_WIDGET_SAMPLE "XWPCenterPop3CheckerWidget"

static XCENTERWIDGETCLASS G_WidgetClasses[]
    = {
        WNDCLASS_WIDGET_SAMPLE,     // PM window class name
        0,                          // additional flag, not used here
        "Pop3CheckerWidget",           // internal widget class name
        "Pop3Checker",                 // widget class name displayed to user
        WGTF_TRAYABLE/*WGTF_SIZEABLE*/,                 // widget class flags
        WgtShowSettingsDlg
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
 *      The actual imports are then made by WgtInitModule.
 */

// resolved function pointers from XFLDR.DLL
PCMNQUERYDEFAULTFONT pcmnQueryDefaultFont = NULL;
PCMNSETCONTROLSFONT pcmnSetControlsFont = NULL;

PWINHCENTERWINDOW pwinhCenterWindow = NULL;

PTMRSTARTXTIMER ptmrStartXTimer = NULL;
PTMRSTOPXTIMER ptmrStopXTimer = NULL;

PCTRDISPLAYHELP pctrDisplayHelp = NULL;
PCTRFREESETUPVALUE pctrFreeSetupValue = NULL;
PCTRPARSECOLORSTRING pctrParseColorString = NULL;
PCTRSCANSETUPSTRING pctrScanSetupString = NULL;
PCTRSETSETUPSTRING pctrSetSetupString = NULL;

PGPIHDRAW3DFRAME pgpihDraw3DFrame = NULL;
PGPIHSWITCHTORGB pgpihSwitchToRGB = NULL;

PWINHFREE pwinhFree = NULL;
PWINHQUERYPRESCOLOR pwinhQueryPresColor = NULL;
PWINHQUERYWINDOWFONT pwinhQueryWindowFont = NULL;
PWINHSETWINDOWFONT pwinhSetWindowFont = NULL;

PXSTRCAT pxstrcat = NULL;
PXSTRCLEAR pxstrClear = NULL;
PXSTRINIT pxstrInit = NULL;

PTHRCREATE pthrCreate = NULL;

RESOLVEFUNCTION G_aImports[] =
    {
        "cmnQueryDefaultFont", (PFN*)&pcmnQueryDefaultFont,
        "cmnSetControlsFont", (PFN*)&pcmnSetControlsFont,

        "winhCenterWindow", (PFN*)&pwinhCenterWindow,

        "tmrStartXTimer", (PFN*)&ptmrStartXTimer,
        "tmrStopXTimer", (PFN*)&ptmrStopXTimer,

        "ctrDisplayHelp", (PFN*)&pctrDisplayHelp,
        "ctrFreeSetupValue", (PFN*)&pctrFreeSetupValue,
        "ctrParseColorString", (PFN*)&pctrParseColorString,
        "ctrScanSetupString", (PFN*)&pctrScanSetupString,
        "ctrSetSetupString", (PFN*)&pctrSetSetupString,

        "gpihDraw3DFrame", (PFN*)&pgpihDraw3DFrame,
        "gpihSwitchToRGB", (PFN*)&pgpihSwitchToRGB,

        "winhFree", (PFN*)&pwinhFree,
        "winhQueryPresColor", (PFN*)&pwinhQueryPresColor,
        "winhQueryWindowFont", (PFN*)&pwinhQueryWindowFont,
        "winhSetWindowFont", (PFN*)&pwinhSetWindowFont,

        "xstrcat", (PFN*)&pxstrcat,
        "xstrClear", (PFN*)&pxstrClear,
        "xstrInit", (PFN*)&pxstrInit,

        "thrCreate", (PFN*)&pthrCreate
    };

/* ******************************************************************
 *
 *   Private widget instance data
 *
 ********************************************************************/

/*
 *@@ POPPERSETUP:
 *      instance data to which setup strings correspond.
 *      This is also a member of popperPRIVATE.
 *
 *      Putting these settings into a separate structure
 *      is no requirement, but comes in handy if you
 *      want to use the same setup string routines on
 *      both the open widget window and a settings dialog.
 */
typedef struct _POPPERSETUP
{
    LONG        lcolBackground,         // background color
                lcolForeground;         // foreground color (for text)

    PSZ         pszFont;
            // if != NULL, non-default font (in "8.Helv" format);
            // this has been allocated using local malloc()!

    char        szAccountName[31];
    char        szPopServer[65];
    ULONG       ulPopPort;
    char        szPopUser[65];
    char        szPopPassword[65];
    BOOL        fSavePass;

    ULONG       ulCheckTimer;
} POPPERSETUP, *PPOPPERSETUP;



/*
 *@@ popperPRIVATE:
 *      more window data for the widget.
 *
 *      An instance of this is created on WM_CREATE in
 *      fnwpSampleWidget and stored in XCENTERWIDGET.pUser.
 */
typedef struct _popperPRIVATE
{
    PXCENTERWIDGET pWidget;
            // reverse ptr to general widget data ptr; we need
            // that all the time and don't want to pass it on
            // the stack with each function call

    POPPERSETUP Setup;
            // widget settings that correspond to a setup string


    HPOINTER hptrEmail;
    ULONG    ulNumMails;
    ULONG    ulSocketError;
    long     lCX;

    ULONG    ulTimerID;
    ULONG    ulBlinkTimerID;

    BOOL     fIconVisible;

    HWND     hwndSelf;
    HWND     hwndPopup;

    TID      tidPopperThread;
} popperPRIVATE, *PpopperPRIVATE;


// not defined in the toolkit-headers
BOOL APIENTRY WinStretchPointer(HPS hps, long lX, long ly, long lcy, long lcx, HPOINTER hptr, ULONG ulHalf);

// prototypes
void _Optlink fntPop3CheckThread(PTHREADINFO pti);

HMODULE hmod;
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
 *@@ WgtClearSetup:
 *      cleans up the data in the specified setup
 *      structure, but does not free the structure
 *      itself.
 */
void WgtClearSetup(PPOPPERSETUP pSetup)
{
 if(pSetup)
  {
   if(pSetup->pszFont)
    {
     free(pSetup->pszFont);
     pSetup->pszFont = NULL;
    }
  }
}

/*
 *@@ WgtScanSetup:
 *      scans the given setup string and translates
 *      its data into the specified binary setup
 *      structure.
 *
 *      NOTE: It is assumed that pSetup is zeroed
 *      out. We do not clean up previous data here.
 */
void WgtScanSetup(const char *pcszSetupString,
                  PPOPPERSETUP pSetup)
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
        pSetup->lcolBackground = WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0);

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



    ///////////////////////////////////////////////////

    // account-name
    p = pctrScanSetupString(pcszSetupString,
                            "NAME");
    if(p)
     {
      strcpy(pSetup->szAccountName, p);
      pctrFreeSetupValue(p);
     }
    else
      memset(&pSetup->szAccountName, 0, sizeof(pSetup->szAccountName));

    // pop-server
    p = pctrScanSetupString(pcszSetupString,
                            "SERVER");
    if(p)
     {
      strcpy(pSetup->szPopServer, p);
      pctrFreeSetupValue(p);
     }
    else
      memset(&pSetup->szPopServer, 0, sizeof(pSetup->szPopServer));

    // pop-port
    p = pctrScanSetupString(pcszSetupString,
                            "PORT");
    if(p)
     {
      pSetup->ulPopPort=(ULONG)atol(p);
      pctrFreeSetupValue(p);
     }
    else
      pSetup->ulPopPort=110;

    // user
    p = pctrScanSetupString(pcszSetupString,
                            "USER");
    if(p)
     {
      strcpy(pSetup->szPopUser, p);
      pctrFreeSetupValue(p);
     }
    else
      memset(&pSetup->szPopUser, 0, sizeof(pSetup->szPopUser));

    // password
    p = pctrScanSetupString(pcszSetupString,
                            "PASS");
    if(p)
     {
      strcpy(pSetup->szPopPassword, p);
      pctrFreeSetupValue(p);
     }
    else
      memset(&pSetup->szPopPassword, 0, sizeof(pSetup->szPopPassword));

    // save-password
    p = pctrScanSetupString(pcszSetupString,
                            "SAVEPASS");
    if(p)
     {
      if(atol(p)==0)
        pSetup->fSavePass=FALSE;
      else
        pSetup->fSavePass=TRUE;

      pctrFreeSetupValue(p);
     }
    //else
      //pSetup->fSavePass=FALSE;




    // timer
    p = pctrScanSetupString(pcszSetupString,
                            "TIMER");
    if(p)
     {
      pSetup->ulCheckTimer=(ULONG)atol(p);
      pctrFreeSetupValue(p);
     }
}

/*
 *@@ WgtSaveSetup:
 *      composes a new setup string.
 *      The caller must invoke xstrClear on the
 *      string after use.
 */
void WgtSaveSetup(PXSTRING pstrSetup,       // out: setup string (is cleared first)
                  PPOPPERSETUP pSetup)
{
    CHAR    szTemp[100];
    PSZ     psz = 0;
    pxstrInit(pstrSetup, 100);

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


    ///////////////////////////////////////////////////


    // name
    sprintf(szTemp, "NAME=%s;",
            pSetup->szAccountName);
    pxstrcat(pstrSetup, szTemp, 0);

    // server
    sprintf(szTemp, "SERVER=%s;",
            pSetup->szPopServer);
    pxstrcat(pstrSetup, szTemp, 0);

    // port
    sprintf(szTemp, "PORT=%d;",
            pSetup->ulPopPort);
    pxstrcat(pstrSetup, szTemp, 0);

    // user
    sprintf(szTemp, "USER=%s;",
            pSetup->szPopUser);
    pxstrcat(pstrSetup, szTemp, 0);


    // save-pass
    if(pSetup->fSavePass)
      pxstrcat(pstrSetup, "SAVEPASS=1;", 0);
    else
     pxstrcat(pstrSetup, "SAVEPASS=0;", 0);

    // pass
    sprintf(szTemp, "PASS=%s;",
            pSetup->szPopPassword);
    pxstrcat(pstrSetup, szTemp, 0);


    // timer
    sprintf(szTemp, "TIMER=%d;",
            pSetup->ulCheckTimer);
    pxstrcat(pstrSetup, szTemp, 0);
}

/* ******************************************************************
 *
 *   Widget settings dialog
 *
 ********************************************************************/

// None currently. To see how a setup dialog can be done,
// see the window list widget (w_winlist.c).

/* ******************************************************************
 *
 *   Callbacks stored in XCENTERWIDGETCLASS
 *
 ********************************************************************/

// If you implement a settings dialog, you must write a
// "show settings dlg" function and store its function pointer
// in XCENTERWIDGETCLASS.

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
 *@@ WgtCreate:
 *      implementation for WM_CREATE.
 */
MRESULT WgtCreate(HWND hwnd,
                  PXCENTERWIDGET pWidget)
{
    MRESULT mrc = 0;
    PSZ p;
    PpopperPRIVATE pPrivate = (PpopperPRIVATE)malloc(sizeof(popperPRIVATE));
    memset(pPrivate, 0, sizeof(popperPRIVATE));
    // link the two together
    pWidget->pUser = pPrivate;
    pPrivate->pWidget = pWidget;

    // initialize binary setup structure from setup string
    WgtScanSetup(pWidget->pcszSetupString,
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

    // pWidget->pcszHelpLibrary = pcmnQueryHelpLibrary();
    // pWidget->ulHelpPanelID = ID_XSH_WIDGET_WINLIST_MAIN;
                                                       /*
    pPrivate->hptrDrive = WinLoadPointer(HWND_DESKTOP,
                                         hmod,
                                         2500);*/


    pPrivate->lCX=10;
    pPrivate->ulSocketError=0;
    pPrivate->ulNumMails=0;
    pPrivate->hwndSelf=hwnd;
    pPrivate->hptrEmail = WinLoadPointer(HWND_DESKTOP,
                                         hmod,
                                         2500);

    // start update timer
    pPrivate->ulTimerID = ptmrStartXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                                         hwnd,
                                         1,
                                         pPrivate->Setup.ulCheckTimer*1000);

    pPrivate->ulBlinkTimerID=0;
    pPrivate->fIconVisible=TRUE;
    pPrivate->tidPopperThread=0;

    // preload load context menu
    pPrivate->hwndPopup=WinLoadMenu(hwnd,
                                    hmod,
                                    3000);

    return (mrc);
}

/*
 *@@ MwgtControl:
 *      implementation for WM_CONTROL.
 *
 *      The XCenter communicates with widgets thru
 *      WM_CONTROL messages. At the very least, the
 *      widget should respond to XN_QUERYSIZE because
 *      otherwise it will be given some dumb default
 *      size.
 *
 *@@added V0.9.7 (2000-12-14) [umoeller]
 */
BOOL WgtControl(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    BOOL brc = FALSE;

    // get widget data from QWL_USER (stored there by WM_CREATE)
    PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
    if (pWidget)
    {
        // get private data from that widget data
        PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;
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
                        pszl->cx = pPrivate->lCX;      // desired width
                        pszl->cy = 20;      // desired minimum height
                        brc = TRUE;
                    break; }

                    /*
                     * XN_SETUPCHANGED:
                     *      XCenter has a new setup string for
                     *      us in mp2.
                     *
                     *      NOTE: This only comes in with settings
                     *      dialogs. Since we don't have one, this
                     *      really isn't necessary.
                     */
                    case XN_SETUPCHANGED:
                    {
                        const char *pcszNewSetupString = (const char*)mp2;

                        // reinitialize the setup data
                        WgtClearSetup(&pPrivate->Setup);
                        WgtScanSetup(pcszNewSetupString, &pPrivate->Setup);

                        // 'restart' timer
                        if(pPrivate->ulTimerID)
                           ptmrStopXTimer((PXTIMERSET)pPrivate->pWidget->pGlobals->pvXTimerSet,
                                          hwnd,
                                          pPrivate->ulTimerID);

                         pPrivate->ulTimerID = ptmrStartXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                                                                hwnd,
                                                                1,
                                                                pPrivate->Setup.ulCheckTimer*1000);

                        pPrivate->lCX=10;
                        WinInvalidateRect(pWidget->hwndWidget, NULL, FALSE);
                    break; }
                }
            }
        } // end if (pPrivate)
    } // end if (pWidget)

    return (brc);
}

/*
 *@@ WgtPaint:
 *      implementation for WM_PAINT.
 *
 *      This really does nothing, except painting a
 *      3D rectangle and printing a question mark.
 */
void WgtPaint(HWND hwnd,
              PXCENTERWIDGET pWidget)
{
 HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);

 if(hps)
  {
   PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;


   if(pPrivate)
    {
     RECTL       rclWin;
     POINTL      aptlText[TXTBOX_COUNT];
     char        szText[64];


     // now paint frame
     WinQueryWindowRect(hwnd,
                        &rclWin);        // exclusive
     pgpihSwitchToRGB(hps);


     WinFillRect(hps,
                 &rclWin,                // exclusive
                 pPrivate->Setup.lcolBackground);


     // draw border
     if(pPrivate->pWidget->pGlobals->flDisplayStyle & XCS_SUNKBORDERS)
      {
       ULONG ulBorder=1;
       RECTL rcl2;


       memcpy(&rcl2, &rclWin, sizeof(RECTL));
       rcl2.xRight--;
       rcl2.yTop--;

       pgpihDraw3DFrame(hps,
                        &rcl2,
                        ulBorder,
                        pPrivate->pWidget->pGlobals->lcol3DDark,
                        pPrivate->pWidget->pGlobals->lcol3DLight);

       rclWin.xLeft += ulBorder;
       rclWin.yBottom += ulBorder;
       rclWin.xRight -= ulBorder;
       rclWin.yTop -= ulBorder;
      }



     // 'prepare' text to draw
     if(pPrivate->ulSocketError>0)
      {
       switch(pPrivate->ulSocketError)
        {
         case 1:
           sprintf(szText, "%s (unknown user)", pPrivate->Setup.szAccountName);
           break;

         case 2:
           sprintf(szText, "%s (wrong password)", pPrivate->Setup.szAccountName);
           break;

         case 5:
           sprintf(szText, "%s (no server)", pPrivate->Setup.szAccountName);
           break;
        }
      }
     else
       sprintf(szText, "%s (%d)", pPrivate->Setup.szAccountName,
                                  pPrivate->ulNumMails);


     // now check if we have enough space
     GpiQueryTextBox(hps,
                     strlen(szText),
                     szText,
                     TXTBOX_COUNT,
                     aptlText);

     if(((aptlText[TXTBOX_TOPRIGHT].x+26) > (rclWin.xRight+2)) || pPrivate->lCX==10)
      {
       // we need more space: tell XCenter client
       pPrivate->lCX = (aptlText[TXTBOX_TOPRIGHT].x + 32);

       WinPostMsg(WinQueryWindow(hwnd, QW_PARENT),
                  XCM_SETWIDGETSIZE,
                  (MPARAM)hwnd,
                  (MPARAM)pPrivate->lCX);
      }
     else
      {
       // sufficient space:

       // draw email-icon
       WinStretchPointer(hps,
                         rclWin.xLeft+4,
                         (rclWin.yTop-rclWin.yBottom-17)/2+1,
                         17,
                         17,
                         pPrivate->hptrEmail,
                         DP_NORMAL);


       rclWin.xLeft+=26;
       WinDrawText(hps,
                   strlen(szText),
                   szText,
                   &rclWin,                // exclusive
                   pPrivate->Setup.lcolForeground,
                   pPrivate->Setup.lcolBackground,
                   DT_LEFT| DT_VCENTER);
      }
    }
   WinEndPaint(hps);
  }
}

/*
 *@@ WgtPresParamChanged:
 *      implementation for WM_PRESPARAMCHANGED.
 *
 *      While this isn't exactly required, it's a nice
 *      thing for a widget to react to colors and fonts
 *      dropped on it. While we're at it, we also save
 *      these colors and fonts in our setup string data.
 */
void WgtPresParamChanged(HWND hwnd,
                         ULONG ulAttrChanged,
                         PXCENTERWIDGET pWidget)
{
    PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;
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

        if(fInvalidate)
        {
            // something has changed:
            XSTRING strSetup;

            // repaint
            pPrivate->lCX=10;
            WinInvalidateRect(hwnd, NULL, FALSE);

            // recompose our setup string
            WgtSaveSetup(&strSetup,
                         &pPrivate->Setup);
            if (strSetup.ulLength)
                // we have a setup string:
                // tell the XCenter to save it with the XCenter data
                WinSendMsg(WinQueryWindow(hwnd, QW_PARENT),//pPrivate->pWidget->pGlobals->hwndClient,
                           XCM_SAVESETUP,
                           (MPARAM)hwnd,
                           (MPARAM)strSetup.psz);
            pxstrClear(&strSetup);
        }
    } // end if (pPrivate)
}



/*
 *@@ WgtDestroy:
 *      implementation for WM_DESTROY.
 *
 *      This must clean up all allocated resources.
 */
void WgtDestroy(HWND hwnd,
                PXCENTERWIDGET pWidget)
{
    PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
     if(pPrivate->tidPopperThread) // a popper-thread is running right now, so kill it
       DosKillThread(pPrivate->tidPopperThread);

        if(pPrivate->ulTimerID)
          ptmrStopXTimer((PXTIMERSET)pPrivate->pWidget->pGlobals->pvXTimerSet,
                          hwnd,
                          pPrivate->ulTimerID);

        if(pPrivate->ulBlinkTimerID)
          ptmrStopXTimer((PXTIMERSET)pPrivate->pWidget->pGlobals->pvXTimerSet,
                          hwnd,
                          pPrivate->ulBlinkTimerID);


        WinDestroyPointer(pPrivate->hptrEmail);

        // check if we should store our password or not
        if(!pPrivate->Setup.fSavePass)
         {
          // something has changed:
          XSTRING strSetup;

          memset(&pPrivate->Setup.szPopPassword, 0, sizeof(pPrivate->Setup.szPopPassword));

          // recompose our setup string
          WgtSaveSetup(&strSetup,
                       &pPrivate->Setup);
          if(strSetup.ulLength)
              // we have a setup string:
              // tell the XCenter to save it with the XCenter data
              WinSendMsg(pPrivate->pWidget->pGlobals->hwndClient,
                         XCM_SAVESETUP,
                         (MPARAM)hwnd,
                         (MPARAM)strSetup.psz);
          pxstrClear(&strSetup);
         }



        WgtClearSetup(&pPrivate->Setup);

        WinDestroyWindow(pPrivate->hwndPopup);

        free(pPrivate);
                // pWidget is cleaned up by DestroyWidgets
    }
}

/*
 *@@ fnwpSampleWidget:
 *      window procedure for the winlist widget class.
 *
 *      There are a few rules which widget window procs
 *      must follow. See ctrDefWidgetProc in src\shared\center.c
 *      for details.
 *
 *      Other than that, this is a regular window procedure
 *      which follows the basic rules for a PM window class.
 */

MRESULT EXPENTRY fnwpSampleWidget(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
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
         *      popperPRIVATE for our own stuff.
         */

        case WM_CREATE:
            WinSetWindowPtr(hwnd, QWL_USER, mp1);
            pWidget = (PXCENTERWIDGET)mp1;
            if ((pWidget) && (pWidget->pfnwpDefWidgetProc))
                mrc = WgtCreate(hwnd, pWidget);
            else
                // stop window creation!!
                mrc = (MPARAM)TRUE;
        break;

        /*
         * WM_CONTROL:
         *      process notifications/queries from the XCenter.
         */

        case WM_CONTROL:
            mrc = (MPARAM)WgtControl(hwnd, mp1, mp2);
        break;

        /*case WM_MOUSEMOVE:
         {
          PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);


          if(pWidget)
           {*/
            // get private data from that widget data
            /*PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;

            WinSetPointer(HWND_DESKTOP, pPrivate->hptrHand);
           }
         }
        break;*/
            /*
        case WM_BUTTON1UP:
          if((long)WinGetKeyState(HWND_DESKTOP, VK_CTRL) & 0x8000)
            GetDrive(hwnd, pWidget, FALSE);
          else
            GetDrive(hwnd, pWidget, TRUE);
          break;*/

        /*
         * WM_PAINT:
         *
         */
        case WM_PAINT:
          WgtPaint(hwnd, pWidget);
          break;

        case WM_INITMENU:
         {
//          PmPrintf("INITMENU: %d", SHORT1FROMMP(mp1));
         }
        break;
                /*
        case WM_CONTEXTMENU:
         {
          PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

          if(pWidget)
           {      */
            // get private data from that widget data
/*          PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;
            POINTL         pt;


            WinQueryPointerPos(HWND_DESKTOP, &pt);
            WinPopupMenu(HWND_DESKTOP,
                         hwnd,
                         pPrivate->hwndPopup,
                         pt.x,
                         pt.y,
                         0,
                         PU_HCONSTRAIN | PU_VCONSTRAIN | PU_MOUSEBUTTON1
                                       | PU_MOUSEBUTTON2 | PU_KEYBOARD);
            return(MRFROMSHORT(TRUE));
           }
         }
        break;
  */
        case WM_COMMAND:
         {
//          PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);

          if(pWidget)
           {
            // get private data from that widget data
            PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;


            switch(SHORT1FROMMP(mp1))
             {
              case 3010: // check now
                if(pPrivate->tidPopperThread>0)
                  DosKillThread(pPrivate->tidPopperThread);
                pthrCreate(NULL,
                           fntPop3CheckThread,
                           &pPrivate->tidPopperThread,
                           "PopperThread",
                           THRF_PMMSGQUEUE | THRF_TRANSIENT,
                           (ULONG)pPrivate);
                break;

              case 3020: // reset
               {
                // turn off blinking
                if(pPrivate->ulBlinkTimerID)
                 {
                   ptmrStopXTimer((PXTIMERSET)pPrivate->pWidget->pGlobals->pvXTimerSet,
                                  hwnd,
                                  pPrivate->ulBlinkTimerID);

                  pPrivate->ulBlinkTimerID=0;


                  // redraw everything
                  pPrivate->fIconVisible=TRUE;
                  WinInvalidateRect(hwnd, NULLHANDLE, FALSE);
                 }
               }
              break;

              default:
                mrc = pWidget->pfnwpDefWidgetProc(hwnd, msg, mp1, mp2);
             }
           }
          else
            mrc = pWidget->pfnwpDefWidgetProc(hwnd, msg, mp1, mp2);
         }
        break;

        case WMXINT_NOMAILS:
         {
//          PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);


          if(pWidget)
           {
            // get private data from that widget data
            PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;



            if(pPrivate->ulBlinkTimerID)
             {
              ptmrStopXTimer((PXTIMERSET)pPrivate->pWidget->pGlobals->pvXTimerSet,
                                         hwnd,
                                         pPrivate->ulBlinkTimerID);
              pPrivate->ulBlinkTimerID=0;
             }
            pPrivate->lCX=10;
            WinInvalidateRect(pWidget->hwndWidget, NULL, FALSE);
           }
         }
        break;

        case WMXINT_NEWMAIL:
         {
//          PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);


          if(pWidget)
           {
            // get private data from that widget data
            PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;


            // redraw all to show correct number of mails
            pPrivate->lCX=10;
            WinInvalidateRect(hwnd, NULLHANDLE, FALSE);

            // start blink timer, if not started already
            if(!pPrivate->ulBlinkTimerID)
             {
               pPrivate->ulBlinkTimerID = ptmrStartXTimer((PXTIMERSET)pWidget->pGlobals->pvXTimerSet,
                                                          hwnd,
                                                          10,
                                                          350);
             }
           }
         }
        break;

        case WM_TIMER:
         {
//          PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);


          if(pWidget)
           {
            // get private data from that widget data
            PpopperPRIVATE pPrivate = (PpopperPRIVATE)pWidget->pUser;


            if(SHORT1FROMMP(mp1)==10)
             {
              // blink-timer
              HPS    hps;
              RECTL  r;


              WinQueryWindowRect(hwnd, &r);
              if(pPrivate->pWidget->pGlobals->flDisplayStyle & XCS_SUNKBORDERS)
               {
                ULONG ulBorder=1;


                r.xLeft += ulBorder;
                r.yBottom += ulBorder;
                r.xRight -= ulBorder;
                r.yTop -= ulBorder;
               }


              hps=WinGetPS(hwnd);
                if(pPrivate->fIconVisible)
                 {
                  // icon is visible -> draw a rect with the backgroundcolor
                  pgpihSwitchToRGB(hps);

                  r.xLeft+=4;
                  r.yBottom=(r.yTop-r.yBottom-17)/2+1;
                  r.xRight=r.xLeft+18;
                  r.yTop=r.yBottom+18;

                  WinFillRect(hps,
                              &r,
                              pPrivate->Setup.lcolBackground);

                  pPrivate->fIconVisible=FALSE;
                 }
                else
                 {
                  // icon is not visible -> draw it
                  WinStretchPointer(hps,
                                    r.xLeft+4,
                                    (r.yTop-r.yBottom-17)/2+1,
                                    17,
                                    17,
                                    pPrivate->hptrEmail,
                                    DP_NORMAL);

                  pPrivate->fIconVisible=TRUE;
                 }
              WinReleasePS(hps);
             }
            else
             {
              if(pPrivate->tidPopperThread>0)
                DosKillThread(pPrivate->tidPopperThread);

              // check for new-mails
              pthrCreate(NULL,
                         fntPop3CheckThread,
                         &pPrivate->tidPopperThread,
                         "PopperThread",
                         THRF_PMMSGQUEUE | THRF_TRANSIENT,
                         (ULONG)pPrivate);
             }
           }
         }
        break;

        /*
         * WM_PRESPARAMCHANGED:
         *
         */
        case WM_PRESPARAMCHANGED:
            if(pWidget)
                // this gets sent before this is set!
                WgtPresParamChanged(hwnd, (ULONG)mp1, pWidget);
        break;

        /*
         * WM_DESTROY:
         *      clean up. This _must_ be passed on to
         *      ctrDefWidgetProc.
         */

        case WM_DESTROY:
            WgtDestroy(hwnd, pWidget);
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
 *@@ WgtInitModule:
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
ULONG EXPENTRY WgtInitModule(HAB hab,         // XCenter's anchor block
                              HMODULE hmodPlugin, // module handle of the widget DLL
                              HMODULE hmodXFLDR,    // XFLDR.DLL module handle
                              PXCENTERWIDGETCLASS *ppaClasses,
                              PSZ pszErrorMsg)  // if 0 is returned, 500 bytes of error msg
{
    ULONG   ulrc = 0,
            ul = 0;
    BOOL    fImportsFailed = FALSE;

    // resolve imports from XFLDR.DLL (this is basically
    // a copy of the doshResolveImports code, but we can't
    // use that before resolving...)

    hmod=hmodPlugin;
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
                              WNDCLASS_WIDGET_SAMPLE,
                              fnwpSampleWidget,
                              CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT,
                              sizeof(PpopperPRIVATE))
                                    // extra memory to reserve for QWL_USER
                             )
            strcpy(pszErrorMsg, "WinRegisterClass failed.");
        else
        {
            // no error:
            // return widget classes
            *ppaClasses = G_WidgetClasses;

            // return no. of classes in this DLL (one here):
            ulrc = sizeof(G_WidgetClasses) / sizeof(G_WidgetClasses[0]);
        }
    }

    return (ulrc);
}

/*
 *@@ WgtUnInitModule:
 *      optional export with ordinal 2, which can clean
 *      up global widget class data.
 *
 *      This gets called by the XCenter right before
 *      a widget DLL gets unloaded. Note that this
 *      gets called even if the "init module" export
 *      returned 0 (meaning an error) and the DLL
 *      gets unloaded right away.
 */
void EXPENTRY WgtUnInitModule(void)
{
}

void EXPENTRY WgtQueryVersion(PULONG pulMajor,
                              PULONG pulMinor,
                              PULONG pulRevision)
{
 // report 0.9.9
 *pulMajor = 0;
 *pulMinor = 9;
 *pulRevision = 9;
}


/*
 *@@ WwgtShowSettingsDlg:
 *      this displays the winlist widget's settings
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
void EXPENTRY WgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData)
{//            PmPrintf("SHOW SETTINGS");
    HWND hwnd = WinLoadDlg(HWND_DESKTOP,         // parent
                           pData->hwndOwner,
                           fnwpSettingsDlg,
                           hmod,//pcmnQueryNLSModuleHandle(FALSE),
                           1200,
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
 *@@ fnwpSettingsDlg:
 *      dialog proc for the winlist settings dialog.
 */
MRESULT EXPENTRY fnwpSettingsDlg(HWND hwnd,
                                 ULONG msg,
                                 MPARAM mp1,
                                 MPARAM mp2)
{
 MRESULT mrc = 0;
 static PWIDGETSETTINGSDLGDATA pData;


 switch(msg)
  {
   case WM_INITDLG:
    {
     pData=(PWIDGETSETTINGSDLGDATA)mp2;


     WinPostMsg(hwnd, WMXINT_SETUP, (MPARAM)0, (MPARAM)0); // otherwise all auto(radio)controls are resetted??
    }
   break;

   case WMXINT_SETUP:
    {
     char sz[32];

     // setup-string-handling
     PPOPPERSETUP pSetup=(PPOPPERSETUP)malloc(sizeof(POPPERSETUP));


     // set max.length of 'title' to 30
     WinSendMsg(WinWindowFromID(hwnd, 110),
                EM_SETTEXTLIMIT,
                MPFROMSHORT(30),
                (MPARAM)0);

     // set max.length of 'server' to 64
     WinSendMsg(WinWindowFromID(hwnd, 111),
                EM_SETTEXTLIMIT,
                MPFROMSHORT(64),
                (MPARAM)0);

     // set max.length of 'port' to 4
     WinSendMsg(WinWindowFromID(hwnd, 112),
                EM_SETTEXTLIMIT,
                MPFROMSHORT(4),
                (MPARAM)0);

     // set max.length of 'user' to 64
     WinSendMsg(WinWindowFromID(hwnd, 113),
                EM_SETTEXTLIMIT,
                MPFROMSHORT(64),
                (MPARAM)0);

     // set max.length of 'password' to 64
     WinSendMsg(WinWindowFromID(hwnd, 114),
                EM_SETTEXTLIMIT,
                MPFROMSHORT(64),
                (MPARAM)0);

     if(pSetup)
      {
       memset(pSetup, 0, sizeof(POPPERSETUP));
       // store this in WIDGETSETTINGSDLGDATA
       pData->pUser = pSetup;

       WgtScanSetup(pData->pcszSetupString, pSetup);
      }


     // fill the entry-fields
     WinSetDlgItemText(hwnd, 110, pSetup->szAccountName);
     WinSetDlgItemText(hwnd, 111, pSetup->szPopServer);

     if(0==pSetup->ulPopPort)
       WinSetDlgItemText(hwnd, 112, "110");
     else
      {
       sprintf(sz, "%d", pSetup->ulPopPort);
       WinSetDlgItemText(hwnd, 112, sz);
      }

     WinSetDlgItemText(hwnd, 113, pSetup->szPopUser);


     WinSetDlgItemText(hwnd, 114, pSetup->szPopPassword);
     if(pSetup->fSavePass)
       WinCheckButton(hwnd, 116, 1);
     else
       WinCheckButton(hwnd, 116, 0);
    }
   break;

   case WM_DESTROY:
    {
     if(pData)
      {
       PPOPPERSETUP pSetup = (PPOPPERSETUP)pData->pUser;
       if(pSetup)
        {
         WgtClearSetup(pSetup);
         free(pSetup);
        } // end if (pSetup)
      } // end if (pData)
    }
   break;



   case WM_COMMAND:
    {
     switch(SHORT1FROMMP(mp1))
      {
       case 210: // ok-button
        {
         PPOPPERSETUP pSetup=(PPOPPERSETUP)pData->pUser;
         char         sz[32];
         XSTRING strSetup;

         // 'store' settings in pSetup
         // account name
         WinQueryDlgItemText(hwnd, 110, sizeof(pSetup->szAccountName), (PSZ)pSetup->szAccountName);

         // server
         WinQueryDlgItemText(hwnd, 111, sizeof(pSetup->szPopServer), (PSZ)pSetup->szPopServer);

         // port
         WinQueryDlgItemText(hwnd, 112, sizeof(sz), (PSZ)sz);
         pSetup->ulPopPort=(ULONG)atol(sz);

         // user
         WinQueryDlgItemText(hwnd, 113, sizeof(pSetup->szPopUser), (PSZ)pSetup->szPopUser);

         // password
         WinQueryDlgItemText(hwnd, 114, sizeof(pSetup->szPopPassword), (PSZ)pSetup->szPopPassword);


         // save password
         if(WinQueryButtonCheckstate(hwnd, 116))
           pSetup->fSavePass=TRUE;
         else
           pSetup->fSavePass=FALSE;

         // checktimer
         WinQueryDlgItemText(hwnd, 115, sizeof(sz), (PSZ)sz);
         pSetup->ulCheckTimer=(ULONG)atol(sz);


         // something has changed:
         WgtSaveSetup(&strSetup,
                      pSetup);
         pData->pctrSetSetupString(pData->hSettings,
                                   strSetup.psz);
         pxstrClear(&strSetup);


         WinDismissDlg(hwnd, TRUE);
        }
       break;
      }
    }
   break;


   default:
     mrc = WinDefDlgProc(hwnd, msg, mp1, mp2);
  }

 return (mrc);
}


void _Optlink fntPop3CheckThread(PTHREADINFO pti)
{
 HAB             habAnchor=WinInitialize(0);
 HMQ             hmqQueue=WinCreateMsgQueue(habAnchor, 0);
 PpopperPRIVATE  pPrivate = (PpopperPRIVATE)(pti->ulData);
 ULONG           ulNumMails=0;


 pPrivate->ulSocketError=0;

 if(0==strlen(pPrivate->Setup.szPopPassword))
  {
   //we dont know password so fire-up settings-dlg and exit this thread
   WinPostMsg(pPrivate->hwndSelf,
              WM_COMMAND,
              (MPARAM)ID_CRMI_PROPERTIES,
              (MPARAM)0);
  }
 else
  {
   int sok;
   char sz[255];


   sok=sockOpen(pPrivate->Setup.szPopServer,
                pPrivate->Setup.ulPopPort);
   if(sok>-1)
    {
     // read 'greeting-text'
     sockReadln(sok, (PSZ)sz, sizeof(sz));
     //if(sz[0]=='-')


     // send user
     sockWriteln(sok, "USER %s", pPrivate->Setup.szPopUser);
     sockReadln(sok, (PSZ)sz, sizeof(sz));

     // send pass
     if(sz[0]=='+')
      {
       sockWriteln(sok, "PASS %s", pPrivate->Setup.szPopPassword);
       sockReadln(sok, (PSZ)sz, sizeof(sz));


       if(sz[0]=='-')
         pPrivate->ulSocketError=2; //WRONG PASSWORD
       else
        {
         PSZ  pszBlank1, pszBlank2;
         char szNumMails[10];


         sockWriteln(sok, "STAT");
         sockReadln(sok, (PSZ)sz, sizeof(sz));     // server schickt zb:  '+OK 5 27805'       + bedeutet ok, 5=anzahl mails, 27805=groesse aller mails in byte
                                                   //                     (alles durch blanks getrennt)
         pszBlank1=strchr((PSZ)sz, ' ');
         if(pszBlank1)
          {
           pszBlank1++;
           pszBlank2=strchr(pszBlank1, ' ');
           if(pszBlank2)
            {
             memset(&szNumMails, 0, sizeof(szNumMails));
             memcpy((PSZ)szNumMails, pszBlank1, pszBlank2-pszBlank1);

             ulNumMails=(ULONG)atol(szNumMails);
             if(ulNumMails==0)
              {
               pPrivate->ulNumMails=0;

               WinPostMsg(pPrivate->hwndSelf,
                          WMXINT_NOMAILS,
                          (MPARAM)0,
                          (MPARAM)0);
              }
             else
              {
               // if ulNumMails!=pPrivate->ulNumMails then start blink-timer
               if(ulNumMails!=pPrivate->ulNumMails)
                {
                 pPrivate->ulNumMails=ulNumMails;

                 WinPostMsg(pPrivate->hwndSelf,
                            WMXINT_NEWMAIL,
                            (MPARAM)0,
                            (MPARAM)0);
                }
              }
            }
          }


         // abmelden, aus hoeflichkeit
         sockWriteln(sok, "QUIT");
        }
      }
     else
       pPrivate->ulSocketError=1; // UNKNOWN USER
    }
   else
     pPrivate->ulSocketError=5; // NO SERVER

   sockClose(sok);
  }

 if(pPrivate->ulSocketError>0)
   WinPostMsg(pPrivate->hwndSelf,
              WMXINT_NOMAILS,
              (MPARAM)0,
              (MPARAM)0);


 WinDestroyMsgQueue(hmqQueue);
 WinTerminate(habAnchor);
}

