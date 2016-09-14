/*****************************************************************************\
* ModuleGlobals.cpp -- NeoWPS * USB Widget                                    *
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
 * This module holds various global variables used by the Widget at runtime.
 * One of them is a pointer to the Widget State that that might point to
 * shared memory provided by the DAEMON. Otherwise it points to locally
 * allocated memory.
 */


#include    "ModuleGlobals.hpp"

char            pfd_unusable_problem[] =
                    "\n\n"
                    "** Device Unusable **\n"
                    "Somehow the LVM Engine has marked this storage device as unuable.\n"
                    "This cannot be fixed from within this problem fixer.\n"
                    "Click on \"Show Fix Commands\" on the right of this dialog for some hints.\n"
                    ;

char            pfd_unusable_fix[] =
                    "\n\n"
                    "** Device Unusable **\n"
                    "Try to eject and reinsert the device.\n"
                    "See if removing other attached mass storage devices make a difference.\n"
                    "Run the Disk Checker and see what it says.\n"
                    ;

char            pfd_ioerror_problem[] =
                    "\n\n"
                    "** I/O Error **\n"
                    "The LVM Engine has detected an I/O error on the device.\n"
                    "This could be caused by unreadable sectors or a partition definition that\n"
                    "exceeds the capacity of the device.\n"
                    "Click on \"Show Fix Commands\" on the right of this dialog for some hints.\n"
                    ;

char            pfd_ioerror_fix[] =
                    "\n\n"
                    "** I/O Error **\n"
                    "Try to create an Image Backup to a file and then restore that Image Backup.\n"
                    "This could potentially 'revive' some sectors that cause the I/O error.\n"
                    "If this works know that these sectors might become bad again in the near future.\n"
                    "\n\n"
                    "Use the Disk Checker to verify that the partition does not exceed the device capacity.\n"
                    "You can also use DFSVOS2 or DFSee for this purpose.\n"
                    ;

char            pfd_corrupted_problem[] =
                    "\n\n"
                    "** Device Corrupted **\n"
                    "The LVM Engine has detected that the device is corrupted.\n"
                    "This is usually the case with an LVM hostile partition layout.\n"
                    "It can possibly be fixed by altering the layout of the partition.\n"
                    "Click on \"Show Fix Commands\" on the right of this dialog to see the fix commands.\n"
                    ;

char            pfd_corrupted_fix[] =
                    "\n\n"
                    "** Device Corrupted **\n"
                    "The following command will be run to fix this problem:\n"
                    "COMMAND: %s\n"
                    "This command will be run when you press the \"Fix\" button.\n"
                    "The output of the fix command is captured in this logwindow so you can copy-paste\n"
                    "it in some editor to save it.\n"
                    "The \"Fix\" button will change to \"Close\" and pressing it will eject the device.\n"
                    "You need to re-insert it to see if the problem is solved.\n"
                    "Reinserting can potentially cause a Trap, so it's best to do a reboot first.\n"
                    ;

char            pfd_big_floppy_problem[] =
                    "\n\n"
                    "** Device formatted as BIG Floppy **\n"
                    "This mass storage device is formatted as *BIG Floppy*.\n"
                    "BIG Floppy is not (yet) supported by eComStation.\n"
                    "This cannot be fixed from within this problem fixer.\n"
                    "Click on \"Show Fix Commands\" on the right of this dialog for a manual\n"
                    "procedure of how to fix this problem.\n"
                    ;

char            pfd_big_floppy_fix[] =
                    "\n\n"
                    "** Device formatted as BIG Floppy **\n"
                    "Insert the storage device into a Linux or Windows machine.\n"
                    "Copy all the files from the device to a directory on that machine.\n"
                    "For Linux you can use the cp -a command from a terminal or use some GUI File Manager.\n"
                    "For Windows you can use the xcopy /s /e from a command prompt or use File Explorer.\n"
                    "When using a GUI program make sure it also displays / copies any hidden files.\n"
                    "\n\n"
                    "Now resinsert the storage device in the eComStation machine.\n"
                    "Use MiniLVM to create a primary partition on the device.\n"
                    "(BIG Floppy will be shown as a partition so you'll have to first delete it)\n"
                    "If the file-system was FAT16 you can use eCS to format the new partition\n"
                    "which must be smaller than 2047MB (2GB).\n"
                    "\n\n"
                    "If the file-system was FAT32 the best is to use Windows to format the partition.\n"
                    "For Linux you can use GParted.\n"
                    "Make sure the partition identifier is 0x0b or 0x0c to indentify FAT32.\n"
                    "\n\n"
                    "Now insert the storage device into the eCS machine, it should be accessible.\n"
                    "If so, eject it and restore the files you backed up to the Linux or Windows machine.\n"
                    "eComStation should now be able to access them.\n"
                    ;

char            pfd_bad_hiddensecs_problem[] =
                    "\n\n"
                    "** Incorrect value for Hidden Sectors **\n"
                    "This only affects partitions formatted with FAT16.\n"
                    "Usually it is caused by using some third-party formatting tool that does\n"
                    "not set this value correctly. Linux GParted is known to have this problem.\n"
                    "This problem can be fixed from within this problem fixer.\n"
                    "Click on \"Show Fix Commands\" on the right of this dialog to see the fix commands.\n"
                    ;

char            pfd_bad_hiddensecs_fix[] =
                    "\n\n"
                    "** Incorrect value for Hidden Sectors **\n"
                    "The following command will be run to fix this problem:\n"
                    "COMMAND: %s\n"
                    "This command will be run when you press the \"Fix\" button.\n"
                    "The output of the fix command is captured in this logwindow so you can copy-paste\n"
                    "it in some editor to save it.\n"
                    "The \"Fix\" button will change to \"Close\" and pressing it will eject the device.\n"
                    "You need to re-insert it to see if the problem is solved.\n"
                    "Reinserting can potentially cause a Trap, so it's best to do a reboot first.\n"
                    ;

char            pfd_dirty_fs_problem[] =
                    "\n\n"
                    "** Dirty File System **\n"
                    "The layout of the storage device looks OK but the File System is Dirty.\n"
                    "This is usually caused by forcefully removing the device without doing a proper eject.\n"
                    "Even when doing a proper ejects it's always a good idea to way 2 seconds before removing\n"
                    "the device. This ensures all data it written from the caches to the device.\n"
                    "This problem can be fixed from within this problem fixer.\n"
                    "Click on \"Show Fix Commands\" on the right of this dialog to see the fix commands.\n"
                    ;

char            pfd_dirty_fs_fix[] =
                    "\n\n"
                    "** Dirty File System **\n"
                    "The following command will be run to fix this problem:\n"
                    "COMMAND: %s\n"
                    "This command will be run when you press the \"Fix\" button.\n"
                    "The output of the fix command is captured in this logwindow so you can copy-paste\n"
                    "it in some editor to save it.\n"
                    "The \"Fix\" button will change to \"Close\" and pressing it will eject the device.\n"
                    "You need to re-insert it to see if the problem is solved.\n"
                    "Most probably the device will be accessible now.\n"
                    "Fixing a dirty FAT32 is not supported yet, only HPFS and JFS are.\n"
                    ;

char            pfd_too_many_partitions_problem[] =
                    "\n\n"
                    "** More than 1 partition found **\n"
                    "The attached mass storage device contains more than one partition.\n"
                    "This problem fixer is not yet able to handle multiple partitions.\n"
                    "Click on \"Show Fix Commands\" on the right of this dialog to see the fix commands.\n"
                    ;

char            pfd_too_many_partitions_fix[] =
                    "\n\n"
                    "** More than 1 partition found **\n"
                    "No suggestions available at this time.\n"
                    ;


/* Complete path to DFSVOS2 if found */
char            g_dfsvos2[256]          = "\0";
//~ char            g_dfsvos2_flags[]       = " -7 -a -B- ";
char            g_dfsvos2_flags[]       = " -a -B- -b -E:q ";
bool            g_dfsvos2_exists        = FALSE;


/* Format strings for parsing DFSVOS2 xml output */
char            g_dfsvos2_ecsdisks_number[]         = ":number=";
char            g_dfsvos2_ecsdisks_disknr[]         = ":disknr=";
char            g_dfsvos2_ecsdisks_letter[]         = ":letter=";
char            g_dfsvos2_ecsdisks_label[]          = ":label=";
char            g_dfsvos2_ecsdisks_beyondendisk[]   = ":beyondenddisk=";
char            g_dfsvos2_ecsdisks_resize[]         = ":resize=";
char            g_dfsvos2_ecsdisks_status[]         = ":status=";
char            g_dfsvos2_ecsdisks_filesystem[]     = ":filesystem=";






/* Pointer to the named shared memory */
PVOID           shmem                       = NULL;

/* Pointer to the Widget State structure */
PWIDGETSTATE    pws                         = NULL;

bool            g_debug                     = true;
bool            g_the_very_first_time       = true;
bool            g_use_new_implementation    = false;

USBWidget*               g_myUSBWidget           = NULL;
DebugDialog*             g_myDebugDialog         = NULL;
ProblemFixerDialog*      g_ProblemFixerDialog    = NULL;
WidgetSettingsDialog*    g_WidgetSettingsDialog  = NULL;

void*           g_usbdev                = NULL;

char        drvletter_safeeject         = '?';
ULONG       drivemap_safeeject          = 0;
BOOL        foreign_eject               = true;
BOOL        warm_start                  = false;

/*
// Global drive masks before and after events.
*/

/* General */
ULONG       g_drives_before         = 0L;
ULONG       g_drives_after          = 0L;
ULONG       g_drives_changed        = 0L;
ULONG       g_drives_new            = 0L;
ULONG       g_drives_gone           = 0L;
ULONG       g_drives_temp           = 0L;

/* Eject */
ULONG       g_drives_before_eject           = 0L;
ULONG       g_drives_after_eject            = 0L;
ULONG       g_drives_changed_after_eject    = 0L;
ULONG       g_drives_new_after_eject        = 0L;       // Some drive could be added during the eject.
ULONG       g_drives_gone_after_eject       = 0L;       // This will be the drives on the disk ejected.

/* Insert */
ULONG       g_drives_before_insert          = 0L;
ULONG       g_drives_after_insert           = 0L;
ULONG       g_drives_changed_after_insert   = 0L;
ULONG       g_drives_new_after_insert       = 0L;       // It could be no drives are added.
ULONG       g_drives_gone_after_insert      = 0L;       // Some drives could be removed during this event.

/* Remove */
ULONG       g_drives_before_remove          = 0L;
ULONG       g_drives_after_remove           = 0L;
ULONG       g_drives_changed_after_remove   = 0L;
ULONG       g_drives_new_after_remove       = 0L;       // Some drives could be added during this event.
ULONG       g_drives_gone_after_remove      = 0L;       // If there are drives removed it could indicate a force remove ! compare drive with usbid.

/* Rediscover */
ULONG       g_drives_before_rediscover          = 0L;
ULONG       g_drives_after_rediscover           = 0L;
ULONG       g_drives_changed_after_rediscover   = 0L;   // Changed drives.
ULONG       g_drives_new_after_rediscover       = 0L;   // New drives.
ULONG       g_drives_gone_after_rediscover      = 0L;   // Removed drives.

/* Refresh */
ULONG       g_drives_before_refresh         = 0L;
ULONG       g_drives_after_refresh          = 0L;
ULONG       g_drives_changed_after_refresh  = 0L;       // Changed drives.
ULONG       g_drives_new_after_refresh      = 0L;       // New drives.
ULONG       g_drives_gone_after_refresh     = 0L;       // Removed drives.





/*
// Define module-global variables
*/
HPIPE   hpipe               = -1;
ULONG   drive_map_old       = 0;
ULONG   drive_map_new       = 0;
ULONG   drive_map_changed   = 0;
HWND    dialogs[NUM_DIALOGS];
HWND    windows[NUM_WINDOWS];


CHAR    g_buf[1024]         = "\0";

HAB     myhab               = 0;
HMODULE hmodMe              = NULL;

BOOL        enablebeeps         = 1;
BOOL        prev_enablebeeps    = 1;
BOOL        monitorzip          = 1;
BOOL        prev_monitorzip     = 1;

DRIVESTATUS drvstate[26];

char        Drives[26][128];

HFILE       PhysDisks[10];
HFILE       LogDrives[26];


BOOL    term            = FALSE;
BOOL    nosound         = FALSE;
BOOL    checkdrives     = FALSE;
BOOL    startupcheck    = FALSE;
BOOL    initialization  = TRUE;                 // First time ever to get usb-info

/*
// Lists of usb-devices.
*/
//~ CList   currentUSBDevicesList;                  // Current list
//~ CList   previousUSBDevicesList;                 // Previous list
//~ CList   removedUSBDevicesList;                  // Previous list


HWND        hwndWidget              = NULL;     // handle of widget window

HWND        hwndPopup               = NULL;     // popup menu
HWND        hdlgDebugDialog         = NULL;     // Debug Dialog
HWND        hdlgProblemFixerDialog  = NULL;     // Problem Fixer Dialog
HWND        hdlgNotification        = NULL;     // New dialog based notification window

HWND        hwndIndiDevInsert       = NULL;     // insert notifier window
HWND        hwndIndiSafeEject       = NULL;     // safe eject notifier window
HWND        hwndIndiDevRemove       = NULL;     // force remove notifier window
HWND        hwndIndiDrivesChanged   = NULL;     // drives changed notifier window



/*
// Icons for popup-menu.
*/
HPOINTER        hptrEject       = NULL;         // icon for widget
HPOINTER        hptrCD          = NULL;         // icon "CD"
HPOINTER        hptrHDD         = NULL;         // icon for LVM-managed drives
HPOINTER        hptrHDDBAD      = NULL;         // icon for drives with bad partition-table
HPOINTER        hptrHDDPHANTOM  = NULL;         // icon for phantom-drives (after force-remove)
HPOINTER        hptrZIP         = NULL;         // icon for zip-drives

/*
// Png icons for usb-devices.
*/
HBITMAP2        hbm2DevInserted = NULL;         // png for the device inserted
HBITMAP2        hbm2DevRemoved  = NULL;         // png for the device removed

HBITMAP2        hbm2Wireless    = NULL;         // png for wireless device
HBITMAP2        hbm2BlueTooth   = NULL;         // png for bluetooth device
HBITMAP2        hbm2Dasd        = NULL;         // png for general dasd-device
HBITMAP2        hbm2FlashDisk   = NULL;         // png for flashdisk
HBITMAP2        hbm2Zip250      = NULL;         // png for 250MB zipdrive
HBITMAP2        hbm2Zip750      = NULL;         // png for 750MB zipdrive
HBITMAP2        hbm2SDCard      = NULL;         // png for sd cards
HBITMAP2        hbm2SmartCard   = NULL;         // png for smartcard
HBITMAP2        hbm2SmartPhone  = NULL;         // png for smartphone

LANGE       BatteryLange;

/* Init to -1 since 0 in DosWaitThread would wait for the next thread to end */
TID     tid_lvm             = -1;
TID     tid_monitor         = -1;
TID     tid_eject           = -1;
TID     tid_zipdrive        = -1;
TID     tid_phantom_slayer  = -1;
TID     tid_drive_monitor   = -1;
TID     tid_wmquit_monitor  = -1;

/* Event Semaphores */
HEV     hevNeverPosted      = NULL;     // never posted to allow blocked delays
HEV     hevEventInserted    = NULL;     // posted when an usb-device is inserted
HEV     hevEventRemoved     = NULL;     // posted when an usb-device is removed
HEV     hevPushLVMthread    = NULL;     // posted when the LVM engine needs a refresh
HEV     hevPSthread         = NULL;     // posted when phantoms need to be purged
HEV     hevDMthread         = NULL;     // posted when a check from drive changes is needed

BOOL    lvm_thread_working              = TRUE;     // looped
BOOL    monitor_thread_working          = TRUE;     // looped
BOOL    phantom_slayer_thread_working   = TRUE;     // looped
BOOL    drive_monitor_thread_working    = TRUE;     // looped
BOOL    wmquit_monitor_thread_working   = TRUE;     // looped
BOOL    zipdrive_thread_working         = TRUE;     // looped
BOOL    eject_thread_working            = TRUE;     // non-looped

BOOL    phantom_slayer_periodic         = FALSE;    // do periodic phantom slaying

/* Access Semaphores */
HMTX    hmtxLVMaccess       = NULL;     // serialize access to the LVM-Engine
HMTX    hmtxAuxDebug        = NULL;     // serialize access to the Com Port

BOOL    file_logging        = FALSE;
BOOL    pipe_logging        = FALSE;
BOOL    dlg_logging         = FALSE;

/* Array of physical disks -- Note that entry 0 is not used as disks are numbered 1-based. */
//~ PHYSICAL_DISK   pdisks[100];
/* Array of logical drives -- Note that floppies A and B are included. */

/* Not Used */
//~ LOGICAL_DRIVE   ldrives[26];


// Root members were here

