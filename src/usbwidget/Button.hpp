/*****************************************************************************\
* Button.hpp -- NeoWPS * USB Widget                                           *
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
 * This module handles Buttons.
 */

/* Always declare the class(es) to resolve class dependencies */
class   Button;

#ifndef     __BUTTON_HPP__
#define     __BUTTON_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

//~ #include    "Master.hpp"

#include    "Window.hpp"

class   Button : public Window {

    public:

    /* Public Constructors and Destructors */
    Button();
    Button(unsigned parent, unsigned id);
    virtual ~Button();

    protected:

    private:

};

#ifdef      __cplusplus
    }
#endif

#endif // __Button_HPP__
