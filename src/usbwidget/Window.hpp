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

/*
// According to the documentation, CtlData for Windows and CreateParams for
// Dialogs needs to point to a structure with the 1st USHORT holding its size.
// This structure is used by WM_INIT or WM_INITDLG to associate the
// Class Instance to the Window or Dialog Procedure. This makes it possible to
// defer messages to instance members and allow for overrides with member
// granularity, instead of subclassing the whole Dialog or Window Procedure.
*/
typedef struct {
    USHORT  cb;                 // Size of this structure
    PVOID   pvClassInstance;    // Pointer to Class Instance
} WND_CLASS_INSTANCE;

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

    //! FIXME: Access problems using pointers in derived classes
    HWND    hwndParent;
    HWND    hwndOwner;
    HWND    hwndSelf;
    protected:
    char    buf[256];
    WND_CLASS_INSTANCE  wci;

    private:

};

#ifdef      __cplusplus
    }
#endif

#endif // __WINDOW_HPP__
