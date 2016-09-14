/*****************************************************************************\
* Monitor.cpp -- NeoWPS * USB Widget                                          *
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
 * This module contains the base monitor class.
 */

//~ #include    "GUIHelpers.hpp"

#include    "Monitor.hpp"


Monitor::Monitor() {
}

Monitor::~Monitor() {
}

ulong   Monitor::start() {
    return 0;
}

ulong   Monitor::stop() {
    return 0;
}

ulong   Monitor::block() {
    return 0;
}

ulong   Monitor::unblock() {
    return 0;
}

ulong   Monitor::suspend() {
    return 0;
}

ulong   Monitor::resume() {
    return 0;
}


ulong   Monitor::setStatus() {
    return 0;
}

ulong   Monitor::captureStatus() {
    return 0;
}

ulong   Monitor::getStatus() {
    return 0;
}


ulong   Monitor::eventStatusChanged() {
    return 0;
}
