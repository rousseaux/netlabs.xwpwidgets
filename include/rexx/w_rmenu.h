
/*
 *@@sourcefile w_rmenu.h:
 *      private header file for the REXX menu widget.
 *      See src\shared\center.c for an introduction to
 *      the XCenter and widgets.
 *
 */

/*
 *      Copyright (C) 2001 Martin Lafaix.
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

#ifndef RMENU_HEADER_INCLUDED
    #define RMENU_HEADER_INCLUDED

    VOID EXPENTRY RwgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData);

    PFNWP G_pfnwpOldIconFile;

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
        pszAlreadyRunning,
        pszThreadingRequired,
        pszScriptError,
        pszSettingsStatus,
        pszThreadCreationFailed;

    // REXX button settings dialog ids
    #define ID_CRD_RMENUWGT_SETTINGS      1000
    #define ID_CRDI_RMENU_MENU            1001
    #define ID_CRDI_RMENU_SEPARATETHREAD  1002
    #define ID_CRDI_RMENU_TITLE           1003
    #define ID_CRDI_RMENU_ICONFILE        1004
    #define ID_CRDI_RMENU_ICON            1005
    #define ID_CRDI_SCRIPT_ICON           ID_CRDI_RMENU_ICON
    #define ID_CRDI_RMENU_TEXT            1006
    #define DID_APPLY                     1007
    #define DID_RESET                     1008
    #define ID_CRDI_RMENU_STATUS          1009
    #define ID_CRDI_RMENU_STATUS2         1010

    // REXX menu resources ids
    #define ID_RMENUICON                  1000

    #define ID_CRH_RMENU_SETTINGS         1000
    #define ID_CRH_RMENU_MAIN             1001
    #define ID_CRH_RMENU_SCRIPTERROR      1002
    #define ID_CRH_RMENU_ALREADYRUNNING   1003
    #define ID_CRH_RMENU_THREADCREATION   1004

    #define ID_CRSI_NAME                  1000
    #define ID_CRSI_ALREADYRUNNING        1006
    #define ID_CRSI_THREADINGREQUIRED     1007
    #define ID_CRSI_SCRIPTERROR           1008
    #define ID_CRSI_THREADCREATIONFAILED  1009
    #define ID_CRSI_SETTINGSSTATUS        1010

    #ifndef DID_HELP
        #define DID_HELP                  97
    #endif

#endif

