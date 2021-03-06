/*****************************************************************************\
* LVMHelpers.cpp -- NeoWPS * USB Widget                                       *
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
 * This module contains several helper functions for the LVM System.
 * It is used to simplify things.
 */

#include    "Debug.hpp"
#include    "APIHelpers.hpp"
#include    "USBHelpers.hpp"

#include    "LVMHelpers.hpp"

void    LVMHelpers(void) {
}




// BOOKMARK: Refresh LVM
void RefreshLVM()
{
    CARDINAL32 Error_Code = 1;
    ULONG   DriveMapOld;
    ULONG   DriveMapNew;
    ULONG   start = 2;                              // Initial disk num.
    ULONG   dnum;                                   // Disk num variable.
    CHAR     DeviceName[4];
    CHAR     ChkString[200];
    FILE *fp1,*fp2;
    FILESTATUS3  PathInfo = {{0}};                  // Buffer for path information.
    ULONG       BufSize  = sizeof(FILESTATUS3);     // Size of above buffer.
    char buf[256];


    if (enablebeeps) {
        //DosBeep(500,50);
    }

    // DosQueryCurrentDisk
    // 1 - A, 2 - B, 3 - C, ..
    DosQueryCurrentDisk(NULL, &DriveMapOld);

//!: REDISCOVER DELAY HERE ?
    Rediscover_PRMs( &Error_Code );



    // 0 - ­¥ «®¢¨â
    // 1000 - ­¥ «®¢¨â
    // 2000 - «®¢¨â

    //~ DosBlock(2000);                                 // Waar is deze voor nodig ?
    MyDosSleep(2000);                                 // Waar is deze voor nodig ?
    PushLVM();


    /*
    // Capture the current drives map just after this usb-event.
    // I hope the device not mounted yet so we can catch the drive-letters.
    */
    /*
    drives_after = drive_map_dqcd();
    drive_letters(buf, drives_after);
    __debug(NULL, "AFTER", DBG_LBOX);
    __debug(NULL, buf, DBG_LBOX);

    drives_temp = drives_before ^ drives_after;
    drive_letters(buf, drives_temp);
    __debug(NULL, "CHANGE", DBG_LBOX);
    __debug(NULL, buf, DBG_LBOX);
    */

    if ( Error_Code != LVM_ENGINE_NO_ERROR ) {
        sprintf(buf, "  RefreshLVM: Rediscover_PRMs error: %d", Error_Code);
        __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

        //fprintf(stderr,"ERROR: Rediscover_PRMs returned %d\n", Error_Code ); //
    }

    DosQueryCurrentDisk(NULL, &DriveMapNew);

    DriveMapNew>>=start-1;          /* Shift to the first drive */
    DriveMapOld>>=start-1;

    //myletter=0;
    for (dnum = 0; dnum <= 26; dnum++) {
        drvstate[dnum].attached=0;
    }

    for (dnum = start; dnum <= 26; dnum++) {

        if ((DriveMapNew&1) && (DriveMapOld&1)==0) {

            sprintf(DeviceName, "%c:", dnum+'A'-1);
            //sprintf(ChkString,"3 %s \n",DeviceName); // Device %s added.


            // Rousseau: THIS MESSAGE DOES NOT OCCUR WITH RODERICK's STICK !
            //          Search flashdisks invokes this code.
            //          ZIP-drive laat ook Device xx added niet zien !! (Maar driveletter wel present !)
            sprintf(buf, "  RefreshLVM: Device %s added", DeviceName);

            __debug(NULL, buf, DBG_LBOX);

            //myletter=dnum+'A'-1;
            drvstate[dnum].attached=1;


            strcpy(buf, "  RefreshLVM: DosQueryPathInfo()");


            // checking new device
            DosError(FERR_DISABLEHARDERR);
            Error_Code = DosQueryPathInfo(DeviceName, FIL_STANDARD,  &PathInfo, BufSize);

            checkdrives=0;

            if (Error_Code!=0 && checkdrives==TRUE) {

                //sprintf(ChkString,"4 %c: \n",dnum+'A'-1); // Checking drive

                sprintf(buf, "  RefreshLVM: starting chkdsk %c", dnum+'A'-1);
                __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

                sprintf(ChkString," chkdsk %c: /f >nul 2>nul",dnum+'A'-1);
                Error_Code = system(ChkString);

                if(Error_Code) {
                    sprintf(ChkString,"  ERROR: chkdsk %c: /f returned rc %d\n",
                    dnum+'A'-1,Error_Code); //
                }
                else {
                sprintf(ChkString,"  Drive %c: checked\n", dnum+'A'-1);
                }
            }
        }
        DriveMapNew>>=1;          /* Shift to the next drive    */
        DriveMapOld>>=1;
    }

    strcpy(buf, "RefreshLVM: end");
    __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

    if (enablebeeps) {
        //DosBeep(500,50);
    }

    return;
}




/*
// ##################################################
// ## PushLVM()                                     #
// ## ...                                           #
// ##################################################
*/
int PushLVM() {

    Volume_Control_Array        vca;
    Volume_Information_Record   volinfo;

    Drive_Control_Array         dca;
    Drive_Information_Record    driveinfo;


    CARDINAL32      rc;
    MENUITEM        mi;
    FSINFO          fsinfo;
    DRIVEDATA       DrvData;
    int             drvNumb, numItems, id, i;


    char temp[256];
    char msg[256];
    char buf[256];

    int retry = 5;
    int delay = 200;

    // Try to open the LVM Engine multiple times with delay if it fails to open.
    // This accounts for slow usb-devices.
    do {
        Open_LVM_Engine(TRUE, &rc); // Open the LVM engine
        if (rc == LVM_ENGINE_NO_ERROR)
            break;
        //~ DosBlock(delay);
        MyDosSleep(delay);
        retry--;
    }
    while (retry > 0);

    //Open_LVM_Engine(TRUE, &rc);   // Open the LVM engine
    if ( rc != LVM_ENGINE_NO_ERROR ) {
        sprintf(buf, "  !!!!!!!!!!!!!! ERROR: PushLVM: ERROR: Open_LVM_Engine failed %d", rc);
        __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);
        return 1;
    }
    else {
        __debug(__FUNCTION__, " Open_LVM_Engine SUCCESS !", DBG_MLE | DBG_AUX);
    }

    sprintf(temp, " PushLVM: let's call Get_Volume_Control_Data");
    __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

    vca = Get_Volume_Control_Data(&rc); // Get array of volume handles
    if ( rc != LVM_ENGINE_NO_ERROR )  {
        sprintf(temp, " PushLVM: ERROR: Get_Volume_Control_Data");
        __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);
        Close_LVM_Engine();
        return 1;
    }

    // Count blijft 16 op T60 met Roderick's stick !!!!!!!
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " vca.Count=%d", vca.Count);


    dca = Get_Drive_Control_Data(&rc); // Get array of drive handles
    if ( rc != LVM_ENGINE_NO_ERROR )  {
        sprintf(temp, " PushLVM: ERROR: Get_Drive_Control_Data");
        __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);
        Close_LVM_Engine();
        return 1;
    }


    // Count blijft 16 op T60 met Roderick's stick !!!!!!!
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " dca.Count=%d", dca.Count);

    strcpy(buf, "   PushLVM: Get the volume information for each volume");
    __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

    for (i = 0; i < vca.Count; i++) { // Get the volume information for each volume

        //sprintf(temp, "Menu: vca.Count: %d", i);
        //cmnLog(__FILE__, __LINE__, __FUNCTION__, temp);

        volinfo = Get_Volume_Information(vca.Volume_Control_Data[i].Volume_Handle, &rc);

        if ( rc != LVM_ENGINE_NO_ERROR ) continue;

        //sprintf(temp, "   vca, Current Drive Letter: [%c]", volinfo.Current_Drive_Letter);
        //cmnLog(__FILE__, __LINE__, __FUNCTION__, temp);

        if ( !volinfo.Current_Drive_Letter ) continue;

        DrvData.dletter = volinfo.Current_Drive_Letter;
    }

    Free_Engine_Memory(vca.Volume_Control_Data);
    Close_LVM_Engine();

    strcpy(buf, "PushLVM: end");
    __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

    return 0;
}



/*
// ###########################
// # FillMenu
// ###########################
*/
//!: << Fillmenu >>
void FillMenu(HWND hwndMenu) {

    Volume_Control_Array        vca;
    Volume_Information_Record   volinfo;

    Drive_Control_Array         dca;
    Drive_Information_Record    driveinfo;

    APIRET      ulrc        = -1;
    CARDINAL32  rc          = -1;
    MENUITEM    mi;
    FSINFO      fsinfo;
    DRIVEDATA   DrvData;
    int         drvNumb     = 0;
    int         numItems    = 0;
    int         id          = 0;
    int         i           = 0;
    USHORT      drvstatus   = 0;
    int         disk        = 0;
    char        phdrv       = '*';

    char    temp[256]       = "\0";
    char    msg[256]        = "\0";
    char    buf[256]        = "\0";
    char    buf2[256]       = "\0";

    /* These are used for finding out which drives are not handled by LVM and need special processing */
    ULONG   dqcd_drives     = 0;    // drive-map using DosQueryCurrentDisk
    ULONG   lvm_drives      = 0;    // drive-map using Get_Available_Drive_Letters
    ULONG   other_drives    = 0;    // drive-map of intersection
    ULONG   mask_drives     = 0;    // drive-map to mask already handled drive-letters
    ULONG   mask_drives2    = 0;    // drive-map to mask conflicting drive-letters
    ULONG   od_count        = 0;    // number of other drives to handle
    ULONG   od_offset       = 0;    // used to keep track while shifting bits


    /*
    // Hide any open notification windows.
    */
    WinShowWindow(hwndIndiDevInsert, FALSE);
    WinShowWindow(hwndIndiSafeEject, FALSE);
    WinShowWindow(hwndIndiDevRemove, FALSE);


    /*
    // MENU Stuff
    */

    /* Get the number of items in the popup-menu */
    numItems = LONGFROMMR(WinSendMsg(hwndMenu, MM_QUERYITEMCOUNT, 0, 0));

    /* Remove all items below the separator */
    for (i=2; i<numItems; i++) {
        id = LONGFROMMR(WinSendMsg(hwndMenu, MM_ITEMIDFROMPOSITION, MPFROMSHORT(2), 0));
        WinSendMsg(hwndMenu, MM_DELETEITEM, MPFROM2SHORT(id, FALSE), 0);
    }


    /*
    // LVM Stuff
    */

    /* Clear drives info */
    memset(Drives, 0, sizeof(Drives));

//!: o----------------------------------------------------> [ OPEN LVM ENGINE ]
    /* Open the LVM-Engine to gather information */
    Open_LVM_Engine(TRUE, &rc);
    if ( rc != LVM_ENGINE_NO_ERROR ) {
        sprintf(buf, "  FillMenu: ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!! Open_LVM_Engine failed %d", rc);
        __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);
        return;
    }

    sprintf(temp, " FillMenu: let's call Get_Volume_Control_Data");
    //~ __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

    /* Get the array of volume-control-records */
    vca = Get_Volume_Control_Data(&rc); // Get array of volume handles
    if ( rc != LVM_ENGINE_NO_ERROR ) {
        Close_LVM_Engine();
        //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " !!ERROR Get_Volume_Control_Data rc=%d", rc);
        return;
    }

    strcpy(buf, "   FillMenu: Get the volume information for each volume");
    __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

    dca = Get_Drive_Control_Data(&rc); // Get array of drive handles
    if ( rc != LVM_ENGINE_NO_ERROR ) {
        sprintf(temp, " ERROR: Get_Drive_Control_Data");
        __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);
        Close_LVM_Engine();
      return;
    }


    // Rousseau: added
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " Get_Volume_Control_Data rc=%d", rc);

    // Rousseau: added
    // Count blijft 16 op T60 met Roderick's stick !!!!!!!
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " vca.Count=%d", vca.Count);

    // Rousseau: added
    // Count blijft 16 op T60 met Roderick's stick !!!!!!!
    //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, " dca.Count=%d", dca.Count);




    dqcd_drives = drive_map_dqcd();

    lvm_drives = drive_map_lvm();

    lvm_drives = drive_map_reserved_lvm();

    lvm_drives = drive_map_reserved_lvm();

    lvm_drives = drive_map_lvm() | drive_map_reserved_lvm();
    lvm_drives &= ~0x03;


    lvm_drives = drive_map_lvm() | drive_map_reserved_lvm();
    lvm_drives &= ~0x03;


    other_drives = dqcd_drives ^ lvm_drives;
    other_drives &= ~0x03;


    //other_drives = dqcd_drives ^ lvm_drives;
    other_drives = dqcd_drives;
    other_drives &= ~0x03;

//!: o-----------------------------------------> [ READ VOLUME CONTROL ARRAYS ]
    // Get the volume information for each volume
    for (i = 0; i < vca.Count; i++) {

        char rd = 'A';

        //sprintf(temp, "Menu: vca.Count: %d", i);
        //~ __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

        volinfo = Get_Volume_Information(vca.Volume_Control_Data[i].Volume_Handle, &rc);
        if ( rc != LVM_ENGINE_NO_ERROR ) continue;

        //sprintf(temp, "   vca, Current Drive Letter: [%c]", volinfo.Current_Drive_Letter);


        if ( !volinfo.Current_Drive_Letter ) continue;

        DrvData.dletter = volinfo.Current_Drive_Letter;


        sprintf(buf, "DriveLetterConflict: %08X, Pref: %c:, Cur: %c:",
            volinfo.Drive_Letter_Conflict,
            volinfo.Drive_Letter_Preference,
            volinfo.Current_Drive_Letter
        );
        //~ __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);


        /*
        // Mask out this LVM-handled drive.
        */
        rd = volinfo.Current_Drive_Letter;
        mask_drives = 1;
        mask_drives <<= rd-'A';

        /*
        // Mask out all LVM-handled drives.
        */
        other_drives &= ~mask_drives;

        //~ sprintf(buf, "%d: - %d - %d", DrvData.dletter-'A'+1, vca.Volume_Control_Data[i].Device_Type, vca.Volume_Control_Data[i].Status);
        //~ __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);

/*
        #define LVM_HARD_DRIVE  0
        #define LVM_PRM      1
        #define NON_LVM_CDROM   2
        #define NETWORK_DRIVE   3
        #define NON_LVM_DEVICE  4
*/


//!: o---------------------------------------------> [ ASSIGN MENU DRIVE ICON ]
        /*
        // Assign an icon corresponding to the device-type.
        // This should use data from a higher level where more device-types are defined.
        // Best would be a custom drive-list where an information-record is present for every drive-letter.
        */
        switch (vca.Volume_Control_Data[i].Device_Type) {
            case  1: {
                mi.hItem = (ULONG)hptrHDD;
                break;
            }

            case  2: {
                mi.hItem = (ULONG)hptrCD;
                break;
            }

            /// BOOKMARK: UDF Fix
            /* Fix for UDF mastered CDROMs */
            case  4: {
                // If the device is a CD-ROM, add icon, else continue scanning.
                if (IOCtlIsCDROM(DrvData.dletter))
                    mi.hItem = (ULONG)hptrCD;
                else
                    continue;
                break;
            }

            // This continues the volume enumeration !
            default: {
                continue;
                break;
            }

        }

        drvNumb = DrvData.dletter-'A'+1;


        /*
        // Override with zip-icon and heal any lost connections.
        */
        {
            USBDeviceReport* usbdev = NULL;
            struct usb_device_descriptor * usbdescr = NULL;

            /*
            // Try using the usb-device-list.
            */
            usbdev = DriveLetterToUsbDevice(DrvData.dletter);
            usbdescr = (struct usb_device_descriptor *) usbdev->data;

            sprintf(buf, "FillMenu: USBDEV: %08X", usbdev);



            /* Disk this drive resides on */
            disk = DriveToDisk(DrvData.dletter);

            /* If this is a zipdrive we update the drivemap etc. */
            if (pws->pdisks[disk].zipdrive) {

                /* Override with zipicon */
                mi.hItem = (ULONG)hptrZIP;

                /* Recreate drivemap */
                pws->pdisks[disk].drivemap = DriveMapFromDrive(DrvData.dletter);

                /* If there is a link to an usb-device, follow it */
                if (pws->pdisks[disk].usbid) {

                    /* Get the pointer to the device if it exists */
                    usbdev = UsbIdToUsbDevice(pws->pdisks[disk].usbid);


                    if (usbdev) {
                        usbdescr = (struct usb_device_descriptor *) usbdev->data;

                        /* Recreate drivemap */
                        usbdev->drivemap = DriveMapFromDrive(DrvData.dletter);
                        /* Recreate first drive */
                        usbdev->dletter = FirstDriveFromMap(usbdev->drivemap);

                        sprintf(buf, "FillMenu: USBDEV-VIA-PDISK: %08X", usbdev);
                        __debug(NULL, buf, DBG_LBOX);
                    }
                    else {
                        sprintf(buf, "FillMenu: USBDEV-VIA-PDISK FAILED");
                        __debug(NULL, buf, DBG_LBOX);
                    }

                }
            }

        }

//!: o---------------------------------------------------> [ QUERY DRIVE INFO ]
        memset((char *)&fsinfo, 0, sizeof(FSINFO));
        DosQueryFSInfo(drvNumb, FSIL_VOLSER, &fsinfo, sizeof(FSINFO));


        if(fsinfo.vol.szVolLabel) {
            if (strlen(fsinfo.vol.szVolLabel))
                sprintf(DrvData.label, "[%s]", fsinfo.vol.szVolLabel);
            else
                sprintf(DrvData.label, "", fsinfo.vol.szVolLabel);
        }
        else
            sprintf(DrvData.label, "%s", "[xxx]");

        /* Check if this if a phantom-drive */
        drvstatus=IOCtlGetLockStatus(DrvData.dletter, NULL, NULL);

        sprintf(buf, "drive:%c, drvstatus:%d", DrvData.dletter, drvstatus);
        __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);


        LangeGetString(BatteryLange, "Eject %c:   %s", (char*)msg, sizeof(msg));

        // 2009/.. ¡ë«®:
        //sprintf(Drives[drvNumb-1], msg, DrvData.dletter, DrvData.label);
        //WinSendMsg(hwndMenu, MM_INSERTITEM, MPFROMP(&mi), Drives[drvNumb-1]);


//!: o-------------------------------------> [ DETERMINE PHANTOM BY DRVSTATUS ]
        if ( drvstatus == 0x03) {
        //if (TRUE) {
            mi.hItem = (ULONG)hptrHDDPHANTOM;

            __debug(NULL, buf, DBG_LBOX);
            sprintf(DrvData.label, "== PHANTOM DRIVE ==");
        }

        /* Construct the menu-item */
        sprintf(buf, msg, DrvData.dletter, DrvData.label);
        strcpy(Drives[drvNumb-1], buf);

        // add to menu
        mi.iPosition    = MIT_END;
        mi.afStyle      = MIS_OWNERDRAW;
        mi.afAttribute  = 0;
        mi.hwndSubMenu  = 0;
        mi.id           = ID_MENU_REDISCOVER+drvNumb;

        //memset((char *)msg, 0, sizeof(msg));

        /*
        if(BatteryLange==0) {
        strcpy(buf, "FillMenu: BatteryLange=0 !!!!!");
        cmnLog(__FILE__, __LINE__, __FUNCTION__, buf);
        }
        */



        //sprintf(temp, "Menu: insert {%s}", msg);
        //cmnLog(__FILE__, __LINE__, __FUNCTION__, temp);

        ulrc = (ULONG) WinSendMsg(hwndMenu, MM_INSERTITEM, MPFROMP(&mi), buf);  // Add the drive to the menu
        sprintf(buf2,"ULRC1: %d",ulrc);


        DrvData.dletter = '\0';
        DrvData.label[0] = '\0';
    }




//!: o--------------------------------------------------> [ SCAN OTHER DRIVES ]


    od_count = drive_count(other_drives);
    for (i=0; i<od_count; i++) {
        int d=1;

        while ((other_drives&0x1) != 1) {
            other_drives >>= 1;
            d++;
        }

        od_offset += d;
        other_drives >>= 1;

        DrvData.dletter = od_offset+'A'-1;

        memset((char *)&fsinfo, 0, sizeof(FSINFO));
        DosQueryFSInfo(od_offset, FSIL_VOLSER, &fsinfo, sizeof(FSINFO));
        if(fsinfo.vol.szVolLabel) {
            if (strlen(fsinfo.vol.szVolLabel))
                sprintf(DrvData.label, "[%s]", fsinfo.vol.szVolLabel);
            else
                sprintf(DrvData.label, "", fsinfo.vol.szVolLabel);
        }
        else
            sprintf(DrvData.label, "%s", "[yyy]");

        LangeGetString(BatteryLange, "Eject %c:   %s", (char*)msg, sizeof(msg));
        __debug(NULL, msg, DBG_LBOX);





//!: o-------------------------------------> [ DETERMINE PHANTOM BY DRVSTATUS ]

        /* Check if this is a phantom-drive */
        // wrong drive !
        drvstatus = IOCtlGetLockStatus(DrvData.dletter, NULL, NULL);

        if ( drvstatus == 0x03) {
        //if (TRUE) {
            mi.hItem = (ULONG)hptrHDDPHANTOM;

            __debug(NULL, buf, DBG_LBOX);

            LangeGetString(BatteryLange, "is a Phantom Drive !", (char*)buf2, sizeof(buf2));

//!: o----------------------------------------------> [ MARK PHANTOM TO PURGE ]
            phdrv = DrvData.dletter;
            //~ PurgeDrive(DrvData.dletter);

            DosPostEventSem(hevPSthread);


            if (strlen(DrvData.label)) {

                strcat(DrvData.label, "   ");
                strcat(DrvData.label, buf2);

                //sprintf(DrvData.label, "%s is a Phantom Drive !", fsinfo.vol.szVolLabel);
                sprintf(msg, "Drive %c:   %s", DrvData.dletter, DrvData.label);
            }
            else {
                //sprintf(DrvData.label, "%s is a Phantom Drive !", fsinfo.vol.szVolLabel);
                sprintf(msg, "Drive %c: %s", DrvData.dletter, buf2);


            }

        }
        else {
            mi.hItem        = (ULONG)hptrHDDBAD;
        }

        /* Construct menu-item */
        sprintf(buf, msg, od_offset+'A'-1, DrvData.label);
        strcpy(Drives[od_offset-1], buf);
        __debug(NULL, buf, DBG_LBOX);

        sprintf(buf, msg, DrvData.dletter, DrvData.label);

        sprintf(buf, "drive=%c, status=%d", DrvData.dletter, drvstatus);

        __debug(__FUNCTION__, buf, DBG_MLE | DBG_AUX);
        __debug(__FUNCTION__, DrvData.label, DBG_LBOX);



        mi.iPosition    = MIT_END;
        mi.afStyle      = MIS_OWNERDRAW;
        mi.afAttribute  = 0;
        mi.hwndSubMenu  = 0;
        mi.id           = ID_MENU_REDISCOVER+od_offset;     // ID of the menu-item


        __debug(NULL, DrvData.label, DBG_LBOX);

        /*
        // Only add the driveletter if it's a prm.
        // This uses an undocumented bit (bit 3) in the returned
        // datapacket of DSK_GETDEVICEPARAMS.
        */
        if (IOCtlIsPrmUndoc(DrvData.dletter)) {
            ulrc = (ULONG) WinSendMsg(hwndMenu, MM_INSERTITEM, MPFROMP(&mi), "Drive: ");    // Add the drive to the menu
        }

        sprintf(buf2,"ULRC2: %d",ulrc);

        __debug(NULL, buf2, DBG_LBOX);

        DrvData.dletter = '\0';
        DrvData.label[0] = '\0';

    }


//!: o---------------------------------------------------> [ CLOSE LVM ENGINE ]
    Free_Engine_Memory(vca.Volume_Control_Data);
    Close_LVM_Engine();

    strcpy(buf, "FillMenu: end\n");
    //cmnLog(__FILE__, __LINE__, __FUNCTION__, buf);

    //sprintf(temp, "Menu: FillMenu OK");
    //cmnLog(__FILE__, __LINE__, __FUNCTION__, temp);

//!: o--------------------------------------------------> [ PURGE THE PHANTOM ]
    /* Purge the Phantom !! */
    //~ PurgeDrive(phdrv);

    DosPostEventSem(hevPSthread);

}


//!: LvmThread was here


/*
// #########################################################################################
// # drive_map_lvm :: Get a drive-map ala DosQueryCurrentDisk but now using the LVM-Engine #
// #########################################################################################
*/
ULONG   drive_map_lvm(VOID) {
    CARDINAL32  rc = -1;
    CARDINAL32  ulDriveMap = 0;

    ulDriveMap = Get_Available_Drive_Letters(&rc);
    ulDriveMap = ~ulDriveMap;                           // invert to get the drives in-use
    ulDriveMap &= 0x3ffffff;                            // mask-off high bits as can be only 26 drives
    if (rc)
        ulDriveMap = NULL;
    return ulDriveMap;
}

/*
// #########################################################################################
// # drive_map_lvm :: Get a drive-map ala DosQueryCurrentDisk but now using the LVM-Engine #
// #########################################################################################
*/
ULONG   drive_map_reserved_lvm(VOID) {
    CARDINAL32  rc = -1;
    CARDINAL32  ulDriveMap = 0;

    ulDriveMap = Get_Reserved_Drive_Letters(&rc);
    if (rc)
        ulDriveMap = NULL;
    return ulDriveMap;
}

//!: DriveMonitorThread was here

//!: ZipDriveMonitorThreads were here


/*
// Refresh the LVM Engine.
*/
CARDINAL32  RefreshLVMEngine() {
    CARDINAL32                  rc      = -1;
    CARDINAL32                  rc_oe   = -1;
    CARDINAL32                  rc_rle  = -1;
    CARDINAL32                  rc_dcd  = -1;
    CARDINAL32                  rc_vcd  = -1;
    Drive_Control_Array         dca;
    Volume_Control_Array        vca;
    char    buf[256] = "\0";

    Open_LVM_Engine(TRUE, &rc_oe);
    /* Refresh if open succeeded */
    if (rc_oe == LVM_ENGINE_NO_ERROR) {
        Refresh_LVM_Engine(&rc_rle);                    // Do the refresh
        dca = Get_Drive_Control_Data(&rc_dcd);          // Reread DCD
        Free_Engine_Memory(dca.Drive_Control_Data);
        vca = Get_Volume_Control_Data(&rc_vcd);         // Reread VCD
        Free_Engine_Memory(vca.Volume_Control_Data);
        Close_LVM_Engine();                             // Close Engine
        rc = rc_dcd | rc_vcd;   // Will be non-zero if any call (except close)
                                // failed.
        sprintf(buf, "rc_rle:%d, rc_dcd:%d, rc_vcd:%d", rc_rle, rc_dcd, rc_vcd);
        __debug("RefreshLVMEngine", buf, DBG_LBOX);
    }
    /* Open failed so we'll return it's rc */
    else {
        rc = rc_oe;
    }
    return rc;
}


/*
// Dump all LVM disks and partitions attributes to a buffer.
*/
CARDINAL32  DumpDrives(PCHAR txtbuf) {
    /* Local Variables */
    int     i = 0;
    int     j = 0;
    int     k = 0;
    char    buf[256] = "\0";
    CARDINAL32  rc = -1;                    // LVM-info error-code
    Drive_Control_Array         dca;        // LVM-info about physical disks
    Drive_Information_Record    dir;        // LVM-info about logical drives
    Partition_Information_Array pia;        // LVM-info about partitions);

    /* Empty Buffer */
    txtbuf[0] = '\0';

    /*
    // Get an array of all physical disk-slots.
    // Even when a disk is not physically present stuff like drive-size or serial-number is set.
    // Also a handle is present so more information about this disk-slot can be obtained.
    // Using this handle we can get the disk-status and here is information present that has
    // more meaning. It has a drive-is-usable indicator and and IO-indicator.
    // The code below traverses the various information-records.
    // The rc is non-zero when an error occurs, like the LVM-Engine not being opened.
    */
    dca = Get_Drive_Control_Data(&rc);

    if (rc == LVM_ENGINE_NO_ERROR) {
        sprintf(buf, "Drives: rc=%d, Count=%d%s", rc, dca.Count,"\n");
        strcat(txtbuf, buf);

        /* Iterate over the drive-control-data structures */
        for (i=0; i<dca.Count; i++) {
            sprintf(buf, "DCR%02d: num=%02d, size=%d, sn=%08X, h=%08X, cyls=%05d, heads=%03d, secs=%02d, prm=%d%s",
                i,
                dca.Drive_Control_Data[i].Drive_Number,         // this is the disk index-number
                dca.Drive_Control_Data[i].Drive_Size,           // the size of the raw disk in sectors
                dca.Drive_Control_Data[i].Drive_Serial_Number,  // the serial-number (LVM generated ?)
                dca.Drive_Control_Data[i].Drive_Handle,         // a handle for the raw disk
                dca.Drive_Control_Data[i].Cylinder_Count,       // number of cylinders
                dca.Drive_Control_Data[i].Heads_Per_Cylinder,   // number of heads
                dca.Drive_Control_Data[i].Sectors_Per_Track,    // spt * heads * cyls = raw disk-size in sectors
                dca.Drive_Control_Data[i].Drive_Is_PRM,         // is this a removeable disk or not
                "\n");
            strcat(txtbuf, buf);

            /* Now we use the raw disk-handle to get more info on the disk */
            dir = Get_Drive_Status(dca.Drive_Control_Data[i].Drive_Handle, &rc);

            if (!rc) {
                sprintf(buf, "  DIR%02d: avail-secs=%d, lfb=%d, corrupt=%d, unusable=%d, ioerr=%d, bigflp=%d, diskname=\"%s\"%s",
                    i,
                    dir.Total_Available_Sectors,                // this is the free space in sectors
                    dir.Largest_Free_Block_Of_Sectors,          // free space can be scattered, this is the largest block
                    dir.Corrupt_Partition_Table,                // true if the partition-table is corrupted
                    dir.Unusable,                               // true if this disk is unusable, non-present disks have this indicator set
                    dir.IO_Error,                               // non-present disks have this set too
                    dir.Is_Big_Floppy,                          // is this a big-floppy partitioning ?
                    dir.Drive_Name,                             // the name of the disk
                    "\n");
                strcat(txtbuf, buf);
            }
            else {
                sprintf(buf, "  DIR%02d: rc=%d ERROR%s", i, rc, "\n");
                strcat(txtbuf, buf);
            }

            /*
            // A physical disk can contain more than one partition.
            // Partitions can also be combined to form a logical volume.
            // Therefore, to enumberate partitions, both the disk and volume handles can be used.
            // They give different results of course, the partitions on a disk or the partitions in a volume.
            // Below we use the raw disk and enumberate it's partitions.
            */
            pia = Get_Partitions(dca.Drive_Control_Data[i].Drive_Handle, &rc);

            if (!rc) {
                sprintf(buf, "  Partitions: rc=%d, Count=%d%s", rc, pia.Count, "\n");
                strcat(txtbuf, buf);

                /* Iterate over the partition-array */
                for (j=0; j<pia.Count; j++) {
                    /* Part I */
                    sprintf(buf, "  PRT%02d: ph=%08X, vh=%08X, dh=%08X, psn=%08X, ps=%09d%s",
                        j,
                        pia.Partition_Array[j].Partition_Handle,        // handle for this partition
                        pia.Partition_Array[j].Volume_Handle,           // handle of the volume this partition is part of, 0 otherwise
                        pia.Partition_Array[j].Drive_Handle,            // handle of the disk this partition resides on
                        pia.Partition_Array[j].Partition_Serial_Number, // serial-number (LVM generated ?)
                        pia.Partition_Array[j].Partition_Start,         // start of the partition in sectors
                        "\n");
                    strcat(txtbuf, buf);
                    /* Part II */
                    sprintf(buf, "  PRT%02d: total-part-size=%08d, usable-part-size=%08d, bl=%d, spanned=%d%s",
                        j,
                        pia.Partition_Array[j].True_Partition_Size,     // raw size
                        pia.Partition_Array[j].Usable_Partition_Size,   // size as reported to the IFSM excluding LVM-overhead
                        pia.Partition_Array[j].Boot_Limit,              // booteable partition threshold in sectors
                        pia.Partition_Array[j].Spanned_Volume,          // is the partition part of a multi-partition volume ?
                        "\n");
                    strcat(txtbuf, buf);
                    /* Part III */
                    sprintf(buf, "  PRT%02d: pri-part=%d, active=%02X, ptype=%03d, pt=%d, pstat=%d, obm=%d%s",
                        j,
                        pia.Partition_Array[j].Primary_Partition,       // true if this is a primary-partition
                        pia.Partition_Array[j].Active_Flag,             // 0x80 if marked booteable, 0x0 otherwise
                        pia.Partition_Array[j].OS_Flag,                 // partition type, returns 130/131 for 82/83 (bug ?)
                        pia.Partition_Array[j].Partition_Type,          // free space, LVM or compatibility
                        pia.Partition_Array[j].Partition_Status,        // free space, in use by a vol or not assigned
                        pia.Partition_Array[j].On_Boot_Manager_Menu,    // true if not part of a volume but is on boot-manager
                        "\n");
                    strcat(txtbuf, buf);
                    /* Part IV */
                    sprintf(buf, "  PRT%02d: vdl=\"%c\", diskname=\"%s\", filesys=\"%s\", partname=\"%s\", volname=\"%s\"%s",
                        j,
                        pia.Partition_Array[j].Volume_Drive_Letter ? pia.Partition_Array[j].Volume_Drive_Letter : '*',  // drive-letter for this partition
                        pia.Partition_Array[j].Drive_Name,              // name of the disk the partition resides on
                        pia.Partition_Array[j].File_System_Name,        // name of the file-system if recognized
                        pia.Partition_Array[j].Partition_Name,          // name of the partition
                        pia.Partition_Array[j].Volume_Name,             // name of the volume this partition is part of
                        "\n");
                    strcat(txtbuf, buf);
                    strcat(txtbuf, "\r\n");
                }
                /* Free the LVM-Engine memory for the partition-array */
                Free_Engine_Memory(pia.Partition_Array);
            }
            else {
                sprintf(buf, "  PRT%02d: rc=%d ERROR%s", j, rc, "\n");
                strcat(txtbuf, buf);
            }
            strcat(txtbuf, "\r\n");
        }
        /* Free the LVM-Engine memory for the drive-control-data */
        Free_Engine_Memory(dca.Drive_Control_Data);
    }
    else {
        sprintf(buf, "Drives: rc=%d ERROR%s", rc, "\n");
        strcat(txtbuf, buf);
    }

    return rc;
}


/*
// Dump all Volumes attributes to a buffer.
*/
CARDINAL32  DumpVolumes(PCHAR txtbuf) {
    /* Local Variables */
    int     i = 0;
    int     j = 0;
    int     k = 0;
    char    buf[256] = "\0";
    CARDINAL32  rc = -1;
    Volume_Control_Array        vca;
    Volume_Information_Record   vir;

    /* Empty Buffer */
    txtbuf[0] = '\0';

    vca = Get_Volume_Control_Data(&rc);
    if (rc == LVM_ENGINE_NO_ERROR) {
        sprintf(buf, "Volumes: rc=%d, Count=%d%s", rc, vca.Count, "\n");
        strcat(txtbuf, buf);

        for (i=0; i<vca.Count; i++) {
            sprintf(buf, "VCR%02d: vsn=%08X, vh=%08X, compat=%d, device-type=%d%s",
                i,
                vca.Volume_Control_Data[i].Volume_Serial_Number,    // lvm assigned serial-number
                vca.Volume_Control_Data[i].Volume_Handle,           // handle used to perform operations
                vca.Volume_Control_Data[i].Compatibility_Volume,    // compatibility volume
                vca.Volume_Control_Data[i].Device_Type,             // 0=HD, 1=PRM, 2=CDROM, 3=NETWORK, 4=UNKNOWN
                "\n");
            strcat(txtbuf, buf);

            /* Now we use the volume-handle to get more info on the volume */
            vir = Get_Volume_Information(vca.Volume_Control_Data[i].Volume_Handle, &rc);

            if (!rc) {

                sprintf(buf, "  VIR%02d: vs=%09d, pcount=%d, dlc=%d, compat=%d, boot=%d, pref=\"%c\", initial=\"%c\", current=\"%c\"%s",
                    i,
                    vir.Volume_Size,                // volume-size in 512-byte sectors
                    vir.Partition_Count,            // number of partitions comprising this volume
                    vir.Drive_Letter_Conflict,      // 0=UNIQUE, 1=NOTUNQ with PREFDRV, 2=NOTUNQ no PD, 3=, 4=HIDDEN
                    vir.Compatibility_Volume,       // compatibility volume
                    vir.Bootable,                   // on BM or active pri-part on disk0
                    vir.Drive_Letter_Preference ? vir.Drive_Letter_Preference : '*',    // desired drive-letter
                    vir.Initial_Drive_Letter ? vir.Initial_Drive_Letter : '*',          // initial drive-letter assigned
                    vir.Current_Drive_Letter ? vir.Current_Drive_Letter : '*',          // drive-letter currently assigned
                    "\n");

                strcat(txtbuf, buf);
                sprintf(buf, "  VIR%02d: new-vol=%d, status=%d, volname=\"%s\", filesys=\"%s\"%s",
                    i,
                    vir.New_Volume,             // this is a volume created after an lvm-open
                    vir.Status,                 // 0=NONE, 1=BOOTABLE, 2=STARTABLE, 3=INSTALLABLE
                    vir.Volume_Name,            // name of this volume
                    vir.File_System_Name,       // name of the file-system (FAT16,FAT32,HPFS,JFS,NTFS,UDF,CDFS,LINUX, etc)
                    "\n");
                strcat(txtbuf, buf);
            }
            else {
                sprintf(buf, "VIR ERROR rc=%d%s", rc, "\n");
                strcat(txtbuf, buf);
            }
            strcat(txtbuf, "\r\n");
        }
        Free_Engine_Memory(vca.Volume_Control_Data);
    }
    else {
        sprintf(buf, "Volumes: rc=%d ERROR%s", rc, "\n");
        strcat(txtbuf, buf);
    }

    return NULL;
}


/*
// Read the MBR from a Volume / Drive.
*/
CARDINAL32  ReadMBR(int disk, char* buf, ulong bufsize) {
    CARDINAL32  rc          = -1;
    CARDINAL32  rc2         = -1;

    /* Clear buffer */
    memset(buf, 0, bufsize);
    sprintf(buf, "NO MBR DATA READ");

    /* Read the first sector from the disk */
    Open_LVM_Engine(TRUE, &rc);
    if (rc == LVM_ENGINE_NO_ERROR || rc == LVM_ENGINE_ALREADY_OPEN) {
        Read_Sectors(disk, 0L, 1, buf, &rc2);
    }

    /* Only close if it was sucessfully opened; don't close if it was already open */
    if (rc == LVM_ENGINE_NO_ERROR)
        Close_LVM_Engine();

    if (rc == LVM_ENGINE_ALREADY_OPEN)
        rc = LVM_ENGINE_NO_ERROR;

    return rc;
}

