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


// Vendor
//~ #define     BLDLVL_VENDOR           "RDPe"
#define     BLDLVL_VENDOR           "*UNKNOWN*"
// Version
#define     BLDLVL_MAJOR_VERSION    "2"
#define     BLDLVL_MIDDLE_VERSION   "2"
#define     BLDLVL_MINOR_VERSION    "3"
// Build date
#define     BLDLVL_YEAR             "2016"
#define     BLDLVL_MONTH            "09"
#define     BLDLVL_DAY              "04"
// Build time
//~ #define     BLDLVL_HOURS            "02"
//~ #define     BLDLVL_MINUTES          "20"
//~ #define     BLDLVL_SECONDS          "03"
#define     BLDLVL_HOURS            "99"
#define     BLDLVL_MINUTES          "99"
#define     BLDLVL_SECONDS          "99"
// Build machine
//~ #define     BLDLVL_MACHINE          "ECS-DEVBOX"
#define     BLDLVL_MACHINE          "*UNKNOWN*"
// Build language
#define     BLDLVL_LANGUAGE         "ML"
