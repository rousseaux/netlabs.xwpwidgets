/*****************************************************************************\
* NotebookPage.hpp -- NeoWPS * USB Widget                                     *
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
 * This module contains the NotebookPage Class Definition.
 */


#ifndef     __NOTEBOOKPAGE_HPP__
#define     __NOTEBOOKPAGE_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"
#include    "ModuleGlobals.hpp"
#include    "ecomedia.h"

/* Prototypes */
MRESULT EXPENTRY NotebookPageHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

class   NotebookPage {
    public:
    /* Do these public for now */
    ULONG   idPage;
    ULONG   idResource;
    HWND    hwndParent;
    HWND    hwndSelf;
    PFNWP   dlgProc;
    USHORT  pageStyle;
    USHORT  pageOrder;
    PSZ     tabTitle;
    PSZ     statusText;
    NotebookPage* prev;
    NotebookPage* next;
    /* Constructor and Destructor */
    NotebookPage();
    virtual ~NotebookPage();
};


#ifdef      __cplusplus
    }
#endif

#endif // __NOTEBOOKPAGE_HPP__
