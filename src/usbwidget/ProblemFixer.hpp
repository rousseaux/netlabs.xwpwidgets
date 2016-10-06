/*****************************************************************************\
* ProblemFixer.hpp -- NeoWPS * USB Widget                                     *
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
 * This is the public header for it's corresponding source.
 */


#ifndef     __PROBLEMFIXER_HPP__
#define     __PROBLEMFIXER_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    "Master.hpp"
#include    "ModuleGlobals.hpp"
#include    "USBHelpers.hpp"

#include    "DebugDialog.hpp"

/* Masks for QueryDeviceSanity() */
#define     DSKSTAT_VALID           0x80000000L     // 31
#define     DSKSTAT_PRM             0x40000000L     // 30
#define     DSKSTAT_UNUSABLE        0x20000000L     // 29
#define     DSKSTAT_IOERROR         0x10000000L     // 28

#define     DSKSTAT_CORRUPTED       0x08000000L     // 27
#define     DSKSTAT_BIGFLOPPY       0x04000000L     // 26
#define     DSKSTAT_RESERVED_25     0x02000000L     // 25
#define     DSKSTAT_RESERVED_24     0x01000000L     // 24

#define     DSKSTAT_BEYONDENDDISK   0x00800000L     // 23
#define     DSKSTAT_HIDDENSECSZERO  0x00400000L     // 22
#define     DSKSTAT_DIRTY           0x00200000L     // 21
#define     DSKSTAT_NOACCESS        0x00100000L     // 20

#define     DSKSTAT_RESIZE          0x00080000L     // 19
#define     DSKSTAT_RESERVED_18     0x00040000L     // 18
#define     DSKSTAT_RESERVED_17     0x00020000L     // 17
#define     DSKSTAT_RESERVED_16     0x00010000L     // 16

#define     DSKSTAT_FILESYS         0x0000ff00L     // 8-15
#define     DSKSTAT_PARTCOUNT       0x000000ffL     // 0-7


void    dump_para(char* p, char* b, ULONG address);
void    dump_page(char* p, char* b, ULONG address);
char*   get_next_line(char* start, char* buf);
char*   RunCommandAndCaptureOutput(char* command);
void    EnumPartitions(int disk);
ULONG   GetDiskStatus(int disk);
APIRET  QueryFileSystem(char drive, PFSDTYPE pfsdt);
APIRET  ParseDisk(int disk, char* key, char* buf);
APIRET  QueryFileSystemDFSVOS2(int disk, PFSDTYPE pfsdt);
ULONG   CheckDeviceSanity(USBDeviceReport* usbdev);
BOOL    IsBeyondEndDisk(int disk);
BOOL    IsDirty(int disk);
BOOL    IsResized(int disk);
int     QueryPartitionNumer(int disk);



#ifdef      __cplusplus
    }
#endif

#endif // __PROBLEMFIXER_HPP__
