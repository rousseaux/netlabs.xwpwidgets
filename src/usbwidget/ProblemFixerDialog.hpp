/*****************************************************************************\
* ProblemFixerDialog.hpp -- NeoWPS * USB Widget                               *
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
 * This module is a dummy to invoke the .obj.lib inference rule.
 */


#ifndef     __PROBLEMFIXERDIALOG_HPP__
#define     __PROBLEMFIXERDIALOG_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"

#include    "Dialog.hpp"

#define     PF_FIX_SIZE     1
#define     PF_FIX_HSECS    2
#define     PF_FIX_DIRTY    3

class   ProblemFixerDialog : public Dialog {

    public:
    ProblemFixerDialog();
    virtual ~ProblemFixerDialog();
    virtual ulong   create();
    virtual ulong   process();
    virtual ulong   destroy();
    virtual ulong   redraw();
    virtual ulong   msgInitDialog(ulong mp1, ulong mp2);
    virtual ulong   msgCommand(ulong mp1, ulong mp2);
    virtual ulong   commandDrawButton();
    virtual ulong   commandDestroyButton();
    virtual void    mleAppendText(char* text);
    virtual void    mleClear();
    virtual char    getDriveLetter();
    virtual char    setDriveLetter(char dletter);
    virtual int     getPhysDisk();
    virtual int     setPhysDisk(int pdisk);
    virtual ulong   getDiskStatus();
    virtual ulong   setDiskStatus(ulong dskstat);
    virtual void    help();
    virtual void    noDFSVOS2();
    virtual void    setupFixCommands();
    virtual ulong   instanceMessageHandler2(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    virtual ulong   instanceMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    //~ static ulong EXPENTRY ProblemFixerDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    static ulong ProblemFixerDialog::classMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);

    protected:

    private:
    char    dletter;
    int     pdisk;
    ulong   dskstat;
    ulong   fixmask;
    ulong   effmask;
    ulong   tofixmap;
    int     fixcommands_count;
    char    fixcommands[5][256];
    ulong   fixes[5];
    ulong   action_taken;
    virtual void    init();
    virtual int     mle_cmdname(int id);
    virtual int     set_fix_state();
    virtual int     set_close_state(int fixed);
    virtual ulong   get_fixmask();
    virtual ulong   set_fixmask(ulong fixmask);
    virtual ulong   get_effmask();
    virtual ulong   set_effmask(ulong effmask);
    virtual ulong   get_tofixmap();
    virtual ulong   set_tofixmap(ulong tofixmap);
};


#ifdef      __cplusplus
    }
#endif

#endif // __PROBLEMFIXERDIALOG_HPP__
