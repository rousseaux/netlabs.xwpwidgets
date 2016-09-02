/*****************************************************************************\
* Debug.hpp -- NeoWPS * USB Widget                                            *
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
 * This is the public header for it's corresponding source.
 */


#ifndef     __DEBUG_HPP__
#define     __DEBUG_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif


#include    "ModuleGlobals.hpp"

/* Prototypes */
void    CreateDebugDialog();
void    DestroyDebugDialog();
void    __debug(char* title, char* message, unsigned long flags);

#ifdef      __cplusplus
    }
#endif

#endif // __DEBUG_HPP__
