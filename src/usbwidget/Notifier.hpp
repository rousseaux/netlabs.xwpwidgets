/*****************************************************************************\
* Notifier.hpp -- NeoWPS * USB Widget                                         *
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
 * This module is a dummy to invoke the .obj.lib inference rule.
 */

/* Always declare the class(es) to resolve class dependencies */
class   Notifier;

#ifndef     __NOTIFIER_HPP__
#define     __NOTIFIER_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"

#include    "Window.hpp"

class   Notifier : public Object {

    public:
    Notifier();
    virtual ~Notifier();
    virtual ulong   show();
    virtual ulong   hide();

    protected:

    private:
    Window* myNotifierWindow;
};

#ifdef      __cplusplus
    }
#endif

#endif // __NOTIFIER_HPP__
