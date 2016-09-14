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

// From WidgetSettings.cpp

/**
 * This module handles Windows.
 */

#include    "GUIHelpers.hpp"

#include    "Window.hpp"


Window::Window() {
    this->handle = NULL;
    sprintf(this->buf, "Window() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
}

Window::~Window() {
    sprintf(this->buf, "~Window() : hdlg=%08X", this->handle);
    __debug(NULL, this->buf, DBG_LBOX);
}

int     Window::show() {
    if (this->handle) {
        WinSetWindowPos(this->handle, HWND_TOP, 0, 0, 0, 0, SWP_SHOW|SWP_ZORDER);
        WinShowWindow(this->handle, true);
        WinSetFocus(HWND_DESKTOP, this->handle);
    }
    return this->handle;
}

void    Window::center() {
    if (this->handle) {
        CenterWindow(WinQueryWindow(this->handle, QW_PARENT), this->handle);
    }
}

void    Window::centerToDesktop() {
    if (this->handle) {
        CenterWindow(HWND_DESKTOP, this->handle);
    }
}

void    Window::centerToOther(ulong toCenterTo) {
    if (this->handle) {
        CenterWindow(toCenterTo, this->handle);
    }
}

int     Window::hide() {
    if (this->handle) {
        WinShowWindow(this->handle, false);
    }
    return this->handle;
}

ulong   Window::getHandle() {
    return this->handle;
}
