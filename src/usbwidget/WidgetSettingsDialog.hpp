/*****************************************************************************\
* WidgetSettingsDialog.hpp -- NeoWPS * USB Widget                             *
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

/******************************************************************************
* This module holds the new Widget Settings Dialog
* -----------------------------------------------------------------------------
* It contains a Notebook Control so that settings can be grouped according to
* their category. From the C++ point-of-view, an instance wraps around a Dialog
* and messages are deferred to instance-members.
*
*/

/* Always declare the class(es) to resolve class dependencies */
class   WidgetSettingsDialog;

#ifndef     __WIDGETSETINGSDIALOG_HPP__
#define     __WIDGETSETINGSDIALOG_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"
#include    "ModuleGlobals.hpp"
#include    "ecomedia.h"

#include    "GUIHelpers.hpp"

#include    "Notebook.hpp"
#include    "NotebookPage.hpp"

#include    "WidgetSettingsDialog.ids"

/* Prototypes */
MRESULT EXPENTRY DlgProcWidgetSettingsDialog(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/* Widget Settings Dialog */
class   WidgetSettingsDialog : public Dialog {

    public:

    /* Public Constructor and Destructors */
    WidgetSettingsDialog();
    virtual ~WidgetSettingsDialog();

    /* Public Methods */
    virtual int create(void);
    virtual int process(void);
    virtual int destroy(void);
    virtual int test123(void);

    /* Public Message Handlers */
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

    /* Public Attributes */
    Notebook*   notebook;

    protected:

    private:

};

/* Derived Widget Settings Dialog with override method */
class   WidgetSettingsDialogEx : public WidgetSettingsDialog {
    public:
    virtual MRESULT wmCommand(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
};




class   Notebook1 : public Notebook {

    public:

    /* Public Contructor(s) and Destructor */
    Notebook1::Notebook1();
    virtual Notebook1::~Notebook1();

    /* Public Methods */
    virtual void    init(HWND parent, ULONG id);
    virtual void    appendPage(NotebookPage*);
    virtual void    appendPages();
    virtual void    removePage(NotebookPage*);
    virtual void    removePages();
    virtual int     test123(void);

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

#endif // __WIDGETSETINGSDIALOG_HPP__
