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

/* Always declare the class(es) to resolve class dependencies */
class   Window;

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
    virtual int     maximize();
    virtual int     minimize();
    virtual int     setText(char* text);
    virtual ulong   getHandle();

    /* Public Message Handlers */
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Public Attributes */
    char    buf[256];
    HWND    hwndParent;         // public for now
    HWND    hwndOwner;          // public for now
    HWND    hwndSelf;           // public for now
    WND_CLASS_INSTANCE  wci;    // public for now

    protected:

    private:

};

#ifdef      __cplusplus
    }
#endif

#endif // __WINDOW_HPP__
