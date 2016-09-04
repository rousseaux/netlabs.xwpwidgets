/*****************************************************************************\
* ModuleGlobals.hpp -- NeoWPS * USB Widget                                    *
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
 * This is the public header file for it's correcponding source.
 */


#ifndef     __MODULEGLOBALS_HPP__
#define     __MODULEGLOBALS_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif


#include    "Master.hpp"


/*
// Messages for the ProblemFixerDialog.
*/
extern  char            pfd_unusable_problem[];
extern  char            pfd_unusable_fix[];
extern  char            pfd_ioerror_problem[];
extern  char            pfd_ioerror_fix[];
extern  char            pfd_corrupted_problem[];
extern  char            pfd_corrupted_fix[];
extern  char            pfd_big_floppy_problem[];
extern  char            pfd_big_floppy_fix[];
extern  char            pfd_bad_hiddensecs_problem[];
extern  char            pfd_bad_hiddensecs_fix[];
extern  char            pfd_dirty_fs_problem[];
extern  char            pfd_dirty_fs_fix[];
extern  char            pfd_too_many_partitions_problem[];
extern  char            pfd_too_many_partitions_fix[];


/* Complete path to DFSVOS2 if found */
extern  char            g_dfsvos2[];
extern  char            g_dfsvos2_flags[];
extern  bool            g_dfsvos2_exists;

/* Array of fix commands and counter */
extern  int             g_fixcommands_count;
extern  char            g_fixcommands[5][256];

/* Format strings for parsing DFSVOS2 xml output */
extern  char            g_dfsvos2_ecsdisks_number[];
extern  char            g_dfsvos2_ecsdisks_disknr[];
extern  char            g_dfsvos2_ecsdisks_letter[];
extern  char            g_dfsvos2_ecsdisks_label[];
extern  char            g_dfsvos2_ecsdisks_beyondendisk[];
extern  char            g_dfsvos2_ecsdisks_resize[];
extern  char            g_dfsvos2_ecsdisks_status[];
extern  char            g_dfsvos2_ecsdisks_filesystem[];




/* Pointer to the named shared memory */
extern  PVOID           shmem;


/* Needed because of cyclic dependancies; fix later */
class   USBWidget;
class   DebugDialog;
class   ProblemFixerDialog;

extern  void*   g_usbdev;

extern  bool            g_debug;
extern  bool            g_the_very_first_time;
extern  bool            g_use_new_implementation;

extern  USBWidget*          g_myUSBWidget;
extern  DebugDialog*        g_myDebugDialog;
extern  ProblemFixerDialog* g_ProblemFixerDialog;

extern  char            drvletter_safeeject;
extern  ULONG           drivemap_safeeject;
extern  BOOL            foreign_eject;
extern  BOOL            warm_start;

/* Kunnen deze weg ? */
typedef struct _DRIVESTATUS {
    char    dletter;
    int     attached;
} DRIVESTATUS;

typedef struct _DRIVEDATA {
    char        dletter;
    char        label[64];
} DRIVEDATA, *PDRIVEDATA;



/*
// Info about a logical drive (volume)
*/
// Not Used
//~ typedef struct  _LOGICAL_DRIVE {
    //~ BOOL        safe_eject;
//~ } LOGICAL_DRIVE, *PLOGICAL_DRIVE;

/*
// Info about a physical disk
*/
typedef struct  _PHYSICAL_DISK {
    ULONG       drivemap;       // Bit set for every driveletter, lsb is 'A'
    ULONG       usbid;          // The ID of the usb-device
    BOOL        present;        // Marks if the device is present (inserted)
    BOOL        safe_eject;     // Set if the device was safely ejected
    BOOL        phantom;        // Set when the device was removed without
                                // proper eject.
    BOOL        zipdrive;       // Set if the device is a zip-drive
} PHYSICAL_DISK, *PPHYSICAL_DISK;



/*
// Widget State structure.
// It holds the state of the widget between widget removals or wps restarts.
// If a preallocated structure is found in shared memory, that one is used,
// otherwise the structure is allocated from the local heap, in which case
// the widget does *not* retain it's state across removals or wps restarts.
*/
// BOOKMARK: Widget State structure
typedef     struct  _WIDGETSTATE {
    ULONG           signature;      // Signature to indicate a valid structure

    HEV             wait4daemon;    // Event semaphore posted by the Daemon
                                    // when it has allocated the memory.

    ULONG           drivemap;       // Used to store the drivemap so changes
                                    // can be determined after ie a WPS
                                    // restart.

    PHYSICAL_DISK   pdisks[100];    // Array to hold physical disk info.

    CHAR            phantoms[26];   // Array to hold the phantom status of a
                                    // drive-letter.
    /* Lists to hold USB Devices */
    CList*          currentUSBDevicesList;
    CList*          previousUSBDevicesList;
    CList*          removedUSBDevicesList;
}   WIDGETSTATE, *PWIDGETSTATE;


/* Declaration of the pointer to the Widget State structure */
extern  PWIDGETSTATE    pws;




/*
// Global drive masks before and after events.
*/

/* General */
extern      ULONG       g_drives_before;
extern      ULONG       g_drives_after;
extern      ULONG       g_drives_changed;
extern      ULONG       g_drives_new;
extern      ULONG       g_drives_gone;
extern      ULONG       g_drives_temp;

/* Eject */
extern      ULONG       g_drives_before_eject;
extern      ULONG       g_drives_after_eject;
extern      ULONG       g_drives_changed_after_eject;
extern      ULONG       g_drives_new_after_eject;           // Some drive could be added during the eject.
extern      ULONG       g_drives_gone_after_eject;          // This will be the drives on the disk ejected.

/* Insert */
extern      ULONG       g_drives_before_insert;
extern      ULONG       g_drives_after_insert;
extern      ULONG       g_drives_changed_after_insert;
extern      ULONG       g_drives_new_after_insert;          // It could be no drives are added.
extern      ULONG       g_drives_gone_after_insert;         // Some drives could be removed during this event.

/* Remove */
extern      ULONG       g_drives_before_remove;
extern      ULONG       g_drives_after_remove;
extern      ULONG       g_drives_changed_after_remove;
extern      ULONG       g_drives_new_after_remove;          // Some drives could be added during this event.
extern      ULONG       g_drives_gone_after_remove;         // If there are drives removed it could indicate a force remove ! compare drive with usbid.

/* Rediscover */
extern      ULONG       g_drives_before_rediscover;
extern      ULONG       g_drives_after_rediscover;
extern      ULONG       g_drives_changed_after_rediscover;  // Changed drives.
extern      ULONG       g_drives_new_after_rediscover;      // New drives.
extern      ULONG       g_drives_gone_after_rediscover;     // Removed drives.

/* Refresh */
extern      ULONG       g_drives_before_refresh;
extern      ULONG       g_drives_after_refresh;
extern      ULONG       g_drives_changed_after_refresh;     // Changed drives.
extern      ULONG       g_drives_new_after_refresh;         // New drives.
extern      ULONG       g_drives_gone_after_refresh;        // Removed drives.



/*
// Globals from usbcalls.c, just for verifying !
*/
//extern        HFILE       g_hUSBDrv;
//extern        ULONG       g_cInit;


/*
// Declare module-global variables
*/
extern      HPIPE   hpipe;
extern      ULONG   drive_map_old;
extern      ULONG   drive_map_new;
extern      ULONG   drive_map_changed;
extern      HWND    dialogs[NUM_DIALOGS];
extern      HWND    windows[NUM_WINDOWS];

extern      CHAR    g_buf[1024];

extern      HAB     myhab;
extern      HMODULE hmodMe;

//~ extern      CList   currentUSBDevicesList;
//~ extern      CList   previousUSBDevicesList;
//~ extern      CList   removedUSBDevicesList;

extern      BOOL    initialization;
extern      BOOL    enablebeeps;
extern      BOOL    prev_enablebeeps;
extern      BOOL    monitorzip;
extern      BOOL    prev_monitorzip;

extern      DRIVESTATUS drvstate[26];
extern      char        Drives[26][128];
extern      char        Phantoms[26];

extern      HFILE       PhysDisks[10];
extern      HFILE       LogDrives[26];

extern      BOOL    term;
extern      BOOL    nosound;
extern      BOOL    checkdrives;
extern      BOOL    startupcheck;


extern      HWND        hwndWidget;             // handle of widget window

extern      HWND        hwndPopup;              // popup menu
extern      HWND        hdlgDebugDialog;        // Debug Dialog
extern      HWND        hdlgProblemFixerDialog; // Problem Fixer Dialog
extern      HWND        hdlgNotification;       // New dialog based notification window

extern      HWND        hwndIndiDevInsert;      // insert notifier window
extern      HWND        hwndIndiSafeEject;      // safe eject notifier window
extern      HWND        hwndIndiDevRemove;      // force remove notifier window
extern      HWND        hwndIndiDrivesChanged;  // drives changed notifier window


extern      HPOINTER        hptrEject;          // icon for widget
extern      HPOINTER        hptrCD;             // icon "CD"
extern      HPOINTER        hptrHDD;            // icon for LVM-managed drives
extern      HPOINTER        hptrHDDBAD;         // icon for drives with bad partition-table
extern      HPOINTER        hptrHDDPHANTOM;     // icon for phantom-drives (after force-remove)
extern      HPOINTER        hptrZIP;            // icon for zip-drives

extern      LANGE       BatteryLange;


// Rousseau: added (init to -1 since 0 in DosWaitThread would wait for the next thread to end)
extern      TID     tid_lvm;
extern      TID     tid_monitor;
extern      TID     tid_eject;
extern      TID     tid_zipdrive;
extern      TID     tid_phantom_slayer;
extern      TID     tid_drive_monitor;
extern      TID     tid_wmquit_monitor;

extern      HEV     hevNeverPosted;
extern      HEV     hevEventInserted;
extern      HEV     hevEventRemoved;
extern      HEV     hevPushLVMthread;
extern      HEV     hevPSthread;
extern      HEV     hevDMthread;

extern      BOOL    lvm_thread_working;
extern      BOOL    monitor_thread_working;
extern      BOOL    eject_thread_working;
extern      BOOL    zipdrive_thread_working;
extern      BOOL    phantom_slayer_thread_working;
extern      BOOL    drive_monitor_thread_working;
extern      BOOL    wmquit_monitor_thread_working;

extern      BOOL    phantom_slayer_periodic;

/* Access Semaphores */
extern      HMTX    hmtxLVMaccess;
extern      HMTX    hmtxAuxDebug;

//~ extern      PHYSICAL_DISK   pdisks[100];

/*
typedef struct  _DRIVEINFO {
    char        dletter,
    char        label[12],
    boel        isusb,
    boel        iszip,
    boel        caneject
} DRIVEINFO, *PDRIVEINFO;
*/


#ifdef      __cplusplus
    }
#endif

#endif // __MODULEGLOBALS_HPP__





/*
// Wordt niet gevonden als-ie binnen de modele ifndef staat.
// Waarom niet ?
*/

#ifndef __ROOT__
#define __ROOT__
/*
// This is the root-class of the whole shebang.
// It has some stuff common to all classes, like a debug-flag.
*/
class   Root {
    private:
    bool    debug;
    int     level;

    public:
    Root();
    ~Root();
    bool    debugMe();
    bool    debugMe(bool flag);
    int     debugLevel();
    int     debugLevel(int level);
};
#endif


