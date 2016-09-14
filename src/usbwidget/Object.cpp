/*****************************************************************************\
* Object.cpp -- NeoWPS * USB Widget                                           *
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
 * This module holds the top-level Object.
 */

//~ #include    "GUIHelpers.hpp"

#include    "Object.hpp"

/*
// This is where all other classes descend from.
*/
Root::Root() {}
Root::~Root() {}

bool    Root::debugMe() {
    return this->debug;
}

bool    Root::debugMe(bool flag) {
    this->debug = flag;
    return this->debug;
}

int     Root::debugLevel() {
    return this->level;

}
int     Root::debugLevel(int level) {
    this->level = level;
    return this->level;
}
