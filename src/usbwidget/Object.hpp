/*****************************************************************************\
* Object.hpp -- NeoWPS * USB Widget                                           *
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

/* Always declare the class(es) to resolve class dependencies */
class   Object;

#ifndef     __OBJECT_HPP__
#define     __OBJECT_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"

/*
// This is the top-level class of the whole shebang.
// It has some stuff common to all classes, like a debug-flag.
*/
class   Object {

    public:
    Object();
    virtual ~Object();
    virtual bool    debugMe();              // Query debug status
    virtual bool    debugMe(bool flag);     // Set debug status
    virtual int     debugLevel();           // Query debug level
    virtual int     debugLevel(int level);  // Set debug level

    protected:

    private:
    bool    debug;      // TRUE when debugging is active, FALSE when not
    int     level;      // Indicates debug level -- 0 is least detailed

};

#ifdef      __cplusplus
    }
#endif

#endif // __OBJECT_HPP__
