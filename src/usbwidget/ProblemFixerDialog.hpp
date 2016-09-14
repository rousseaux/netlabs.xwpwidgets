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
    ~ProblemFixerDialog();
    ulong   create();
    ulong   process();
    ulong   destroy();


    ulong   redraw();

    ulong   msgInitDialog(ulong mp1, ulong mp2);
    ulong   msgCommand(ulong mp1, ulong mp2);

    ulong   commandDrawButton();
    ulong   commandDestroyButton();

    void    mleAppendText(char* text);
    void    mleClear();

    char    getDriveLetter();
    char    setDriveLetter(char dletter);
    int     getPhysDisk();
    int     setPhysDisk(int pdisk);
    ulong   getDiskStatus();
    ulong   setDiskStatus(ulong dskstat);

    void    help();
    void    noDFSVOS2();
    void    setupFixCommands();

    ulong   instanceMessageHandler2(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
    ulong   instanceMessageHandler(ulong hwnd, ulong msg, ulong mp1, ulong mp2);
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


    void    init();
    int     mle_cmdname(int id);
    int     set_fix_state();
    int     set_close_state(int fixed);

    ulong   get_fixmask();
    ulong   set_fixmask(ulong fixmask);
    ulong   get_effmask();
    ulong   set_effmask(ulong effmask);
    ulong   get_tofixmap();
    ulong   set_tofixmap(ulong tofixmap);
};


#ifdef      __cplusplus
    }
#endif

#endif // __PROBLEMFIXERDIALOG_HPP__
