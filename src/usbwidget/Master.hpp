/*****************************************************************************\
* Master.hpp -- NeoWPS * USB Widget                                           *
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
 * This public header file holds the many global definitions.
 * It includes other public header files so including this header will make
 * all that stuff available.
 */


#ifndef     __MASTER_HPP__
#define     __MASTER_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif


/*
// Include Debug Settings.
*/
#include    "Debug.h"


/*
// Here we have some global definitions for use by all modules.
*/
#define     true            1
#define     false           0
#define     null            0
typedef     int             bool;
typedef     unsigned long   ulong;



typedef enum    _FSDTYPE {
    /* reserved */
    FSDT_NULL,
    /* OS/2 - eComStation */
    FSDT_FAT,
    FSDT_FAT32,
    FSDT_HPFS,
    FSDT_JFS,
    /* Linux */
    FSDT_EXT2,
    FSDT_EXT3,
    FSDT_REISER3,
    FSDT_REISER4,
    FSDT_JFSLX,
    FSDT_XFS,
    FSDT_BTRFS,
    FSDT_ZFS,
    /* Unknown */
    FSDT_UNKNOWN = -1
} FSDTYPE;
typedef FSDTYPE*    PFSDTYPE;


#define     WM_INITDLG2     WM_USER+1000
#define     WM_DEVINFO      WM_USER+1010

#define     DRIVE_MAP_MASK  0x3FFFFFF
#define     OSDIR_VAR_NAME  "OSDIR"

#define     NUM_DIALOGS ID_DIALOG_NEXT - ID_DIALOG_1
#define     NUM_WINDOWS ID_WINDOW_NEXT - ID_WINDOW_1

//~: #define EJECT_TIMEOUT
#define     EJECT_TIMEOUT   10

/*
// USB Device Flags.
// Some ofthese are set from the usb-descriptor info while
// others are set at runtime.
// For instance, if a device gets mounted at the end of the time-window,
// it is set as USBDEV_MOUNTS_SLOWLY so that next time the time-window will be increased.
*/
#define     USBDEV_HID                      0x00000001L
#define     USBDEV_KEYBOARD                 0x00000002L
#define     USBDEV_MOUSE                    0x00000004L
#define     USBDEV_RES03                    0x00000008L
#define     USBDEV_WIRELESS                 0x00000010L
#define     USBDEV_BLUETOOTH                0x00000020L
#define     USBDEV_RADIOFREQ                0x00000040L
#define     USBDEV_DASD                     0x00000080L
#define     USBDEV_MEDIUM_CAN_CHANGE        0x00000100L
#define     USBDEV_MOUNTS_SLOWLY            0x00000200L
#define     USBDEV_NON_EJECTABLE            0x00000400L
#define     USBDEV_IS_PHANTOM               0x00000800L
#define     USBDEV_RES12                    0x00001000L
#define     USBDEV_RES13                    0x00002000L
#define     USBDEV_RES14                    0x00004000L
#define     USBDEV_RES15                    0x00008000L
#define     USBDEV_RES16                    0x00010000L
#define     USBDEV_RES17                    0x00020000L
#define     USBDEV_RES18                    0x00040000L
#define     USBDEV_RES19                    0x00080000L
#define     USBDEV_RES20                    0x00100000L
#define     USBDEV_RES21                    0x00200000L
#define     USBDEV_RES22                    0x00400000L
#define     USBDEV_RES23                    0x00800000L
#define     USBDEV_RES24                    0x01000000L
#define     USBDEV_RES25                    0x02000000L
#define     USBDEV_RES26                    0x04000000L
#define     USBDEV_RES27                    0x08000000L
#define     USBDEV_RES28                    0x10000000L
#define     USBDEV_RES29                    0x20000000L
#define     USBDEV_RES30                    0x40000000L
#define     USBDEV_RES31                    0x80000000L



/*
// OS/2 sub-system
*/
#define     INCL_DOS
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



// C library headers
#include    <stdio.h>
#include    <setjmp.h>                                                          // needed for except.h
#include    <assert.h>                                                          // needed for except.h
#include    <stdlib.h>
#include    <string.h>
#include    <stdarg.h>
#include    <ctype.h>

/*
// According to the documentation, CreateParams needs to point to a structure
// where the first USHORT contains its size.
// This structure is passed to WinLoadDlg to connect the Class Instance to
// the Dialog Procedure. This makes it possible to defer messages to instance
// members and allow for overrides with member granularity, instead of
// subclassing the whole Dialog Procedure.
*/
typedef struct {
    USHORT  cb;                 // Size of this structure
    PVOID   pvClassInstance;    // Pointer to Class Instance
} DLG_CLASS_INSTANCE;

// Used to communicate with extern logpipe program (obsolete)
#define     PIPE_NAME           "\\PIPE\\PIPE1"
#define     PIPE_SIZE   256

// Event Semaphore that is cleared when the DAEMON is ready
#define     EVSEM_WAIT4DAEMON   "\\SEM32\\NEOWPS_USBWIDGET\\WAIT4DAEMON"

// Event Semaphore that is cleared to stop the DAEMON
#define     EVSEM_WAIT4STOP     "\\SEM32\\NEOWPS_USBWIDGET\\WAIT4STOP"

// Name of the Shared Memory that the DAEMON provides
#define     SHMEM_NAME          "\\SHAREMEM\\NEOWPS_USBWIDGET\\MASTERPOOL"
#define     SHMEM_SIZE  256000


#include    "ecomedia.h"
#include    "CList.hpp"
#include    "Dialogids.h"
#include    "Notebookids.h"
//#include  "Testing123.hpp"
#include    "lange.h"

#include    "Debug.hpp"
#include    "AllocMem.hpp"
#include    "Threads.hpp"
#include    "APIHelpers.hpp"
//~ #include    "GUIHelpers.hpp"
#include    "USBHelpers.hpp"
#include    "LVMHelpers.hpp"
#include    "Testing123.hpp"
#include    "ProblemFixer.hpp"
#include    "USBWidget.hpp"


//! PROBLEMS WITH THE ABOVE INCLUDES


#ifdef      __cplusplus
    }
#endif

#endif // __MASTER_HPP__

