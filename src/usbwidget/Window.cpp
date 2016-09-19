/*****************************************************************************\
* Window.cpp -- NeoWPS * USB Widget                                           *
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
 * This module handles Windows.
 */

#include    "Debug.hpp"
#include    "GUIHelpers.hpp"

#include    "Window.hpp"


Window::Window() {
    this->debugMe();
    this->hwndParent = NULL;
    this->hwndOwner = NULL;
    this->hwndSelf = NULL;
    this->wci.cb = sizeof(WND_CLASS_INSTANCE);
    this->wci.pvClassInstance = this;
    sprintf(this->buf, "Window() : hdlg=%08X", this->hwndSelf);
    __debug(NULL, this->buf, DBG_LBOX);
}

Window::~Window() {
    sprintf(this->buf, "~Window() : hdlg=%08X", this->hwndSelf);
    __debug(NULL, this->buf, DBG_LBOX);
}

int     Window::show() {
    if (this->hwndSelf) {
        WinSetWindowPos(this->hwndSelf, HWND_TOP, 0, 0, 0, 0, SWP_SHOW|SWP_ZORDER);
        WinShowWindow(this->hwndSelf, true);
        WinSetFocus(HWND_DESKTOP, this->hwndSelf);
    }
    return this->hwndSelf;
}

void    Window::center() {
    if (this->hwndSelf) {
        CenterWindow(WinQueryWindow(this->hwndSelf, QW_PARENT), this->hwndSelf);
    }
}

void    Window::centerToDesktop() {
    if (this->hwndSelf) {
        CenterWindow(HWND_DESKTOP, this->hwndSelf);
    }
}

void    Window::centerToOther(ulong toCenterTo) {
    if (this->hwndSelf) {
        CenterWindow(toCenterTo, this->hwndSelf);
    }
}

int     Window::hide() {
    if (this->hwndSelf) {
        WinShowWindow(this->hwndSelf, false);
    }
    return this->hwndSelf;
}

int     Window::maximize() {
    if (this->hwndSelf) {
        WinSetWindowPos(this->hwndSelf, HWND_TOP, 0, 0, 0, 0, SWP_MAXIMIZE|SWP_ZORDER);
        WinShowWindow(this->hwndSelf, true);
        WinSetFocus(HWND_DESKTOP, this->hwndSelf);
    }
    return this->hwndSelf;
}

ulong   Window::getHandle() {
    return this->hwndSelf;
}

/* Default WM_COMMAND implementation for Window */
MRESULT Window::wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2) {
    MRESULT mresReply = 0;
    MessageBox("Window","wmCommand");
    return (MRESULT) mresReply;
}
