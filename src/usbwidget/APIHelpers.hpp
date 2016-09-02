/*****************************************************************************\
* APIHelpers.hpp -- NeoWPS * USB Widget                                       *
*                                                                             *
* Copyright (c) 2014 RDP Engineering                                          *
*                                                                             *
* Author: Ben Rietbroek <rousseau.ecsdev@gmail.com>                           *
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


#ifdef      __cplusplus
    }
#endif

#endif // __APIHELPERS_HPP__
