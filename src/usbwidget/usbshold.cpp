/*****************************************************************************\
* usbshold.cpp -- NeoWPS * USB Widget                                         *
*                                                                             *
* Copyright (c) 2014 RDP Engineering                                          *
*                                                                             *
* Author: Ben Rietbroek <rousseau.ecsdev@gmail.com>                           *
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


/**
 * This module is kind of a proxy in the sense that it does not contain much.
 * However, it's .def file is used to redirect the exported entries that
 * eCenter calls to create and initialize the Widget.
 *
 * The .def file is used to redirect these calls to either the old
 * implementation or the new implementation.
 *
 * The true code starts in USBWidget.c for the old implementation and in
 * USBWidget.cpp for the new implementation.
 */


#include    "usbshold.hpp"
//~ #include    "usbshold.hpv"
#include    "USBWidget.hpp"

/*
// This includes the BLDLEVEL information.
// It defines a it defines a global character string so it is a source file
// and not an include file. The character string holds the BLDLEVEL information
// and is thus embedded in the resulting object file.
*/
#include    "usbshold.lvl"




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


