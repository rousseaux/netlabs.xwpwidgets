/*****************************************************************************\
* Bldlevel.h -- OS/2 BLDLEVEL Information (Date/Time)                         *
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


/**
 * This public header contains the generic BLDLEVEL information,
 * (Vendor, Date/Time, Build Machine and Language), that both the Widget and
 * the Daemon use. Module specific parts are in *.lvl files.
 */

/* Include the generated build variables */
#include    "Bldvars.h"

/* Vendor (publisher) of the software */
#define     BLDLVL_VENDOR           VENDOR_STR

/* WarpIN-like version number */
#define     BLDLVL_MAJOR_VERSION    VERSION_MAJOR_STR
#define     BLDLVL_MIDDLE_VERSION   VERSION_MIDDLE_STR
#define     BLDLVL_MINOR_VERSION    VERSION_MINOR_STR

/* Date and Time of build */
#define     BLDLVL_YEAR             BLD_YEAR_STR
#define     BLDLVL_MONTH            BLD_MONTH_STR
#define     BLDLVL_DAY              BLD_DAY_STR
#define     BLDLVL_HOURS            BLD_HOURS_STR
#define     BLDLVL_MINUTES          BLD_MINUTES_STR
#define     BLDLVL_SECONDS          BLD_SECONDS_STR

/* Machine that was used to build the software */
#define     BLDLVL_MACHINE          BLD_MACHINE_STR

/* Language(s) this software supports */
#define     BLDLVL_LANGUAGE         BLD_LANGUAGE_STR
