/*****************************************************************************\
* DebugDialog.hpp -- NeoWPS * USB Widget                                      *
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
 * This module contains the DebugDialog.
 */

/* Always declare the class(es) to resolve class dependencies */
class   debugDialog;

#ifndef     __DEBUGDIALOG_HPP__
#define     __DEBUGDIALOG_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"

#include    "Dialog.hpp"

class   DebugDialog : public Dialog {

    private:

    public:
    DebugDialog();
    virtual ~DebugDialog();

    virtual ulong   create();
    virtual ulong   destroy();
    virtual ulong   redraw();
    virtual ulong   msgInitDialog(ulong mp1, ulong mp2);
    virtual ulong   msgCommand(ulong mp1, ulong mp2);
    virtual ulong   commandDrawButton();
    virtual ulong   commandDestroyButton();

    //~ static ulong EXPENTRY DebugDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    static ulong DebugDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
};

#ifdef      __cplusplus
    }
#endif

#endif // __DEBUGDIALOG_HPP__
