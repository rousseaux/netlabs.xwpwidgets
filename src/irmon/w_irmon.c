
/*
 *@@sourcefile w_irmon.c:
 *      XCenter "IR-Monitor" widget.
 *
 *      This is all new with V0.9.9. Thanks to fonz for the
 *      contribution.
 *
 *@@added V0.9.9 (2001-02-28) [umoeller]
 *@@header "shared\center.h"
 */

#pragma strings(readonly)

#define INCL_DOSMODULEMGR
#define INCL_DOSERRORS
#define INCL_DOSDEVICES
#define INCL_DOSFILEMGR
#define INCL_DOSPROCESS

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
#define INCL_WINTIMER

#define INCL_GPIPRIMITIVES
#define INCL_GPILOGCOLORTABLE
#include <os2.h>

// C library headers
#include <stdio.h>
#include <string.h>
#include <setjmp.h>             // needed for except.h
#include <assert.h>             // needed for except.h

// generic headers
#define DONT_REPLACE_MALLOC         // in case mem debug is enabled
#include "setup.h"                      // code generation and debugging options

// disable wrappers, because we're not linking statically
#ifdef WINH_STANDARDWRAPPERS
    #undef WINH_STANDARDWRAPPERS
#endif

// headers in /helpers
#include "helpers\dosh.h"               // Control Program helper routines
#include "helpers\gpih.h"               // GPI helper routines
#include "helpers\prfh.h"               // INI file helper routines;
                                        // this include is required for some
                                        // of the structures in shared\center.h
#include "helpers\winh.h"               // PM helper routines
#include "helpers\xstring.h"            // extended string helpers
#include "helpers\comctl.h"

// XWorkplace implementation headers
#include "shared\center.h"              // public XCenter interfaces
#include "shared\common.h"              // the majestic XWorkplace include file

//
#include "irmon\w_irmonrc.h"
#pragma hdrstop                     // VAC++ keeps crashing otherwise

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

#define WNDCLASS_WIDGET_IRMON "XWPCenterIrMonWidget"

static XCENTERWIDGETCLASS G_WidgetClasses[]
    = {
        WNDCLASS_WIDGET_IRMON,     // PM window class name
        0,                          // additional flag, not used here
        "IRMonWidget",              // internal widget class name
        "IR-Monitor",               // widget class name displayed to user
        WGTF_UNIQUEGLOBAL | WGTF_TRAYABLE | WGTF_TOOLTIP,      // widget class flags
        NULL                        // no settings dialog
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

RESOLVEFUNCTION G_aImports[] =
{
  "cmnQueryDefaultFont", (PFN*)&pcmnQueryDefaultFont,

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
  "xstrInit", (PFN*)&pxstrInit
};


#pragma pack(1)

// IOCtl category for the management interface
#define IRCOMM_MANAGEMENT_CATEGORY 0x80
// IOCtl command to query the IrCOMM status
#define IRCOMM_DISCOVER 0x60
#define IRCOMM_QUERY_STATUS 0x61
#define IRCOMM_DISCONNECT 0x62
#define IRCOMM_DOCONNECT 0x63

// structure that is passed on a IrCOMM status request
typedef struct
{
  BYTE connected;     // indicates whether we have a valid connection
  BYTE remoteLSAP;  // determines the remote LSAP, 0 if none
  #define MAX_DEVICE_NAME_LENGTH 25
  char deviceName[MAX_DEVICE_NAME_LENGTH];  // tells the name of the IrCOMM device we're using
  #define MAX_PORT_NAME_LENGTH 9
  char COMPortName[MAX_PORT_NAME_LENGTH];   // tells the name of the COM port we're using
} IrCOMMStatusStruct;

#pragma pack()

#define IRMON_SOUND_INRANGE           333
#define IRMON_SOUND_OUTRANGE          334
#define IRMON_SOUND_NOTIFYNEWFILES    335
#define IRMON_SOUND_BEGINTRANSMISSION 336
#define IRMON_SOUND_ENDTRANSMISSION   337
#define IRMON_SOUND_INTERRUPTION      338

#define TIMER_DOIRDA 1
#define TIMER_ANIM   2
#define WM_DOCONNECT (WM_USER + 1)


/* ******************************************************************
 *
 *   Private widget instance data
 *
 ********************************************************************/

/*
 *@@ IRSETUP:
 *      instance data to which setup strings correspond.
 *      This is also a member of IRPRIVATE.
 *
 *      Putting these settings into a separate structure
 *      is no requirement, but comes in handy if you
 *      want to use the same setup string routines on
 *      both the open widget window and a settings dialog.
 */

typedef struct _IRSETUP
{
  LONG        lcolBackground,         // background color
              lcolForeground;         // foreground color (for text)
  BOOL    fIrActive;
  BOOL    fSearchForDevices;
  USHORT  usSearchInterval;
  BOOL    fInstallSoftForPNPDevs;
  ULONG   ulMaxSpeed; // 0 = unlimited;
  BOOL    fOpenOnDisconnect;
  BOOL    fAudibleNotifyOnDisconnect;
  CHAR    szName[32];
  CHAR    szDesc[32];
} IRSETUP, *PIRSETUP;

/*
 *@@ IRPRIVATE:
 *      more window data for the widget.
 *
 *      An instance of this is created on WM_CREATE in
 *      fnwpSampleWidget and stored in XCENTERWIDGET.pUser.
 */

typedef struct _IRPRIVATE
{
  PXCENTERWIDGET pWidget;
          // reverse ptr to general widget data ptr; we need
          // that all the time and don't want to pass it on
          // the stack with each function call

  IRSETUP Setup;
          // widget settings that correspond to a setup string
  BOOL fDriverInstalled;
  BOOL fPlayAnimation;
  ULONG ulFrame;
  HBITMAP hBmpNoDriver;
  HBITMAP hBmpSearch[11];
  HBITMAP hBmpConnected[11];
  BOOL fTooltipShowing;
  IrCOMMStatusStruct *pStatus;
} IRPRIVATE, *PIRPRIVATE;

HMODULE g_hResDLL;
HMODULE g_hRexxDLL;
HAB     g_hab;
PIRPRIVATE g_pPrv;
IrCOMMStatusStruct *g_pIrCOMMStatus = NULL;
typedef VOID EXPENTRY REXXINITDLL(IrCOMMStatusStruct *statusStruct);
typedef REXXINITDLL *PREXXINITDLL;
typedef VOID EXPENTRY REXXUNINITDLL(void);
typedef REXXUNINITDLL *PREXXUNINITDLL;

PREXXUNINITDLL pUninitRexx;
PREXXINITDLL   pInitRexx;
void APIENTRY IrCOMMExitFnc(void);

BOOL DriverPresent(void)
{
  ULONG ulAction;
  HFILE IrDaDriver;

  if (DosOpen("IRCOMM$", &IrDaDriver, &ulAction, 0, FILE_NORMAL,
              OPEN_ACTION_OPEN_IF_EXISTS,
              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE | OPEN_FLAGS_FAIL_ON_ERROR,
              0) != 0)
    return FALSE;
  DosClose(IrDaDriver);
  return TRUE;
}

void GetStatus(IrCOMMStatusStruct *pStatus)
{
  ULONG ulAction;
  ULONG dataLen;
  HFILE IrDaDriver;
  APIRET rc;
  if (DosOpen("IRCOMM$", &IrDaDriver, &ulAction, 0, FILE_NORMAL,
              OPEN_ACTION_OPEN_IF_EXISTS,
              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE | OPEN_FLAGS_FAIL_ON_ERROR,
              0) != 0)
  {
    memset(pStatus,0, sizeof(IrCOMMStatusStruct));
    _Pmpf((" GetStatus: Open Driver failed"));
  }
  else
  {
    rc = DosDevIOCtl( IrDaDriver, IRCOMM_MANAGEMENT_CATEGORY,
                      IRCOMM_QUERY_STATUS, NULL, 0, 0, pStatus,
                      sizeof(IrCOMMStatusStruct), &dataLen);
    _Pmpf((" GetStatus: IOCtl rc=%d",rc));
    DosClose(IrDaDriver);
  }
}

void SendDiscovery(void)
{
  ULONG dataLen;
  HFILE IrDaDriver;
  ULONG ulAction;
  APIRET rc;
  if (DosOpen("IRCOMM$", &IrDaDriver, &ulAction, 0, FILE_NORMAL,
              OPEN_ACTION_OPEN_IF_EXISTS,
              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE | OPEN_FLAGS_FAIL_ON_ERROR,
              0) == 0)
  {
    rc = DosDevIOCtl(IrDaDriver, IRCOMM_MANAGEMENT_CATEGORY,
                     IRCOMM_DISCOVER, NULL, 0, NULL, NULL, 0, &dataLen);
    _Pmpf((" SendDiscovery: IOCtl rc=%d",rc));
    DosClose(IrDaDriver);
  }
  else
  {
    _Pmpf((" SendDiscovery: Open Driver failed"));
  }
}

void SendDoConnect(void)
{
  ULONG dataLen;
  ULONG ulAction;
  HFILE IrDaDriver;
  APIRET rc;
  // just do an I/O control to the device driver, no data passed
  if (DosOpen("IRCOMM$", &IrDaDriver, &ulAction, 0, FILE_NORMAL,
              OPEN_ACTION_OPEN_IF_EXISTS,
              OPEN_SHARE_DENYNONE | OPEN_ACCESS_READWRITE | OPEN_FLAGS_FAIL_ON_ERROR,
              0) == 0)
  {
    rc = DosDevIOCtl(IrDaDriver, IRCOMM_MANAGEMENT_CATEGORY,
                     IRCOMM_DOCONNECT, NULL, 0, NULL, NULL, 0, &dataLen);
    _Pmpf((" SendDoConnect: IOCtl rc=%d",rc));
    DosClose(IrDaDriver);
  }
  else
  {
    _Pmpf((" SendDoConnect: Open Driver failed"));
  }
}

void Connected(void)
{
  WinAlarm(HWND_DESKTOP, IRMON_SOUND_INRANGE);
  _Pmpf((" Connected!"));
}

void Disconnected(void)
{
  WinAlarm(HWND_DESKTOP, IRMON_SOUND_OUTRANGE);
  _Pmpf((" Disconnected!"));
}


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

VOID WgtClearSetup(PIRSETUP pSetup)
{
  if (pSetup)
  {
    memset(&pSetup->fIrActive, 0, (sizeof(IRSETUP) - 2*sizeof(LONG)) );
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

VOID WgtScanSetup(const char *pcszSetupString,
                  PIRSETUP pSetup)
{
  PSZ p;
  ULONG ulTmp;
  // background color
  if (p = pctrScanSetupString(pcszSetupString,
                              "BGNDCOL"))
  {
    pSetup->lcolBackground = pctrParseColorString(p);
    pctrFreeSetupValue(p);
  }
  else
    // default color:
    pSetup->lcolBackground = WinQuerySysColor(HWND_DESKTOP, SYSCLR_DIALOGBACKGROUND, 0);

  // text color:
  if (p = pctrScanSetupString(pcszSetupString,
                              "TEXTCOL"))
  {
    pSetup->lcolForeground = pctrParseColorString(p);
    pctrFreeSetupValue(p);
  }
  else
    pSetup->lcolForeground = WinQuerySysColor(HWND_DESKTOP, SYSCLR_WINDOWSTATICTEXT, 0);
/*
  if (p = pctrScanSetupString(pcszSetupString,
                              "ACTIVE"))
  {
    pSetup->fIrActive = stricmp(p,"YES")==0;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->fIrActive = TRUE;

  if (p = pctrScanSetupString(pcszSetupString,
                              "SEARCH"))
  {
    pSetup->fSearchForDevices = stricmp(p,"YES")==0;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->fSearchForDevices = TRUE;

  if (p = pctrScanSetupString(pcszSetupString,
                              "SEARCHTIME"))
  {
    ulTmp = atoi(p);
    if( (ulTmp>1) && (ulTmp<301))
      pSetup->usSearchInterval = ulTmp;
    else
      pSetup->usSearchInterval = 3;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->usSearchInterval = 3;

  if (p = pctrScanSetupString(pcszSetupString,
                              "SOFTPNP"))
  {
    pSetup->fInstallSoftForPNPDevs = stricmp(p,"YES")==0;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->fInstallSoftForPNPDevs = TRUE;

  if (p = pctrScanSetupString(pcszSetupString,
                              "SPEEDLIMT"))
  {
    ulTmp = atoi(p);
    pSetup->ulMaxSpeed =  (ulTmp<7)?ulTmp:0;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->ulMaxSpeed = 0;

  if (p = pctrScanSetupString(pcszSetupString,
                              "DISCONNECT_DLG"))
  {
    pSetup->fOpenOnDisconnect = stricmp(p,"YES")==0;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->fOpenOnDisconnect = FALSE;

  if (p = pctrScanSetupString(pcszSetupString,
                              "DISCONNECT_AUDIO"))
  {
    pSetup->fAudibleNotifyOnDisconnect = stricmp(p,"YES")==0;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->fAudibleNotifyOnDisconnect = FALSE;

  if (p = pctrScanSetupString(pcszSetupString,
                              "IR_NAME"))
  {
    strncpy(pSetup->szName,p,sizeof(pSetup->szName)-1);
    pSetup->szName[31] = 0;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->szName[0]=0;

  if (p = pctrScanSetupString(pcszSetupString,
                              "IR_DESC"))
  {
    strncpy(pSetup->szDesc,p,sizeof(pSetup->szDesc)-1);
    pSetup->szDesc[31] = 0;
    pctrFreeSetupValue(p);
  }
  else
    pSetup->szDesc[0]=0;
    */
}

/*
 *@@ WgtSaveSetup:
 *      composes a new setup string.
 *      The caller must invoke xstrClear on the
 *      string after use.
 */

VOID WgtSaveSetup(PXSTRING pstrSetup,       // out: setup string (is cleared first)
                  PIRSETUP pSetup)                                       
{
  CHAR    szTemp[100];
  // PSZ     psz = 0;
  pxstrInit(pstrSetup, 200);

  sprintf(szTemp, "BGNDCOL=%06lX;",
          pSetup->lcolBackground);
  pxstrcat(pstrSetup, szTemp, 0);

  sprintf(szTemp, "TEXTCOL=%06lX;",
          pSetup->lcolForeground);
  pxstrcat(pstrSetup, szTemp, 0);

  if(!pSetup->fIrActive)
  {
    sprintf(szTemp, "ACTIVE=NO;");
    pxstrcat(pstrSetup, szTemp, 0);
  }
  if(!pSetup->fSearchForDevices)
  {
    sprintf(szTemp, "SEARCH=NO;");
    pxstrcat(pstrSetup, szTemp, 0);
  }

  if(pSetup->usSearchInterval != 3)
  {
    sprintf( szTemp, "SEARCHTIME=%d;",
             pSetup->usSearchInterval);
    pxstrcat(pstrSetup, szTemp, 0);
  }

  if(!pSetup->fInstallSoftForPNPDevs)
  {
    sprintf(szTemp, "SOFTPNP=NO;");
    pxstrcat(pstrSetup, szTemp, 0);
  }

  if(pSetup->ulMaxSpeed)
  {
    sprintf( szTemp, "SPEEDLIMT=%d;",
             pSetup->ulMaxSpeed);
    pxstrcat(pstrSetup, szTemp, 0);
  }

  if (pSetup->fOpenOnDisconnect)
  {
    sprintf(szTemp, "DISCONNECT_DLG=YES;");
    pxstrcat(pstrSetup, szTemp, 0);
  }

  if(pSetup->fAudibleNotifyOnDisconnect)
  {
    sprintf(szTemp, "DISCONNECT_AUDIO=YES;");
    pxstrcat(pstrSetup, szTemp, 0);
  }

  if(strlen(pSetup->szName))
  {
    sprintf( szTemp, "IR_NAME=%s;",
             pSetup->szName);
    pxstrcat(pstrSetup, szTemp, 0);
  }

  if(strlen(pSetup->szDesc))
  {
    sprintf( szTemp, "IR_DESC=%s;",
             pSetup->szDesc);
    pxstrcat(pstrSetup, szTemp, 0);
  }

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
  HPS hps;

  // PSZ p;
  PIRPRIVATE pPrivate = malloc(sizeof(IRPRIVATE));
  g_pPrv = pPrivate;
  memset(pPrivate, 0, sizeof(IRPRIVATE));
  // link the two together
  pWidget->pUser = pPrivate;
  pPrivate->pWidget = pWidget;

  // initialize binary setup structure from setup string
  WgtScanSetup(pWidget->pcszSetupString,
               &pPrivate->Setup);

  // if you want the context menu help to be enabled,
  // add your help library here; if these fields are
  // left NULL, the "Help" context menu item is disabled

  // pWidget->pcszHelpLibrary = pcmnQueryHelpLibrary();
  // pWidget->ulHelpPanelID = ID_XSH_WIDGET_WINLIST_MAIN;
  hps  = WinGetPS(hwnd);
  pPrivate->hBmpNoDriver      = GpiLoadBitmap(hps, g_hResDLL, ID_BmpOFF,0,0);
  pPrivate->hBmpSearch[0]     = GpiLoadBitmap(hps, g_hResDLL, ID_BmpSCAN1,0,0);
  pPrivate->hBmpSearch[1]     = GpiLoadBitmap(hps, g_hResDLL, ID_BmpSCAN2,0,0);
  pPrivate->hBmpSearch[2]     = pPrivate->hBmpSearch[0];
  pPrivate->hBmpSearch[3]     = pPrivate->hBmpSearch[1];
  pPrivate->hBmpSearch[4]     = pPrivate->hBmpSearch[0];
  pPrivate->hBmpSearch[5]     = pPrivate->hBmpSearch[1];
  pPrivate->hBmpSearch[6]     = pPrivate->hBmpSearch[0];
  pPrivate->hBmpSearch[7]     = pPrivate->hBmpSearch[1];
  pPrivate->hBmpSearch[8]     = pPrivate->hBmpSearch[0];
  pPrivate->hBmpSearch[9]     = pPrivate->hBmpSearch[1];
  pPrivate->hBmpSearch[10]    = pPrivate->hBmpSearch[0];
  pPrivate->hBmpConnected[0]  = GpiLoadBitmap(hps, g_hResDLL, ID_BmpCONN1,0,0);
  pPrivate->hBmpConnected[1]  = GpiLoadBitmap(hps, g_hResDLL, ID_BmpCONN2,0,0);
  pPrivate->hBmpConnected[2]  = GpiLoadBitmap(hps, g_hResDLL, ID_BmpCONN3,0,0);
  pPrivate->hBmpConnected[3]  = GpiLoadBitmap(hps, g_hResDLL, ID_BmpCONN4,0,0);
  pPrivate->hBmpConnected[4]  = GpiLoadBitmap(hps, g_hResDLL, ID_BmpCONN5,0,0);
  pPrivate->hBmpConnected[5]  = GpiLoadBitmap(hps, g_hResDLL, ID_BmpCONN6,0,0);
  pPrivate->hBmpConnected[6]  = GpiLoadBitmap(hps, g_hResDLL, ID_BmpCONN7,0,0);
  pPrivate->hBmpConnected[7]  = pPrivate->hBmpConnected[3];
  pPrivate->hBmpConnected[8]  = GpiLoadBitmap(hps, g_hResDLL, ID_BmpCONN8,0,0);
  pPrivate->hBmpConnected[9]  = pPrivate->hBmpConnected[1];
  pPrivate->hBmpConnected[10] = pPrivate->hBmpConnected[0];
  WinReleasePS(hps);

	DosAllocSharedMem((PPVOID)&g_pIrCOMMStatus, NULL,
	                  sizeof(IrCOMMStatusStruct), PAG_READ | PAG_WRITE |
	                  PAG_COMMIT | OBJ_GETTABLE);
  pPrivate->pStatus  = g_pIrCOMMStatus;	
	memset(g_pIrCOMMStatus, 0, sizeof(IrCOMMStatusStruct));
	pPrivate->fDriverInstalled = DriverPresent();
	// call Rexx interface init function
  if(pInitRexx)
  {
    _Pmpf((" Calling Init in Rexx DLL"));
    pInitRexx(g_pIrCOMMStatus);
  }
//	DosExitList(EXLST_ADD, (PFNEXITLIST)IrCOMMExitFnc);
  if(pPrivate->fDriverInstalled)
  {
    WinStartTimer(g_hab, hwnd, TIMER_DOIRDA, 3000);
    WinStartTimer(g_hab, hwnd, TIMER_ANIM, 100);
  }
  return (mrc);
}

// exit handler, cleanup resources. Note: this function must never block
void APIENTRY IrCOMMExitFnc(void)
{
  int i;
	// delete bitmap handles
	GpiDeleteBitmap(g_pPrv->hBmpNoDriver);
	for(i=0;i<11;i++)
	{
  	GpiDeleteBitmap(g_pPrv->hBmpSearch[i]);
	  GpiDeleteBitmap(g_pPrv->hBmpConnected[i]);
	}
	// now delete the presentation space and the memory context
	// uninit Rexx DLL
  if(pUninitRexx)
  {
    _Pmpf((" Calling UnInit in Rexx DLL"));
    pUninitRexx();
    DosFreeModule(g_hRexxDLL);
  }
	// free status struct memory
	DosFreeMem(g_pIrCOMMStatus);
	// ok, now we can exit
	DosExitList(EXLST_EXIT, (PFNEXITLIST)NULL);
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
char szTTip[200];

BOOL WgtControl(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
  BOOL brc = FALSE;

  // get widget data from QWL_USER (stored there by WM_CREATE)
  PXCENTERWIDGET pWidget = (PXCENTERWIDGET)WinQueryWindowPtr(hwnd, QWL_USER);
  if (pWidget)
  {
    // get private data from that widget data
    PIRPRIVATE pPrivate = (PIRPRIVATE)pWidget->pUser;
    if (pPrivate)
    {
      USHORT  usID = SHORT1FROMMP(mp1),
              usNotifyCode = SHORT2FROMMP(mp1);

      // is this from the XCenter client?
      switch(usID)
      {
        case ID_XCENTER_CLIENT:
        {
          switch (usNotifyCode)
          {
            /*
             * XN_QUERYSIZE:
             *      XCenter wants to know our size.
             */

            case XN_QUERYSIZE:
            {
                PSIZEL pszl = (PSIZEL)mp2;
                pszl->cx = 20;      // desired width
                pszl->cy = 16;      // desired minimum height
                brc = TRUE;
              break;
            }

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

              WinInvalidateRect(pWidget->hwndWidget, NULL, FALSE);
              break;
            }
            default:
              break;
          }
          break;
        }
        case ID_XCENTER_TOOLTIP:
        {
          switch (usNotifyCode)
          {
            case TTN_NEEDTEXT:
            {
              PTOOLTIPTEXT pttt;
              if(pPrivate->fDriverInstalled)
              {
                if(pPrivate->pStatus->connected)
                {
                  sprintf( szTTip,
                           "Connected to %s\nAt port %s\nLSAP : %d",
                           pPrivate->pStatus->deviceName,
                           pPrivate->pStatus->COMPortName,
                           pPrivate->pStatus->remoteLSAP);
                }
                else
                {
                  strcpy(szTTip,"No Devices in range");
                }
              }
              else
                strcpy(szTTip,"No IR Driver");
              pttt = (PTOOLTIPTEXT)mp2;
              pttt->pszText = szTTip;
              pttt->ulFormat = TTFMT_PSZ;
              break;
            }
            case TTN_SHOW:
              pPrivate->fTooltipShowing = TRUE;
              break;
            case TTN_POP:
              pPrivate->fTooltipShowing = FALSE;
              break;
          }
        }
        break;
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

VOID WgtPaint(HWND hwnd,
              PXCENTERWIDGET pWidget)
{
  HPS hps = WinBeginPaint(hwnd, NULLHANDLE, NULL);
  if (hps)
  {
    RECTL  rclWin;
    PIRPRIVATE pPrivate = (PIRPRIVATE)pWidget->pUser;
    WinQueryWindowRect( hwnd,
                        &rclWin);        // exclusive
    WinFillRect( hps,
                 &rclWin,                // exclusive
                 CLR_PALEGRAY);//pPrivate->Setup.lcolBackground);
    if (pPrivate)
    {
      POINTL ptlPos;
      HBITMAP hBmp;
      ptlPos.x = 0;
      ptlPos.y = 0;

      if(!pPrivate->fDriverInstalled)
      {
        hBmp = pPrivate->hBmpNoDriver;
      }
      else
      {
        _Pmpf((" Paint ANIM Frame %d",pPrivate->ulFrame % 11));
        if(pPrivate->pStatus->connected)
        {
          hBmp = pPrivate->hBmpConnected[pPrivate->ulFrame % 11];
        }
        else
        {
          hBmp = pPrivate->hBmpSearch[pPrivate->ulFrame % 11];
        }
      }

      if(!hBmp)
      {
        WinFillRect( hps,
                     &rclWin,                // exclusive
                     CLR_RED);//pPrivate->Setup.lcolBackground);

      }
      else
        WinDrawBitmap( hps,
                       hBmp,
                       NULL, &ptlPos, 0,0, DBM_NORMAL);

    }
    else
      WinFillRect( hps,
                   &rclWin,                // exclusive
                   CLR_BLUE);//pPrivate->Setup.lcolBackground);

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
 *
 *@@changed V0.9.13 (2001-06-21) [umoeller]: changed XCM_SAVESETUP call for tray support
 */

VOID WgtPresParamChanged(HWND hwnd,
                         ULONG ulAttrChanged,
                         PXCENTERWIDGET pWidget)
{
  PIRPRIVATE pPrivate = (PIRPRIVATE)pWidget->pUser;
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
      WgtSaveSetup(&strSetup,
                   &pPrivate->Setup);
      if (strSetup.ulLength)
        // we have a setup string:
        // tell the XCenter to save it with the XCenter data
        // changed V0.9.13 (2001-06-21) [umoeller]:
        // post it to parent instead of fixed XCenter client
        // to make this trayable
        WinSendMsg(WinQueryWindow(hwnd, QW_PARENT), // pPrivate->pWidget->pGlobals->hwndClient,
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

VOID WgtDestroy(HWND hwnd,
                PXCENTERWIDGET pWidget)
{
  PIRPRIVATE pPrivate = (PIRPRIVATE)pWidget->pUser;
  if (pPrivate)
  {
    WgtClearSetup(&pPrivate->Setup);
    IrCOMMExitFnc();
    free(pPrivate);
            // pWidget is cleaned up by DestroyWidgets
  }
}

/*
 *@@ fnwpSampleWidget:
 *      window procedure for the winlist widget class.
 *
 *      There are a few rules which widget window procs
 *      must follow. See XCENTERWIDGETCLASS in center.h
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
     *      IRPRIVATE for our own stuff.
     */

    case WM_CREATE:
      WinSetWindowPtr(hwnd, QWL_USER, mp1);
      pWidget = (PXCENTERWIDGET)mp1;
      if ((pWidget) && (pWidget->pfnwpDefWidgetProc))
      {
        mrc = WgtCreate(hwnd, pWidget);
      }
      else
      {
          // stop window creation!!
          mrc = (MPARAM)TRUE;
      }
    break;

    /*
     * WM_CONTROL:
     *      process notifications/queries from the XCenter.
     */

    case WM_CONTROL:
      mrc = (MPARAM)WgtControl(hwnd, mp1, mp2);
    break;

    /*
     * WM_PAINT:
     *
     */

    case WM_PAINT:
      WgtPaint(hwnd, pWidget);
    break;

    /*
     * WM_PRESPARAMCHANGED:
     *
     */

    case WM_PRESPARAMCHANGED:
      if (pWidget)
          // this gets sent before this is set!
          WgtPresParamChanged(hwnd, (ULONG)mp1, pWidget);
    break;

    case WM_TIMER:
    {
      PIRPRIVATE pPrivate = (PIRPRIVATE)pWidget->pUser;
      if ((int)mp1 == TIMER_DOIRDA)
      {
        BOOL oldStatus = pPrivate->pStatus->connected;
        _Pmpf((" Timer: DoIrda"));
        // discover new devices
        SendDiscovery();
        pPrivate->fPlayAnimation = TRUE;
        // ask for the IrCOMM status
        GetStatus(pPrivate->pStatus);
        if (oldStatus != pPrivate->pStatus->connected)
        {
          _Pmpf((" Timer: Status changed"));
          // repaint window to indicate status change
          WinInvalidateRect(hwnd, NULL, TRUE);
          if (pPrivate->pStatus->connected == TRUE)
          {
            Connected();
          }
          else
          {
            Disconnected();
          }
        }
        else
        {
          _Pmpf((" Timer: Status no changed"));
          // if not connected and we got an LSAP
          if ((pPrivate->pStatus->connected == FALSE) &&
              (pPrivate->pStatus->remoteLSAP != 0))
          {
            _Pmpf((" Timer: Send DOCONNECT"));
            // confirm that we want to connect
            WinPostMsg(hwnd, WM_DOCONNECT, NULL, NULL);
          }
        }
      }
      else
      if ((int)mp1 == TIMER_ANIM)
      {
        if (pPrivate->fDriverInstalled &&
            pPrivate->fPlayAnimation)
        {
          _Pmpf((" Timer: ANIM"));
          pPrivate->ulFrame++;
          if(pPrivate->ulFrame==11)
          {
            pPrivate->fPlayAnimation = FALSE;
            pPrivate->ulFrame=0;
            _Pmpf((" Timer: ANIM ended"));
          }
          else
            WinInvalidateRect(hwnd, NULL, TRUE);
        }
      }
      return (MRESULT)TRUE;
    }
    case WM_DOCONNECT:
    {
      _Pmpf((" WM_DOCONNECT"));
      // confirm that we want to connect
      SendDoConnect();
      break;
    }

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
  char szError[256];
  ULONG   ulrc = 0;
  ULONG   ul = 0;
  BOOL    fImportsFailed = FALSE;
  g_hResDLL = hmodPlugin;
  g_hab = hab;
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

  if(!DosLoadModule( szError, sizeof(szError), "rxirda.dll", &g_hRexxDLL))
  {
    pInitRexx = NULL;
    pUninitRexx = NULL;
    DosQueryProcAddr(g_hRexxDLL,0,"InitRxIrDA", (PFN*)&pInitRexx);
    DosQueryProcAddr(g_hRexxDLL,0,"UninitRxIrDA", (PFN*)&pUninitRexx);
  }
  else
  {
    _Pmpf((" Rexx DLL Load failed with r%s",szError));
  }

  if(!pInitRexx || !pUninitRexx)
  {
    _Pmpf((" Rexx DLL Load Functions failed InitRxIrDA %d UninitRxIrDA %d",pInitRexx, pUninitRexx));
    pInitRexx = NULL;
    pUninitRexx = NULL;
    DosFreeModule(g_hRexxDLL);
    g_hRexxDLL = NULL;
  }

  if (!fImportsFailed)
  {
    // all imports OK:
    // register our PM window class
    if (!WinRegisterClass(hab,
                          WNDCLASS_WIDGET_IRMON,
                          fnwpSampleWidget,
                          CS_PARENTCLIP | CS_SIZEREDRAW | CS_SYNCPAINT,
                          sizeof(PIRPRIVATE))
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

VOID EXPENTRY WgtUnInitModule(VOID)
{
    _Pmpf((" WgtUnInitModule"));
}

/*
 *@@ WgtQueryVersion:
 *      this new export with ordinal 3 can return the
 *      XWorkplace version number which is required
 *      for this widget to run. For example, if this
 *      returns 0.9.10, this widget will not run on
 *      earlier XWorkplace versions.
 *
 *      NOTE: This export was mainly added because the
 *      prototype for the "Init" export was changed
 *      with V0.9.9. If this returns 0.9.9, it is
 *      assumed that the INIT export understands
 *      the new FNWGTINITMODULE_099 format (see center.h).
 *
 *@@added V0.9.9 (2001-02-06) [umoeller]
 */

VOID EXPENTRY WgtQueryVersion(PULONG pulMajor,
                              PULONG pulMinor,
                              PULONG pulRevision)
{
    // report 0.9.9
    *pulMajor = 0;
    *pulMinor = 9;
    *pulRevision = 9;
}

