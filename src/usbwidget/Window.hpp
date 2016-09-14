/*****************************************************************************\
* Window.hpp -- NeoWPS * USB Widget                                           *
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

// From WidgetSettings.hpp

/**
 * This module handles Windows.
 */


#ifndef     __WINDOW_HPP__
#define     __WINDOW_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"

#include    "Object.hpp"

class   Window : public Object {
    public:
    Window();
    virtual ~Window();
    virtual int     show();
    virtual void    center();
    virtual void    centerToDesktop();
    virtual void    centerToOther(ulong toCenterTo);
    virtual int     hide();
    virtual ulong   getHandle();
    protected:
    ulong   handle;
    char    buf[256];
    private:
};

#ifdef      __cplusplus
    }
#endif

#endif // __WINDOW_HPP__
