/*****************************************************************************\
* APIHelpers.cpp -- NeoWPS * USB Widget                                       *
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


#include    "APIHelpers.hpp"


void    APIHelpers(void) {
}






/* Block on a never posted semaphore instead of using DosSleep */
APIRET  DosBlock(ULONG timeout) {
    APIRET  ulrc = -1;

    ulrc = DosWaitEventSem(hevNeverPosted, timeout);
    return ulrc;
}

APIRET  QueryFSDType(char drive, PFSDTYPE pfsdt) {
    APIRET      ulrc        = -1;
    FSDTYPE     fsdt        = FSDT_UNKNOWN;
    ULONG       buflen      = 0;
    CHAR        buf[256]    = "\0";
    PFSQBUFFER2 pfsqb       = (PFSQBUFFER2) buf;

    buflen = sizeof(buf);
    buf[0] = drive;
    buf[1] = ':';
    buf[2] = '\0';

    /* Query the attached FileSystem Driver */
    ulrc = DosQueryFSAttach(buf, 1, FSAIL_QUERYNAME, (PFSQBUFFER2) buf, &buflen);

    if (!ulrc) {
        if (!strcmpi((char*)pfsqb->szName+pfsqb->cbName+1, "FAT"))
            fsdt = FSDT_FAT;
        if (!strcmpi((char*)pfsqb->szName+pfsqb->cbName+1, "FAT32"))
            fsdt = FSDT_FAT32;
        if (!strcmpi((char*)pfsqb->szName+pfsqb->cbName+1, "HPFS"))
            fsdt = FSDT_HPFS;
        if (!strcmpi((char*)pfsqb->szName+pfsqb->cbName+1, "JFS"))
            fsdt = FSDT_JFS;
    }

    *pfsdt = fsdt;

    return ulrc;
}


/*
// PIPES
*/
APIRET  OpenPipe(VOID) {
    APIRET  ulrc = -1;
    ULONG   ulAction;
    ULONG   ulRead;
    ULONG   ulWrite=0;

    // Open Named Pipe
    ulrc =  DosOpen(PIPE_NAME,
                &hpipe,
                &ulAction,
                0,
                FILE_NORMAL,
                FILE_OPEN,
                OPEN_ACCESS_READWRITE |
                OPEN_SHARE_DENYNONE,
                (PEAOP2) NULL
            );

    return ulrc;
}


APIRET  WritePipe(PVOID buffer, ULONG length, PULONG written) {
    APIRET  ulrc = -1;
    ulrc = DosWrite(hpipe, buffer, length, written);
    return ulrc;
}

APIRET  WritePipe2(PVOID buffer) {
    APIRET  ulrc = -1;
    size_t  length;
    ULONG   written = 0;

    length = strlen((const char*)buffer);
    ulrc = DosWrite(hpipe, buffer, length, &written);
    return ulrc;
}

APIRET  ClosePipe(VOID) {
    APIRET  ulrc = -1;

    ulrc = DosClose(hpipe);

    return ulrc;
}


/*
// # Log to Pipe #
*/
//VOID cmnLogToPipe(const char *pcszSourceFile,       // in: source file name
VOID cmnLog(const char *pcszSourceFile,             // in: source file name
            ULONG ulLine,                           // in: source line
            const char *pcszFunction,               // in: function name
            const char *pcszFormat,                 // in: format string (like with printf)
            ... ){

    /* Locals */
    va_list args;
    BYTE    abBuf[PIPE_SIZE];
    ULONG   ulWritten=0;
    APIRET  ulrc;
    UCHAR   PipeName[256];
    CHAR    buf[16];
    CHAR    buf2[256];


    return;

    if (hpipe != -1) {

        DATETIME DT;
        DosGetDateTime(&DT);

        sprintf((char*)abBuf, "%s %s [%d]: ", tstamp(buf), pcszFunction, ulLine);
        va_start(args, pcszFormat);
        vsprintf(buf2, pcszFormat, args);
        va_end(args);
        strcat(buf2, "\n");
        strcat((char*)abBuf, buf2);
        ulrc = DosWrite(hpipe, (char*)abBuf, strlen((char*)abBuf), &ulWritten);
    }
}


/*
// Get the current timestamp
*/
PCHAR   tstamp(PCHAR buf) {
    DATETIME    dt;
    APIRET      ulrc;

    ulrc=DosGetDateTime(&dt);
    sprintf(buf, "%02d:%02d:%02d.%02d", dt.hours, dt.minutes, dt.seconds, dt.hundredths);
    return buf;
}

// BOOKMARK: Drive Functions (get-map, drive-count, letter-to-map, etc)
ULONG   drive_map_dqcd(VOID) {
    ULONG   ulDriveNum = 0;
    ULONG   ulDriveMap = 0;

    DosQueryCurrentDisk(&ulDriveNum, &ulDriveMap);
    return ulDriveMap;
}


ULONG   drive_count(ULONG drivemap) {
    int     i = 0;
    ULONG   count = 0;

    for (i=0; i<32; i++) {
        if (drivemap & 0x01)
            count++;
        drivemap >>= 1;
    }
    return count;
}


ULONG   drive_letter_to_map(char dletter) {
    ULONG   drivemap = 0;

    //~ drivemap = (dletter == '*' || dletter == '\0') ? 0 : 1;
    drivemap |= ((dletter >= 'A') && (dletter <= 'Z')) ? 1 : 0;
    drivemap |= ((dletter >= 'a') && (dletter <= 'a')) ? 1 : 0;

    dletter -= 'A';
    drivemap <<= dletter;
    drivemap &= 0x03FFFFFF;

    return drivemap;
}


PCHAR   drive_letters(PCHAR buf, ULONG drivemap) {
    int     i = 0;
    ULONG   count = 0;
    CHAR    dbuf[4];

    buf[0] = '\0';
    drivemap &= 0x03FFFFFF;

    for (i=0; i<32; i++) {
        if (drivemap & 0x01) {
            dbuf[0] = 'A'+(char)i;
            dbuf[1] = ':';
            dbuf[2] = ' ';
            dbuf[3] = '\0';
            strcat(buf, dbuf);
            count++;
        }
        drivemap >>= 1;
    }
    return buf;
}


char*   bin_digits_8(char* buf, unsigned char value) {
    int     i = 0;

    buf[0] = '\0';
    for (i=7; i>=0; i--) {
        buf[i] = (value & 0x01) ? '1' : '0';
        value >>= 1;
    }
    buf[8] = '\0';
    return buf;
}

char*   bin_digits_16(char* buf, unsigned short value) {
    int     i = 0;

    buf[0] = '\0';
    for (i=15; i>=0; i--) {
        buf[i] = (value & 0x01) ? '1' : '0';
        value >>= 1;
    }
    buf[16] = '\0';
    return buf;
}

char*   bin_digits_32(char* buf, unsigned long value) {
    int     i = 0;

    buf[0] = '\0';
    for (i=31; i>=0; i--) {
        buf[i] = (value & 0x01) ? '1' : '0';
        value >>= 1;
    }
    buf[32] = '\0';
    return buf;
}

/*
char*   bin_digits_64(char* buf, unsigned long long value) {
    int     i = 0;

    buf[0] = '\0';
    for (i=63; i>=0; i--) {
        buf[i] = (value & 0x01) ? '1' : '0';
        value >>= 1;
    }
    buf[64] = '\0';
    return buf;
}
*/

PCHAR   drive_digits(PCHAR buf, ULONG drivemap) {
    return bin_digits_32(buf, drivemap & DRIVE_MAP_MASK);
}

/*
// Determine drive changes.
*/
ULONG   drive_changes(  ULONG drives_before,
                        ULONG drives_after,
                        PULONG drives_changed,
                        PULONG drives_new,
                        PULONG drives_gone ){

    *drives_changed = drives_before ^ drives_after;     // Drives new and removed.
    *drives_new     = *drives_changed & drives_after;   // Only new drives.
    *drives_gone    = *drives_changed & drives_before;  // Only removed drives.

    return *drives_changed;
}


/*
// Dump drive changes.
*/
PCHAR   dump_drive_changes( PCHAR txtbuf,
                            ULONG drives_before,
                            ULONG drives_after,
                            ULONG drives_changed,
                            ULONG drives_new,
                            ULONG drives_gone ){
    /* Locals */
    char    bufs[5][256] = {"\0","\0","\0","\0","\0"};

    /* Get the drive changes */
    drive_changes(
        drives_before,
        drives_after,
        &drives_changed,
        &drives_new,
        &drives_gone
    );

    /* Compose message in buffer */
    sprintf(
        txtbuf,
        "  before  : %s\n  after   : %s\n  changed : %s\n  new     : %s\n  gone    : %s\n",
        drive_letters(bufs[0], drives_before),
        drive_letters(bufs[1], drives_after),
        drive_letters(bufs[2], drives_changed),
        drive_letters(bufs[3], drives_new),
        drive_letters(bufs[4], drives_gone)
    );

    return txtbuf;
}


/*
// MERGED FROM OTHER SOURCES
*/


//
// Note: _BPB etc. declarations moved to .h     (rousseau)
//

/*
// TODO: Improve Device Eject
// Recusivity
//  o Block additional menu clicks
//  o Implement timeout
// Change category
//  - test 1
//  - test 2
*/
// BOOKMARK: Eject Removable Drive (could be running on a thread)
ULONG EjectRemovableDrive(char *drive) {

    /* Locals */
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    USHORT              i=drive[0] - 'A';
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;
    USHORT              timeout = EJECT_TIMEOUT;


    /* Get the USB device corresponding to this drive */
    usbdev = DriveLetterToUsbDevice(drive[0]);

    //~ MessageBox("Debug Message", "Before Pending Test");

    if (usbdev == NULL) {
        return -3;
    }

    /*
    // This is not thread-safe and it returns from this funtion here.
    */
    //!: FIXME: Make thread safe and and single point of return
    if (usbdev->eject_pending || usbdev->safe_eject) {
        sprintf(buf, "eject is pending or is safe ejected [ep:%d, se:%d]", usbdev->eject_pending, usbdev->safe_eject);
        __debug("EjectRemovableDrive", buf, DBG_LBOX | DBG_AUX);
        return -1;
    }
    else {
        sprintf(buf, " USB device eject starting...");
        __debug("EjectRemovableDrive", buf, DBG_LBOX | DBG_AUX);
    }


    usbdev->eject_pending = TRUE;


    //~ MessageBox("Debug Message", "Before DosDevIOCtl->UNLOCK");

    /* Unlock */
    // Geeft error 87 -- invalid parameter !
    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_UNLOCKEJECTMEDIA,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    sprintf(buf, "unit=%d, ulrc=%d", i, ulrc);
    //~ __debug("DosDevIOCtl->UnLock", buf, DBG_LBOX);


    //~ MessageBox("Debug Message", "After DosDevIOCtl->UNLOCK");

    /* Clear eject-pending status */
    if (!ulrc && usbdev) {
        usbdev->eject_pending = FALSE;
    }


    //~ MessageBox("Debug Message", "Before DosDevIOCtl->EJECT-LOOP");

    // BOOKMARK : Wait for device ejection
    // FIXME    : recurse and timeout
    /* Eject untill succesful */
    while (ulrc && timeout) {
    //~ while (ulrc) {
        /* Eject */
        param.command = 2;
        param.unit = i;
        ulParmLen = sizeof(param);
        ulDataLen = sizeof(data);

        ulrc =  DosDevIOCtl(
                    -1,
                    IOCTL_DISK,
                    DSK_UNLOCKEJECTMEDIA,
                    &param,
                    sizeof(param),
                    &ulParmLen,
                    &data,
                    sizeof(data),
                    &ulDataLen
                );

        sprintf(buf, "DosDevIOCtl->Eject unit=%d, ulrc=%d", i, ulrc);
        __debug("DosDevIOCtl->Eject", buf, DBG_LBOX);

        //~ DosBlock(1000);
        MyDosSleep(1000);
        --timeout;
    }

    //~ MessageBox("Debug Message", "After DosDevIOCtl->EJECT-LOOP");

    /* Security wait */
    //~ DosBlock(500);
    MyDosSleep(500);

    //~ //~: Eject Timeout elapsed
    if (timeout == 0 && ulrc) {
        sprintf(buf, "  unit=%d, ulrc=%d; eject timeout: %d sec interval elapsed", i, ulrc, EJECT_TIMEOUT);
        //~ __debug("DosDevIOCtl->Eject", buf, DBG_LBOX);
        sprintf(buf, "Retry Eject");
        //~ __debug("Suggestion", buf, DBG_LBOX);
        usbdev->eject_pending = FALSE;
        return -2;
    }

    /* Set Safe Eject flag */
    if (usbdev) {
        BOOL    se1 = FALSE;
        BOOL    se2 = FALSE;
        usbdev->safe_eject = TRUE;
        usbdev->eject_pending = FALSE;
        se1 = usbdev->safe_eject;
        usbdev = DriveLetterToUsbDevice(drive[0]);
        se2 = usbdev->safe_eject;
        sprintf(buf, "%c:, from USB-DEVICE: %s -- %08X -- se1=%d, se2=%d -- Z=%08X, S=%08X", drive[0], usbdev->devicename, usbdev, se1, se2, &(usbdev->zipdrive), &(usbdev->safe_eject));

    }
    else {
        //~: Eject drive without corresponding device (after wps restart)
        sprintf(buf, "%c: -- NO CORRESPONDING USB-DEVICE", drive[0]);
    }
    //~ __debug("EJECTED DRIVE", buf, DBG_LBOX);

    return 0;
}


ULONG filesize(char *filename) {
    FILESTATUS3 fib;
    if (DosQueryPathInfo(filename, FIL_STANDARD, &fib, sizeof(FILESTATUS3))) return 0;
    if (fib.attrFile & FILE_DIRECTORY) return 0;
    return fib.cbFile;
}

BOOL file_exists(char *filename) {
    FILESTATUS3 f;
    if(!filename) return 0;
    return !DosQueryPathInfo(filename, FIL_STANDARD, &f, sizeof(FILESTATUS3)) && !(f.attrFile & FILE_DIRECTORY);
}

APIRET eSysInfo_QueryBootableDisk(char *bootdrv)
{
    ULONG   aulSysInfo[QSV_MAX] = {0};              // System Information Data Buffer
    APIRET  rc              = NO_ERROR;             // Return code

    if(bootdrv==0) return 1;

    rc =    DosQuerySysInfo(
                1L,                     // Request all available system
                QSV_MAX ,               // information
                (PVOID)aulSysInfo,      // Pointer to buffer
                sizeof(ULONG)*QSV_MAX   // Size of the buffer
            );

    if (rc != NO_ERROR) {
        return 1;
    }
    else {
        //printf("Bootable drive: %c:\n",
        //      aulSysInfo[QSV_BOOT_DRIVE-1]+'A'-1);    /* Max length of path name */
        bootdrv[0] = aulSysInfo[QSV_BOOT_DRIVE-1]+'A'-1;

        /*
        printf("Total physical memory is %u bytes.\n",
                aulSysInfo[QSV_TOTPHYSMEM-1]);
        */

        return 0;
    }


}

char    QueryBootDrive(VOID) {
    char    buf[16] = "\0";

    eSysInfo_QueryBootableDisk(buf);
    return buf[0];
}

/*
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

*/


// BOOKMARK: Get the device paramaters
char*   IOCtlGetDeviceParams(char drive, char* buf, int bufsize) {
    IOCTLPAR    param;
    IOCTLDATA   data;
    APIRET      ulrc;
    ULONG       ulParmLen = 0;
    ULONG       ulDataLen = 0;
    USHORT      i=drive - 'A';

    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETDEVICEPARAMS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    sprintf(
        buf,
        "DSK_GETDEVICEPARAMS: [%c:] ulrc=%d, ulParmLen=%d, ulDataLen=%d, cyls=%05d, devtype=%02X, devattr=%04X",
        drive,
        ulrc,
        ulParmLen,
        ulDataLen,
        data.cylinders,
        data.devtype,
        data.devattr
    );

    return buf;
}


char*   IOCtlIsBlockRemovable(char drive, char* buf, int bufsize) {
    IOCTLPAR    param;
    IOCTLDATA   data;
    APIRET      ulrc;
    ULONG       ulParmLen = 0;
    ULONG       ulDataLen = 0;
    USHORT      i=drive - 'A';

    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_BLOCKREMOVABLE,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    sprintf(
        buf,
        "DSK_BLOCKREMOVABLE: [%c:] ulrc=%d, ulParmLen=%d, ulDataLen=%d, removable=%0d",
        drive,
        ulrc,
        ulParmLen,
        ulDataLen,
        //(CHAR) *(PCHAR) &data);
        IOCtlIsPrmUndoc(drive)
    );

    return buf;
}


BOOL    IOCtlIsPrmUndoc(char drive) {
    BOOL        brc = FALSE;
    IOCTLPAR    param;
    IOCTLDATA   data;
    APIRET      ulrc;
    ULONG       ulParmLen = 0;
    ULONG       ulDataLen = 0;
    USHORT      i=drive - 'A';

    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETDEVICEPARAMS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    brc = !!((BOOL) data.devattr & 0x08);
    return brc;

}


BOOL    IOCtlIsCDROM(char drive) {
    BOOL        brc = FALSE;
    IOCTLPAR    param;
    IOCTLDATA   data;
    APIRET      ulrc;
    ULONG       ulParmLen = 0;
    ULONG       ulDataLen = 0;
    USHORT      i=drive - 'A';

    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETDEVICEPARAMS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    /*
    // A never mounted CDROM returns a devtype of 0x07.
    // Once a CDROM has been mounted the devtype will return 0x08, even
    // when later ejected.
    // It seems that the number of cylinders is always 65535.
    // These criteria might not be sufficient to identify a drive as a CDROM.
    */
    if (!ulrc && (data.devtype == 0x07 || data.devtype == 0x08)) {
        if ((data.devattr & 0x02) && (data.cylinders == 65535)) {
            brc = TRUE;
        }
    }

    return brc;
}


USHORT  IOCtlGetLockStatus(char drive, char* buf, int bufsize) {
    IOCTLPAR    param;
    IOCTLDATA   data;
    APIRET      ulrc;
    ULONG       ulParmLen = 0;
    ULONG       ulDataLen = 0;
    USHORT      i=drive - 'A';
    USHORT      status = 0;

    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETLOCKSTATUS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    if (buf) {
        sprintf(
            buf,
            "DSK_GETLOCKSTATUS: [%c:] ulrc=%d, ulParmLen=%d, ulDataLen=%d, status=%04X",
            drive,
            ulrc,
            ulParmLen,
            ulDataLen,
            data.bpb.usBytesPerSector
        );
    }

    return data.bpb.usBytesPerSector;
}


USHORT  IOCtlPurge(char drive, char* buf, int bufsize) {
    IOCTLPAR    param;
    IOCTLDATA   data;
    APIRET      ulrc;
    ULONG       ulParmLen = 0;
    ULONG       ulDataLen = 0;
    USHORT      i=drive - 'A';
    USHORT      status = 0;

    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_UNLOCKDRIVE,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    if (buf) {
        sprintf(
            buf,
            "DSK_PURGE: [%c:] ulrc=%d, ulParmLen=%d, ulDataLen=%d, status=%04X",
            drive,
            ulrc,
            ulParmLen,
            ulDataLen,
            data.bpb.usBytesPerSector
        );
    }

    return ulrc;
}


USHORT  IOCtlUnlockMedium(char drive, char* buf, int bufsize) {
    IOCTLPAR    param;
    IOCTLDATA   data;
    APIRET      ulrc;
    ULONG       ulParmLen = 0;
    ULONG       ulDataLen = 0;
    USHORT      i=drive - 'A';
    USHORT      status = 0;

    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETLOCKSTATUS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    if (buf) {
        sprintf(
            buf,
            "DSK_UNLOCKEJECTMEDIA: ulrc=%d, ulParmLen=%d, ulDataLen=%d",
            ulrc,
            ulParmLen,
            ulDataLen
        );
    }

    return ulrc;
}

void    MyDosSleep(ULONG msecs) {
    DosSleep(msecs);
}

USHORT  IOCtlUnlockMedium2(char drive, char* buf, int bufsize) {
    IOCTLPAR    param;
    IOCTLDATA   data;
    APIRET      ulrc;
    ULONG       ulParmLen = 0;
    ULONG       ulDataLen = 0;
    USHORT      i=drive - 'A';
    USHORT      status = 0;

    param.command = 0;
    param.unit = i;
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETLOCKSTATUS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    if (buf) {
        sprintf(
            buf,
            "DSK_UNLOCKEJECTMEDIA: ulrc=%d, ulParmLen=%d, ulDataLen=%d",
            ulrc,
            ulParmLen,
            ulDataLen
        );
    }

    return ulrc;
}


VOID    ShowPhysicalDiskHandles(void) {
    APIRET      ulrc            = -1;
    USHORT      IOCTLHandle     = -1;
    char        params[256]     = "\0";
    char        txtbuf[4096]    = "\n";
    int         i               = 0;

    /* Show Logical Drive Handles */
    for (i=0; i<sizeof(PhysDisks)/sizeof(HFILE); i++) {
        sprintf(
            params,
            "\t%d : %08X\n",
            i,
            PhysDisks[i]
        );
        strcat(txtbuf, params);
    }
    //~ __debug("PhysDisk Handles", txtbuf, DEBUG | DBG_MLE & ~DBG_LBOX);   // bad precedence !
    __debug("PhysDisk Handles", txtbuf, (DEBUG & ~DBG_LBOX) | DBG_MLE);
}


/*
// Open Physical Disk.
*/
APIRET  PhysOpen(char unit) {
    APIRET      ulrc            = -1;
    APIRET      ulrc2           = -1;
    USHORT      IOCTLHandle     = -1;
    char        params[256]     = "\0";
    char        buf[512]        = "\0";
    char        buf2[4096]      = "\0";
    char        txtbuf[4096]    = "\0";
    ULONG       bread           = 0;
    int         i               = 0;
    CARDINAL32  rc              = -1;
    CARDINAL32  rc2             = -1;


    /* Pre-read the first sector from the disk */
    Open_LVM_Engine(TRUE, &rc);
    if (rc == LVM_ENGINE_NO_ERROR || rc == LVM_ENGINE_ALREADY_OPEN) {
        Read_Sectors(unit - '0', 0L, 1, buf, &rc2);
    }
    /* Only close if it was sucessfully opened; don't close if it was already open */
    if (rc == LVM_ENGINE_NO_ERROR)
        Close_LVM_Engine();

    /* Get Handle to Entire Physical Disk */
    params[0]   = unit;
    params[1]   = ':';
    params[2]   = 0;

    /* Opening every number from '1' to '9' always succeeds; non-existand drives have CHS of 512/12/32 ~ 100MiB */
    /* 1-9 depends on number of dummy-drives in USBMSD. Once attached CHS gets changed, even after detach */
    ulrc =  DosPhysicalDisk(
                INFO_GETIOCTLHANDLE,
                &IOCTLHandle,
                sizeof(IOCTLHandle),
                params,
                strlen(params)+1
            );

    //ulrc = DosPhysicalDisk(INFO_GETIOCTLHANDLE, &IOCTLHandle, sizeof(IOCTLHandle),"1:" , 3);

    if (ulrc == NO_ERROR && PhysDisks[unit - '0'] == -1) {
        PhysDisks[unit - '0'] = 0;
        PhysDisks[unit - '0'] = (ULONG) IOCTLHandle;
    }

    sprintf(txtbuf, "\n\tulrc=%d\n\tunit=%c\n\tIOCTLHandle=%04X", ulrc, unit, IOCTLHandle);
    __debug("PhysOpen", txtbuf, (DEBUG & ~DBG_LBOX) | DBG_MLE);
    //~ __debug(NULL, "\n", DBG_MLE);

    sprintf(buf2, "Read_Sectors: rc:%d", rc2);
    __debug("PhysOpen", buf2, DBG_MLE);

    if (rc2 == LVM_ENGINE_NO_ERROR) {
        dump_page(buf, buf2, 0);
        __debug(NULL, buf2, DBG_MLE);
        dump_page(buf+256, buf2, 256);
        __debug(NULL, buf2, DBG_MLE);
    }

    ShowPhysicalDiskHandles();

    return ulrc;

}


/*
// CLose Physical Disk.
*/
APIRET  PhysClose(char unit) {
    APIRET      ulrc            = -1;
    USHORT      IOCTLHandle     = -1;
    char        params[256]     = "\0";
    char        txtbuf[4096]    = "\0";
    int         i               = 0;


    IOCTLHandle = (USHORT) PhysDisks[unit - '0'];

    /* Free Physical Disk Handle */
    if (IOCTLHandle != -1) {
        ulrc =  DosPhysicalDisk(
                    INFO_FREEIOCTLHANDLE,
                    NULL,
                    0,
                    &IOCTLHandle,
                    sizeof(IOCTLHandle)
                );
    }

    PhysDisks[unit - '0'] = -1;

    sprintf(txtbuf, "\n\tulrc=%d\n\tunit=%c\n\tIOCTLHandle=%04X", ulrc, unit, IOCTLHandle);
    __debug("PhysClose", txtbuf, (DEBUG & ~DBG_LBOX) | DBG_MLE);
    //~ __debug(NULL, "\n", DBG_MLE);

    ShowPhysicalDiskHandles();

    return ulrc;
}


VOID    ShowLogicalDriveHandles(void) {
    APIRET      ulrc            = -1;
    USHORT      IOCTLHandle     = -1;
    char        params[256]     = "\0";
    char        txtbuf[4096]    = "\n";
    int         i               = 0;
    int         s               = 0;

    /* Show Logical Drive Handles */
    s = sizeof(LogDrives)/sizeof(HFILE)/2;
    for (i=0; i<s; i++) {
        sprintf(
            params,
            "\t%c : %08X,\t%c : %08X\n",
            i+'A',
            LogDrives[i],
            i+'A'+s,
            LogDrives[i+s]
        );
        strcat(txtbuf, params);
    }
    __debug("LogDriveHandles", txtbuf, (DEBUG & ~DBG_LBOX) | DBG_MLE);
}


/*
// Open Logical Drive.
*/
APIRET  DasdOpen(char drive) {
    APIRET      ulrc            = -1;
    APIRET      ulrc2           = -1;
    USHORT      IOCTLHandle     = -1;
    char        params[256]     = "\0";
    char        buf[512]        = "\0";
    char        buf2[4096]      = "\0";
    char        txtbuf[4096]    = "\0";
    HFILE       fhandle         = NULL;
    ULONG       bread           = 0;
    ULONG       ulAction        = -1;

    params[0]   = drive;
    params[1]   = ':';
    params[2]   = 0;

    /* Open DASD */
    ulrc =  DosOpen(
                params,
                &fhandle,
                &ulAction,
                0,
                FILE_NORMAL,
                OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_FLAGS_DASD | OPEN_SHARE_DENYWRITE | OPEN_ACCESS_READONLY,
                NULL
            );

    if (ulrc == NO_ERROR && LogDrives[drive - 'A'] == -1) {
        LogDrives[drive - 'A'] = fhandle;
    }

    sprintf(txtbuf, "\n\tulrc=%d\n\tdrive=%c:\n\tfhandle=%08X\n\tulaction=%d", ulrc, drive, fhandle, ulAction);
    __debug("DasdOpen", txtbuf, (DEBUG & ~DBG_LBOX) | DBG_MLE);
    //~ __debug(NULL, "\n", DBG_MLE);

    ulrc2 = DosRead(fhandle, buf, 512, &bread);
    sprintf(buf2, "DosRead: ulrc:%d, bread:%d", ulrc2, bread);
    __debug("DasdOpen", buf2, DBG_MLE);
    if (bread) {
        dump_page(buf, buf2, 0);
        __debug(NULL, buf2, DBG_MLE);
        dump_page(buf+256, buf2, 256);
        __debug(NULL, buf2, DBG_MLE);
    }

    return ulrc;
}


/*
// CLose Logical Drive.
*/
APIRET  DasdClose(char drive) {
    APIRET      ulrc            = -1;
    USHORT      IOCTLHandle     = -1;
    char        params[256]     = "\0";
    char        txtbuf[4096]    = "\0";
    HFILE       fhandle         = NULL;


    fhandle = LogDrives[drive - 'A'];

    /* Close DASD */
    if (fhandle != 0) {
        ulrc = DosClose(fhandle);
    }

    LogDrives[drive - 'A'] = -1;

    sprintf(txtbuf, "\n\tulrc=%d\n\tdrive=%c:\n\tfhandle=%08X", ulrc, drive, fhandle);
    __debug("DasdClose", txtbuf, (DEBUG & ~DBG_LBOX) | DBG_MLE);

    return ulrc;
}


/*
// Read the PBR from a Volume / Drive.
*/
APIRET  ReadPBR(char drive, char* buf, ulong bufsize) {
    APIRET      ulrc        = -1;
    APIRET      ulrc2       = -1;
    HFILE       fhandle     = NULL;
    ULONG       bread       = 0;
    ULONG       ulAction    = -1;
    CHAR        buf2[16]    = "\0";

    /* Clear buffer */
    memset(buf, 0, bufsize);
    sprintf(buf, "NO PBR DATA READ");

    /* Misuse buf2 to construct drive-letter as string */
    sprintf(buf2, "%c:", drive);

    /* Open DASD */
    ulrc =  DosOpen(
                buf2,
                &fhandle,
                &ulAction,
                0,
                FILE_NORMAL,
                OPEN_ACTION_OPEN_IF_EXISTS,
                OPEN_FLAGS_DASD | OPEN_SHARE_DENYWRITE | OPEN_ACCESS_READONLY,
                NULL
            );

    if (!ulrc) {
        /* Read first sector from volume */
        ulrc2 = DosRead(fhandle, buf, 512, &bread);
        /* Close Volume */
        DosClose(fhandle);
    }

    return ulrc;
}


/*
// Check if the hidden sectors field is zero.
*/
BOOL    HiddenSecsZero(char drive) {
    APIRET  ulrc        = -1;
    char    buf[512]    = "\0";
    ULONG*  ulp         = (ULONG*) &buf[28];

    ulrc = ReadPBR(drive, buf, sizeof(buf));

    /* If ReadPBR successful return TRUE if hiddensecs is zero. */
    if (!ulrc)
        ulrc = (!*ulp);

    /* Returns -1 if failed, FALSE if hiddensecs<>0 and TRUE if hiddensecs==0 */
    return ulrc;
}




/*
// Show Device Info.
*/
VOID    UsbDevInfo(char drive, char unit) {
    IOCTLPAR                param;
    IOCTLDATA               data;
    APIRET                  ulrc;
    BOOL                    brc;
//  APIRET                  ulrc_dpd;
//  APIRET                  ulrc_do;
//  APIRET                  ulrc_ld;
    ULONG                   ulParmLen       = 0;
    ULONG                   ulDataLen       = 0;
    char                    buf[256]        = "\0";
    char                    params[256]     = "\0";
    char                    results[256]    = "\0";
    USBDeviceReport*        usbdev          = NULL;
    BOOL                    found           = FALSE;
    DEVICEPARAMETERBLOCK*   dpmb;
    USHORT                  IOCTLHandle     = -1;
    USHORT                  IOCTLHandle_0   = -1;
    USHORT                  IOCTLHandle_1   = -1;
    USHORT                  IOCTLHandle_2   = -1;
    UCHAR*                  ucptr           = NULL;
    USHORT*                 usptr           = NULL;
    HFILE                   fhandle         = NULL;
    HFILE                   uhandle         = NULL;
    ULONG                   ulAction        = -1;
    char                    txtbuf[32000]   = "\0";
    IPT                     ipt             = 0;
    CARDINAL32              dnumber         = -1;
    CARDINAL32              plba            = -1;
    char                    dletter         = '#';
    UCHAR                   unitid          = -1;
    ULONG                   drivemap        = 0;
    ULONG                   u               = 0;
    CHAR                    d               = '*';
    int                     i               = 0;




#if     DEBUG
    /* Initialize MLE */
    WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_SETTEXTLIMIT, (MPARAM) sizeof(txtbuf), (MPARAM) NULL);
    WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_FORMAT, (MPARAM) MLFIE_CFTEXT, (MPARAM) NULL);
    WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_SETIMPORTEXPORT, (MPARAM) txtbuf, (MPARAM) sizeof(txtbuf));
#endif

    /* Header */
    __debug("## UsbDevInfo ##", NULL, (DEBUG | DBG_MLE) & ~DBG_LBOX);

    /* Show Parameters */
    sprintf(txtbuf, "drive=%c, unit=%c%s", drive, unit, "\r\n\r\n");
    __debug("UsbDevInfo->PARAMETERS", txtbuf, DBG_MLE);

    /*
    // ####################
    // ## PHYSICAL STUFF ##
    // ####################
    */

    /* Count Physical Disks */
    ulrc =  DosPhysicalDisk(
                INFO_COUNT_PARTITIONABLE_DISKS,
                results,
                2,
                NULL,
                0
            );

    sprintf(txtbuf, "ulrc=%d, disks=%d", ulrc, (USHORT) results[0]);
    __debug("DosPhysicalDisk", txtbuf, DBG_MLE);

    IOCTLHandle = (USHORT) PhysDisks[unit - '0'];

    /* Get Physical Disk Device Parameters */
    param.command   = 0;
    param.unit      = 0;
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);

    ulrc =  DosDevIOCtl(
                IOCTLHandle,
                IOCTL_PHYSICALDISK,
                PDSK_GETPHYSDEVICEPARAMS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    dpmb = (DEVICEPARAMETERBLOCK*) &data;

    sprintf(
        txtbuf,
        "\n\tulrc=%d\n\tcommand=%d\n\tunit=%c\n\tcyls=%d\n\theads=%d\n\tspt=%d",
        ulrc,
        param.command,
        unit,
        dpmb->cCylinders,
        dpmb->cHeads,
        dpmb->cSectorsPerTrack
    );

    __debug("DosDevIOCtl->IOCTL_PHYSICALDISK->PDSK_GETPHYSDEVICEPARAMS", txtbuf, DBG_MLE);


    usptr = (USHORT*) dpmb;

    sprintf(
        txtbuf,
        "\n\tulrc=0\n\tr1=%04X\n\tcyls=%04X (%d)\n\tr2=%04X\n\tr3=%04X\n\tr4=%04X\n\tr5=%04X",
        usptr[0],
        usptr[1], usptr[1],
        usptr[4],
        usptr[5],
        usptr[6],
        usptr[7]
    );

    __debug("DosDevIOCtl->IOCTL_PHYSICALDISK->PDSK_GETPHYSDEVICEPARAMS-2", txtbuf, DBG_MLE);



    /*
    // ###################
    // ## LOGICAL STUFF ##
    // ###################
    */


    IOCTLHandle = (USHORT) LogDrives[drive - 'A'];

    /* Lock Drive */
    /*
    param.command   = 0;
    param.unit      = i;
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);


    ulrc =  DosDevIOCtl(
                IOCTLHandle,
                IOCTL_DISK,
                DSK_LOCKDRIVE,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    sprintf(
        txtbuf,
        " UsbDevInfo->DSK_LOCKDRIVE : ulrc=%d,\n\tIOCTLHandle=%04X",
        ulrc,
        IOCTLHandle
    );

    ulrc_ld = ulrc;

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_LOCKDRIVE", txtbuf, DBG_MLE);
    */


    /* Get Device Parameters (-1/0) */

    param.command   = 0;
    param.unit      = drive - 'A';
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);

    fhandle = LogDrives[drive - 'A'];

    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETDEVICEPARAMS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    sprintf(
        txtbuf,
        "\n\tulrc=%d\n\tfhandle=%08X\n\tunit=%04X (%c:)\n\tcyls=%04X\n\tdevtype=%02X\n\tdevattr=%04X",
        ulrc,
        -1,
        param.unit, drive,
        data.cylinders,
        data.devtype,
        data.devattr
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_GETDEVICEPARAMS(-1/0)", txtbuf, DBG_MLE);



    /* Get Device Parameters (-1/1) */

    param.command   = 1;
    param.unit      = drive - 'A';
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);


    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETDEVICEPARAMS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    sprintf(
        txtbuf,
        "\n\tulrc=%d\n\tfhandle=%04X\n\tunit=%08X (%c:)\n\tcyls=%04X\n\tdevtype=%02X\n\tdevattr=%04X",
        ulrc,
        -1,
        param.unit, drive,
        data.cylinders,
        data.devtype,
        data.devattr
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_GETDEVICEPARAMS(-1/1)", txtbuf, DBG_MLE);


    /* Get Device Parameters (handle/0) */

    param.command   = 0;
    param.unit      = drive - 'A';
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);


    fhandle = LogDrives[drive - 'A'];

    ulrc =  DosDevIOCtl(
                fhandle,
                IOCTL_DISK,
                DSK_GETDEVICEPARAMS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    sprintf(
        txtbuf,
        "\n\tulrc=%d\n\tfhandle=%08X\n\tunit=%04X (%c:)\n\tcyls=%04X\n\tdevtype=%02X\n\tdevattr=%04X",
        ulrc,
        fhandle,
        param.unit, drive,
        data.cylinders,
        data.devtype,
        data.devattr
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_GETDEVICEPARAMS(handle/0)", txtbuf, DBG_MLE);

    /* Get Device Parameters (handle/1) */

    param.command   = 1;
    param.unit      = drive - 'A';
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);


    fhandle = LogDrives[drive - 'A'];

    ulrc =  DosDevIOCtl(
                fhandle,
                IOCTL_DISK,
                DSK_GETDEVICEPARAMS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    sprintf(
        txtbuf,
        "\n\tulrc=%d\n\tfhandle=%08X\n\tunit=%04X (%c:)\n\tcyls=%04X\n\tdevtype=%02X\n\tdevattr=%04X",
        ulrc,
        fhandle,
        param.unit, drive,
        data.cylinders,
        data.devtype,
        data.devattr
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_GETDEVICEPARAMS(handle/1)", txtbuf, DBG_MLE);



    /* Get Lock Status (-1) */

    param.command   = 0;
    param.unit      = drive - 'A';
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);


    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_GETLOCKSTATUS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    sprintf(
        txtbuf,
        "\n\tulrc=%d\n\tfhandle=%08X\n\tunit=%04X (%c:)\n\tstatus=%04X",
        ulrc,
        -1,
        param.unit, drive,
        (USHORT) *((USHORT*) &data)
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_GETLOCKSTATUS(-1)", txtbuf, DBG_MLE);


    fhandle = LogDrives[drive - 'A'];

    /* Get Lock Status (handle) */

    param.command   = 0;
    param.unit      = drive - 'A';
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);


    ulrc =  DosDevIOCtl(
                fhandle,
                IOCTL_DISK,
                DSK_GETLOCKSTATUS,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    sprintf(
        txtbuf,
        " UsbDevInfo->DSK_GETLOCKSTATUS(handle) : ulrc=%d\n\tfhandle=%08X\n\tunit=%04X (%c:)\n\tstatus=%04X",
        ulrc,
        fhandle,
        param.unit, drive,
        (USHORT) *((USHORT*) &data)
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_GETLOCKSTATUS(handle)", txtbuf, DBG_MLE);


    ulrc =  UsbOpen(
                &uhandle,
                0x059b,
                0x0032,
                0x0400,
                0
            );

    sprintf(
        txtbuf,
        " UsbDevInfo->UsbOpen : ulrc=%d\n\tuhandle=%08X",
        ulrc,
        uhandle
    );

    __debug("UsbOpen(handle)", txtbuf, DBG_MLE);


    /* Get Lock Status (usb) */

    param.command   = 0;
    param.unit      = drive - 'A';
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);


    ulrc =  DosDevIOCtl(
            uhandle,
            IOCTL_DISK,
            DSK_GETLOCKSTATUS,
            &param,
            sizeof(param),
            &ulParmLen,
            &data,
            sizeof(data),
            &ulDataLen
        );


    sprintf(
        txtbuf,
        " UsbDevInfo->DSK_GETLOCKSTATUS(usb) : ulrc=%d\n\tuhandle=%08X\n\tunit=%04X (%c:)\n\tstatus=%04X",
        ulrc,
        uhandle,
        param.unit, drive,
        (USHORT) *((USHORT*) &data)
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_GETLOCKSTATUS(usb)", txtbuf, DBG_MLE);


    sprintf(
        txtbuf,
        "\n\tulrc=%d\n\tfhandle=%08X\n\tunit=%04X (%c:)\n\tstatus=%04X",
        ulrc,
        uhandle,
        param.unit,
        drive,
        (USHORT) *((USHORT*) &data)
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_GETLOCKSTATUS(usbhandle)", txtbuf, DBG_MLE);

    ulrc = UsbClose(uhandle);

    sprintf(
        txtbuf,
        "\n\tulrc=%d\n\tuhandle=%08X",
        ulrc,
        uhandle
    );

    __debug("UsbClose(handle)", txtbuf, DBG_MLE);


    brc =   Get_LVM_View(
                drive,
                &dnumber,
                &plba,
                &dletter,
                &unitid
            );


    if (!dletter)
        dletter='#';

    sprintf(
        txtbuf,
        "\n\tbrc=%d\n\tdrive=%c:\n\tdnumber=%08X\n\tplba=%08X\n\tdletter=%c\n\tunitid=%02X",
        brc,
        drive,
        dnumber,
        plba,
        dletter,
        unitid
    );

    __debug("Get_LVM_View", txtbuf, DBG_MLE);



    for (i=0; i<=12; i++) {
        sprintf(
            txtbuf,
            "pdisk[%02d]->usbid=%08X, safe_eject=%d, zipdrive=%d, phantom=%d, drivemap=%s%s",
            i,
            pws->pdisks[i].usbid,
            pws->pdisks[i].safe_eject,
            pws->pdisks[i].zipdrive,
            pws->pdisks[i].phantom,
            drive_letters(buf, pws->pdisks[i].drivemap),
            "\n"
        );
        __debug("Get_LVM_View", txtbuf, DBG_MLE);

    }

    __debug(NULL,"\n", DBG_MLE);

    u = DriveToDisk(drive);
    sprintf(
        txtbuf,
        "(%c:)     : %c",
        drive,
        u + '0'
    );

    __debug("DriveToDisk", txtbuf, DBG_MLE);


    drivemap = DriveMapFromDisk(unit - '0');
    drive_letters(buf, drivemap);
    d = FirstDriveFromMap(drivemap);
    if (!d) d='*';
    sprintf(
        txtbuf,
        "UsbDevInfo->DrivesOnThisDisk (%c) : %s, FirstDrive: %c:",
        unit,
        buf,
        d
    );

    __debug("DriveMapFromDisk(drives-on-this-disk)", txtbuf, DBG_MLE);


    drivemap = DriveMapFromDrive(drive);
    drive_letters(buf, drivemap);
    d = FirstDriveFromMap(drivemap);
    if (!d) d='*';
    sprintf(
        txtbuf,
        "(%c:): [%08X] %s, FirstDrive: %c:",
        drive,
        drivemap,
        buf,
        d
    );

    __debug("DriveMapFromDisk(sibling-drives)", txtbuf, DBG_MLE);
    __debug(NULL,"\n", DBG_MLE);


    drive_letters(buf, g_drives_before_eject);
    sprintf(txtbuf, "UsbDevInfo->DrivesBeforeEject     : %s", buf);

    __debug("DriveMapFromDisk(before-eject)", txtbuf, DBG_MLE);


    drive_letters(buf, g_drives_after_eject);
    sprintf(txtbuf, "UsbDevInfo->DrivesAfterEject      : %s", buf);

    __debug("DriveMapFromDisk(after-eject)", txtbuf, DBG_MLE);


    /*
    // Dump Drive Changes On Eject.
    */
    {
        /* Drive Maps */
        ULONG   drives_before   = NULL;     // before the event
        ULONG   drives_after    = NULL;     // after the event
        ULONG   drives_changed  = NULL;     // changed drives
        ULONG   drives_new      = NULL;     // new drives
        ULONG   drives_gone     = NULL;     // drives gone

        /* Get Drive Changes */
        drives_before   = g_drives_before_eject;
        drives_after    = g_drives_after_eject;
        drive_changes(drives_before, drives_after, &drives_changed, &drives_new, &drives_gone);

        /* Dump Header */
        __debug(NULL, "\nDRIVE-STATUS-ON-EJECT", DBG_MLE);

        /* Dump Drive Changes */
        dump_drive_changes(txtbuf, drives_before, drives_after, drives_changed, drives_new, drives_gone);
        __debug(NULL, txtbuf, DBG_MLE);
    }


    /*
    // Dump Drive Changes Rediscover Zip.
    */
    {
        /* Drive Maps */
        ULONG   drives_before   = NULL;     // before the event
        ULONG   drives_after    = NULL;     // after the event
        ULONG   drives_changed  = NULL;     // changed drives
        ULONG   drives_new      = NULL;     // new drives
        ULONG   drives_gone     = NULL;     // drives gone

        /* Get Drive Changes */
        drives_before   = g_drives_before_rediscover;
        drives_after    = g_drives_after_rediscover;
        drive_changes(drives_before, drives_after, &drives_changed, &drives_new, &drives_gone);

        /* Dump Header */
        __debug(NULL, "\nDRIVE-STATUS-ON-REDISCOVER-ZIP", DBG_MLE);

        /* Dump Drive Changes */
        dump_drive_changes(txtbuf, drives_before, drives_after, drives_changed, drives_new, drives_gone);
        __debug(NULL, txtbuf, DBG_MLE);
    }


    //OpenDriveView('C');



    /* UnLock Drive */
    /*
    // Klapt vast (trap).
    */
    /*
    param.command   = 0;
    param.unit      = drive - 'A';
    ulParmLen       = sizeof(param);
    ulDataLen       = sizeof(data);

    ulrc =  DosDevIOCtl(
                IOCTLHandle,
                IOCTL_DISK,
                DSK_UNLOCKDRIVE,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );


    sprintf(
        txtbuf, " UsbDevInfo->DSK_UNLOCKDRIVE : ulrc=%d\n\t%04X (%c:)\n\tIOCTLHandle=%04X",
        ulrc,
        param.unit, drive,
        IOCTLHandle
    );

    __debug("DosDevIOCtl->IOCTL_DISK->DSK_UNLOCKDRIVE", txtbuf, DBG_MLE);
    */
}



/*
// See on what physical disk this drive resides.
*/
ULONG   DriveToDisk(char drive) {
    BOOL                    brc;
    CARDINAL32              dnumber         = -1;
    CARDINAL32              plba            = -1;
    char                    dletter         = '#';
    UCHAR                   unitid          = -1;
    int                     rc = 0;

    brc =   Get_LVM_View(
                drive,
                &dnumber,
                &plba,
                &dletter,
                &unitid
            );

    if (brc)
        rc = dnumber;
    else
        rc = 0;

    return rc;

}


BOOL    DriveIsAccessible(char drive) {
    CHAR    szFileSpec[256];    /*  Address of the ASCIIZ path name of the file or subdirectory to be found. */
    HDIR    hdir;               /*  Address of the handle associated with this DosFindFirst request. */
    ULONG   flAttribute;        /*  Attribute value that determines the file objects to be searched for. */
    CHAR    findbuf[sizeof(FILEFINDBUF3)*100];       /*  Result buffer. */
    ULONG   cbBuf;              /*  The length, in bytes, of pfindbuf. */
    ULONG   cFileNames;         /*  Pointer to the number of entries. */
    ULONG   ulInfoLevel;        /*  The level of file information required. */
    APIRET  ulrc;               /*  Return Code. */
    CHAR    buf[256] = "\0";
    PFILEFINDBUF3   pff3 = NULL;

    sprintf(szFileSpec, "%c:\\*.*", drive);
    hdir = HDIR_CREATE;
    flAttribute =   FILE_ARCHIVED | FILE_DIRECTORY | FILE_SYSTEM |
                    FILE_HIDDEN | FILE_READONLY | FILE_NORMAL;
    cbBuf = sizeof(findbuf);
    cFileNames = 99;
    ulInfoLevel = FIL_STANDARD;

    ulrc = DosFindFirst(szFileSpec, &hdir, flAttribute,
            findbuf, cbBuf, &cFileNames, ulInfoLevel);

    /* FAT does not count . and .. but return with ERROR_NO_MORE_FILES */
    if (ulrc == ERROR_NO_MORE_FILES)
        ulrc = NO_ERROR;

    sprintf(buf, "found: %d, ulrc:%d", cFileNames, ulrc);
    __debug("DriveIsAccessible", buf, DBG_MLE);

    pff3 = &((PFILEFINDBUF3)findbuf)[5];
    __debug("DriveIsAccessible->Entry[0]", pff3->achName, DBG_MLE);

    DosFindClose(hdir);

    return (BOOL) !ulrc;
}



/*
// See what drives reside on this disk.
*/
ULONG   DriveMapFromDisk(ULONG disk) {
    ULONG   drivemap    = NULL;
    ULONG   tmpdisk     = NULL;
    int                 i = 0;

    if (!disk) return 0;

    for (i='A'; i<='Z'; i++) {
        tmpdisk = DriveToDisk(i);
        if (tmpdisk == disk) {
            drivemap |= drive_letter_to_map(i);
        }
    }

    return drivemap;
}

/*
// See what other drives also reside on the disk this drive resides on.
*/
ULONG   DriveMapFromDrive(char drive) {
    ULONG   drivemap    = NULL;
    ULONG   tmpdisk     = NULL;

    tmpdisk = DriveToDisk(drive);
    return DriveMapFromDisk(tmpdisk);
}


/*
// Get the first drive from the drivemap.
// This drive can be used to reconstruct the drivemap of sibling drives.
*/
CHAR    FirstDriveFromMap(ULONG drivemap) {
    int     i = 0;
    BOOL    found = FALSE;

    drivemap &= DRIVE_MAP_MASK;

    for (i=0; i<32; i++) {
        if (drivemap & (1<<i)) {
            found = TRUE;
            break;
        }
    }

    return found ? i+'A' : 0;
}


///
//!: -------------------------- PHANTOM PROBLEM -------------------------------
///

APIRET  LockDrive(char drive) {
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;
    HFILE               hdasd = NULL;


    sprintf(buf, "drive:%c", drive);
    __debug("LockDrive", buf, DBG_LBOX);

    ulrc = DasdOpen(drive);
    hdasd = LogDrives[drive - 'A'];

    /* Lock */
    param.command = 0;
    param.unit = drive - 'A';
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    /* Do the I/O */
    ulrc =  DosDevIOCtl(
                hdasd,
                IOCTL_DISK,
                DSK_LOCKDRIVE,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    sprintf(
        buf,
        "DosDevIOCtl->(hdasd)->IOCTL_DISK->DSK_LOCKDRIVE: command:%d, unit=%d, ulrc=%d",
        param.command,
        param.unit,
        ulrc
    );
    __debug("LockDrive", buf, DBG_LBOX);

    return ulrc;
}



APIRET  UnLockDrive(char drive) {
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;
    HFILE               hdasd = NULL;

    sprintf(buf, "drive:%c", drive);
    __debug("UnlockDrive", buf, DBG_LBOX);

    //~ ulrc = DasdOpen(drive);
    hdasd = LogDrives[drive - 'A'];

    /* Unlock */
    param.command = 0;
    param.unit = drive - 'A';
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    /* Do the I/O */
    ulrc =  DosDevIOCtl(
                hdasd,
                IOCTL_DISK,
                DSK_UNLOCKDRIVE,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    sprintf(
        buf,
        "DosDevIOCtl->(hdasd)->IOCTL_DISK->DSK_UNLOCKDRIVE: command:%d, unit=%d, ulrc=%d",
        param.command,
        param.unit,
        ulrc
    );
    __debug("UnLockDrive", buf, DBG_LBOX);

    DasdClose(drive);

    return ulrc;
}



APIRET  LockMedium(char drive) {
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;
    HFILE               hdasd = NULL;

    //~ hdasd = LogDrives[drive - 'A'];

    sprintf(buf, "drive:%c", drive);
    __debug("LockMedium", buf, DBG_LBOX);
    /* Lock */
    param.command = 1;
    param.unit = drive - 'A';
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    /* Do the I/O */
    ulrc =  DosDevIOCtl(
                -1,
                //~ hdasd,
                IOCTL_DISK,
                DSK_UNLOCKEJECTMEDIA,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    sprintf(
        buf,
        "DosDevIOCtl->(-1)->IOCTL_DISK->DSK_UNLOCKEJECTMEDIA: command:%d, unit=%d, ulrc=%d",
        param.command, param.unit,
        ulrc
    );
    __debug("LockDrive", buf, DBG_LBOX);

    return ulrc;
}


/* This is actually EjectMedium */
APIRET  EjectDrive(char drive) {
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;

    sprintf(buf, "drive:%c", drive);
    __debug("EjectDrive", buf, DBG_LBOX);
    /* Eject */
    param.command = 2;
    param.unit = drive - 'A';
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    /* Do the I/O */
    ulrc =  DosDevIOCtl(
                -1,
                IOCTL_DISK,
                DSK_UNLOCKEJECTMEDIA,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    sprintf(
        buf,
        "DosDevIOCtl->(-1)->IOCTL_DISK->DSK_UNLOCKEJECTMEDIA: command:%d, unit=%d, ulrc=%d",
        param.command, param.unit,
        ulrc
    );
    __debug("EjectDrive", buf, DBG_LBOX);

    return ulrc;
}


APIRET  RedetermineMedia(char drive) {
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;
    HFILE               hdasd = FALSE;

    sprintf(buf, "drive:%c", drive);
    __debug("RedetermineMedia", buf, DBG_LBOX);

    ulrc = LockDrive(drive);
    hdasd = LogDrives[drive - 'A'];


    /* Redetermine */
    param.command = 0;
    param.unit = drive - 'A';
    ulParmLen = sizeof(param);
    ulDataLen = sizeof(data);

    /* Do the I/O */
    ulrc =  DosDevIOCtl(
                //~ -1,
                hdasd,
                IOCTL_DISK,
                DSK_REDETERMINEMEDIA,
                &param,
                sizeof(param),
                &ulParmLen,
                &data,
                sizeof(data),
                &ulDataLen
            );

    sprintf(
        buf,
        "DosDevIOCtl->(-1)->IOCTL_DISK->DSK_REDETERMINEMEDIA: command:%d, unit=%d, ulrc=%d",
        param.command, param.unit,
        ulrc
    );
    __debug("RedetermineMedia", buf, DBG_LBOX);

    UnLockDrive(drive);

    return ulrc;
}

APIRET  GetFSAttachInfo(char drive) {
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    char                qbuf[1024] = "\0";
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;
    PFSQBUFFER2         pfsqb = (PFSQBUFFER2) qbuf;
    ULONG               buflen = 1024;
    sprintf(buf, "drive:%c", drive);
    __debug("DosQueryFSAttach", buf, DBG_LBOX);

    qbuf[0] = drive;
    qbuf[1] = ':';
    qbuf[2] = '\0';
    ulrc = DosQueryFSAttach(qbuf, 1, FSAIL_QUERYNAME, (PFSQBUFFER2) qbuf, &buflen);

    sprintf(buf, "drive:%c, ulrc:%d", drive, ulrc);
    __debug("DosQueryFSAttach", buf, DBG_LBOX);

    sprintf(buf, "iType:%d, szName:%s, szFSDName:%s",
        pfsqb->iType,
        pfsqb->szName,
        pfsqb->szName+pfsqb->cbName+1
    );
    __debug("FSQBUFFER", buf, DBG_LBOX);

    qbuf[0] = drive;
    qbuf[1] = ':';
    qbuf[2] = '\0';
    buf[0] = '\0';
    buf[1] = '\0';
    buf[2] = '\0';
    ulrc = DosFSAttach(qbuf, "FAT32", buf, 0, FS_DETACH);

    sprintf(buf, "drive:%c, ulrc:%d", drive, ulrc);
    __debug("DosFSAttach(FS_DETACH)", buf, DBG_LBOX);

    return ulrc;
}




/*
// Query the (phantom) Drive.
*/
//!: BOOKMARK: QUERY DRIVE -- USE HERE TO TACKLE PHANTOMS
VOID    QueryDrive(char drive) {
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    BOOL                brc = FALSE;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;
    FSDTYPE             fsdt = FSDT_UNKNOWN;
    ULONG               pp = 0;
    ULONG               pc = 0;

    /* Show Parameters */
    sprintf(buf, "QueryDrive->PARAMETERS drive=%c", drive);
    __debug(NULL, buf, DBG_LBOX);

    /* Show Device Parameters */
    IOCtlGetDeviceParams(drive, buf, sizeof(buf));
    __debug("DosDevIOCtl", buf, DBG_LBOX);

    /* Is this a CDROM drive ? */
    brc = IOCtlIsCDROM(drive);
    sprintf(buf, "CDROM:%d", brc);
    __debug("IOCtlIsCDROM", buf, DBG_LBOX);

    /* Sub Calls */
    RedetermineMedia(drive);
    GetFSAttachInfo(drive);

    DriveIsAccessible(drive);

    ulrc = QueryFSDType(drive, &fsdt);
    sprintf(buf, "Drive: %c:, fsdtype:%d, ulrc: %d", drive, fsdt, ulrc);
    __debug("QueryFSDType", buf, DBG_LBOX);

    pp = (BOOL) PhantomsPresent();
    sprintf(buf, "%d", pp);
    __debug("PhantomsPresent", buf, DBG_LBOX);

    pc = PurgeAllPhantoms();
    sprintf(buf, "phantoms-purged:%d", pc);
    __debug("PurgeAllPhantoms", buf, DBG_LBOX);

    return;
}




/*
// Purge the (phantom) Drive.
*/
//!: BOOKMARK: PURGE DRIVE -- USE HERE TO TACKLE PHANTOMS
VOID    PurgeDrive(char drive) {
    IOCTLPAR            param;
    IOCTLDATA           data;
    APIRET              ulrc;
    ULONG               ulParmLen = 0;
    ULONG               ulDataLen = 0;
    char                buf[256] = "\0";
    USBDeviceReport*    usbdev = NULL;
    BOOL                found = FALSE;

    /* Show Parameters */
    sprintf(buf, "PurgeDrive->PARAMETERS drive=%c", drive);
    __debug(NULL, buf, DBG_LBOX);

    /* Sub Calls */
    //~ LockDrive(drive);
    //~ LockMedium(drive);
    //~ EjectDrive(drive);
    sprintf(buf, "cmd /c eject %c:", drive);
    system(buf);
    __debug("Command", buf, DBG_LBOX);
    //~ DosBlock(100);

    return;
}


/*
// Purge all the drive-letters that are phantoms.
// Their state is not cleared because they will come back after a refresh prms
// or a refresh of the LVM Engine or in some other situation where the
// drive gets accessed.
*/
ULONG   PurgeAllPhantoms() {
    int     i = 0;
    ULONG   c = 0;

    /*
    // Iterate over the phantom array and purge each drive that is a phantom.
    // Drive letters A: and B: can never be phantoms because they are reserved
    // for floppy disks. C: could be a phantom if the user has no C: partition.
    */
    for (i=2; i<sizeof(pws->phantoms); i++) {
        if (pws->phantoms[i]) {
            PurgeDrive('A'+i);
            c++;
        }
    }

    /* Return number of phantoms purged */
    return (ULONG) c;
}

/*
// Check if any phantoms are present.
*/
BOOL    PhantomsPresent() {
    int     i = 0;
    BOOL    brc = FALSE;

    /* OR all phantom states; if result != 0 phantoms are present */
    for (i=2; i<sizeof(pws->phantoms); i++) {
        brc |= (BOOL) pws->phantoms[i];
    }

    return brc;
}






/*
// Query the Default Browser.
*/
APIRET  QueryDefaultBrowser(char* cmdline) {
    APIRET  ulrc        = -1;
    ULONG   ulLength    = 0;
    char    buf[256]    = "\0";

    cmdline[0] = '\0';
    ulLength =  PrfQueryProfileString(
                    HINI_USERPROFILE,
                    "WPURLDEFAULTSETTINGS",
                    "DefaultBrowserExe",
                    NULL,
                    buf,
                    sizeof(buf)
                );

    if (ulLength) {
        strcpy(cmdline, buf);
        ulrc = NO_ERROR;
    }
    else {
        ulrc = ERROR_PATH_NOT_FOUND;
    }

    return ulrc;
}





/*
// FIREFOX!.EXE is used instead of FIREFOX.EXE because the former sets-up the
// correct environment for FireFox. The latter cannot find the XUL stuff.
*/
APIRET  StartFireFoxAsSession(char* cmdline) {
    APIRET      rc          = -1;
    char        bd          = '\0';
    char        buf[256]    = "\0";

    //bd=QueryBootDrive();
    //sprintf(buf,"%c:\\Programs\\FireFox\\FIREFOX!.EXE", bd);
    rc = QueryDefaultBrowser(buf);


    if (rc == NO_ERROR) rc=StartSession(buf, cmdline);

    return rc;
}



/*
// Deze hangt ineens vast !
// Met nieuwe !k executable.
// Heeft denk ik te maken met stay-hatched feature en zou dus niet SYNC
// maar ASYNC moeten worden gestart.
// StartSession lijkt me sowieso beter...
*/

/*
// Note that FIREFOX!.EXE is started SYNC in ExecProgram() !
// This is because FIREFOX!.EXE is just a front-end and returns as soon as the
// real starting is on it's way.
// FIREFOX!.EXE is used instead of FIREFOX.EXE because the former sets-up the
// correct environment for FireFox. The latter cannot find the XUL stuff.
*/
APIRET  StartFireFoxAsProgram(char* cmdline) {
    APIRET      rc          = -1;
    char        bd          = '\0';
    char        buf[256]    = "\0";

    //bd=QueryBootDrive();
    //sprintf(buf,"%c:\\Programs\\FireFox\\FIREFOX!.EXE", bd);
    rc = QueryDefaultBrowser(buf);


    if (rc == NO_ERROR) rc=ExecProgram(buf, cmdline);

    return rc;
}


APIRET  StartDaemon(char* progname) {
    APIRET      rc          = -1;
    rc = ExecProgramDetached(progname, "");
    return rc;
}


APIRET  StartDaemonAsSession(char* progname) {
    APIRET      rc          = -1;
    char        buf[256]    = "\0";

    sprintf(buf, "START %s", progname);

    //~ rc = StartSession("CMD.EXE", progname);

    system(buf);

    return 0;
}


APIRET  ExecProgram(char* progname, char* cmdline) {
    APIRET      rc          = -1;
    RESULTCODES crc         = {-1,-1};
    char        bd          = '\0';
    char        buf[256]    = "\0";
    char        buffer[512] = "\0";     // Error buffer

    bd=QueryBootDrive();

    memset(buf, 0, sizeof(buf));
    strcpy(buf, progname);
    strcpy(buf+strlen(buf)+1, cmdline);

    rc =    DosExecPgm(
                buffer,
                sizeof(buffer),
                EXEC_SYNC,
                buf,
                NULL,
                &crc,
                progname
            );

    return rc;
}


APIRET  ExecProgramDetached(char* progname, char* cmdline) {
    APIRET      rc          = -1;
    RESULTCODES crc         = {-1,-1};
    char        bd          = '\0';
    char        buf[256]    = "\0";
    char        buffer[512] = "\0";     // Error buffer

    memset(buf, 0, sizeof(buf));
    strcpy(buf, progname);
    strcpy(buf+strlen(buf)+1, cmdline);

    rc =    DosExecPgm(
                buffer,
                sizeof(buffer),
                EXEC_BACKGROUND,
                buf,
                NULL,
                &crc,
                progname
            );

    return rc;
}


APIRET  StartSession(char* progname, char* cmdline) {
    APIRET      rc;                         // Return Code from API-Calls
    STARTDATA   sd;                         // Structure for DosStartSession
    PID         pidSession;                 // Session ID
    PID         pidProcess;                 // Process ID
    TID         tid;                        // Thread ID
    char        buffer[512]="\0";           // Error buffer


    sd.Length=sizeof(STARTDATA);            // Size of this structure
    sd.Related=SSF_RELATED_INDEPENDENT;     // Relationship with calling process (_CHILD causes DosStartSession() to fail when TasmFe is run from the WorkFrame)
    sd.FgBg=SSF_FGBG_BACK;                  // Start session in Fore or Background
    sd.TraceOpt=SSF_TRACEOPT_NONE;          // No Trace Options
    //sd.PgmTitle=szPgmTitle;               // Session Title
    sd.PgmTitle="XX";

    //sd.PgmName=szPgmName;                 // Command to execute now fully specified so assign it
    sd.PgmName=progname;



    //sd.PgmInputs=command;                 // Parameters to command
    sd.PgmInputs=cmdline;



    sd.TermQ=NULL;                          // Queue
    sd.Environment=NULL;                    // Inherit process environment
    sd.InheritOpt=SSF_INHERTOPT_SHELL;      // Inherit from shell

    sd.SessionType=SSF_TYPE_PM;             // PM Session

    sd.IconFile=NULL;                       // No Icon File
    sd.PgmHandle=NULL;                      // Not used
    sd.PgmControl=SSF_CONTROL_INVISIBLE;    // Invisible Session
    sd.InitXPos=0;                          // X-Position (not used)
    sd.InitYSize=0;                         // Y-Position (not used)
    sd.InitXSize=0;                         // X-Size     (not used)
    sd.InitYSize=0;                         // Y-Size     (not used)
    sd.Reserved=0;                          // As it says
    sd.ObjectBuffer=buffer;                 // Scratch Buffer
    sd.ObjectBuffLen=sizeof(buffer);        // Length of Scratch Buffer

    rc=DosStartSession(&sd, &pidSession, &pidProcess);

    buffer[sizeof(buffer)-1]=0;

    sprintf(buffer,"DosStartSession: rc=%d",rc);
    //~ __debug("StartSession", buffer, DBG_LBOX);

    return rc;
}


VOID    AllocSharedMem() {
    APIRET  ulrc = -1;
    PVOID   address = NULL;
    char    buf[256] = "\0";

    ulrc = DosAllocSharedMem(&address, SHMEM_NAME, SHMEM_SIZE, PAG_COMMIT | PAG_READ | PAG_WRITE);
    sprintf(buf, "AllocSharedMem: ulrc=%d, address=%08X", ulrc, address);
    __debug(NULL, buf, DBG_LBOX);

    shmem = address;

}

VOID    GetSharedMem() {
    APIRET  ulrc = -1;
    PVOID   address = NULL;
    char    buf[256] = "\0";

    ulrc = DosGetNamedSharedMem(&address, SHMEM_NAME, PAG_READ | PAG_WRITE);
    sprintf(buf, "GetSharedMem: ulrc=%d, address=%08X", ulrc, address);
    __debug(NULL, buf, DBG_LBOX);

    shmem = address;
}


VOID    FreeSharedMem() {
    APIRET  ulrc = -1;
    PVOID   address = NULL;
    char    buf[256] = "\0";

    address = shmem;

    ulrc = DosFreeMem(address);
    sprintf(buf, "FreeSharedMem: ulrc=%d, address=%08X", ulrc, address);
    __debug(NULL, buf, DBG_LBOX);
}



/*
// Allocate the sub block.
// If succesfull, initialize the 8-byte structure at its beginning,
// and return a pointer to the address following the structure.
*/
APIRET  DosSubAllocMemEx(PVOID pbBase, PPVOID ppb, ULONG cb) {
    APIRET          ulrc        = -1;
    PSUBMEMBLOCK    psmb        = NULL;
    char            buf[256]    = "\0";

    // FIXME: Needs access protection ! (not mt-safe !)

    /* Allocate the sub block from the base block */
    ulrc = DosSubAllocMem(pbBase, (PPVOID) &psmb, cb + sizeof(SUBMEMBLOCK));
    sprintf(buf, "DosSubAllocMemEx: ppbase: %08lX, psmb: %08lX, size: %d, ulrc: %d", pbBase, psmb, cb + sizeof(SUBMEMBLOCK), ulrc);
    //~ MessageBox("DosSubAllocMemEx", buf);
    if (!ulrc) {
        psmb->base = pbBase;                    // set pointer to base block
        psmb->size = cb + sizeof(SUBMEMBLOCK);  // set size of this loocated sub block
        *ppb = &psmb[1];                        // return pointer to requested space
    }

    return ulrc;
}


/*
// DeAllocate the sub block.
// The base block it is sub allocated from and it's size are in the structure
// preceding the block.
// These values are used for DosSubFreeMem().
*/
APIRET  DosSubFreeMemEx(PVOID ppb) {
    APIRET          ulrc    = 0;
    PSUBMEMBLOCK    psmb    = NULL;

    psmb = (PSUBMEMBLOCK) ppb;  // Cast to SUBMEMBLOCK pointer
    psmb--;                     // Point to base and size info

    /* Free the sub block using this info */
    ulrc = DosSubFreeMem(psmb->base, psmb, psmb->size);

    return ulrc;
}

