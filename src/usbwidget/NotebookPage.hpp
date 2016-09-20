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

/* Always declare the class(es) to resolve class dependencies */
class   NotebookPage;
class   NotebookPage1;

#ifndef     __NOTEBOOKPAGE_HPP__
#define     __NOTEBOOKPAGE_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"
#include    "ModuleGlobals.hpp"
#include    "ecomedia.h"

#include    "Dialog.hpp"
#include    "Button.hpp"
#include    "Notebook.hpp"

/* Prototypes */
MRESULT EXPENTRY NotebookPageHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

///////////////////////////////////////////////////////////////////////////////
// NotebookPage
///////////////////////////////////////////////////////////////////////////////
class   NotebookPage : public Dialog {

    public:
    /* Public Constructor and Destructors */
    NotebookPage();
    NotebookPage(Notebook* notebook);
    virtual ~NotebookPage();

    /* Public Methods */
    virtual init(void);
    virtual initItems(void);
    virtual test123(void);

    /* Public Attributes */
    ULONG   idPage;
    USHORT  pageStyle;
    USHORT  pageOrder;
    PSZ     tabTitle;
    ULONG   idResource;
    PSZ     statusText;
    Notebook*       notebook;
    NotebookPage*   prev;
    NotebookPage*   next;
    PFNWP   dlgProc;

    protected:

    private:

};

///////////////////////////////////////////////////////////////////////////////
// NotebookPage1
///////////////////////////////////////////////////////////////////////////////
class   NotebookPage1 : public NotebookPage {
    public:
    NotebookPage1(Notebook* notebook);
    virtual ~NotebookPage1();
    virtual init(void);
    virtual initItems(void);
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
};
#ifdef      __cplusplus
    }
#endif

#endif // __NOTEBOOKPAGE_HPP__

