/*****************************************************************************\
* usbshold.cpp -- NeoWPS * USB Widget                                         *
*                                                                             *
* Copyright (c) RDP Engineering                                               *
*                                                                             *
* Author: Ben Rietbroek <rousseau.os2dev@gmx.com>                             *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,                *
*   MA 02110-1301, USA.                                                       *
*                                                                             *
\*****************************************************************************/

/******************************************************************************
* This is the main module for the Widget DLL
* -----------------------------------------------------------------------------
* It holds the entrypoints that XCenter uses to query, initialize and
* uninitialize a widget. These entrypoints are invoked by ordinal with the
* following mappings:
*
* 1. Initialize Widget
* 2. UnInitialize Widget
* 3. Query required XWorkplace version
*
*/

#include    "GUIHelpers.hpp"

#include    "UsbWidget.hpp"

/* Implementation Header */
#include    "usbsimpl.hpp"

/* Module Header */
#include    "usbshold.hpp"

/* BLDLEVEL Information */
#include    "usbshold.lvl"


Widget*     ThisWidget  = NULL;




unsigned long _System   _DLL_InitTerm(unsigned long hModule, unsigned long ulFlag) {


    switch (ulFlag) {
        case 0: {
            int     rc = -88;
            char    buf[256];
            rc = _CRT_init();
            __ctordtorInit();
            sprintf(buf, "ATTACH: _CRT_init: %d", rc);
            //! Normal MessageBox definitely breaks messaging !
            //~ MessageBox("DLL_INIT", buf);
            return 1;
            break;
        }
        case 1: {
            //~ MessageBox("DLL_INIT", "DETACH");
            //~ MessageBox2("DLL_INIT", "DETACH");
            __ctordtorTerm();
            _CRT_term();
            return 1;
            break;
        }
        default: {
            //~ MessageBox2("DLL_INIT", "**UNKNOWN**");
            return 0;
            break;
        }
    }

    return 0;
}


/*****************************************************************************\
 *
 *@@UsbWgtInitModule()
 *
 * This is the entry-point that is exported at ordinal @1 and that gets called
 * by the eCenter to initiate the module initialization. It forwards to a
 * function in another module that does the actual work.
 *
 */
ULONG EXPENTRY  UsbWgtInitModule(   HAB hab,                // XCenter's anchor block
                                    HMODULE hmodPlugin,     // module handle of the widget DLL
                                    HMODULE hmodXFLDR,      // XFLDR.DLL module handle
                                    PXCENTERWIDGETCLASS *ppaClasses,
                                    PSZ pszErrorMsg ){

    g_use_new_implementation = true;

    //~ if (g_use_new_implementation) {
        //~ ThisWidget = new UsbWidget();
        //~ ThisWidget->test123();
        //~ delete ThisWidget;
        //~ ThisWidget = NULL;
    //~ }

    /* Call the function that does the actual module initialization */
    return  WgtInitModule(
                hab,
                hmodPlugin,
                hmodXFLDR,
                ppaClasses,
                pszErrorMsg
            );
}


/*****************************************************************************\
 *
 *@@UsbWgtQueryVersion()
 *
 * This is the entry-point that is exported at ordinal @2 and that gets called
 * by the eCenter to investigate the version. It forwards to a function in
 * another module that does the actual work. Note that this is actually the
 * first function called by the eCenter, thus before UsbWgtInitModule().
 *
 */
VOID EXPENTRY   UsbWgtQueryVersion( PULONG pulMajor,
                                    PULONG pulMinor,
                                    PULONG pulRevision ){

    /* Call the function that does the actual function quering */
    WgtQueryVersion(
        pulMajor,
        pulMinor,
        pulRevision
    );
}


/*****************************************************************************\
 *
 *@@UsbWgtUnInitModule()
 *
 * This is the entry-point that is exported at ordinal @3 and that gets called
 * by the eCenter to initiate module uninitialization. It forwards to a
 * function in another module that does the actual work.
 *
 */
VOID EXPENTRY   UsbWgtUnInitModule() {

    /* Call the function that does the actual module uninitialization */
    WgtUnInitModule();
}


//~ int     DummyExport() {
    //~ return 0;
//~ }


