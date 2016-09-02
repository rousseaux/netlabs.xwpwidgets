/*****************************************************************************\
* ProblemFixer.cpp -- NeoWPS * USB Widget                                     *
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
 * This module implements the stuff needed to fix an LVM hostile disk layout.
 */

#include    "ProblemFixer.hpp"


/*
// Dump a 16-byte memory block in HEX to a buffer.
*/
void    dump_para(char* p, char* b, ULONG address) {
    int     i = 0;
    char    c = '\0';
    char    c2 = '\0';

    sprintf(b, "%08lX:    ", address);
    b += 13;

    for (i=0; i<16; i++) {
        c = p[i];
        c2 = c & 0xf0;
        c2 >>= 4;
        c2 += '0';
        if (c2 > '9')
            c2 += 7;
        *b++ = c2;
        c2 = c & 0x0f;
        c2 += '0';
        if (c2 > '9')
            c2 += 7;
        *b++ = c2;
        *b++ = ' ';
    }

    *b++ = ' ';
    *b++ = ' ';
    *b++ = ' ';

    for (i=0; i<16; i++) {
        c = p[i];
        if (c < 0x20 || c > 0x7e)
            *b++ = '.';
        else
            *b++ = c;
    }

    *b = '\0';
}


/*
// Dump a 256-byte memory block in HEX to a buffer.
*/
void    dump_page(char* p, char* b, ULONG address) {
    int     i = 0;
    int     l = 0;

    for (i=0; i<16; i++) {
        dump_para(p, b, address);
        p += 16;
        if (!i)
            l = strlen(b);
        b += l;
        *b++ = '\n';
        address += 16;
    }

    *--b = '\0';
}


/*
// Get a string from a pipe.
*/
int     pgets(HFILE ph, PCHAR b) {
    int     cnt     = 0;
    CHAR    c       = '\0';
    ULONG   bread   = 0;
    APIRET  ulrc    = NO_ERROR;

    /*
    // Read 1 byte from the pipe to set the condition for entering the
    // loop below.
    */
    ulrc = DosRead(ph, &c, 1, &bread);

    /*
    // Either the sentinel was found or we enter the loop and
    // keep on reading until CRLF or the sentinel is found.
    */
    while (ulrc == NO_ERROR && bread > 0 && c != 0x1a) {
        switch (c) {
            case 0x0d:
                ulrc = DosRead(ph, &c, 1, &bread);
                break;
            case 0x0a:
                *b = '\0';
                bread = 0;
                cnt++;
                break;
            default:
                *b++ = c;
                cnt++;
                ulrc = DosRead(ph, &c, 1, &bread);
                break;
        }
    }

    /*
    // Return the length of the line read.
    */
    return cnt;
}

/*
// Read next line from buffer.
*/
char*   get_next_line(char* start, char* buf) {
    char    c = '\0';
    int     i = 0;
    bool    loop = true;

    while (loop) {
        c = *start++;
        switch (c) {
            case 0x00:
                start = NULL;
                loop = false;
                break;
            case 0x0d:
                break;
            case 0x0a:
                buf[i++] = '\0';
                loop = false;
                break;
            default:
                buf[i++] = c;
                break;
        }
    }

    return start;
}

/*
// Run command and capture output.
*/
char*   RunCommandAndCaptureOutput(char* command) {
    HFILE   rhandle     = NULL;
    HFILE   whandle     = NULL;
    HFILE   stdout      = 1;
    ULONG   psize       = 32768;
    CHAR    buf[256]    = "\0";
    ULONG   bread       = 0;
    PCHAR   pipebuf     = NULL;
    PCHAR   output      = NULL;
    ULONG   i           = 0;
    APIRET  ulrc        = NO_ERROR;

    __debug("RunCommandAndCaptureOutput", "START", DBG_MLE);

    sprintf(buf,"command: %s", command);
    __debug("RunCommandAndCaptureOutput", buf, DBG_MLE);

    /*
    // Create the unamed pipe.
    // This pipe is used to capture the output from the command we invoke.
    */
    __debug("RunCommandAndCaptureOutput", "Create Pipe", DBG_MLE);
    ulrc = DosCreatePipe(&rhandle, &whandle, psize);
    sprintf(buf,"DosCreatePipe: ulrc: %d", ulrc);
    __debug("RunCommandAndCaptureOutput", buf, DBG_MLE);

    /*
    // Duplicate the pipe handle to standard output.
    // This redirects the output of the command to the pipe.
    */
    __debug("RunCommandAndCaptureOutput", "Dup Handle", DBG_MLE);
    ulrc = DosDupHandle(whandle, &stdout);
    sprintf(buf,"DosDupHandle: ulrc: %d", ulrc);
    __debug("RunCommandAndCaptureOutput", buf, DBG_MLE);

    __debug("RunCommandAndCaptureOutput", "Issue Command", DBG_MLE);
    /* Issue the command */
    system(command);

    __debug("RunCommandAndCaptureOutput", "Allocate Local Mem", DBG_MLE);
    /* Allocate memory for the entire output */
    ulrc = DosAllocMem((PPVOID)&pipebuf, 32768, PAG_COMMIT|PAG_READ|PAG_WRITE);

    __debug("RunCommandAndCaptureOutput", "Read output to buffer", DBG_MLE);
    /* Read the entire output from the pipe into the buffer */
    ulrc = DosRead(rhandle, pipebuf, 32768, &bread);

    __debug("RunCommandAndCaptureOutput", "Allocate User Mem", DBG_MLE);
    /* Allocate a new buffer with the size of the output; caller frees */
    ulrc = DosAllocMem((PPVOID)&output, bread+16, PAG_COMMIT|PAG_READ|PAG_WRITE);

    __debug("RunCommandAndCaptureOutput", "Copy Data", DBG_MLE);
    /* Terminate the data */
    output[bread] = '\0';
    /* Put length at start */
    *((int*)output) = bread;
    /* Copy data */
    for (i=0; i<bread; i++) {
        output[i+4] = pipebuf[i];
    }

    __debug("RunCommandAndCaptureOutput", "Free Local Mem", DBG_MLE);
    /* Free initial buffer */
    ulrc = DosFreeMem(pipebuf);

    __debug("RunCommandAndCaptureOutput", "Close Pipe", DBG_MLE);
    /* Close the handles of the unnamed pipe */
    ulrc = DosClose(rhandle);
    ulrc = DosClose(whandle);

    __debug("RunCommandAndCaptureOutput", "END", DBG_MLE);
    /* Return the buffer to the caller */
    return output;
}


/*
// Enumerate partitions on disk using DFSVOS2 executable.
*/
void    EnumPartitions(int disk) {
    int     i = 0;
    char    drive = '*';
    char    unit = '0';
    char    buf[256] = "\0";
    char    buf2[256] = "\0";
    char    command[256] = "\0";
    char*   output = NULL;
    char*   s = NULL;
    bool    xmlsf = false;

    __debug("EnumPartitions", "START", DBG_MLE);

    sprintf(command, "%s part -xml -file- -d:%d", g_dfsvos2, disk);
    output = RunCommandAndCaptureOutput(command);
    s = output+4;
    i = 0;
    while (s) {
        s = get_next_line(s, buf);
        if (strstr(buf, "<?xml"))
            xmlsf = true;
        if (s && xmlsf) {
            sprintf(buf2, "%02d :: %s\n", i++, buf);
            WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) buf2, (MPARAM) NULL);
        }
    }

    DosFreeMem(output);
     __debug("EnumPartitions", "END", DBG_MLE);
}


/*
// Get the status of a disk using the LVM Engine.
*/
ULONG   GetDiskStatus(int disk) {
    CARDINAL32                  rc;
    BOOL                        rv          = FALSE;
    Drive_Control_Array         dca;
    Drive_Information_Record    dir;
    Partition_Information_Array pia;
    char                        buf[256]    = "\0";
    int                         i           = 0;
    ULONG                       dskstat     = 0;

    /* Open the LVM Engine */
    Open_LVM_Engine(TRUE, &rc);

    /*
    // If success, search for the disk requested and get several status
    // values and put them in the bitmap to return.
    */
    if (!rc) {
        dca = Get_Drive_Control_Data(&rc);
        for (i=0; i<dca.Count; i++) {
            if (dca.Drive_Control_Data[i].Drive_Number == disk) {
                dskstat |= DSKSTAT_VALID;
                dskstat |= dca.Drive_Control_Data[i].Drive_Is_PRM << 30;
                dir = Get_Drive_Status(dca.Drive_Control_Data[i].Drive_Handle, &rc);
                dskstat |= dir.Unusable << 29;
                dskstat |= dir.IO_Error << 28;
                dskstat |= dir.Corrupt_Partition_Table << 27;
                dskstat |= dir.Is_Big_Floppy << 26;
                pia = Get_Partitions(dca.Drive_Control_Data[i].Drive_Handle, &rc);
                dskstat |= (pia.Count & DSKSTAT_PARTCOUNT);

            }
        }

        /* Free the LVM resources and close the Engine */
        Free_Engine_Memory(dca.Drive_Control_Data);
        Free_Engine_Memory(pia.Partition_Array);
        Close_LVM_Engine();
    }

    sprintf(buf, "dskstat:%08lX", dskstat);
    __debug("GetDiskStatus", buf, DBG_MLE);

    /* Return the bitmap contaning the status of the disk */
    return dskstat;
}

/*
// Get the File System from the raw PBR.
*/
APIRET  QueryFileSystem(char drive, PFSDTYPE pfsdt) {
    APIRET      ulrc        = -1;
    APIRET      ulrc2       = -1;
    HFILE       fhandle     = NULL;
    ULONG       bread       = 0;
    ULONG       ulAction    = -1;
    CHAR        buf[512]    = "\0";
    PCHAR       p           = NULL;
    FSDTYPE     fsdt        = FSDT_UNKNOWN;

    ulrc = ReadPBR(drive, buf, sizeof(buf));

    if (!ulrc) {
        p = &buf[0x36];
        if (!strncmp(p, "FAT", 3)) {
            fsdt = FSDT_FAT;
        }
        else {
            if (!strncmp(p, "HPFS", 4)) {
                fsdt = FSDT_HPFS;
            }
            else {
                if (!strncmp(p, "JFS", 3)) {
                    fsdt = FSDT_JFS;
                }
                else {
                    p = &buf[0x52];
                    if (!strncmp(p, "FAT32", 5)) {
                        fsdt = FSDT_FAT32;
                    }
                }
            }
        }
    }

    *pfsdt = fsdt;
    return ulrc;
}


/*
// Parse several attributes of a disk using DFSVOS2 xml output and
// then search for a key and return it.
*/
APIRET  ParseDisk(int disk, char* key, char* buf) {
    APIRET      ulrc        = -1;
    char        buf2[256]   = "\0";
    char*       output      = NULL;
    char*       p1          = NULL;
    char*       p2          = NULL;

    __debug("ParseDisk", "START", DBG_MLE);

    /* Gather information on disk in xml format */
    sprintf(buf2, "%s part -xml -file- -d:%d", g_dfsvos2, disk);
    output = RunCommandAndCaptureOutput(buf2);

    __debug("ParseDisk", " >> Searck Key", DBG_MLE);

    /* Search for the requested key and get it's value */
    p1 = strstr(output+4, key);
    if (p1) {
        p1 = strstr(p1, "\"");
        p1++;
        p2 = strstr(p1, "\"");
        *p2='\0';
        strcpy(buf, p1);
    }

    __debug("ParseDisk", " >> Release Mem", DBG_MLE);

    /* Release the memory allocated by RunCommand... */
    DosFreeMem(output);

    __debug("ParseDisk", "END", DBG_MLE);

    return 0;
}

/*
// Query the FileSystem using DFSVOS2.
*/
APIRET  QueryFileSystemDFSVOS2(int disk, PFSDTYPE pfsdt) {
    APIRET      ulrc        = -1;
    FSDTYPE     fsdt        = FSDT_UNKNOWN;
    char        buf[256]    = "\0";

    ulrc = ParseDisk(disk, g_dfsvos2_ecsdisks_filesystem, buf);


    if (!strcmp(buf, "FAT16")) {
        fsdt = FSDT_FAT;
    }
    else {
        if (!strcmp(buf, "HPFS")) {
            fsdt = FSDT_HPFS;
        }
        else {
            if (!strcmp(buf, "JFS")) {
                fsdt = FSDT_JFS;
            }
            else {
                if (!strcmp(buf, "FAT32")) {
                    fsdt = FSDT_FAT32;
                }
            }
        }
    }

    *pfsdt = fsdt;
    return ulrc;
}


/*
// Check the sanity of the usb mass storage device just inserted.
*/
ULONG   CheckDeviceSanity(USBDeviceReport* usbdev) {
    APIRET      ulrc        = -1;
    ULONG       dskstat     = 0;
    BOOL        hsecs0      = -1;
    FSDTYPE     fsdt        = FSDT_UNKNOWN;
    CHAR        buf[256]    = "\0";

    if (usbdev) {
        dskstat = GetDiskStatus(usbdev->diskindex);
        if (dskstat & DSKSTAT_VALID) {
            hsecs0 = HiddenSecsZero(usbdev->dletter);
            if (hsecs0 == 1)
                dskstat |= DSKSTAT_HIDDENSECSZERO;
            ulrc = QueryFileSystemDFSVOS2(usbdev->diskindex, &fsdt);
            dskstat |= ((fsdt & 0xff) << 8);
            if (IsBeyondEndDisk(usbdev->diskindex))
                dskstat |= DSKSTAT_BEYONDENDDISK;
            //~ if (IsDirty(usbdev->diskindex))
                //~ dskstat |= DSKSTAT_DIRTY;
            if (IsResized(usbdev->diskindex))
                dskstat |= DSKSTAT_RESIZE;
            if (!DriveIsAccessible(usbdev->dletter))
                dskstat |= DSKSTAT_NOACCESS;
        }
    }

    sprintf(buf, "dskstat:%08lX", dskstat);
    __debug("CheckDeviceSanity", buf, DBG_MLE);

    return dskstat;
}


/*
// Check if a partition is beyond the end of disk.
*/
BOOL    IsBeyondEndDisk(int disk) {
    APIRET      brc         = FALSE;
    APIRET      ulrc        = -1;
    char        buf[256]    = "\0";

    ulrc = ParseDisk(disk, g_dfsvos2_ecsdisks_beyondendisk, buf);
    if (!strcmp(buf, "yes"))
        brc = TRUE;

    sprintf(buf, "beyondenddisk:%d, ulrc(ParseDisk):%d", brc, ulrc);
    __debug("IsBeyondEndDisk", buf, DBG_MLE);

    return brc;
}


/*
// Check if the FileSystem is dirty.
*/
BOOL    IsDirty(int disk) {
    APIRET      brc         = FALSE;
    APIRET      ulrc        = -1;
    char        buf[256]    = "\0";

    ulrc = ParseDisk(disk, g_dfsvos2_ecsdisks_status, buf);
    if (!strcmp(buf, "dirty"))
        brc = TRUE;

    return brc;
}


/*
// Check if the FileSystem is resized.
*/
BOOL    IsResized(int disk) {
    APIRET      brc         = FALSE;
    APIRET      ulrc        = -1;
    char        buf[256]    = "\0";

    ulrc = ParseDisk(disk, g_dfsvos2_ecsdisks_resize, buf);
    if (!strcmp(buf, "true"))
        brc = TRUE;

    return brc;
}



/*
// Get the DFSVOS2 number of the partition.
*/
int     QueryPartitionNumer(int disk) {
    APIRET      brc         = FALSE;
    APIRET      ulrc        = -1;
    char        buf[256]    = "\0";
    int         pnumber     = 0;

    ulrc = ParseDisk(disk, g_dfsvos2_ecsdisks_number, buf);
    pnumber = strtol(buf, NULL, 10);

    return pnumber;
}
