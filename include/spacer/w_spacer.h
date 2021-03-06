
/*
 *@@sourcefile w_spacer.h:
 *      private header file for the spacer widget.
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

#ifndef SPACER_HEADER_INCLUDED
    #define SPACER_HEADER_INCLUDED

    VOID EXPENTRY SwgtShowSettingsDlg(PWIDGETSETTINGSDLGDATA pData);

    // this module handle
    HMODULE G_hmodThis = NULLHANDLE;

    // this module name
    CHAR G_szThis[CCHMAXPATH] = {0};

    // Spacer widget styles
    #define SWS_GREEDYX 0x1
    #define SWS_GREEDYY 0x2

    // NLS strings
    PSZ pszName;

    // Spacer settings dialog ids
    #define ID_CRD_SPACER_SETTINGS         1000
    #define ID_CRDI_SPACER_GREEDYX         1001
    #define ID_CRDI_SPACER_GREEDYY         1002
    #define ID_CRDI_SPACER_USERX           1003
    #define ID_CRDI_SPACER_USERY           1004
    #define ID_CRDI_SPACER_WIDTH           1005
    #define ID_CRDI_SPACER_HEIGHT          1006
    #define ID_CRDI_SPACER_TRANSPARENT     1007
    #define DID_APPLY                      1011
    #define DID_RESET                      1012

    #define ID_XSH_WIDGET_SPACER_SETTINGS  1000
    #define ID_XSH_WIDGET_SPACER_MAIN      1001

    #define ID_CRSI_NAME                   1000

    #ifndef DID_HELP
        #define DID_HELP                     97
    #endif

#endif

