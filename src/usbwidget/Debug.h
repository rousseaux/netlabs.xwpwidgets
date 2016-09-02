/*****************************************************************************\
* Debug.h -- NeoWPS * USB Widget                                              *
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
 * This header defines the Debug Flags.
 */

/* Flags for __debug() */
#define     DBG_ON          0x00000001
#define     DBG_STDOUT      0x00000002
#define     DBG_STDERR      0x00000004
#define     DBG_POPUP       0x00000008
#define     DBG_LBOX        0x00000010
#define     DBG_MLE         0x00000020
#define     DBG_PIPE        0x00000040
#define     DBG_FILE        0x00000080
#define     DBG_AUX         0x00000100
#define     DBG_USE_POST    0x80000000
#define     DBG_USE_SEND    0x7FFFFFFF  // Use & to clear

/* Global Debug Settings */
#define     DEBUG           DBG_ON|DBG_LBOX
//~ #define     DEBUG           0
#define     AUX_DEBUG       DBG_ON
//~ #define     AUX_DEBUG       0

