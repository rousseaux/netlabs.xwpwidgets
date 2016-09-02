/*****************************************************************************\
* usbshlpr.hpp -- Support DAEMON for the NeoWPS * USB Widget                  *
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
 * This is the public header file for it's corresponding source.
 * It includes other public header files so it can use the stuff declared
 * there.
 */


#ifndef     __USBSHLPR_HPP__
#define     __USBSHLPR_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    <conio.h>
#include    "ModuleGlobals.hpp"


/* Prototypes */
int     startd();
int     stopd();


#ifdef      __cplusplus
    }
#endif

#endif // __USBSHLPR_HPP__
