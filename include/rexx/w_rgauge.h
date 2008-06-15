
/*
 *@@sourcefile w_rgauge.h:
 *      private header file for the REXX gauge widget.
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

#ifndef RGAUGE_HEADER_INCLUDED
    #define RGAUGE_HEADER_INCLUDED

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

    // REXX gauge settings dialog ids
    #define ID_CRD_RGAUGE_SETTINGS         1000
    #define ID_CRDI_RGAUGE_SCRIPT          1001
    #define ID_CRDI_RGAUGE_DBLCLK          1002
    #define ID_CRDI_RGAUGE_REFRESH         1003
    #define ID_CRDI_RGAUGE_TITLE           1004
    #define ID_CRDI_RGAUGE_COLOR1          1005
    #define ID_CRDI_RGAUGE_COLOR2          1006
    #define ID_CRDI_RGAUGE_COLOR3          1007
    #define ID_CRDI_RGAUGE_RESIZEABLE      1008
    #define ID_CRDI_RGAUGE_FIXEDWIDTH      1009
    #define ID_CRDI_RGAUGE_WIDTH           1010
    #define DID_APPLY                      1011
    #define DID_RESET                      1012
    #define ID_CRDI_RGAUGE_STATUS          1013
    #define ID_CRDI_RGAUGE_STATUS2         1014

    #define ID_CRH_RGAUGE_SETTINGS         1000
    #define ID_CRH_RGAUGE_MAIN             1001
    #define ID_CRH_RGAUGE_DBLCLKERROR1     1002
    #define ID_CRH_RGAUGE_DBLCLKERROR2     1003
    #define ID_CRH_RGAUGE_TIMERERROR       1004

    #define ID_CRSI_NAME                   1000
    #define ID_CRSI_INTERPRETER_DBLCLK     1001
    #define ID_CRSI_INTERPRETER_TIMER      1002
    #define ID_CRSI_ALREADYRUNNING         1006
    #define ID_CRSI_SETTINGSSTATUS         1007

    #ifndef DID_HELP
        #define DID_HELP               97
    #endif

#endif

