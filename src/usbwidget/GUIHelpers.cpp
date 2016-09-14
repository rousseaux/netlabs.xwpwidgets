/*****************************************************************************\
* GUIHelpers.cpp -- NeoWPS * USB Widget                                       *
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
 * This module contains a few several wrapper functions for the GUI System.
 * It is used to simplify things.
 */


#include    "GUIHelpers.hpp"


void    GUIHelpers(void) {
}

// Window members were here

// Dialog members were here

// DebugDialog members were here

// ProblemFixerDialog members were here

HBITMAP     hbmUSBThumb;
HBITMAP     hbmIconInfo;
HBITMAP     hbmIconWarning;


/*
// MESSAGES
*/
ULONG   MessageBox(PSZ title, PSZ text) {
    ULONG   ulresp = -1;

    ulresp = WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, text, title, NULL, MB_OK);
    return ulresp;
}





void LoadPictures(void)
{
    //HWND hWndMLE, hwndMLEClient, hwndMLEFrame;
    int         MLEcounter  = 0;
    char        picpath1[256]   = "\0";
    char        picpath2[256]   = "\0";
    HBITMAP     hbm             = NULL;
    POINTL      lbPos           = {0,0};
    char        buf[256]        = "\0";
    BOOL        q               = FALSE;
    BOOL        q2              = FALSE;
    HPS         hpsScreenPS     = NULL;
    HPS         hps             = NULL;
    HWND        hWnd            = NULL;
    POINTL      ptl             = {0,0};
    int         dnum            = 0;
    BYTE*       pngdata         = 0;
    ULONG       pngsize         = 0;
    FILE*       pngfile         = 0;
    HBITMAP2    hbm2            = 0;
    BOOL        nopng           = FALSE;

    unsigned int    val         = 0;

    // Disabled in this version.
    // Might make a come-back in the redisigned widget.
    return;

    //ULONG flFrame =  FCF_SIZEBORDER | FCF_SHELLPOSITION | FCF_NOBYTEALIGN;

    /*
    hwndMLEFrame = WinCreateStdWindow( HWND_DESKTOP, 0,
                                    &flFrame, WC_STATIC, "mamba",
                                    WS_VISIBLE,
                                    NULLHANDLE, MLEcounter, &hwndMLEClient );

    WinSetPresParam(hwndMLEClient,PP_FONTNAMESIZE,strlen("9.WarpSans")+1,"9.WarpSans");
    */

    char c;
    //eSysInfo_QueryBootableDisk(&c);

  ULONG aulSysInfo[QSV_MAX] = {0};      /* System Information Data Buffer */
  APIRET  rc                  = NO_ERROR;  /* Return code                   */

  //if(bootdrv==0) return 1;

  rc = DosQuerySysInfo(1L,               /* Request all available system    */
                        QSV_MAX ,   /* information                  */
                        (PVOID)aulSysInfo,
                        sizeof(ULONG)*QSV_MAX);

  c=aulSysInfo[QSV_BOOT_DRIVE-1]+'A'-1;


    CList*                          usblist = pws->currentUSBDevicesList;
    struct  usb_device_descriptor*  pDevDesc;

    //CLIst *pUSBDevicesList;

    for(int a=0; a<usblist->length(); a++) {
      //printf("Add record: %d\n", a);
      USBDeviceReport *usbdev;

      usbdev=(USBDeviceReport *)usblist->query(a);

      if (usbdev->iamnew==1) {

         pDevDesc= (struct usb_device_descriptor *)usbdev->data;
         //printf("LEN: %04x:%04x = %d\n", pDevDesc->idVendor, pDevDesc->idProduct, descrLen);

         //sprintf(picpath, "E:\\ecs\\SYSTEM\\USBDock\\pics\\mamba.png");
         sprintf(picpath1, "%c:\\ecs\\SYSTEM\\USBDock\\pics\\%04x%04x-128.png", c, pDevDesc->idVendor, pDevDesc->idProduct);

         val= pDevDesc->bcdDevice;
         sprintf(picpath2, "%c:\\ecs\\SYSTEM\\USBDock\\pics\\%04x%04x%d%d%d%d-128.png", c, pDevDesc->idVendor, pDevDesc->idProduct, (val>>12)&0xF, (val>>8)&0xF, (val>>4)&0xF, (val)&0xF);

         q2=file_exists(picpath2);
         q=file_exists(picpath1);

        //usbdev->hbm=hbmUSBThumb;
        //usbdev->loadedbitmap=0;

         //hps = WinGetPS(hwndIndiDevInsert);

         if(q2) {
            // 2010/03
            //printf("0\n");
            //usbdev->hbm=Gpi2GetBitmap (hps, (char*)picpath2);
            pngsize=filesize(picpath2);

            if(pngsize) {
                pngdata=new BYTE[pngsize];

                pngfile = fopen(picpath2, "rb");
                fread(pngdata, 1, pngsize, pngfile);
                fclose(pngfile);

                hbm2=Gpi2ReadPng((PBYTE)pngdata,          // PNG data
                        0x0, // Background color (see below)
                        FL_STRIPALPHA);

                if(hbm2==NULLHANDLE) {
                  nopng=1;
                }
                else {
                  usbdev->hbm2=hbm2;
                  usbdev->loadedbitmap=1;
                }
                delete []pngdata;
            }
         }
         else {
            if(q) {
                // 2010/03
                //usbdev->hbm=Gpi2GetBitmap (hps, (char*)picpath1);
                pngsize=filesize(picpath1);
                if(pngsize) {
                  pngdata=new BYTE[pngsize];

                  pngfile = fopen(picpath1, "rb");
                  fread(pngdata, 1, pngsize, pngfile);
                  fclose(pngfile);

                  hbm2=Gpi2ReadPng((PBYTE)pngdata,        // PNG data
                        0x0, // Background color (see below)
                        FL_STRIPALPHA);

                  if(hbm2==NULLHANDLE) {
                     nopng=1;
                  }
                  else {
                     usbdev->hbm2=hbm2;
                     usbdev->loadedbitmap=1;
                  }
                  delete pngdata;
                }

            }
            else {
                nopng=1;
            }
         }

         //WinReleasePS(hps);
         if(nopng) {
            usbdev->hbm=hbmUSBThumb;
            usbdev->loadedbitmap=0;
         }
      }
    }
}




/*
// Stolen from PPSERIAL.CPP
*/
BOOL    CenterWindow(HWND toCenterTo, HWND thisWindow)
{
    POINTL  ptTo;                                                               // Midpoint of window
    POINTL  ptThis;                                                             // Midpoint of window
    POINTL  vector;                                                             // 2D translation-vector (x,y)
    SWP     swpTo;                                                              // Dimensions of window to be centered to
    SWP     swpThis;                                                            // Dimensions of window to be centered to

    /* Query window-dimensions (also includes offset-vector from HWND_DESKTOP) */
    WinQueryWindowPos(toCenterTo, &swpTo);
    WinQueryWindowPos(thisWindow, &swpThis);

    /* Calculate mid-point of To Window */
    ptTo.x = swpTo.cx/2 + swpTo.x;
    ptTo.y = swpTo.cy/2 + swpTo.y;

    /* Calculate mid-point of This Window */
    ptThis.x = swpThis.cx/2 + swpThis.x;
    ptThis.y = swpThis.cy/2 + swpThis.y;

    /* Calculate translation-vector */
    vector.x = ptTo.x - ptThis.x;
    vector.y = ptTo.y - ptThis.y;

    /* Move the window */
    WinSetWindowPos(
        thisWindow,             // Window handle
        0,                      // Relative window-placement order
        swpThis.x+vector.x,     // Window position, x-coordinate
        swpThis.y+vector.y,     // Window position, y-coordinate
        0,                      // Window size
        0,                      // Window size
        SWP_MOVE                // Window-positioning options
    );

    return (BOOL) TRUE;                                                         // Job done
}

/* Opens the WPS view for the specified drive */
BOOL    OpenDriveView(char drive) {
    APIRET      ulrc        = -1;
    BOOL        brc         = FALSE;
    HOBJECT     hobject     = NULL;
    CHAR        buf[256]    = "\0";

    sprintf(buf, "<WP_DRIVE_%c>", toupper(drive));
    //sprintf(buf, "<WP_DRIVES>", toupper(drive));
    __debug(NULL, buf, DBG_LBOX);
    hobject = WinQueryObject(buf);

    sprintf(buf, "HOBJECT: %08X", hobject);
    __debug(NULL, buf, DBG_LBOX);

    if (hobject) {
        brc = WinOpenObject(hobject, 1, FALSE);                     // OPEN_CONTENTS (1) NOT DEFINED ?
        sprintf(buf, "BRC: %0d", brc);
    __debug(NULL, buf, DBG_LBOX);
    }

    return brc;


}
