
/*
 *@@sourcefile w_rscrlr.h:
 *      private header file for the REXX scroller widget.
 *      See src\shared\center.c for an introduction to
 *      the XCenter and widgets.
 *
 */

/*
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

#ifndef RSCRLR_HEADER_INCLUDED
    #define RSCRLR_HEADER_INCLUDED

    #ifndef CENTER_HEADER_INCLUDED
        #error shared\center.h must be included before rexx\w_rscrlr.h.
    #endif

    VOID EXPENTRY RwgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData);

    BOOL RwgtTimer(HWND hwnd, PXCENTERWIDGET pWidget);

    PFNEXCHOOKERROR G_pfnExcHookError;

    typedef ULONG _System EXCHANDLERLOUD(PEXCEPTIONREPORTRECORD,
                                         PEXCEPTIONREGISTRATIONRECORD2,
                                         PCONTEXTRECORD,
                                         PVOID);
    typedef EXCHANDLERLOUD *PEXCHANDLERLOUD;

    #define CCHMAXTITLE    250
                // max title length, not counting the null char
    #define CCHMAXSCRIPT  5000
                // max script length, not counting the null char

    // this module handle
    HMODULE G_hmodThis = NULLHANDLE;

    // this module name
    CHAR G_szThis[CCHMAXPATH] = {0};

    // this anchor block handle
    HAB G_habThis = NULLHANDLE;

    // current window a message box relates to
    HWND G_hwnd = NULLHANDLE;

    RXSYSEXIT G_exit_list[3];
                // the REXX exit list is constant across the session and is
                // used for the two scripts, so it has been made global in
                // order to save some CPU time.  It is initialized in
                // RwgtInitModule.

    // NLS strings
    PSZ pszName,
        pszSettingsStatus,
        pszInterpreterErrorDblClk,
        pszInterpreterErrorTimer,
        pszAlreadyRunning;

    // REXX scroller settings dialog ids
    #define ID_CRD_RSCRLR_SETTINGS         1000
    #define ID_CRDI_RSCRLR_SCRIPT          1001
    #define ID_CRDI_RSCRLR_DBLCLK          1002
    #define ID_CRDI_RSCRLR_REFRESH         1003
    #define ID_CRDI_RSCRLR_TITLE           1004
    #define ID_CRDI_RSCRLR_COLOR1          1005
    #define ID_CRDI_RSCRLR_COLOR2          1006
    #define ID_CRDI_RSCRLR_COLOR3          1007
    #define ID_CRDI_RSCRLR_RESIZEABLE      1008
    #define ID_CRDI_RSCRLR_FIXEDWIDTH      1009
    #define ID_CRDI_RSCRLR_WIDTH           1010
    #define DID_APPLY                      1011
    #define DID_RESET                      1012
    #define ID_CRDI_RSCRLR_STATUS          1013
    #define ID_CRDI_RSCRLR_STATUS2         1014
    #define ID_CRDI_RSCRLR_FILLGRAPH       1015
    #define ID_CRDI_RSCRLR_LINEGRAPH       1016
    #define ID_CRDI_RSCRLR_AVERAGE         1017
    #define ID_CRDI_RSCRLR_BLMODE          1018

    #define ID_CRH_RSCRLR_SETTINGS         1000
    #define ID_CRH_RSCRLR_MAIN             1001
    #define ID_CRH_RSCRLR_DBLCLKERROR1     1002
    #define ID_CRH_RSCRLR_DBLCLKERROR2     1003
    #define ID_CRH_RSCRLR_TIMERERROR       1004

    #define ID_CRSI_NAME                   1000
    #define ID_CRSI_INTERPRETER_DBLCLK     1001
    #define ID_CRSI_INTERPRETER_TIMER      1002
    #define ID_CRSI_ALREADYRUNNING         1006
    #define ID_CRSI_SETTINGSSTATUS         1007

    #define ID_BGBMP                       1019

    #ifndef DID_HELP
        #define DID_HELP               97
    #endif

#endif

