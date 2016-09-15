/*****************************************************************************\
* Notebook.hpp -- NeoWPS * USB Widget                                         *
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

// From NotebookPage.hpp

/**
 * This module contains the Notebook Class Definition.
 */


#ifndef     __NOTEBOOK_HPP__
#define     __NOTEBOOK_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"
#include    "ModuleGlobals.hpp"
#include    "ecomedia.h"

#include    "NotebookPage.hpp"

/* Prototypes */
MRESULT EXPENTRY NotebookHandler(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

class   Notebook {
    public:
    /* Do these public for now */
    ULONG           idResource;
    HWND            hwndParent;
    HWND            hwndSelf;
    NotebookPage*   pages;
    /* Constructor and Destructor */
    Notebook();
    virtual ~Notebook();
    virtual void    appendPage(NotebookPage*);
    virtual void    appendPages();
    virtual void    removePage(NotebookPage*);
    virtual void    removePages();

    protected:

    private:

};


#ifdef      __cplusplus
    }
#endif

#endif // __NOTEBOOK_HPP__
