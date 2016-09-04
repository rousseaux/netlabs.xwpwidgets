/*****************************************************************************\
* APIHelpers.hpp -- NeoWPS * USB Widget                                       *
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
 * This module contains several wrapper functions around the OS/2 API.
 * It is used to simplify things.
 */


#ifndef     __APIHELPERS_HPP__
#define     __APIHELPERS_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif


#include    "ModuleGlobals.hpp"
//#include  "USBHelpers.hpp"

class   Drive {
};


/* Prototypes */
void    APIHelpers();
APIRET  DosBlock(ULONG timeout);                                    // Block on never posted semaphore instead of using DosSleep
APIRET  QueryFSDType(char drive, PFSDTYPE pfsdt);                   // Query the type of the FileSystem Driver used for the drive
APIRET  OpenPipe(VOID);                                             // Open the LogPipe
APIRET  WritePipe(PVOID buffer, ULONG length, PULONG written);      // Write to the pipe using DosWrite()
APIRET  WritePipe2(PVOID buffer);                                   // Write a zero-term-string to the pipe using DosWrite()
VOID    cmnLog(const char*, ULONG, const char*, const char*, ...);  // Widget log-function (normally to pipe but can also be to file)
APIRET  ClosePipe(VOID);                                            // Close the LogPipe
PCHAR   tstamp(PCHAR buf);                                          // Get a printable time-stamp
ULONG   drive_map_dqcd(VOID);                                       // Get a drive-map using DosQueryCurrentDisk()
ULONG   drive_count(ULONG drivemap);                                // Count the drives in this map
ULONG   drive_letter_to_map(char dletter);                          // Convert a drive-letter to a drivemap
PCHAR   drive_letters(PCHAR buf, ULONG drivemap);                   // Printable map of drive-letters

char*   bin_digits_8(char* buf, unsigned char value);               // ASCII string of 8 binary digits
char*   bin_digits_16(char* buf, unsigned short value);             // ACSII string of 16 binary digits
char*   bin_digits_32(char* buf, unsigned long value);              // ACSII string of 32 binary digits
//char* bin_digits_64(char* buf, unsigned long long value);         // ACSII string of 64 binary digits

PCHAR   drive_digits(PCHAR buf, ULONG drivemap);                    // Printable map of drives; 0=not present, 1=present
ULONG   drive_changes(  ULONG drives_before,
                        ULONG drives_after,
                        PULONG drives_changed,
                        PULONG drives_new,
                        PULONG drives_gone);
PCHAR   dump_drive_changes(
            PCHAR txtbuf,
            ULONG drives_before,
            ULONG drives_after,
            ULONG drives_changed,
            ULONG drives_new,
            ULONG drives_gone
        );

APIRET  EjectDrive(char drive);
ULONG   EjectRemovableDrive(char*);                                 // Use IOCtl to eject a removable drive
ULONG   filesize(char *filename);
BOOL    file_exists(char *filename);
APIRET  eSysInfo_QueryBootableDisk(char *bootdrv);
char*   IOCtlGetDeviceParams(char drive, char* buf, int bufsize);
char*   IOCtlIsBlockRemovable(char drive, char* buf, int bufsize);
BOOL    IOCtlIsPrmUndoc(char drive);
BOOL    IOCtlIsCDROM(char drive);
USHORT  IOCtlGetLockStatus(char drive, char* buf, int bufsize);
USHORT  IOCtlPurge(char drive, char* buf, int bufsize);

APIRET  PhysOpen(char unit);
APIRET  PhysClose(char unit);
APIRET  DasdOpen(char drive);
APIRET  DasdClose(char drive);
APIRET  ReadPBR(char drive, char* buf, ulong bufsize);
BOOL    HiddenSecsZero(char drive);
VOID    ShowLogicalDriveHandles(void);

VOID    UsbDevInfo(char drive, char unit);
VOID    QueryDrive(char drive);
VOID    PurgeDrive(char drive);

ULONG   PurgeAllPhantoms();
BOOL    PhantomsPresent();

ULONG   DriveToDisk(char drive);
BOOL    DriveIsAccessible(char drive);
ULONG   DriveMapFromDisk(ULONG disk);
ULONG   DriveMapFromDrive(char drive);
CHAR    FirstDriveFromMap(ULONG drivemap);

char    QueryBootDrive(VOID);
APIRET  QueryDefaultBrowser(char* cmdline);
APIRET  StartFireFoxAsSession(char* cmdline);
APIRET  StartFireFoxAsProgram(char* cmdline);
APIRET  StartDaemon(char* progname);
APIRET  StartDaemonAsSession(char* progname);
APIRET  ExecProgram(char* progname, char* cmdline);
APIRET  ExecProgramDetached(char* progname, char* cmdline);
APIRET  StartSession(char* progname, char* cmdline);

VOID    AllocSharedMem();
VOID    GetSharedMem();
VOID    FreeSharedMem();

/* Special wrappers */
APIRET  DosSubAllocMemEx(PVOID pbBase, PPVOID ppb, ULONG cb);
APIRET  DosSubFreeMemEx(PVOID ppb);

void    MyDosSleep(ULONG msecs);



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

/*
// IOCtl Stuff
*/
typedef struct _BPB {
    USHORT      usBytesPerSector;       // Number of bytes per sector
    BYTE        bSectorsPerCluster;     // Number of sectors per cluster
    USHORT      usReservedSectors;      // Number of reserved sectors
    BYTE        cFATs;                  // Number of FATs
    USHORT      cRootEntries;           // Number of root directory entries
    USHORT      cSectors;               // Number of sectors
    BYTE        bMedia;                 // Media descriptor
    USHORT      usSectorsPerFAT;        // Number of secctors per FAT
    USHORT      usSectorsPerTrack;      // Number of sectors per track
    USHORT      cHeads;                 // Number of heads
    ULONG       cHiddenSectors;         // Number of hidden sectors
    ULONG       cLargeSectors;          // Number of large sectors
    BYTE        abReserved[6];          // Reserved
} BPB;


typedef struct _IOCTLPAR {
    UCHAR       command;
    USHORT      unit;
} IOCTLPAR;

typedef struct _IOCTLDATA {
    BPB         bpb;
    USHORT      cylinders;
    UCHAR       devtype;
    USHORT      devattr;
} IOCTLDATA;



#ifdef      __cplusplus
    }
#endif

#endif // __APIHELPERS_HPP__
