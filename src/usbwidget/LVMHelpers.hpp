/*****************************************************************************\
* LVMHelpers.hpp -- NeoWPS * USB Widget                                       *
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
 * This module contains several helper functions for the LVM System.
 * It is used to simplify things.
 */


#ifndef     __LVMHELPERS_HPP__
#define     __LVMHELPERS_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif


#include    "ModuleGlobals.hpp"
#include    "lvm_intr.h"
#include    "lange.h"


class   PhysicalDisk {
};



class   LVMEngine {
};


/* Prototypes */
void            LVMHelpers(void);
void            RefreshLVM();
int             PushLVM();
void            FillMenu(HWND hwndMenu);
ULONG           drive_map_lvm(VOID);
ULONG           drive_map_reserved_lvm(VOID);
CARDINAL32      RefreshLVMEngine();
CARDINAL32      DumpDrives(PCHAR txtbuf);
CARDINAL32      DumpVolumes(PCHAR txtbuf);
CARDINAL32      ReadMBR(int disk, char* buf, ulong bufsize);



int             OpenLVM();





class   LvmEngineImpl {
    private:
    int     x;

    public:
    LvmEngineImpl();
    ~LvmEngineImpl();
};



class   LvmEngineXX : public LvmEngineImpl {
    private:
    HSEM    hsemAll;            // generic semaphore handle
    HEV     hevEvent;           // event semaphore handle
    HMTX    hmtxLVMaccess;      // mutex semaphore handle
    HMUX    hmuxAll;            // muxwait semaphore handle


    public:
    bool    open() {
        return false;
    }
    bool    openCache();
    int     enumDisks();
    int     enumPartitions();
    int     enumVolumes();
    char*   driveLetters();
    char*   lanDrives();
    char*   prmDrives();
    char*   availDrives();
    bool    closeCache();
    bool    close();
    int     _get_x();
    int     _set_x(int x);
};




//#include  "usbsimpl.hpp"
//#include  "GUIHelpers.hpp"
//#include  "USBHelpers.hpp"


#define     INCL_DOSPROCESS
#define     INCL_DOSMODULEMGR
#define     INCL_DOSEXCEPTIONS
#define     INCL_DOSSEMAPHORES
#define     INCL_DOSDATETIME
#define     INCL_DOSMISC
#define     INCL_DOSERRORS
#define     INCL_DOSFILEMGR
#define     INCL_DOSDEVICES
#define     INCL_DOSDEVIOCTL
#define     INCL_DOSNMPIPES
#define     INCL_GPI
#define     INCL_WIN
#define     INCL_WINWINDOWMGR
#define     INCL_WINFRAMEMGR
#define     INCL_WINDIALOGS
#define     INCL_WININPUT
#define     INCL_WINPOINTERS
//#define   INCL_WINPROGRAMLIST
//#define   INCL_WINSWITCHLIST
#define     INCL_WINSYS
#define     INCL_WINLISTBOXES
#define     INCL_WINTIMER
#define     INCL_WINMENUS
#define     INCL_WINWORKPLACE

// #define  INCL_WINMESSAGEMGR
// #define  INCL_WINRECTANGLES
// #define  INCL_WINLISTBOXES
// #define  INCL_WINENTRYFIELDS

#define     INCL_GPIPRIMITIVES
#define     INCL_GPILOGCOLORTABLE
#define     INCL_GPIREGIONS
#include    <os2.h>


#include    <stdio.h>
#include    <string.h>



#ifdef      __cplusplus
    }
#endif

#endif // __LVMHELPERS_HPP__
