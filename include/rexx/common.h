
/*
 *@@sourcefile common.h:
 *      private header file for the REXX widgets.
 *      See src\shared\center.c for an introduction to
 *      the XCenter and widgets.
 *
 *      This include file must be used in conjunction with common.c.
 *      (I.e., if you include common.h, you must also include common.c,
 *      as this defines common code, but not a separate module).
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

#ifndef RCOMMON_HEADER_INCLUDED
    #define RCOMMON_HEADER_INCLUDED

    #define USERDATASIZE 100

    PSZ RwgtEncodeString(PSZ pszSource);
    PSZ RwgtDecodeString(PSZ pszEncodedSource);
    VOID RwgtFree(PSZ psz);

    const char *RwgtQueryHelpLibrary(VOID);

    // For REXX widgets, ALL RxxxSETUP structures MUST start with the
    // following three fields (i.e., the background color, foreground
    // color, and the font name).  The pszIconFile and hIcon fields are
    // optional.

    typedef struct _RSETUP
    {
        LONG        lcolBackground,         // background color
                    lcolForeground;         // foreground color (for text)

        PSZ         pszFont;
                // if != NULL, non-default font (in "8.Helv" format);
                // this has been allocated using local malloc()!

        #ifndef INCL_NOICON
        PSZ         pszIconFile;
        HPOINTER    hIcon;
        #endif
    } RSETUP, *PRSETUP;

    // For REXX widgets, ALL RxxxPRIVATE structures MUST start with
    // the following two pointers (i.e., a pointer to the parent and
    // a RxxxSETUP structure).

    typedef struct _RPRIVATE
    {
        PXCENTERWIDGET pWidget;
        RSETUP         Setup;
    } RPRIVATE, *PRPRIVATE;

    /*
     *@@ STORAGE:
     *      structure used in the pUser field in WIDGETSETTINGSDLGDATA
     *
     */
    typedef struct _STORAGE
    {
        PVOID pSetup;
        PSZ   pszInitialSetup;
        PSZ   pszLastCommitted;
        ULONG ulCurrent;
        ULONG ulTotal;
        ULONG ulColumn;
    } STORAGE, *PSTORAGE;

    VOID RwgtPresParamChanged(HWND hwnd,
                              ULONG ulAttrChanged,
                              PXCENTERWIDGET pWidget);

    #ifndef INCL_NOICON
    VOID DrawIconTargetEmphasis(HWND hwnd,
                                HPS hps,
                                BOOL fStatus);
    MRESULT EXPENTRY fnwpIconFile(HWND hwnd,
                                  ULONG msg,
                                  MPARAM mp1,
                                  MPARAM mp2);
    #endif

    char G_szLibraryName[CCHMAXPATH];

    #define LOADSTRING(id, str) if (!WinLoadString(hab, \
                                                   G_hmodThis, \
                                                   id, \
                                                   sizeof(szBuf), \
                                                   szBuf)) \
                                    sprintf(szBuf, "RwgtInitModule error: string resource %d not found in module %s", id, G_szThis); \
                                str = strdup(szBuf)

#endif
