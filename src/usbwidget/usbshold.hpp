/*****************************************************************************\
* usbshold.hpp -- NeoWPS * USB Widget                                         *
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

#ifndef     __USBSHOLD_HPP__
#define     __USBSHOLD_HPP__
#ifdef      __cplusplus
extern "C" {
#endif

/* Includes */
#include    "ModuleGlobals.hpp"
//#include  "APIHelpers.hpp"


/* Prototypes */


/* Module Initialization */
ULONG EXPENTRY  UsbWgtInitModule(   HAB hab,
                                    HMODULE hmodPlugin,
                                    HMODULE hmodXFLDR,
                                    PXCENTERWIDGETCLASS *ppaClasses,
                                    PSZ pszErrorMsg );

/* Module Version Querying */
VOID EXPENTRY   UsbWgtQueryVersion( PULONG pulMajor,
                                    PULONG pulMinor,
                                    PULONG pulRevision );

/* Module Uninitialization */
VOID EXPENTRY   UsbWgtUnInitModule();


//~ #pragma export(DummyExport, "DummyExportAlias", 7)
//~ #pragma export(DummyExport,,9)
//~ int DummyExport();


#ifdef      __cplusplus
    }
#endif
// __USBSHOLD_HPP__
#endif
