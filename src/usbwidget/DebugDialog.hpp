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

#include    "GUIHelpers.hpp"

#include    "Dialog.hpp"
#include    "Notebook.hpp"
#include    "NotebookPage.hpp"

/* Prototypes */
MRESULT EXPENTRY DebugDialogProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

class   DebugDialog : public Dialog {

    public:

    /* Public Constructor(s) and Destructor */
    DebugDialog();
    virtual ~DebugDialog();

    /* Public Methods */
    virtual int create(void);
    virtual int process(void);
    virtual int destroy(void);
    virtual int test123(void);

    /* Public Message Handlers */
    virtual MRESULT wmClose(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmDestroy(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmEraseBackground(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmPaint(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
    virtual MRESULT wmDefault(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Public Attributes */
    Notebook*   notebook;

    protected:

    private:

};

#ifdef      __cplusplus
    }
#endif

#endif // __DEBUGDIALOG_HPP__
