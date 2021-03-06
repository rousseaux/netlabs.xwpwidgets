/*****************************************************************************\
* USBHelpers.cpp -- NeoWPS * USB Widget                                       *
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
 * This module contains several helper functions for the USB System.
 * It is used to simplify things.
 */

#include    "Master.hpp"

#include    "Debug.hpp"
#include    "APIHelpers.hpp"
#include    "GUIHelpers.hpp"
#include    "ProblemFixer.hpp"
#include    "ProblemFixerDialog.hpp"

#include    "USBHelpers.hpp"


void    USBHelpers(void) {
}


/* Determines if the device is a mass-storage device */
BOOL    DeviceIsDASD(struct usb_device_descriptor* usbdescr, int length) {
    char    buf[256]    = "\0";
    int     dataLen     = 0;
    char*   p           = NULL;
    BOOL    dasd        = FALSE;

    /*
    // There is a difference between true being interpreted as 1 or as non-zero.
    // Something like if (b == TRUE) only evaluates to true if and only if b is
    // exactly 1. Something like if (b) evaluates to true if b != 0. So the
    // double !! below in effect moves the state of the USBDEV_DASD bit to
    // position zero.
    */
    dasd = !!(GetUsbDeviceFlags(usbdescr, length) & USBDEV_DASD);

    return dasd;
}

/* Get various device properties in a bitmap */
ULONG   GetUsbDeviceFlags(struct usb_device_descriptor* usbdescr, int length) {
    char    buf[256]    = "\0";
    int     dataLen     = 0;
    char*   p           = NULL;
    ULONG   devflags    = 0L;

    struct usb_descriptor_header*       desc        = NULL;
    struct usb_config_descriptor*       config_desc = NULL;
    struct usb_string_descriptor*       string_desc = NULL;
    struct usb_interface_descriptor*    iface_desc  = NULL;
    struct usb_endpoint_descriptor*     ep_desc     = NULL;

    memset(buf, 0, sizeof(buf));
    //dataLen = usbdescr->bLength;
    dataLen = length;
    sprintf(buf, "DATALEN-1 -----> %02X,%02X (usbdescr=%08X)", dataLen, usbdescr->bLength, usbdescr);

    p = (char *) usbdescr;
    p += usbdescr->bLength;

    desc = (struct usb_descriptor_header *) p;
    dataLen -= usbdescr->bLength;


    /** Parse Device Descriptor **/
    while (dataLen > 2) {

        sprintf(buf, "DATALEN-2 -----> %02X (desc=%08X)", dataLen, desc);


        switch(desc->bDescriptorType) {
            case USB_DT_CONFIG: {
                config_desc = (struct usb_config_descriptor *) desc;
                sprintf(buf, "CONFIG:");

            }
            break;

            case USB_DT_STRING: {
                string_desc = (struct usb_string_descriptor *) desc;
                sprintf(buf, "STRING:");

            }
            break;

            case USB_DT_INTERFACE: {
                iface_desc = (struct usb_interface_descriptor *) desc;
                sprintf(buf, "INTERFACE: class=%02X, subclass=%02X, protocol=%02X",
                    iface_desc->bInterfaceClass,
                    iface_desc->bInterfaceSubClass,
                    iface_desc->bInterfaceProtocol);

                switch (iface_desc->bInterfaceClass) {

                    /* DASD */
                    case USB_CLASS_MASS_STORAGE: {
                        if (iface_desc->bInterfaceSubClass == USB_SUBCLASS_SCSI &&
                            iface_desc->bInterfaceProtocol == USB_PROTOCOL_MASS_BBB) {
                            devflags |= USBDEV_DASD;
                        }
                        break;
                    }

                    /* WIRELESS */
                    case USB_CLASS_WIRELESS: {
                        if (iface_desc->bInterfaceSubClass == 0x01) {   // Radio Frequency
                            devflags |= USBDEV_WIRELESS;
                        }
                        break;
                    }
                }
            }
            break;

            case USB_DT_ENDPOINT: {
                ep_desc = (struct usb_endpoint_descriptor *) desc;
                sprintf(buf, "ENDPOINT:");

            }
            break;


            default: {
                sprintf(buf, "UNHANDLED: type=%02X", desc->bDescriptorType);

            }
            break;
        }

        dataLen -= desc->bLength;
        p += desc->bLength;
        desc = (struct usb_descriptor_header *) p;
    }

    return devflags;
}



/*
// Search the current usb devices-list for a device that holds the given drive-letter.
// This is done by converting the given drive-letter to a drive-map,
// and comparing that drive-map to the individual drive-maps of each usb-device,
// that also is a dasd-device.
// If such a device is found, the pointer to the device-report is returned, otherwise NULL.
// Note that the caller should use the returned pointer only for some fast basic work,
// because on a next insert or remove the pointer will be invalid.
*/
// BOOKMARK: Drive Letter to Usb Device
USBDeviceReport*    DriveLetterToUsbDevice(char dletter) {
    BOOL    found   = false;
    ULONG   smap    = 0;
    ULONG   dmap    = 0;
    CList*  usbdevs = NULL;
    int     i       = 0;
    USBDeviceReport* usbdev = NULL;
    struct usb_device_descriptor* usbdescr = NULL;

    smap = drive_letter_to_map(dletter);    // Convert the given drive-letter
                                            // to a drive-map.

    usbdevs = pws->currentUSBDevicesList;   // Get the list of current
                                            // usb-devices.

    /* Loop over the list to search for a device containing the drive */
    for(i=0; i<usbdevs->length(); i++) {
        usbdev = (USBDeviceReport *) usbdevs->query(i);             // Get the device from the list.
        usbdescr = (struct usb_device_descriptor*) usbdev->data;    // Also get the device desriptor.

        /* Continue with next device if it's not a storage device */
        if (!usbdev->dasd)
            continue;

        /* The drivemap indicates which drives the device holds */
        dmap = usbdev->drivemap;

        /* If both the drive-letter and the map contain the searched drive, we found it */
        if (smap & dmap) {
            found = TRUE;
            break;
        }
    }

    /* Return any device found, otherwise NULL */
    if (found)
        return usbdev;
    else
        return NULL;
}


/*
// Search the current usb devices-list for a device that holds the given usbid.
// If such a device is found, the pointer to the device-report is returned, otherwise NULL.
// Note that the caller should use the returned pointer only for some fast basic work,
// because on a next insert or remove the pointer will be invalid.
*/
USBDeviceReport*    UsbIdToUsbDevice(ULONG usbid) {
    BOOL    found   = false;
    CList*  usbdevs = NULL;
    int     i       = 0;
    USBDeviceReport* usbdev = NULL;
    struct usb_device_descriptor* usbdescr = NULL;
    ULONG   tmpid   = 0;

    usbdevs = pws->currentUSBDevicesList;                           // List of current usb-devices.

    for(i=0; i<usbdevs->length(); i++) {
        usbdev = (USBDeviceReport *) usbdevs->query(i);             // Get the device from the list.
        usbdescr = (struct usb_device_descriptor*) usbdev->data;    // Also get the device desriptor.

        if (!usbdev->dasd)                                          // Continue iteration if it's not a dasd device.
            continue;

        tmpid = usbdescr->idVendor << 16 | usbdescr->idProduct;     // Compose temporary usbid.

        if (tmpid == usbid) {                                       // If id's are the same the device is found.
            found = TRUE;                                           // We found it, break the loop.
            break;
        }
    }

    /*
    // Return any device found, otherwise NULL.
    */
    if (found)
        return usbdev;
    else
        return NULL;
}


VOID    DumpStructOffsets(USBDeviceReport* usbdev) {
    char    buf[256] = "\0";

    sprintf(buf,"descrLen     : %08X (%d)", &(usbdev->descrLen), sizeof(usbdev->descrLen));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"iamnew       : %08X (%d)", &(usbdev->iamnew), sizeof(usbdev->iamnew));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"hbm          : %08X (%d)", &(usbdev->hbm), sizeof(usbdev->hbm));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"hbm2         : %08X (%d)", &(usbdev->hbm2), sizeof(usbdev->hbm2));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"loadedbitmap : %08X (%d)", &(usbdev->loadedbitmap), sizeof(usbdev->loadedbitmap));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"myrec        : %08X (%d)", &(usbdev->myrec), sizeof(usbdev->myrec));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"data         : %08X (%d)", &(usbdev->data), sizeof(usbdev->data));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"shit         : %08X (%d)", &(usbdev->shit), sizeof(usbdev->descrLen));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"devicename   : %08X (%d)", &(usbdev->devicename), sizeof(usbdev->devicename));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"dletter      : %08X (%d)", &(usbdev->dletter), sizeof(usbdev->dletter));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"zipdrive     : %08X (%d)", &(usbdev->zipdrive), sizeof(usbdev->zipdrive));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"safe_eject   : %08X (%d)", &(usbdev->safe_eject), sizeof(usbdev->safe_eject));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"removed      : %08X (%d)", &(usbdev->removed), sizeof(usbdev->removed));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"compared     : %08X (%d)", &(usbdev->compared), sizeof(usbdev->compared));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"drivemap     : %08X (%d)", &(usbdev->drivemap), sizeof(usbdev->drivemap));
    __debug(NULL, buf, DBG_LBOX);
    sprintf(buf,"dasd         : %08X (%d)", &(usbdev->dasd), sizeof(usbdev->dasd));
}


VOID    DumpDeviceList(HWND hwndListBox, CList* usblist) {
    //CList*                usblist = pws->currentUSBDevicesList;
    USBDeviceReport*                    usbdev      = NULL; // Custom type to encapsulate usb-device info.

    struct usb_device_descriptor*       usbdescr    = NULL; // This

    char        devid[64]   = "\0";
    char        buf[256]    = "\0";
    char        buf2[256]   = "\0";
    ULONG       ulrc        = -1;
    USBHANDLE   husb        = -1;
    HFILE       hdos        = -1;
    ULONG       action      = -1;
    int         dataLen     = 0;
    char*       p           = NULL;
    char        dl          = '*';


    //sprintf(buf, "g_hUSBDrv: %08X, g_cInit: %08X", g_hUSBDrv, g_cInit);
    //__debug(NULL, buf, DBG_LBOX);
    //__debug(NULL, "", DBG_LBOX);

    //InitUsbCalls();

    sprintf(buf, "DumpDeviceList: usblist: %08lX, pws->currentUSBDevicesList: %08lX", usblist, pws->currentUSBDevicesList);
    __debug(NULL, buf, DBG_LBOX);


    //return;


    for(int a=0; a<usblist->length(); a++) {
        usbdev = (USBDeviceReport *) usblist->query(a);
        usbdescr = (struct usb_device_descriptor *) usbdev->data;

        //DumpStructOffsets(usbdev);

        sprintf(buf, "USB-DEVICE #%02d -- %08X -- %08X", a, usbdev, usbdescr);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "ID         : %04x:%04x", usbdescr->idVendor, usbdescr->idProduct);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(devid, "%04x:%04x", usbdescr->idVendor, usbdescr->idProduct);
        sprintf(buf, "VENDOR     : %s", usbdev->devicename);
        __debug(NULL, buf, DBG_LBOX);

        /*
        ulrc = UsbOpen(&husb,
                        usbdescr->idVendor,
                        usbdescr->idProduct,
                        usbdescr->bcdUSB,
                        0);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "UsbOpen: ulrc=%d, husb=%08X", ulrc, husb);
        __debug(NULL, buf, DBG_LBOX);
        */

        //sprintf(buf, "DosOpen: ulrc=%d, hdos=%08X", ulrc, hdos);
        //__debug(NULL, buf, DBG_LBOX);


        ////ulrc = UsbInterfaceGetDescriptor(HANDLE, INDEX, LID, LEN, DATA);





/*
    struct usb_config_descriptor*       config_desc = NULL;
    struct usb_string_descriptor*       string_desc = NULL;
    struct usb_interface_descriptor*    iface_desc  = NULL;
    struct usb_endpoint_descriptor*     ep_desc     = NULL;
*/

/*
#define     USB_DT_DEVICE                   0x01
#define     USB_DT_CONFIG                   0x02
#define     USB_DT_STRING                   0x03
#define     USB_DT_INTERFACE                0x04
#define     USB_DT_ENDPOINT                 0x05

#define     USB_DT_HID                      0x21
#define     USB_DT_REPORT                   0x22
#define     USB_DT_PHYSICAL                 0x23
#define     USB_DT_HUB                      0x29

*/

/*
        ulrc = UsbInterfaceGetDescriptor(husb, 1, 1, sizeof(buf2), (UCHAR*) buf2);
        usbdescr2 = (struct usb_interface_descriptor *) buf2;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "UsbInterfaceGetDescriptor     : ulrc=%d, class=%02X, subclass=%02X, protocol=%02X",
            ulrc,
            usbdescr2->bInterfaceClass,
            usbdescr2->bInterfaceSubClass,
            usbdescr2->bInterfaceProtocol);
        __debug(NULL, buf, DBG_LBOX);

        ////ulrc = UsbConfigurationGetDescriptor(HANDLE, INDEX, LID, LEN, DATA);
        memset(buf, 0, sizeof(buf));
        ulrc = UsbConfigurationGetDescriptor(2, 1, 2, sizeof(buf2), (UCHAR*) buf2);
        usbdescr3 = (struct usb_config_descriptor *) buf2;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "UsbConfigurationGetDescriptor : ulrc=%d, attributes=%02X", ulrc, usbdescr3->bmAttributes);
        __debug(NULL, buf, DBG_LBOX);



        ulrc = UsbClose(husb);
        sprintf(buf, "UsbClose: ulrc=%d", ulrc);
        __debug(NULL, buf, DBG_LBOX);

*/

        sprintf(buf, "CLASS        : %02X", usbdescr->bDeviceClass);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "SUBCLASS     : %02X", usbdescr->bDeviceSubClass);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "DESC-TYPE    : %02X", usbdescr->bDescriptorType);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "BCD-USB      : %04X", usbdescr->bcdUSB);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "PROTOCOL     : %02X", usbdescr->bDeviceProtocol);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "BCD-DEVICE   : %04X", usbdescr->bcdDevice);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "SERIAL-NR    : %02X", usbdescr->iSerialNumber);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "CONFIGS      : %02X", usbdescr->bNumConfigurations);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "zipdrive     : %d -- %08X", (BOOL) usbdev->zipdrive, &(usbdev->zipdrive));
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "safe_eject   : %d -- %08X", (BOOL) usbdev->safe_eject, &(usbdev->safe_eject));
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "eject_pending: %d -- %08X", (BOOL) usbdev->eject_pending, &(usbdev->eject_pending));
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "new          : %d -- %08X", (BOOL) usbdev->iamnew, &(usbdev->iamnew));
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "removed      : %d", (BOOL) usbdev->removed);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "compared     : %d", (BOOL) usbdev->compared);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "was_phantom  : %d", (BOOL) usbdev->was_phantom);
        __debug(NULL, buf, DBG_LBOX);

        //sprintf(buf, "name         : %s", usbdev->devicename);
        //__debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "dasd         : %d", (BOOL) usbdev->dasd);
        __debug(NULL, buf, DBG_LBOX);

        dl = usbdev->dletter ? usbdev->dletter : dl;
        sprintf(buf, "driveletter  : %c", dl);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "driveletters : %s", drive_letters(buf2, usbdev->drivemap));
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "diskindex    : %d", usbdev->diskindex);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, "devflags     : %s", bin_digits_32(buf2, GetUsbDeviceFlags(usbdescr, usbdev->descrLen)));
        __debug(NULL, buf, DBG_LBOX);

        __debug(NULL, "", DBG_LBOX);

        sprintf(buf,"0x02 == 0x01, !!0x02 == !!0x01 => %d, %d", 0x02 == 0x01, !!0x02 == !!0x01);
        __debug(NULL, buf, DBG_LBOX);

        /* Scroll to last item */
        WinPostMsg(hwndListBox, LM_SETTOPINDEX, (MPARAM) 32767, (MPARAM) 0);
    }

    //TermUsbCalls();

}

 /**
  * Create the list of current devices.
  */
// BOOKMARK: Query USB List
int QueryUSBList()
{
    unsigned char   ucData[1536];
    char            headid[32];
    char            devid[32];
    char            devid_full[32];
    char            filename[32];
    char            filename_full[32];
    ULONG           ulNumDev;
    ULONG           ulDev;
    APIRET          rc;
    char*           sbuf;
    unsigned int    val;
    char            buf[256];
    char            bufa[256];
    FILE*           out=0;
    struct  usb_device_descriptor* usbdescr;

    /*
    * NULL class is not valid.
    * NULL subclass and protocol is ok.
    * Should the USB layer report that ? I don't know.
    * Better to check here.
    */

    /*
    // Get the number of usb-devices.
    // Note their 1-based index is used in the for-loop below.
    */
    rc = UsbQueryNumberDevices( &ulNumDev );

    if(rc==0 && ulNumDev) {
        /*
        // Iterate over the usb-devices using their 1-based index.
        */
        for (ulDev= 1; ulDev <= ulNumDev; ulDev++) {
            int descrLen;                                               // Holds the total descriptor length.

            descrLen = usb_query_device_report( ulDev, ucData, 1536 );  // Query the info on the usb-device.
            // BOOKMARK: Create new USB Device Report (for list query)
            USBDeviceReport* usbdev = new USBDeviceReport;              // Create a new device-report.
            usbdev->descrLen = descrLen;                                // Store the total-length for this descriptor.
            memcpy(usbdev->data, ucData, 1536);                         // Copy the payload.
            usbdescr = (struct usb_device_descriptor *)usbdev->data;    // Pointer to the descriptor.
            usbdev->iamnew = TRUE;                                      // Initially it's a new device.
            usbdev->dasd = DeviceIsDASD(usbdescr, descrLen);            // See if this is a dasd-device.


            /* Compose the vendor:id string in ASCII vendor:id format */
            sprintf(bufa, "%04x:%04x", usbdescr->idVendor, usbdescr->idProduct);
            /*
            // Get the nameof the device.
            // Note that some missing devices, like zip750, are added in this code.
            */
            GetUsbVendorString(bufa, buf, sizeof(buf));

            /** Copy the name of the usb-device to the device-report **/
            strcpy(usbdev->devicename, buf);

            usbdev->safe_eject = FALSE;                                         // This will get it's proper value on a compare.
            usbdev->eject_pending = FALSE;                                      // No eject can be pending yet.
            usbdev->zipdrive = (usbdescr->idVendor == 0x059b);                  // See if this is a zip-drive.
            usbdev->removed = FALSE;                                            // The device is not removed yet.
            usbdev->compared = FALSE;                                           // The device is not compared yet.
            usbdev->dletter = '*';                                              // We have no drive-letter yet.
            usbdev->drivemap = NULL;                                            // We have no drivemap yet.
            usbdev->devflags = GetUsbDeviceFlags(usbdescr, usbdev->descrLen);   // Get the device-flags.

            pws->currentUSBDevicesList->add(usbdev);                            // Add the device to the global list.
        }
    }

    return 0;
}



/*
// Get the description of the usb-device.
*/
int usb_query_device_report( USBHANDLE dh, unsigned char *ucData, int dataLen )
{
    struct usb_device_descriptor *pDevDesc;
    struct usb_config_descriptor *pCfgDesc;
    int total_len= 0;
    APIRET rc;
    char    buf[256]="\0";

    memset(ucData, 0, dataLen );
    pDevDesc= (struct usb_device_descriptor *)ucData;


    /** @@ Begin van het geheel @@
     *
     * Dit is het
     */




    /** ## USB MACRO's ##
     *
     * Hier wordt UsbDeviceGetDescriptor gemapped op UsbCtrlMessage via een #define macro.
     * Die staat in usbcalls.h en ziet er zo uit:
     *
     *  #define UsbDeviceGetDescriptor(HANDLE, INDEX, LID, LEN, DATA) \
     *      UsbCtrlMessage(HANDLE, 0x80, 0x06, (0x0100|INDEX), LID, LEN, DATA, 0)
     *
     * De functie UsbCtrlMessage staat in usbcalls.c (vbox) en die gebruikt DosDevIOCtl.
     * Dat ziet er ongeveer zo uit:
     *
     * rc = DosDevIOCtl( g_hUSBDrv,
     *          IOCAT_USBRES, IOCTLF_SENDCONTROLURB,
     *          (PVOID)&CtrlRequest,
     *          ulParmLen,
     *          &ulParmLen,
     *          ulDataLen>0?(PVOID)pData:NULL,
     *          ulDataLen, ulDataLen>0?&ulDataLen:NULL
     * );
     *
     * Ik wou dat er ergens documentatie van de USBRES category te vinden was.
     */


    /*
    // Wat zijn dit voor parameters ?
    // usbcalls.h, mapped dus op een control-message.

    */
    rc = UsbDeviceGetDescriptor(
        dh,                 // corresponds to 1-based usb-list
        0,                  // index of what ?  (INDEX)
        0,                  // logical id ?     (LID)
        0x12,               // length of what ? (LEN)
        (UCHAR*)pDevDesc    // buffer for data  (DATA)
    );




    /*
    sprintf(buf, "NATIVE-CLASS: %02X", pDevDesc->bDeviceClass);
    __debug(NULL, buf, DBG_LBOX);

    sprintf(buf, "NATIVE-SUBCLASS: %02X", pDevDesc->bDeviceSubClass);
    __debug(NULL, buf, DBG_LBOX);
    */

    rc = 640;
    if( rc == 640 )
    {
        ULONG ulBufLen;
        ulBufLen = dataLen;
        memset(ucData,0, dataLen);
        rc = UsbQueryDeviceReport( dh,
                                    &ulBufLen,
                                    (CHAR*)ucData);
        if( rc == 0 )
        {
            pCfgDesc= (struct usb_config_descriptor *) (ucData+pDevDesc->bLength);
            total_len = pDevDesc->bLength + pCfgDesc->wTotalLength;
        }
    } else
    {
    pCfgDesc= (struct usb_config_descriptor *) (ucData+pDevDesc->bLength);
    /*
    ** We request Configuration Descryptor separatelly, as
    ** device doesn't return proper data via UsbQueryDeviceReport
    */
    memset( pCfgDesc, 0, 0x200 );
    rc= UsbConfigurationGetDescriptor(dh, 0, 0, 0x0009, (UCHAR*)pCfgDesc);
    rc= UsbConfigurationGetDescriptor(dh, 0, 0, pCfgDesc->wTotalLength, (UCHAR*)pCfgDesc);
    if( rc == 0 ) total_len = pDevDesc->bLength + pCfgDesc->wTotalLength;
    }
    if( rc ) return -1;
    return total_len;
}


/*
// ############################################################################
// # CompareUSBDeviceList
// ############################################################################
*/
// BOOKMARK: Compare Usb Devices Lists (drive change)
void CompareUSBDevicesList() {
    char    buf[256] = "\0";        // temporary buffer
    char    buf2[256] = "\0";       // temporary buffer 2
    char    buf3[256] = "\0";       // remporary buffer 3
    char    txtbuf[4096] = "\0";
    int     flag=0;
    int     i;
    int     a;
    int     b;
    ULONG   usbid = NULL;
    BOOL    found = FALSE;
    APIRET  rc;

    CList*  newlist=NULL;
    CList*  oldlist=NULL;
    CList*  usblist=NULL;
    CList*  remlist=NULL;

    /* Shift the lists */
    newlist = pws->currentUSBDevicesList;
    oldlist = pws->previousUSBDevicesList;
    remlist = pws->removedUSBDevicesList;

    /* Pointers to old and new device-report structures */
    USBDeviceReport* oldusbdev = NULL;
    USBDeviceReport* newusbdev = NULL;
    USBDeviceReport* usbdev = NULL;

    /* Pointers to raw usb-descriptors */
    struct usb_device_descriptor* pOldDevDesc;
    struct usb_device_descriptor* pNewDevDesc;
    struct usb_device_descriptor* pDevDesc;

    sprintf(buf, "BEGIN: Compare: old list: %d, new list: %d", oldlist->length(), newlist->length());
    __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);


    if (newlist->length()) {

        //!: --------------------------------- [ Process New Usb Devices List ]
        /*
        // Iterate over new list.
        */
        for(a=0; a<newlist->length(); a++) {
            newusbdev=(USBDeviceReport *)newlist->query(a);
            pNewDevDesc= (struct usb_device_descriptor *)newusbdev->data;

            newusbdev->iamnew=TRUE;
            newusbdev->removed=FALSE;
            newusbdev->compared=FALSE;

            if(initialization) {
                newusbdev->iamnew=FALSE;
                newusbdev->removed=FALSE;
            }

            //if(newusbdev->iamnew==1)


            /*
            // Iterate over old list.
            */
            for(b=0; b < oldlist->length(); b++) {
                //printf("Add record: %d\n", a);

                oldusbdev = (USBDeviceReport *)oldlist->query(b);
                //oldusbdev->removed = TRUE;

                pOldDevDesc = (struct usb_device_descriptor *)oldusbdev->data;
                //printf("LEN: %04x:%04x = %d\n", pDevDesc->idVendor, pDevDesc->idProduct, descrLen);

                //sprintf(buf, "Compare: %04x:%04x and %04x:%04x",
                //  pOldDevDesc->idVendor,
                //  pOldDevDesc->idProduct,
                //  pNewDevDesc->idVendor,
                //  pNewDevDesc->idProduct);
                //__debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

                /*
                // Also use serial here.
                */
                if((pOldDevDesc->idVendor == pNewDevDesc->idVendor)
                    && (pOldDevDesc->idProduct == pNewDevDesc->idProduct)) {
                    newusbdev->iamnew=0;
                    oldusbdev->removed = FALSE;

                    /*
                    // We need to copy the data below from the old entry because this
                    // data was only gathered when the device was inserted.
                    */
                    newusbdev->safe_eject = oldusbdev->safe_eject;
                    newusbdev->dletter = oldusbdev->dletter;
                    newusbdev->drivemap = oldusbdev->drivemap;

                    oldusbdev->compared = TRUE;
                    //break;
                }
                else {
                    if (!oldusbdev->compared) {
                        oldusbdev->removed = TRUE;
                    }
                }
            }
        }

    } // if list not empty

    // new list was empty so set all in old list as removed.
    else {

        for (a=0; a<oldlist->length(); a++) {
            oldusbdev = (USBDeviceReport *)oldlist->query(b);
            oldusbdev->removed = TRUE;
        }
    }



    //!: --------------------------------- [ Process Current Usb Devices List ]
    /*
    // Do actions on new devices.
    */
    // BOOKMARK: Process Current Usb Devices List
    usblist = pws->currentUSBDevicesList;

    for(a=0; a<usblist->length(); a++) {
        usbdev = (USBDeviceReport *)usblist->query(a);
        pDevDesc = (struct usb_device_descriptor *) usbdev->data;

        // BOOKMARK: New Device discovered
        /*
        // A new USB-Device has been discovered in the list.
        // Here we do some pre-processing.
        */
        /** See if this is a new device **/
        if(usbdev->iamnew){
            //////////////////////////////////////////////////////////////////////////////////////////////////
            //newusbdev->drivemap = drives_after;
            //drive_letters(buf, drives_before);


            //~ if (GetDiskStatus(usbdev->diskindex) & DSKSTAT_BIGFLOPPY) {
                //~ MessageBox("New Device", "Big Floppy !");
            //~ }

            /** Make some sound **/
            if(enablebeeps) {
                InsertionBeep();
                PopUpChangedDevices(SHOW_DEV_INSERT, usbdev);
            }

            /** Get the name of the device **/
            /* Debug stuff */
            sprintf(buf, "NEW: %04x:%04x", pDevDesc->idVendor, pDevDesc->idProduct);
            __debug(NULL, buf, DBG_LBOX);

            /* Compose the vendor:id string in ASCII vendor:id format */
            sprintf(buf2, "%04x:%04x", pDevDesc->idVendor, pDevDesc->idProduct);
            sprintf(buf3, "NEW-BY-NAME: %s", GetUsbVendorString(buf2, buf, sizeof(buf)));

            __debug(NULL, buf3, DBG_LBOX);

            g_drives_temp = drive_map_dqcd();
            drive_letters(buf, g_drives_temp);

            __debug(NULL, buf, DBG_LBOX);

            if (usbdev->dasd) {

                /* Capture drives before insert */
                g_drives_before_insert = drive_map_dqcd();


                usbid = 0;
                found = FALSE;
                usbid = pDevDesc->idVendor << 16 | pDevDesc->idProduct;
                //!: INCREASE NUMBER OF PDISKS !! GET IT FROM LVM !!
                for (i=0; i<12; i++) {
                    /* When the old usbid is the same the pantom is revoverable */
                    if (pws->pdisks[i].usbid == usbid && pws->pdisks[i].phantom) {
                        sprintf(buf, "PHANTOM FOUND %08X, %d", usbid, pws->pdisks[i].phantom);
                        __debug(NULL, buf, DBG_LBOX);

                        pws->pdisks[i].phantom = FALSE;
                        usbdev->was_phantom = TRUE;
                        usbdev->eject_pending = FALSE;
                        /// Copying the drivemap is only valid when the same pdisk index was assingned !
                        usbdev->drivemap = pws->pdisks[i].drivemap;
                        usbdev->dletter = FirstDriveFromMap(usbdev->drivemap);
                        pws->phantoms[usbdev->dletter - 'A'] = FALSE;
                    }
                }



                if (!usbdev->was_phantom) {

                    // COULD OCCUR ON STICKS WITH NO OR INVALID PARTITIONS

                    /*
                    // Wait until drive-letters change.
                    */
                    sprintf(buf, "WAITING FOR DRIVES TO CHANGE...");
                    __debug(NULL, buf, DBG_LBOX);

                    for (i=0; i<40; i++) {
                        Rediscover_PRMs(&rc);
                        g_drives_temp = drive_map_dqcd();
                        if (g_drives_temp != g_drives_before_insert) {
                            break;
                        }
                        //~ DosBlock(250);
                        MyDosSleep(250);
                    }
                    sprintf(buf, "DONE WAITING...");
                    __debug(NULL, buf, DBG_LBOX);
                }

                /* Capture the drives after insert */
                g_drives_after_insert = drive_map_dqcd();


                //!: Determine Changes
                drive_changes(g_drives_before_insert,
                                g_drives_after_insert,
                                &g_drives_changed_after_insert,
                                &g_drives_new_after_insert,
                                &g_drives_gone_after_insert);

                /* Dump Drive Changes */
                dump_drive_changes(txtbuf,
                                    g_drives_before_insert,
                                    g_drives_after_insert,
                                    g_drives_changed_after_insert,
                                    g_drives_new_after_insert,
                                    g_drives_gone_after_insert);

                __debug(NULL, txtbuf, DBG_MLE);


                //!: DIFFERENT USB DEVICES WAS ASSIGNED A PHANTOM DRIVE !!
                {
                    CARDINAL32  rc = NULL;

                    char    dl = '*';
                    dl = FirstDriveFromMap(g_drives_new_after_insert);

                    if (pws->phantoms[dl - 'A']) {

                        usbdev->safe_eject = TRUE;
                        usbdev->was_phantom = FALSE;
                        pws->pdisks[usbdev->diskindex].phantom = FALSE;
                        pws->phantoms[dl - 'A'] = FALSE;


                        //~ __debug(NULL, "[BEFORE EJECT] Waiting before doing Rediscover_PRMs...", DBG_LBOX);
                        //~ DosSleep(5000);
                        //~ __debug(NULL, "Doing Rediscover...", DBG_LBOX);


                        Open_LVM_Engine(TRUE, &rc);
                        //~ sprintf(buf, "Open_LVM_Engine rc:%d",rc);
                        //~ MessageBox("BEFORE EJECT", buf);
                        WinSendMsg(hdlgDebugDialog, WM_COMMAND, (MPARAM) ID_ACTION_4, (MPARAM) NULL);
                        Close_LVM_Engine();
                        //~ MessageBox("BEFORE EJECT", "Close LVM");


                        //~ DosSleep(5000);
                        //! DEZE IS BROODNODIG !! -- FLUSHES THE PHANTOM !!
                        //~ EjectDrive(dl);
                        PurgeDrive(dl);

                        //~ system("start /f eject e:");

                        //~ Rediscover_PRMs(&rc);
                        //~ __debug(NULL, "[AFTER EJECT] Waiting before doing Rediscover_PRMs...", DBG_LBOX);
                        //~ DosSleep(5000);
                        //~ __debug(NULL, "Doing Rediscover...", DBG_LBOX);
                        //~ Rediscover_PRMs(&rc);
                        //~ QueryDrive(dl);
                        //~ DosSleep(5000);

                        //~ __debug(NULL, "Waiting again...", DBG_LBOX);
                        //~ DosSleep(5000);
                        //~ __debug(NULL, "Dumping disks...", DBG_LBOX);

                        MessageBox("USB Widget",
                            "The Widget tried to recover a phantom drive, but\n"
                            "a different usb-device was inserted.\n"
                            "\n"
                            "To prevent the system from becoming unstable\n"
                            "the device has been ejected.\n"
                            "\n"
                            "First press OK and then please re-insert the device\n"
                            );

                        //!: REMOUNTS DL AND GEO IS FLUSHED *ONLY* FOR FAT32 !!
                        Open_LVM_Engine(TRUE, &rc);
                        WinSendMsg(hdlgDebugDialog, WM_COMMAND, (MPARAM) ID_ACTION_4, (MPARAM) NULL);
                        Close_LVM_Engine();

                        // So we purge again.
                        PurgeDrive(dl);

                        return;
                    }
                }




                /*
                // If a dasd usb-device was inserted that did not cause new
                // drive-letters, it cannot be ejected.
                // Since it has no drive-letter, nothing is mounted.
                // We set the dasd-flag to false to remove it from further
                // processing and also set the safe-eject flag (and clear phantom).
                // When the device is removed only a beep will sound.
                */
                // BOOKMARK: Convert dasd device to non-dasd when no new drive-letter
                // CHECKME: Phantom reconnect popup fails to come up
                if (g_drives_new_after_insert == 0 && !usbdev->was_phantom) {
                    sprintf(buf, "No new driveletters and not phantom");
                    __debug(NULL, buf, DBG_LBOX);

                    //~ usbdev->dasd = FALSE;

                    //~ usbdev->safe_eject = TRUE;
                    //~ //usbdev->was_phantom = FALSE;

                    //~ // FIXME: Non structured return
                    //~ return;
                }

                /*
                // Dump Drive Changes On Insert.
                */
                {
                    /* Drive Maps */
                    ULONG   drives_before   = NULL;     // before the event
                    ULONG   drives_after    = NULL;     // after the event
                    ULONG   drives_changed  = NULL;     // changed drives
                    ULONG   drives_new      = NULL;     // new drives
                    ULONG   drives_gone     = NULL;     // drives gone

                    /* Get Drive Changes */
                    drives_before   = g_drives_before_insert;
                    drives_after    = g_drives_after_insert;
                    drive_changes(drives_before, drives_after, &drives_changed, &drives_new, &drives_gone);

                    /* Dump Header */
                    //~ WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) "\r\n", (MPARAM) NULL);
                    __debug(NULL, "\n", DBG_LBOX);
                    //~ WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) "=====> MONITOR-thread: DRIVE-STATUS-ON-INSERT\n", (MPARAM) NULL);
                    //~ __debug(NULL, "=====> MONITOR-thread: DRIVE-STATUS-ON-INSERT", DBG_LBOX);

                    /* Dump Drive Changes */
                    dump_drive_changes(txtbuf, drives_before, drives_after, drives_changed, drives_new, drives_gone);
                    //~ WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) txtbuf, (MPARAM) NULL);
                    __debug(NULL, txtbuf, DBG_MLE);
                }


                /* A recovered phantom drive already got it's drivemap from it's reassociated disk */
                /// That's not true if it got assigned a different pdisk index from the system !
                if (!usbdev->was_phantom) {
                    usbdev->drivemap = g_drives_changed_after_insert;
                }
                else {
                    // Komt voor the recovered message...
                    sprintf(buf, "Phantom: drive: %c, diskindex:%d, dlstatus: %d, usbdevsattus:%d, pdisk-status:%d",
                        usbdev->dletter,
                        usbdev->diskindex,
                        pws->phantoms[usbdev->dletter - 'A'],
                        usbdev->was_phantom,
                        pws->pdisks[usbdev->diskindex].phantom
                    );
                    //~ MessageBox("Message", buf);

                    PopUpChangedDevices(SHOW_DRIVES_CHANGED, usbdev);
                    /* Beep on any drive changed */
                    if (enablebeeps) {
                        NotifyBeep();
                    }
                }

                if (g_drives_changed_after_insert) {
                    usbdev->dletter = FirstDriveFromMap(g_drives_changed_after_insert);
                    usbdev->diskindex = DriveToDisk(usbdev->dletter);
                    pws->pdisks[usbdev->diskindex].usbid    = pDevDesc->idVendor << 16 | pDevDesc->idProduct;
                    pws->pdisks[usbdev->diskindex].zipdrive = usbdev->zipdrive;
                    pws->pdisks[usbdev->diskindex].drivemap = DriveMapFromDisk(usbdev->diskindex);

                    PopUpChangedDevices(SHOW_DRIVES_CHANGED, usbdev);
                    //OpenDriveView(usbdev->dletter);

                    /* Beep on any drive changed */
                    if (enablebeeps) {
                        NotifyBeep();
                    }

                }

                /*
                // Clear phantom status.
                // Is otherwise persistent between adds and removes from the usb-lists.
                */
                usbdev->was_phantom = FALSE;





                //!: o-------------------------------> [ CHECK DISK INTEGRITY ]

                //!------------------------------------------------------------
                //!
                //! Check the disk for sanity
                //!
                //! This uses the DFSVOS2 backend.
                //! This works very slow on JFS and FAT32 and has to be
                //! optimized. It is currently disabled.
                //!
                //!------------------------------------------------------------
                {
                    ULONG               dskstat     = 0;
                    ULONG               fixmask     = 0;
                    ULONG               effmask     = 0;
                    ULONG               tofixmap    = 0;
                    char                buf[4096]   = "\0";
                    int                 i           = 0;
                    ProblemFixerDialog* pfd         = NULL;
                    ULONG               ulResult    = -1;
                    BOOL                hsecs0      = -1;
                    FSDTYPE             fsdt        = FSDT_UNKNOWN;
                    APIRET              ulrc        = -1;




                    ///
                    if (g_dfsvos2_exists) {
                        //~ dskstat = CheckDeviceSanity(usbdev);
                    }
                    else {
                        //~ MessageBox("USB Widget :: Problem Fixer",
                        //~ "?:\\eCS\\System\\eWPS\\Plugins\\XCenter\\DFSVOS2.BIN\n"
                        //~ "-- or --\n"
                        //~ "?:\\eCS\\Install\\DFSVOS2.EXE\n"
                        //~ "\n"
                        //~ "could not be found !\n"
                        //~ "Please fix this and then restart the WPS\n"
                        //~ "\n"
                        //~ "Problem Fixer is diabled !\n"
                        //~ );
                    }
                    ///

//!: PROBLEM FIXER
                    /* Clear fixing of hidden sectors if not FAT16 */
                    if ((dskstat & DSKSTAT_HIDDENSECSZERO) && (((dskstat & DSKSTAT_FILESYS) >> 8) != FSDT_FAT))
                        dskstat &= ~DSKSTAT_HIDDENSECSZERO;

                    /* Create the fixmask to isolate what we are looking for */
                    fixmask =   DSKSTAT_BEYONDENDDISK |
                                DSKSTAT_CORRUPTED |
                                DSKSTAT_DIRTY |
                                DSKSTAT_HIDDENSECSZERO |
                                DSKSTAT_NOACCESS;
                    /*
                    // Create the effective mask by removing things we are not looking for.
                    // An effmask of 1 will indicate a fixable disk.
                    */
                    effmask =   dskstat & ~fixmask & ~DSKSTAT_VALID &
                                ~DSKSTAT_PRM & ~DSKSTAT_RESIZE & ~DSKSTAT_FILESYS;

                    /* Set the tofixmap, this contains flags for each problem to fix */
                    tofixmap = dskstat & fixmask;


                    //~ sprintf(buf, "Mask:%08lX, Disk:%d, Corrupted:%d, Beyond:%d, BigFloppy:%d, HiddenSecsZero:%d, FS:%d",
                        //~ (ULONG) dskstat,
                        //~ usbdev->diskindex,
                        //~ !!(dskstat & DSKSTAT_CORRUPTED),
                        //~ !!(dskstat & DSKSTAT_BEYONDENDDISK),
                        //~ !!(dskstat & DSKSTAT_BIGFLOPPY),
                        //~ !!(dskstat & DSKSTAT_HIDDENSECSZERO),
                        //~ (dskstat & DSKSTAT_FILESYS) >> 8
                        //~ );
                    //~ __debug("oo CheckDeviceSanity oo", buf, DBG_MLE);


                    //~ if (dskstat & DSKSTAT_BIGFLOPPY) {
                    if ((dskstat & DSKSTAT_VALID) && tofixmap) {
                        pfd = new ProblemFixerDialog();
                        if (pfd) {
                            pfd->create();
                            if (g_dfsvos2_exists) {
                                pfd->setDiskStatus(dskstat);
                                pfd->setDriveLetter(usbdev->dletter);
                                pfd->setupFixCommands();
                                pfd->help();
                            }
                            else {
                                pfd->noDFSVOS2();
                            }
                            pfd->show();
                            ulResult = pfd->process();
                            pfd->destroy();
                            delete pfd;
                            sprintf(buf, "ulResult:%d", ulResult);
                            __debug("Problem Fixer", buf, DBG_MLE);
                        }


                        // Eject the drive if fixes were applied.
                        if (ulResult == DID_FIX)
                            PurgeDrive(usbdev->dletter);

                    }

                    //~ for (i=0; i<16; i++) {
                        //~ dump_para((char*)dump_para, buf, i*16);
                        //~ __debug(NULL, buf, DBG_MLE);
                    //~ }

                    //~ dump_page((char*)dump_para, buf, 0L);
                    //~ __debug(NULL, buf, DBG_MLE);

                }

            } // if dasd

        } // if new
    }


    //!: --------------------------------- [ Process Removed Usb Devices List ]
    /*
    // Do actions on removed devices.
    */
    usblist = pws->previousUSBDevicesList;

    for(a=0; a<usblist->length(); a++) {
        usbdev=(USBDeviceReport *)usblist->query(a);
        pDevDesc= (struct usb_device_descriptor *)usbdev->data;


        /*
        // An USB-Device has been removed from the list.
        // Here we do some pre-processing.
        */

        if(usbdev->removed) {

            /* Special actions if it's a dasd-device */
            if (usbdev->dasd) {

                //oldusbdev->safe_eject = FALSE;

                //////////////////////////////////////////////////////////////////////////////////////////////////
                //newusbdev->drivemap = drives_after;
                //drive_letters(buf, drives_before);


                sprintf(buf, "REMOVED: %04x:%04x", pDevDesc->idVendor, pDevDesc->idProduct);
                __debug(NULL, buf, DBG_LBOX);

                sprintf(buf2, "%04x:%04x", pDevDesc->idVendor, pDevDesc->idProduct);
                sprintf(buf3, "REMOVED-BY-NAME: %s", GetUsbVendorString(buf2, buf, sizeof(buf)));


                //strcpy(oldusbdev->devicename, buf);


                /* Capture drives before remove */
                g_drives_before_remove = drive_map_dqcd();

                /* Capture the drives after remove */
                g_drives_after_remove = drive_map_dqcd();

                /* Determine Changes */
                drive_changes(g_drives_before_remove,
                                g_drives_after_remove,
                                &g_drives_changed_after_remove,
                                &g_drives_new_after_remove,
                                &g_drives_gone_after_remove);

                //!------------------------------------------------------------
                //!
                //! Dump Drive Changes On Remove
                //!
                //! This shows the changes between the previous capture and the
                //! current state. It unpredictable how much time has elapsed
                //! between two captures.
                //!
                //!------------------------------------------------------------
                {
                    /* Drive Maps */
                    ULONG   drives_before   = NULL;     // before the event
                    ULONG   drives_after    = NULL;     // after the event
                    ULONG   drives_changed  = NULL;     // changed drives
                    ULONG   drives_new      = NULL;     // new drives
                    ULONG   drives_gone     = NULL;     // drives gone

                    /* Get Drive Changes */
                    drives_before   = g_drives_before_remove;
                    drives_after    = g_drives_after_remove;
                    drive_changes(drives_before, drives_after, &drives_changed, &drives_new, &drives_gone);

                    /* Dump Header */
                    //~ WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) "\r\n", (MPARAM) NULL);
                    __debug(NULL, "\n", DBG_LBOX);
                    //~ WinSendMsg(WinWindowFromID(hdlgDebugDialog, ID_MLE_1), MLM_INSERT, (MPARAM) "=====> MONITOR-thread: DRIVE-STATUS-ON-REMOVE\n", (MPARAM) NULL);
                    __debug(NULL, "=====> MONITOR-thread: DRIVE-STATUS-ON-REMOVE", DBG_LBOX);

                    /* Dump Drive Changes */
                    dump_drive_changes(txtbuf, drives_before, drives_after, drives_changed, drives_new, drives_gone);
                    __debug(NULL, txtbuf, DBG_MLE);
                }


                /* Here we have a safe removal */
                if (usbdev->safe_eject) {
                    if(enablebeeps) {
                        RemovalBeep();
                    }
                    /* Remove assiciation with physical disk */
                    if (usbdev->diskindex) {
                        // CHECKME: pointer reference
                        memset(&(pws->pdisks[usbdev->diskindex]), 0, sizeof(PHYSICAL_DISK));
                    }
                }

                /* Here the device has been removed without a proper eject! */
                else {

                    /* The device has been removed without a proper eject! */
                    PopUpChangedDevices(SHOW_DEV_REMOVE, usbdev);
                    //~: ---------------
                    //~ usbdev->eject_pending = FALSE;
                    if (enablebeeps) {
                        ForcedRemovalBeep();
                    }
                    if (usbdev->diskindex) {
                        /* Mark this pdisk as phantom and not safely reoved */
                        pws->pdisks[usbdev->diskindex].safe_eject = FALSE;
                        pws->pdisks[usbdev->diskindex].phantom = TRUE;

                        /* Mark this drive-letter as a phantom drive */
                        //!: Should also mark it's siblings !
                        pws->phantoms[usbdev->dletter - 'A'] = TRUE;
                        /* Eject the phantom drive to obtain the not-ready state */
                        //!: BOOKMARK: EJECT THE PHANTOM !!!!!!!!!!!!!!!!!
                        //~ DosBlock(100);
                        //~ EjectDrive(usbdev->dletter);
                        PurgeDrive(usbdev->dletter);
                    }
                }
            }

            /* Non-dasd device removed */
            else {
                if(enablebeeps) {
                    RemovalBeep();
                }
            }
        }//if removed
    }//end for

    initialization=0;
}



USBDeviceReport*    GetUsbDeviceByMap(CList* usblist, unsigned long drivemap) {
    /* Locals */
    int     i;
    int     b = false;
    USBDeviceReport*    usbdev = null;

    /* Scan list from start to end to find matching device */
    for (i = 0; i < usblist->length(); i++) {
        usbdev = (USBDeviceReport*) usblist->query(i);
        if (usbdev->drivemap & drivemap) {
            b = true;
            break;
        }
    }

    /* Return device if found, otherwise null */
    return b ? usbdev : null;
}


// reverse to keep correct index
void    RemoveUsbDeviceByMap(CList* usblist, unsigned long drivemap) {
    /* Locals */
    int     i;
    USBDeviceReport*    usbdev = null;

    /* Scan list from end to start and remove device from list if found */
    for (i = usblist->length()-1; i >= 0; i--) {
        usbdev = (USBDeviceReport*) usblist->query(i);
        if (usbdev->drivemap & drivemap) {
            /* Release any bitmaps if present */
            if (usbdev->loadedbitmap && usbdev->hbm) GpiDeleteBitmap(usbdev->hbm);
            if (usbdev->loadedbitmap && usbdev->hbm2) GpiDeleteBitmap(usbdev->hbm2);
            /* Remove the device from the list */
            usblist->remove(i);
            break;
        }
    }
}


/*
// Remove all devices from the list, starting at the end.
// This results in an empty list.
*/
void CleanUSBDevicesList(CList *usblist)
{
    char    buf[256]    = "\0";
    struct usb_device_descriptor *pDevDesc;         // Pointer to device-descriptor.

    //~ sprintf(buf, "CleanUSBDevicesList[bi]: usblist: %08lX, length: %d", usblist, usblist->length());

    //CLIst *pUSBDevicesList;

    /*
    // Iterate over list from end to start.
    */
    for(int a=usblist->length()-1; a>=0; a--) {
        //printf("Add record: %d\n", a);
        USBDeviceReport *usbdev;                      // Pointer to device-report.

        //~ sprintf(buf, "CleanUSBDevicesList: iteration: %d", a);
        //~ __debug(NULL, buf, DBG_LBOX);


        usbdev=(USBDeviceReport *)usblist->query(a);  // Get device at specified index.

        /*
        RECORDCORE *rec=usbdev->myrec;
        if(rec->hbmBitmap) GpiDeleteBitmap(rec->hbmBitmap);
        */

        if(usbdev->loadedbitmap && usbdev->hbm) GpiDeleteBitmap(usbdev->hbm);     // Free bitmap if one exists.
        if(usbdev->loadedbitmap && usbdev->hbm2) Gpi2DeleteBitmap(usbdev->hbm2);  // Free bitmap if one exists.

        usblist->delete_data=0;   // Should the List delete the data...
        usblist->remove(a);       // Remove the device from the list.

        //usbdev=(USBDeviceReport *) pws->currentUSBDevicesList->query(a);
        //pDevDesc= (struct usb_device_descriptor *)usbdev->data;
    }

    //~ sprintf(buf, "CleanUSBDevicesList[ai]: usblist: %08lX, length: %d", usblist, usblist->length());


}


/*
// Copy the list of current devices to the list of previous devices.
*/
void SaveUSBDevicesList()
{
    CList *oldlist;                                         // Pointer to the current to become old usb-devices.
    char    buf[256]    = "\0";


    //~ sprintf(buf, "CleanUSBDevicesList -- before");


    CleanUSBDevicesList(pws->previousUSBDevicesList);       // Empty the list of previous usb-devices.
    //~ sprintf(buf, "CleanUSBDevicesList -- after");



    struct usb_device_descriptor *pDevDesc;
    oldlist = pws->currentUSBDevicesList;                   // List of the current usb-devices.


    //~ sprintf(buf, "SaveUSBDevivesList #1");



    /*
    // Iterate over the current list and copy them
    // all to the previous-devices list.
    */
    for(int a=oldlist->length()-1; a>=0; a--) {
        //printf("Add record: %d\n", a);
        USBDeviceReport* usbdev;                            // Pointer to an usb-device-record.
        usbdev=(USBDeviceReport *)oldlist->query(a);        // Get the device at the specified index.


        //~ sprintf(buf, "SaveUSBDevivesList #2 (loop)");



        /*
        RECORDCORE *rec=usbdev->myrec;
        if(rec->hbmBitmap) GpiDeleteBitmap(rec->hbmBitmap);
        */
        USBDeviceReport* newusbdev=new USBDeviceReport;     // Create a new usb-device-report
        //usbdev->descrLen=descrLen;
        memcpy(newusbdev, usbdev, sizeof(USBDeviceReport)); // Copy the content.
        pws->previousUSBDevicesList->add(newusbdev);        // Add it to the list of previous devices.
        //usblist->delete_data=0;
        //usblist->remove(a);
        //usbdev=(USBDeviceReport *) pws->currentUSBDevicesList->query(a);
        //pDevDesc= (struct usb_device_descriptor *)usbdev->data;
    }
}








/*
// Copy current to previous.
// Empty current.
// Recreate current.
// Compare the two and handle any new and removed devices.
*/
int CollectUSB(int event)
{
    char    buf[256]    = "\0";


    sprintf(buf, "CollectUSB: initialization=%d, event=%d", initialization, event);
    __debug(NULL, buf, DBG_LBOX);


    /*
    // Copy list of current devices to list of previous devices.
    */
    //~ sprintf(buf, "SaveUSBDevivesList before");

    __debug("SaveUSBDevicesList", NULL, DBG_LBOX);
    SaveUSBDevicesList();
    //~ sprintf(buf, "SaveUSBDevivesList after");



    /*
    // Empty the list of current devices.
    */
    __debug("CleanUSBDevicesList", NULL, DBG_LBOX);
    CleanUSBDevicesList(pws->currentUSBDevicesList);
    //~ sprintf(buf, "CleanUSBDevivesList done");



    /*
    // (Re)Create the list of current devices.
    // Note that some previously gathered data on the device, like the drivemap, gets lost here.
    // That data will be restored in the compare-function below if the device is still present.
    */
    __debug("QueryUSBList", NULL, DBG_LBOX);
    QueryUSBList();
    //~ sprintf(buf, "QueryUSBDevivesList done");



    /*
    // Compare the two lists.
    // Any gathered data will be copied from the old list if the device is still present.
    // This is also where actions on insert and remove are handled !!
    */
    __debug("CompareUSBDevicesList", NULL, DBG_LBOX);
    CompareUSBDevicesList();
    //~ sprintf(buf, "CompareUSBDevivesList done");




    return 0;
}


//!: MonitorUsbDevices thread was here

/*
// #############################################################################
// # GetUsbVendorString :: Get the vendor and device-name from
// #                       ?:\eCS\SYSTEM\USBDock\usbdock.dat
// #############################################################################
*/
char*   GetUsbVendorString(char* usbid, char* buf, int bufsize) {
    FILE*       usbdocdat   = NULL;     // Handle for the usbdoc.dat file
    USHORT      svendor     = -1234;    // Vendor we are searching for
    USHORT      sdevid      = -1234;    // Device-id we are searching for
    ULONG       usbdevboth  = NULL;
    USHORT      vendor      = NULL;     // Vendor read from file
    USHORT      devid       = NULL;     // Device-id read from file
    USHORT      subid       = NULL;     // Device-sub-id read from file (some lines)
    char*       p           = NULL;     // Temp pointer
    int         c           = -1;       // Field-count
    char        lbuf[256]   = "\0";     // Local scratch-buffer
    bool        found       = false;    // Found-indicator
    char        boot_drive  = ' ';      // eCS boot-drive
    char*       unknown     = "Unknown Device";



    /* Scan the vendor and device-id into binary shorts */
    c = sscanf(usbid, "%hx:%hx", &svendor, &sdevid);


    /* Get the eCS boot-drive */
    boot_drive = QueryBootDrive();

    /* Compose the path for the vendor-strings file */
    sprintf(lbuf, "%c:\\eCS\\SYSTEM\\USBDock\\usbdock.dat", boot_drive);

    /* Op the file containing the vendor-strings */
    usbdocdat = fopen(lbuf, "r");

    /* Read lines until found or eof */
    while(fgets(lbuf, sizeof(lbuf), usbdocdat)) {
        c = sscanf(lbuf, "%hx:%hx", &vendor, &devid);   // Parse the line into vendor and devid
        if ((vendor == svendor) && (devid == sdevid)) { // Compare with the values we are searching for
            found = true;                               // Yep, we found it
            break;                                      // Break the loop
        }
    }

    /* Close the vendor-strings file */
    fclose(usbdocdat);
    usbdocdat = NULL;

    /* Compose the value to return */
    if (found) {
        lbuf[strlen(lbuf)-1] = '\0';    // Remove nl-char
        p = strchr(lbuf, '=');          // Scan for '=' separating ven:id from vendor-string
        // 2013-03-18 SHL
        // Patch for bugtracker issue: #3341
        //~ p++;                            // Advance to start of vendor-string

        /* '=' found then copy remaining string, otherwise null for not-found */
        if (p) {
            //~ strcpy(buf, p);
            strcpy(buf, p + 1);         // Copy vendor-string which follows =
            p = buf;
        }
        else {
            strcpy(buf, unknown);
            p = buf;
        }
    }
    else {

        /*
        // Special case for Iomega Zip 750, which is not in usbdock.dat.
        // This device is more than 7 years old, so why is it not in there ?
        // Also special case for old USB DISK Pro (QMotion).
        */
        usbdevboth = svendor << 16 | sdevid;
        switch (usbdevboth) {
            case 0x059b0035: {
                strcpy(buf, "Iomega Zip 750");
            }
            break;
            case 0x0d7d1620: {
                strcpy(buf, "USB DISK Pro 256MB");
            }
            break;
            case 0x04131700: {
                strcpy(buf, "Leadtek USB Disk 256MB");
            }
            break;
            case 0x04f30210: {
                strcpy(buf, "Elan Microelectronics Corp. AM-400 Hama Optical Mouse");
            }
            break;
            default: {
                strcpy(buf, unknown);
            }
            break;
        }
        p = buf;
    }

    /* Return either the buffer or a null-pointer */
    return p;
}


VOID    InsertionBeep() {
    DosBeep(1500,50);
    //~ DosBlock(50);
    MyDosSleep(50);
    DosBeep(2000,50);
}


VOID    RemovalBeep() {
    DosBeep(1500,50);
    //~ DosBlock(50);
    MyDosSleep(50);
    DosBeep(1000,50);
}


VOID    ForcedRemovalBeep() {
    DosBeep(300,100);
    //~ DosBlock(50);
    MyDosSleep(50);
    DosBeep(200,300);
}


VOID    NotifyBeep() {
    DosBeep(2500,25);
}


VOID    RefreshBeep() {
    DosBeep(2500,10);
}
