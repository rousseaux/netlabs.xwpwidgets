
/*
 *@@sourcefile w_rbutton.h:
 *      private header file for the REXX button widget.
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

#ifndef RBUTTON_HEADER_INCLUDED
    #define RBUTTON_HEADER_INCLUDED
    
    #ifndef CENTER_HEADER_INCLUDED
        #error shared\center.h must be included before widgets\w_rbutton.h.
    #endif
    
    VOID EXPENTRY RwgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData);
    
    PFNWP G_pfnwpOldIconFile;
    
    typedef BOOL THRCREATE(PTHREADINFO, PTHREADFUNC, PBOOL, ULONG, ULONG);
    typedef THRCREATE *PTHRCREATE;
    
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

    #define CANDROP_ONEFILE   1
    #define CANDROP_FILES     2
    #define CANDROP_ONEOBJECT 3
    #define CANDROP_OBJECTS   4
                // drop operations allowed

    // this module handle
    HMODULE G_hmodThis = NULLHANDLE;
    
    // this module name
    CHAR G_szThis[CCHMAXPATH] = {0};
    
    // this anchor block handle
    HAB G_habThis = NULLHANDLE;
    
    // current window a message box relates to
    HWND G_hwnd = NULLHANDLE;
    
    // NLS strings
    PSZ pszName,
        pszNoDrop,
        pszDropFile,
        pszDropFiles,
        pszDropObject,
        pszDropObjects,
        pszAlreadyRunning,
        pszThreadingRequired,
        pszScriptError;

    // Declare C runtime prototypes, because there are no headers
    // for these:
    
    // _rmem_init is the subsystem run-time environment initialization function.
    // It will return 0 to indicate success and -1 to indicate failure.
    int _rmem_init(void);
    
    // _rmem_term is the subsystem run-time environment termination function.
    // It only needs to be called when the C run-time functions are statically
    // linked, as is the case with XFolder.
    void _rmem_term(void);
    
    // REXX button settings dialog ids
    #define ID_CRD_RBUTTONWGT_SETTINGS      1000
    #define ID_CRDI_SCRIPT_TEXT             1001
    #define ID_CRDI_SCRIPT_SEPARATETHREAD   1002
    #define ID_CRDI_SCRIPT_TITLE            1003
    #define ID_CRDI_SCRIPT_ICONFILE         1004
    #define ID_CRDI_SCRIPT_ICON             1005
    #define ID_CRDI_SCRIPT_CANDROP          1006
    
    // REXX button resources ids
    #define ID_RBUTTONICON                  1000
    
    #define ID_CRH_RBUTTON_SETTINGS         1000
    #define ID_CRH_RBUTTON_MAIN             1001
    #define ID_CRH_RBUTTON_SCRIPTERROR      1002
    
    #define ID_CRSI_NAME                    1000
    #define ID_CRSI_NODROP                  1001
    #define ID_CRSI_DROPFILE                1002
    #define ID_CRSI_DROPFILES               1003
    #define ID_CRSI_DROPOBJECT              1004
    #define ID_CRSI_DROPOBJECTS             1005
    #define ID_CRSI_ALREADYRUNNING          1006
    #define ID_CRSI_THREADINGREQUIRED       1007
    #define ID_CRSI_SCRIPTERROR             1008
    
    #ifndef DID_HELP
        #define DID_HELP               97
    #endif

#endif

