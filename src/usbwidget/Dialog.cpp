/*****************************************************************************\
* Dialog.cpp -- NeoWPS * USB Widget                                           *
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
 * This module handles Dialogs.
 */

#include    "Debug.hpp"
#include    "GUIHelpers.hpp"

#include    "Dialog.hpp"


Dialog::Dialog() {
    this->debugMe();
    sprintf(this->buf, "Dialog() : hdlg=%08X", this->hwndSelf);
    __debug(NULL, this->buf, DBG_LBOX);
    sprintf(this->buf, "Dialog(): this=%08X", this);
    MessageBox("DIALOG", this->buf);
}

Dialog::~Dialog() {
    sprintf(this->buf, "~Dialog() : hdlg=%08X", this->hwndSelf);
    __debug(NULL, this->buf, DBG_LBOX);
}

/* Default WM_COMMAND implementation for Dialog */
MRESULT Dialog::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;
    MessageBox("Dialog","wmCommand");
    return (MRESULT) mresReply;
}
