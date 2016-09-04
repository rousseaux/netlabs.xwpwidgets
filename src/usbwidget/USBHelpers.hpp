/*****************************************************************************\
* USPHelpers.hpp -- NeoWPS * USB Widget                                       *
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


#ifndef     __USBHELPERS_HPP__
#define     __USBHELPERS_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif


/*
// ORDER:
//      PRE-INCLUDES
//      STRUCTURES
//      PROTOTYPES
//      POST-INCLUDES
*/


#include    "Master.hpp"
#include    "ModuleGlobals.hpp"


/*
// Include the USB sub-system.
*/
#include    "usb.h"
#include    "usbcalls.h"
//#include  "APIHelpers.hpp"


class   UsbDevice {
};

/*
// EjectDrive IOCtl werkt niet meer als #pragma(4) actief is.
// Snap niets van want onderstaande structure is custom.
*/
//#pragma pack(4)
// BOOKMARK: USB Device Report Structure
typedef struct _USBDeviceReport {
    int         descrLen;
    int         iamnew;
    HBITMAP     hbm;
    HBITMAP2    hbm2;
    int         loadedbitmap;
    RECORDCORE* myrec;
    char        data[1536];
    char        shit[8096];
    char        devicename[2048];
    ULONG       devflags;
    ULONG       drivemap;
    ULONG       diskindex;
    BOOL        removed;
    BOOL        safe_eject;
    BOOL        eject_pending;
    BOOL        compared;
    BOOL        dasd;
    BOOL        zipdrive;
    BOOL        was_phantom;
    char        dletter;

} USBDeviceReport;
//#pragma pack()



/*
    BOOL        printer;
    BOOL        wifi;
    BOOL        mouse;
    BOOL        keyboard;
    BOOL        audio;
    BOOL        video;
    BOOL        musicplayer;
    BOOL        webcam;
    BOOL        camera;
    BOOL        camcorder;
    BOOL        smartphone;
    BOOL        hub;
*/

/* Prototypes */
void                USBHelpers(void);

BOOL                DeviceIsDASD(struct usb_device_descriptor* usbdescr, int length);
ULONG               GetUsbDeviceFlags(struct usb_device_descriptor* usbdescr, int length);

USBDeviceReport*    DriveLetterToUsbDevice(char dletter);
USBDeviceReport*    UsbIdToUsbDevice(ULONG usbid);

VOID                DumpStructOffsets(USBDeviceReport* usbdev);
VOID                DumpDeviceList(HWND, CList*);
int                 QueryUSBList();
int                 usb_query_device_report( USBHANDLE dh, unsigned char *ucData, int dataLen );
void                CompareUSBDevicesList();
void                SaveUSBDevicesList();
USBDeviceReport*    GetUsbDeviceByMap(CList* usblist, unsigned long drivemap);
void                RemoveUsbDeviceByMap(CList* usblist, unsigned long drivemap);
void                CleanUSBDevicesList(CList *usblist);
int                 CollectUSB(int event);
char*               GetUsbVendorString(char* usbid, char* buf, int bufsize);

// CHECKME: Oud commentaar
/*
// VANWEGE KNIFTIGE INCLUDE STRUCTUUR HIER GEDECLAREED IPV USBWidget.hpp
// TERWIJL DE FUNCTIE IN DE USBWIDGET MODULE ZIT !!!!!!!!!!!!!!!!
*/
int PopUpChangedDevices(int action, USBDeviceReport*);

VOID    InsertionBeep();
VOID    RemovalBeep();
VOID    ForcedRemovalBeep();
VOID    NotifyBeep();
VOID    RefreshBeep();

// CHECKME: Oud commentaar
//////////////// MOET NA USB STRUCT ?????????????????????????????????
//// LVMHelpers.hpp INCLUDE USBHelpers.hpp opnieuw.
//// MAAR DAN IS USBDeviceReport NOG NIET GEDEFINIEERD !!
//// DUS: ONDERVERDELEN IN PRE-INCLUDE EN POST-INCLUDE
//// DIT IS DUS EEN POST-INCLUDE OMDAT-IE EEN FILE INCLUDE DIE WEER DEZE INCLUDE.
//// RECURSIVITEIT.


#include    "LVMHelpers.hpp"





#ifdef      __cplusplus
    }
#endif

#endif // __USBHELPERS_HPP__
