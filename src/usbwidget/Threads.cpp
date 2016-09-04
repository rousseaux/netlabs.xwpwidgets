/*****************************************************************************\
* Threads.cpp -- NeoWPS * USB Widget                                          *
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
 * This module contains all the threads used by the widget.
 */

#include    "Threads.hpp"




#ifndef     _OMIT_TEST_THREADS_
Thread*     TestThread  = null;
Thread*     TestThread2 = null;
#endif





//!: << PhantomSlayerThread >> (Purge Phantom Drives)
/**
 * This thread purges all drive-letters that are phantoms.
 * It is always activated when the Widget Button is pressed.
 * Also, it can run periodically, but this is disabled by default.
 * When a phantom drive is purged, it's drive-letter disappears.
 * However, when the drive-letter is referenced, by a dir ?: for instance,
 * it re-appears. So purging phantom drives is needed to keep them invisible
 * to the system.
 */
void    _Optlink PhantomSlayerThread(void* p) {

    /* Basic Stuff */
    APIRET      ulrc            = -1;
    HAB         hab_thread      = NULL;     // Handle Anchor Block for this thread.
    HMQ         hmq_thread      = NULL;     // handle for the message-queue for this thread
    CARDINAL32  rc              = -1;
    BOOL        brc             = FALSE;    // Boolean return-values.
    int         i,j             = 0;        // Index.
    CHAR        buf[256]        = "\0";     // Local buffer.
    ULONG       post_count      = 0;        // Semaphore posted count.
    ULONG       pp              = 0;        // Phantoms purged.

    /* Immediately return when working is disabled */
    if (!phantom_slayer_thread_working) {
        _endthread();
        return;
    }


    /* Initialize the PM sub-system for this thread */
    hab_thread = WinInitialize(NULL);

    /* Create a message-queue for this thread */
    hmq_thread = WinCreateMsgQueue(hab_thread, NULL);

    /*
    // Don't put WM_QUIT messages on this queue.
    // This solves EStyler Extended Shutdown, which otherwise would wait
    // indefinately. Or, rather the system in progressing to the next queue.
    */
    brc = WinCancelShutdown(hmq_thread, TRUE);

    __debug(NULL, "=====> PHANTOM-SLAYER-thread started", DBG_MLE | DBG_AUX);

    /* Create the event-semaphore if it does not exist yet */
    if (!hevPSthread)
        rc = DosCreateEventSem(NULL, &hevPSthread, DC_SEM_SHARED, FALSE);

    /*
    // ################################
    // # This is the main thread-loop #
    // ################################
    */


    /* Purge Phantom Drives */
    while (phantom_slayer_thread_working) {

        /* Go to sleep for a while */
        ulrc = DosWaitEventSem(hevPSthread, 15000);
        //~ DosBeep(5000, 50);

        if (!phantom_slayer_thread_working) {
            break;
        }

        /* Continue if the timeout elapsed and no periodic slaying is set */
        if (ulrc == ERROR_TIMEOUT && !phantom_slayer_periodic)
            continue;



        __debug(NULL, "=====> PHANTOM-SLAYER-thread ** waking-up **", DBG_MLE | DBG_AUX);

        pp = PurgeAllPhantoms();
        sprintf(buf,"Phantoms Purged: %d", pp);
        __debug(NULL, buf, DBG_MLE | DBG_AUX);

        __debug(NULL, "=====> PHANTOM-SLAYER-thread ** going2sleep **", DBG_MLE | DBG_AUX);


        /* Reset the semaphore */
        DosResetEventSem(hevPSthread, &post_count);

    }


    if (hevPSthread) {
        DosCloseEventSem(hevPSthread);
        hevPSthread = NULL;
    }

    __debug("PhantomSlayerThread", "**ENDING**", DBG_AUX);

    /* Destroy the message-queue */
    brc = WinDestroyMsgQueue(hmq_thread);

    /* Terminate PM usage */
    brc = WinTerminate(hab_thread);

    _endthread();
}



void    _Optlink WmQuitMonitorThread(void* p) {
    APIRET      ulrc = -1;
    HAB         hab_thread = NULL;
    HMQ         hmq_thread = NULL;
    HMQ         hmq_thread2 = NULL;
    HMQ         hmq_thread3 = NULL;
    QMSG        qmsg_thread;
    QMSG        qmsg_thread2;
    QMSG        qmsg_thread3;
    BOOL        brc = FALSE;
    BOOL        brc2 = FALSE;
    BOOL        brc3 = FALSE;
    CHAR        buf[256];

    /* Immediately return when working is disabled */
    if (!wmquit_monitor_thread_working) {
        _endthread();
        return;
    }



    /* Initialize the PM sub-system for this thread */
    hab_thread = WinInitialize(NULL);

    /* Create a message-queue for this thread */
    hmq_thread = WinCreateMsgQueue(hab_thread, NULL);
    hmq_thread2 = WinCreateMsgQueue(hab_thread, NULL);
    hmq_thread3 = WinCreateMsgQueue(myhab, NULL);

    __debug("WmQuitMonitorThread", "**STARTING**", DBG_MLE | DBG_AUX);

    while (wmquit_monitor_thread_working) {

        __debug("WmQuitMonitorThread", "WinGetMsg -- waiting", DBG_AUX);
        //~ brc = WinGetMsg(hab_thread, &qmsg_thread, NULL, 0, 0);
        brc = WinPeekMsg(hab_thread, &qmsg_thread, NULL, 0, 0, PM_NOREMOVE);
        brc2 = WinPeekMsg(hab_thread, &qmsg_thread2, NULL, 0, 0, PM_NOREMOVE);
        brc3 = WinPeekMsg(hab_thread, &qmsg_thread3, NULL, 0, 0, PM_NOREMOVE);

        sprintf(buf, "brc:%d, brc2:%d, brc3:%d", brc, brc2, brc3);
        __debug("WmQuitMonitorThread", buf, DBG_AUX);
        //~ __debug("WmQuitMonitorThread", "WinGetMsg -- got message", BND_MLE | DBG_AUX);


        /// DIT WERKT !!!
        if (brc) {
            //~ StopThreads();
            wmquit_monitor_thread_working = FALSE;
        }
        else {
            MyDosSleep(2000);
        }

    }

    __debug("WmQuitMonitorThread", "**ENDING**", DBG_AUX);

    /* Destroy the message-queue */
    brc = WinDestroyMsgQueue(hmq_thread);

    /* Terminate PM usage */
    brc = WinTerminate(hab_thread);



    _endthread();
}



//!: << DriveMonitorThread >> (Monitor drive-letter changes)
/**
 * This thread monotors changes in drive-letters.
 * When drive-letters appear or disappear, a notification message is popped up
 * to inform the user. The real work is still handled by the USB-monitor-thread
 * that reacts to insertion and removal events of usb-devices.
 */
void    _Optlink DriveMonitorThread(void* p) {

    /* Basic Stuff */
    APIRET      ulrc            = -1;
    HAB         hab_thread      = NULL;     // Handle Anchor Block for this thread.
    HMQ         hmq_thread      = NULL;     // handle for the message-queue for this thread
    QMSG        qmsg_thread;
    CARDINAL32  rc              = -1;
    BOOL        brc             = FALSE;    // Boolean return-values.
    int         i,j             = 0;        // Index.
    CHAR        buf[256]        = "\0";     // Local buffer.
    CHAR        buf2[256]       = "\0";     // Secundairy localbuffer.
    ULONG       post_count      = 0;        // Semaphore posted count.
    CHAR        txtbuf[30000]   = "\0";
    BOOL        rediscovered    = FALSE;
    int         diskindex       = NULL;     // Physical Disk the drive resides on

    /* Drive Maps */
    ULONG       drives_before   = NULL;     // before the event
    ULONG       drives_after    = NULL;     // after the event
    ULONG       drives_changed  = NULL;     // changed drives
    ULONG       drives_new      = NULL;     // new drives
    ULONG       drives_gone     = NULL;     // drives gone
    ULONG       temp_map        = NULL;     // scratch map
    ULONG       drive_mask      = 4;        // mask for C:

    //~ CHAR        drive_letter    = 'C';
    int         disk            = NULL;


    CHAR        ejectme[2]      = {0,0};
    USBDeviceReport*    usbdev  = NULL;


    /* Immediately return when working is disabled */
    if (!drive_monitor_thread_working) {
        _endthread();
        return;
    }


    /* Initialize the PM sub-system for this thread */
    hab_thread = WinInitialize(NULL);

    /* Create a message-queue for this thread */
    hmq_thread = WinCreateMsgQueue(hab_thread, NULL);
    //~ hmq_thread = WinCreateMsgQueue(myhab, NULL);

    /*
    // Don't put WM_QUIT messages on this queue.
    // This solves EStyler Extended Shutdown, which otherwise would wait
    // indefinately. Or, rather the system in progressing to the next queue.
    */
    brc = WinCancelShutdown(hmq_thread, TRUE);

    __debug(NULL, "=====> DRIVE-MONITOR-thread started", DBG_MLE | DBG_AUX);

    /* Create the event-semaphore if it does not exist yet */
    if (!hevDMthread)
        rc = DosCreateEventSem(NULL, &hevDMthread, DC_SEM_SHARED, FALSE);

    /*
    // ################################
    // # This is the main thread-loop #
    // ################################
    */


    // Monitor drive changes
    while (drive_monitor_thread_working) {

        //~ DosBeep(3500, 50);

        /* Get the drivemap at this time */
        drives_before = drive_map_dqcd();

        /* Store the map in the widget structure */
        pws->drivemap = drives_before;

        /* Go to sleep for a while */
        ulrc = DosWaitEventSem(hevDMthread, 3000);
        //~ DosBeep(5000, 50);
        //~ __debug("DriveMonitorThread", "EventSem Expired", DBG_MLE | DBG_AUX);

        if (!drive_monitor_thread_working) {
            __debug("************* DriveMonitorThread", "Working=FALSE -- Breaking out of loop", DBG_MLE | DBG_AUX);
            break;
            //~ continue;
            //! Enable or Disable MyDosSleep() below does not changes UUID of DLL ??
            //! Beam me down Scotty...
            //~ MyDosSleep(1000);
        }

        /* Get a new drivemap */
        drives_after = drive_map_dqcd();

        /* Store the map in the widget structure */
        pws->drivemap = drives_before;

        /* Check for changes */
        drive_changes(drives_before, drives_after, &drives_changed, &drives_new, &drives_gone);

        /* Continue monitoring if no drives disappeared */
        //~ if (!drives_gone)
        if (!drives_changed) {
            if (!ulrc) {
                /* Reset the semaphore */
                DosResetEventSem(hevDMthread, &post_count);
            }
            __debug("DriveMonitorThread", "No Drives Changed", DBG_MLE | DBG_AUX);
            //~ brc = WinPeekMsg(hab_thread, &qmsg_thread, NULL, 0, 0, PM_NOREMOVE);
            //~ sprintf(buf, "brc:%d", brc);
            //~ __debug("DriveMonitorThread", buf, DBG_MLE | DBG_AUX);
            if (brc) {
                //~ brc = WinGetMsg(hab_thread, &qmsg_thread, NULL, 0, 0);
                //~ sprintf(buf, "brc:%d, hwnd:%08lX, msg:%08lX, mp1:%08lX, mp2:%08lX",
                    //~ brc, qmsg_thread.hwnd, qmsg_thread.msg, qmsg_thread.mp1, qmsg_thread.mp2);
                //~ __debug("DriveMonitorThread", buf, DBG_MLE | DBG_AUX);
                //~ WinDispatchMsg(hab_thread, &qmsg_thread);
                brc = WinBroadcastMsg(qmsg_thread.hwnd, qmsg_thread.msg, qmsg_thread.mp1, qmsg_thread.mp2, BMSG_POSTQUEUE);
                //~ sprintf(buf, "brc-broadcast:%d", brc);
                //~ __debug("DriveMonitorThread", buf, DBG_MLE | DBG_AUX);
                /// DIT WERKT !!!
                //~ drive_monitor_thread_working = FALSE;
            }
            continue;
        }

        sprintf(buf,"=====> DRIVE-MONITOR-thread ** waking-up ** (%d)", ulrc);
        __debug(NULL, buf, DBG_MLE | DBG_AUX);

        if (drives_changed) {
            sprintf(buf, "=====> DRIVE-MONITOR-thread, drives_before : %08lX", drives_before);
            __debug(NULL, buf, DBG_MLE | DBG_AUX);

            sprintf(buf, "=====> DRIVE-MONITOR-thread, drives_after  : %08lX", drives_after);
            __debug(NULL, buf, DBG_MLE | DBG_AUX);

            sprintf(buf, "=====> DRIVE-MONITOR-thread, drives_changed: %08lX", drives_changed);
            __debug(NULL, buf, DBG_MLE | DBG_AUX);

            sprintf(buf, "=====> DRIVE-MONITOR-thread, drives_new    : %08lX", drives_new);
            __debug(NULL, buf, DBG_MLE | DBG_AUX);

            sprintf(buf, "=====> DRIVE-MONITOR-thread, drives_gone   : %08lX", drives_gone);
            __debug(NULL, buf, DBG_MLE | DBG_AUX);

            /* For foreign eject */
            g_drives_gone = drives_gone;
        }


        ejectme[0] = FirstDriveFromMap(drives_gone);


        /* Get the corresponding usb-device, if any. */
        usbdev = DriveLetterToUsbDevice(ejectme[0]);

        /* If a related usb-device is found, adjust it's status */
        if (usbdev) {
            usbdev->safe_eject = TRUE;
            usbdev->eject_pending = FALSE;
            usbdev->was_phantom = FALSE;
            usbdev->dletter = ejectme[0];

            /* Show notification */
            PopUpChangedDevices(SHOW_SAFE_EJECT, usbdev);
            if(enablebeeps) {
                NotifyBeep();
            }
        }

        __debug(NULL, "=====> DRIVE-MONITOR-thread ** going2sleep **", DBG_MLE | DBG_AUX);

        /* Reset the semaphore */
        DosResetEventSem(hevDMthread, &post_count);
    }

    //~ __debug(NULL, "=====> DRIVE-MONITOR-thread ended", DBG_MLE | DBG_AUX);

    if (hevDMthread) {
        DosCloseEventSem(hevDMthread);
        hevDMthread = NULL;
    }

    //~ __debug("DriveMonitorThread", "== ENDING B ==", DBG_AUX);
    //~ __debug("DriveMonitorThread", "**ENDING**", DBG_MLE | DBG_AUX);
    //~ __debug("DriveMonitorThread", "**ENDING**", DBG_AUX);
    //~ __debug("DriveMonitorThread", "== ENDING E ==", DBG_AUX);

    //~ while (WinGetMsg(hab_thread, &qmsg_thread, NULL, NULL, NULL))
        //~ WinDispatchMsg(hab_thread, &qmsg_thread);

    //~ MyDosSleep(500);

    /* Destroy the message-queue */
    brc = WinDestroyMsgQueue(hmq_thread);
    //~ brc = WinDestroyMsgQueue(myhab);

    /* Terminate PM usage */
    brc = WinTerminate(hab_thread);



    _endthread();
}


//!: << LVMthread >> (Refresh the LVM Engine)
/**
 * This thread refreshes the LVM-Engine.
 * It is activated when the user clicks on the "Refresh..." item in the
 * Widget Menu.
 */
void    _Optlink LVMthread (void* p) {

    HAB         hab_thread      = NULL;     // Handle Anchor Block for this thread.
    HMQ         hmq_thread      = NULL;     // handle for the message-queue for this thread
    APIRET      rc              = -12345678;
    BOOL        brc             = FALSE;    // Boolean return-values.
    int         dsleep          = 600;
    int         sleep           = 600;
    ULONG       ulReset         = NULL;
    ULONG       ulrc            = -1;
    char        buf[256]        = "\0";
    //SEMRECORD aSems[2]    = {NULL,NULL};;
    //USBNOTIFY NotifyID    = 0;
    //HMUX      hMuxWait    = NULL;



    /* Immediately return when working is disabled */
    if (!lvm_thread_working) {
        _endthread();
        return;
    }

    /* Initialize the PM sub-system for this thread */
    hab_thread = WinInitialize(NULL);

    /* Create a message-queue for this thread */
    hmq_thread = WinCreateMsgQueue(hab_thread, NULL);

    /*
    // Don't put WM_QUIT messages on this queue.
    // This solves EStyler Extended Shutdown, which otherwise would wait
    // indefinately. Or, rather the system in progressing to the next queue.
    */
    brc = WinCancelShutdown(hmq_thread, TRUE);

    __debug(NULL, "=====> LVM-thread started", DBG_AUX);

    /* Create the event-semaphore if it does not exist yet */
    if (!hevPushLVMthread)
        rc = DosCreateEventSem(NULL, &hevPushLVMthread, DC_SEM_SHARED, FALSE);



    /* Does this solve the seagate slowness ? -- Maybe check for usb vendor:devid for selective timeouts */
    //~ DosSleep(1000);

    /* While working is enabled do this loop */
    while(lvm_thread_working) {

        /* Wait for the semaphore to be posted -- this blocks this thread at this point */
        rc = DosWaitEventSem(hevPushLVMthread, 500);

        //~ DosBeep(4000, 20);

        //~ sprintf(buf, "Waiting for Semaphore to be posted rc: %d", rc);
        //~ __debug("LVMthread", buf, DBG_MLE | DBG_AUX);


        /* If any non-zero value, continue loop */
        if (rc)
            continue;

        // Semaphore was posted

        /* Break the loop if working was disabled while blocking on the semaphore */
        if (!lvm_thread_working) {
            break;
        }

        __debug(NULL, "=====> LVM-MONITOR-thread ** waking-up **", DBG_AUX);

        //!: Monitor Thread Wakeup Beep
        //~ DosBeep(2000, 400);
        //~ _endthread();
        //~ __debug("LVMthread", "Semaphore posted", DBG_MLE | DBG_AUX);


        //
        // Do the real work
        //
        //!: Monitor Thread Wakeup Beep 2
        //~ DosBeep(1000, 400);

        /* Reset the semaphore so the next loop-iteration blocks on it again */
        ulrc = DosResetEventSem( hevPushLVMthread, &ulReset );

        __debug("LVMthread", "=====> Refreshing LVM info", DBG_AUX);

        /* Refresh the LVM-Engine */
        RefreshLVM();

        /* Load the custom device-pictures -- disabled in this release */
        //~ sprintf(buf, "LVMthread: LoadPictures");
        //~ LoadPictures();

        /* Popup the device-inserted notification */
        //~ sprintf(buf, "LVMthread: PopUpChangedDevices");
        //~ cmnLog(__FILE__, __LINE__, __FUNCTION__, buf);
        //~ PopUpChangedDevices(SHOW_SAFE_EJECT);
        //~ WinInvalidateRect(hwndIndiDevInsert,NULL,TRUE);

        __debug(NULL, "=====> LVM-MONITOR-thread ** going2sleep **", DBG_AUX);
    }

    //~ __debug("LVM-Thread", "**ENDING**", DBG_MLE | DBG_AUX);
    __debug("LVM-Thread", "**ENDING**", DBG_AUX);

    //!: Monitor Thread End Beep
    //~ DosBeep(750, 200);

    /* Destroy the message-queue */
    brc = WinDestroyMsgQueue(hmq_thread);

    /* Terminate PM usage */
    brc = WinTerminate(hab_thread);

    _endthread();
}


//!: << MonitorUSBDevices >> (Monitor insertion and removal events)
/**
 * This thread monitors the insertion and removal of usb-devices.
 * After such an event, the lists of new, current and previous devices are
 * updated. Because of the problems with phantoms, drives handled by LVM and
 * not handled by LVM, invalid geometries, etc. a whole lot of ugly stuff
 * follows that you better not look at if you want to keep your sanity.
 */
 #pragma handler (MonitorUSBDevices)
void    _Optlink MonitorUSBDevices (void* p) {

    HAB         hab_thread      = NULL;         // Handle Anchor Block for this thread.
    HMQ         hmq_thread      = NULL;         // handle for the message-queue for this thread
    int         dsleep          = 600;          // ?
    int         sleep           = 600;          // ?
    SEMRECORD   aSems[2]        = {NULL,NULL};  // array of event-semaphores to capture insert and remove events
    USBNOTIFY   NotifyID        = 0;            // ?
    HMUX        hMuxWait        = NULL;         // handle for the mux-wait semaphore
    APIRET      rc              = 0;            // return-code
    char        buf[256]        = "\0";         // scratch-buffer
    BOOL        brc             = FALSE;        // boolean return-code
    int         i               = 0;
    ULONG       stackptr        = 0;


    /* Immediately return when working is disabled */
    if (!monitor_thread_working) {
        _endthread();
        return;
    }

    /* Initialize the PM sub-system for this thread */
    hab_thread = WinInitialize(NULL);

    /* Create a message-queue for this thread */
    hmq_thread = WinCreateMsgQueue(hab_thread, NULL);

    /*
    // Don't put WM_QUIT messages on this queue.
    // This solves EStyler Extended Shutdown, which otherwise would wait
    // indefinately. Or, rather the system in progressing to the next queue.
    */
    brc = WinCancelShutdown(hmq_thread, TRUE);

    __debug(NULL, "=====> USB-MONITOR-thread started", DBG_MLE | DBG_AUX);

    sprintf(buf, "buf:%08lX", buf);
    __debug("BUF-ON-STACK-AT-START", buf, DBG_MLE | DBG_AUX);

    /* Create the event-semaphores in reset state */
    // BOOKMARK: ** SEMAPHORE CREATION (INSERT / REMOVE EVENTS) **
    rc = DosCreateEventSem(NULL,&hevEventInserted,DC_SEM_SHARED,FALSE); // semaphore for insertions
    rc = DosCreateEventSem(NULL,&hevEventRemoved,DC_SEM_SHARED,FALSE);  // semaphore for removals

    /* Register the semaphores to catch USB-changes */
    rc = UsbRegisterChangeNotification(&NotifyID,hevEventInserted,hevEventRemoved);

    /* Local MuxWait for several insert and remove events */
    aSems[0].hsemCur = (HSEM)hevEventInserted;  // gets posted when an insert-event occurs
    aSems[0].ulUser  = 0;                       // code associated with the insert-event
    aSems[1].hsemCur = (HSEM)hevEventRemoved;   // gets posted when a remove-event occurs
    aSems[1].ulUser  = 1;                       // code associated with the remove-event


    // 2009/01/05
    //DosSleep(2000);

    /* Enumberate the USB-devices -- initial on-event call !! */
    CollectUSB(3);

    /* Create the muxwait semaphore */
    rc = DosCreateMuxWaitSem(NULL, &hMuxWait, 2, (PSEMRECORD)&aSems, DCMW_WAIT_ANY);

    if ( rc == 0 ) {
        ULONG ulWhich   = -1;
        ULONG ulCnt     = 0;
        ULONG sem_kill  = 0;

        /*
        // This is the main thread-loop.
        // It handles insert and remove events of usb-devices.
        */
        while (monitor_thread_working) {

            /*
            // Capture the current drives map just before a new usb-event.
            // Network drives could be added or removed in this time !!
            // Best would be to capture the drive-map after the event,
            // since rediscover-prms is needed anyway.
            // So the before-after changes meaning to before-after rediscover.
            */
            __debug(NULL, "DRIVES-BEFORE-ANY-EVENT", DBG_MLE | DBG_AUX);
            g_drives_before = drive_map_dqcd();
            drive_letters(buf, g_drives_before);
            __debug(NULL, buf, DBG_MLE | DBG_AUX);


            /*
            // ########################################
            // # Now block on this mux-wait semaphore #
            // ########################################
            */
            __debug(NULL, "=====> USB-MONITOR-thread   ***** BLOCKING ON SEMAPHORE *****", DBG_MLE | DBG_AUX);
            //!: BOOKMARK: ** WAITING FOR USB EVENT **
            rc = DosWaitMuxWaitSem(hMuxWait, SEM_INDEFINITE_WAIT, &ulWhich);

            /*
            // Immediately break the loop if working is disabled.
            // This can only occur in the time-window between te while-test and this if-test.
            // So, it should rarely happen.
            */
            if (!monitor_thread_working) {
                break;
            }

            __debug(NULL, "=====> USB-MONITOR-thread ** waking-up **", DBG_MLE | DBG_AUX);

            /*
            // Capture the current drives map just after this usb-event.
            */

            g_drives_before = drive_map_dqcd();

            //Open_LVM_Engine(TRUE, &rc);
            //Refresh_LVM_Engine(&rc);
            //Close_LVM_Engine();
            ////DosSleep(10000);
            //Rediscover_PRMs(&rc);

            /*
            // Immediately break the loop if working is disabled.
            // It is quite possible that working was disabled while this thread was waiting on
            // the mux-wait semaphore.
            */
            if (!monitor_thread_working) {
                //~ __debug(NULL, "MonitorUSBDevices: Ending loop because working has been set to false.", DBG_MLE | DBG_AUX);
                break;
            }


            /*
            // An USB_Event has taken place, let's find out what it was.
            */


            /*
            // DEVICE INSERTED
            */
            if (ulWhich == 0) {
                /*
                // Reset this insert-event semaphore.
                // This does not neccesarily mean that the next loop will block
                // on the mux-wait again since another event (remove) might have happend.
                */
                rc = DosResetEventSem(hevEventInserted, &ulCnt);
                __debug(NULL, "INSERT-EVENT", DBG_MLE | DBG_AUX);
                CollectUSB(ulWhich);

                /*
                // When properly ejected, the drive-letter is already gone.
                */


                /* Let the LVM-thread run ? */
                //DosPostEventSem(hevPushLVMthread);

                /* Do some sleeping */
                //sleep=600;
                //DosSleep(sleep);

                /* Refresh the LVM-Engine -- is there not a thread for that ? */
                ///////RefreshLVM();

                /* Load the custom-pictures -- this is disabled in this release */
                //LoadPictures();

                /* Show the insert-notification */
                //PopUpChangedDevices(SHOW_DEV_INSERT);
                //WinInvalidateRect(hwndIndiDevInsert,NULL,TRUE);
            }


            /*
            // DEVICE REMOVED
            */
            else {
                /*
                // Reset this remove-event semaphore.
                // This does not neccesarily mean that the next loop will block
                // on the mux-wait again since another event (insert) might have happend.
                */
                rc = DosResetEventSem(hevEventRemoved, &ulCnt);
                __debug(NULL, "REMOVE-EVENT", DBG_MLE | DBG_AUX);
                CollectUSB(ulWhich);

            } // else

            __debug(NULL, "=====> USB-MONITOR-thread ** going2sleep **", DBG_MLE | DBG_AUX);
        } // while working
    } // if create-sem succeeded


    /*
    // Thread is ending.
    */

    /* Deregister the notification semaphores */
    rc = UsbDeregisterNotification(NotifyID);

    /* Close the semaphores */
    rc = DosCloseEventSem(hevEventInserted);
    rc = DosCloseEventSem(hevEventRemoved);

    __debug(NULL, "=====> MONITOR-thread **ENDING**", DBG_AUX);

    /* Destroy the message-queue */
    brc = WinDestroyMsgQueue(hmq_thread);

    /* Terminate PM usage */
    brc = WinTerminate(hab_thread);

    _endthread();

}




//!: << EjectThread >> (Eject a removable medium)
/**
 * This thread ejects a removable medium specified by a drive-letter.
 * It could be a lengthy file-copy is in progress, in which case a the medium
 * cannot be ejected. The eject is retried for some period of time after which
 * the user is informed that the medium is not ejected.
 */
void    _Optlink EjectThread(void FAR  *pParam) {

    HAB     hab_thread          = NULL;     // Handle Anchor Block for this thread.
    HMQ     hmq_thread          = NULL;     // handle for the message-queue for this thread
    APIRET  rc                  = -1;
    BOOL    brc                 = FALSE;    // Boolean return-values.
    char    temp[256]           = "\0";
    char    msgbuf[256]         = "\0";
    char    ejectme[2]          = {0,0};
    char    ejectdrive          = ' ';
    ULONG   eletter             = 0;
    char    first_drive         = 0;
    char    txtbuf[30000]       = "\0";
    USBDeviceReport*    usbdev  = NULL;
    char    buf[256]            = "\0";


    /* Initialize the PM sub-system for this thread */
    hab_thread = WinInitialize(NULL);

    /* Create a message-queue for this thread */
    hmq_thread = WinCreateMsgQueue(hab_thread, NULL);

    /*
    // Don't put WM_QUIT messages on this queue.
    // This solves EStyler Extended Shutdown, which otherwise would wait
    // indefinately. Or, rather the system in progressing to the next queue.
    */
    brc = WinCancelShutdown(hmq_thread, TRUE);

    eletter=(ULONG)pParam;
    ejectdrive=eletter;
    ejectme[0]=ejectdrive;

    /* Assume succesful ejection */
    drvletter_safeeject=ejectme[0];

    /* Check if this if a phantom-drive and if so return immediately without doing an eject */
    if (IOCtlGetLockStatus(ejectme[0], buf, sizeof(buf)) == 0x03) {
        __debug("IOCtlGetLockStatus", buf, DBG_MLE | DBG_AUX);
        brc = WinDestroyMsgQueue(hmq_thread);
        brc = WinTerminate(hab_thread);
        _endthread();
        return;
    }

    /* Record sibling drives if any */
    drivemap_safeeject=DriveMapFromDrive(ejectme[0]);

    /* Capture drives before eject */
    g_drives_before_eject = drive_map_dqcd();

    /* Eject the drive */
    // BOOKMARK: ** EJECT DRIVE ON THREAD **

    //~ MessageBox("Debug Message", "Before EjectRemovableDrive()");


    /* Very, very, very dirty hack to eject CDROM */
    if (IOCtlIsCDROM(ejectme[0])) {
        PurgeDrive(ejectme[0]);
        brc = WinDestroyMsgQueue(hmq_thread);
        brc = WinTerminate(hab_thread);
        _endthread();
        return;
    }


    /* Eject the medium */
    rc = EjectRemovableDrive(ejectme);

    //~ MessageBox("Debug Message", "After EjectRemovableDrive()");

    /* Capture the drives after eject */
    g_drives_after_eject = drive_map_dqcd();


    /* Determine Changes */
    drive_changes(g_drives_before_eject,
                    g_drives_after_eject,
                    &g_drives_changed_after_eject,
                    &g_drives_new_after_eject,
                    &g_drives_gone_after_eject);

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
        __debug(NULL, "\n", DBG_MLE | DBG_AUX);
        __debug(NULL, "=====> EJECT-thread: DRIVE-STATUS-ON-EJECT", DBG_MLE | DBG_AUX);
        /* Dump Drive Changes */
        dump_drive_changes(txtbuf, drives_before, drives_after, drives_changed, drives_new, drives_gone);
        __debug(NULL, txtbuf, DBG_MLE | DBG_AUX);
    }



    ///first_drive = FirstDriveFromMap(


    // BOOKMARK: Entry after eject-thread ended (shows notification message)
    if ( rc == NO_ERROR) {
        sprintf(buf, "Eject: %c: OK", ejectme[0]);
        __debug("EjectThread-after", buf, DBG_MLE | DBG_AUX);

        drvletter_safeeject=ejectme[0];


        /* Get the corresponding usb-device, if any. */
        usbdev = DriveLetterToUsbDevice(ejectme[0]);

        if (usbdev)
            usbdev->dletter = ejectme[0];

        /* Show notification */
        //~ PopUpChangedDevices(SHOW_SAFE_EJECT, usbdev);


        __debug(NULL,"Posting hevDMthread", DBG_MLE | DBG_AUX);
        rc = DosPostEventSem(hevDMthread);

        /*
        if ( hwnd = WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP, InfoDlg,
        hmodMe, ID_DLG_INFO,
        (PVOID)drive) ) { // pass setup str to WM_INITDLG
        pwinhCenterWindow(hwnd);
        WinProcessDlg(hwnd);
        WinDestroyWindow(hwnd);
        }
        */

        // get widget data and its button data from QWL_USER
        //PMONITORPRIVATE pPrivate = (PMONITORPRIVATE)pWidget->pUser;
        //PMONITORSETUP pSetup=(PMONITORSETUP)&(pPrivate->Setup);

        //~ if(enablebeeps) {
            //~ NotifyBeep();
        //~ }

    }
    else {
        switch (rc) {

            /* Eject already in progress */
            case -1:
                sprintf(temp, "EjectThread: Eject error: %c, msg: %s", ejectme[0], msgbuf);
                __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

                sprintf(temp,
                        "An eject is already in progress for the device that holds drive %c:\n"
                        "Please wait for it to complete.",
                        ejectme[0]);

                WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, temp, "USB Widget", 0,
                MB_INFORMATION | MB_OK);
                break;

            /* Eject time-out -- device is not ejected */
            case -2:
                sprintf(temp, "EjectThread: Eject error: %c, msg: %s", ejectme[0], msgbuf);
                __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

                sprintf(temp,
                        "The device holding drive %c: failed to eject within a reasonable time span. (%d sec)\n"
                        "Possibly a copy operation to the drive is still in progress.\n\n"
                        "Please try the eject at a later time.\n"
                        "Drive %c: is *not* ejected, so don't pull out the medium!",
                        ejectme[0],
                        EJECT_TIMEOUT,
                        ejectme[0]);

                WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, temp, "USB Widget", 0,
                MB_WARNING | MB_OK);
                break;

            /* USB-device holding the drive has been removed */
            case -3:
                sprintf(temp, "EjectThread: Eject error: %c, msg: %s", ejectme[0], msgbuf);
                __debug(__FUNCTION__, temp, DBG_MLE | DBG_AUX);

                sprintf(temp,
                        "The device that was holding drive %c: has been removed.\n\n"
                        "Drive %c: is most likely a Phantom Drive !\n\n"
                        "Please re-insert the device that was holding\n"
                        " drive %c: to prevent system instability.",
                        ejectme[0],
                        ejectme[0],
                        ejectme[0]);

                WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, temp, "USB Widget", 0,
                MB_WARNING | MB_OK);

                /* Still do the eject even if no corresponding pdisk */
                PurgeDrive(ejectme[0]);

                break;

        }

    }

    /* Destroy the message-queue */
    brc = WinDestroyMsgQueue(hmq_thread);

    /* Terminate PM usage */
    brc = WinTerminate(hab_thread);

    _endthread();

}





///////////////////////////////// STASHED /////////////////////////////////////


//!: << ZipDriveMonitor >> (Monitor ZIP-drives)
/**
 * This thread is not used right now.
 * It was a test to handle zip-drives which are actually able to physically
 * eject their medium. Notable is that on insertion of a new medium, no event
 * is generated so polling has to be used. Since I currently have no zip-drive
 * available, further development of zip-drive support is in the fridge.
 */
void    _Optlink ZipDriveMonitor(void* p) {

    /* Basic Stuff */
    HAB         hab_thread      = NULL;     // Handle Anchor Block for this thread.
    HMQ         hmq_thread      = NULL;     // handle for the message-queue for this thread
    CARDINAL32  rc              = -1;
    BOOL        brc             = FALSE;    // Boolean return-values.
    int         i,j             = 0;        // Index.
    CHAR        buf[256]        = "\0";     // Local buffer.
    CHAR        buf2[256]       = "\0";     // Secundairy localbuffer.
    CHAR        txtbuf[4096]    = "\0";
    BOOL        rediscovered    = FALSE;

    /* Drive Maps */
    ULONG       drives_before   = NULL;     // before the event
    ULONG       drives_after    = NULL;     // after the event
    ULONG       drives_changed  = NULL;     // changed drives
    ULONG       drives_new      = NULL;     // new drives
    ULONG       drives_gone     = NULL;     // drives gone



    /* Immediately return when working is disabled */
    if (!zipdrive_thread_working) {
        _endthread();
        return;
    }

    /* Initialize the PM sub-system for this thread */
    hab_thread = WinInitialize(NULL);

    /* Create a message-queue for this thread */
    hmq_thread = WinCreateMsgQueue(hab_thread, NULL);

    /*
    // Don't put WM_QUIT messages on this queue.
    // This solves EStyler Extended Shutdown, which otherwise would wait
    // indefinately. Or, rather the system in progressing to the next queue.
    */
    brc = WinCancelShutdown(hmq_thread, TRUE);

    sprintf(buf, "=====> ZIP-thread started (%d)", zipdrive_thread_working);
    /* Send gebruiken met lokale buffer */
    __debug(NULL, buf, DBG_MLE | DBG_AUX);



    /*
    // ################################
    // # This is the main thread-loop #
    // ################################
    */
    while (zipdrive_thread_working) {
        int                 i = 0;
        CList*              usbdevs = pws->currentUSBDevicesList;
        USBDeviceReport*    usbdev = NULL;
        struct usb_device_descriptor* pDevDesc = NULL;

        sprintf(buf, "=====> ZIP-thread loop %08X,%08X", pws->currentUSBDevicesList, pws->previousUSBDevicesList);


        rediscovered = FALSE;

        for (i=0; i<usbdevs->length(); i++) {
            usbdev = (USBDeviceReport*) usbdevs->query(i);
            pDevDesc = (struct usb_device_descriptor*) usbdev->data;
            //if (usbdev->dasd) {
            if (usbdev->zipdrive) {
                    //if (!rediscovered || rc != LVM_ENGINE_NO_ERROR) {
                    if (!rediscovered) {

                        /* Capture drives before rediscover */
                        g_drives_before_rediscover = drive_map_dqcd();
                        sprintf(buf, "=====> ZIP-thread (dev=%d) loop drives before: %s", i, drive_letters(buf2, drives_before));


                        /* Rediscover */
                        Rediscover_PRMs(&rc);
                        sprintf(buf, "=====> ZIP-thread rediscover complete, rc=%d", rc);


                        /* Capture drives after rediscover */
                        g_drives_after_rediscover = drive_map_dqcd();

                        /* Determine Changes */
                        drive_changes(g_drives_before_rediscover,
                                        g_drives_after_rediscover,
                                        &g_drives_changed_after_rediscover,
                                        &g_drives_new_after_rediscover,
                                        &g_drives_gone_after_rediscover);


                        if (g_drives_changed_after_rediscover) {

                            /*
                            // Dump Drive Changes On Rediscover Zip.
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
                                __debug(NULL, "\n", DBG_MLE | DBG_AUX);
                                __debug(NULL, "=====> ZIP-thread: DRIVE-STATUS-ON-REDISCOVER-ZIP", DBG_MLE | DBG_AUX);
                                /* Dump Drive Changes */
                                dump_drive_changes(txtbuf, drives_before, drives_after, drives_changed, drives_new, drives_gone);
                                __debug(NULL, txtbuf, DBG_MLE | DBG_AUX);

                                /* Open WPS view for the drive */
                                //OpenDriveView(usbdev->dletter);

                                usbdev->drivemap = drives_new;
                                PopUpChangedDevices(SHOW_DRIVES_CHANGED, usbdev);
                            }
                        }

                        drives_gone = drives_after & g_drives_after_eject;
                        sprintf(buf, "=====> ZIP-thread loop drives ejected: %s", drive_letters(buf2, drives_gone));


                        /*
                        // When we've done a RediscoverPRMs, all drives-letters are refreshed,
                        // so there's no need to a RediscoverPRM for each usb-device in the list.
                        // So we set the bit that redicovery has taken place which will be cleared on
                        // the next iteration of the main thread loop (in about 3 seconds).
                        */
                        rediscovered = TRUE;

                    }
                    usbdev->drivemap = DriveMapFromDisk(usbdev->diskindex);
                    usbdev->dletter = FirstDriveFromMap(usbdev->drivemap);
                    if (usbdev->diskindex) pws->pdisks[usbdev->diskindex].usbid = pDevDesc->idVendor << 16 | pDevDesc->idProduct;
            }
        }



        /*
        // Wait 3 seconts with 100 msec granularity.
        // So stopping the thread by clearing zipdrive_thread_working is recognized
        // within 100 msec.
        */
        {
            for (int i=0; i<30 && zipdrive_thread_working; i++) {
                DosBlock(100);
            }
        }
    }


    /* SendMessage werkt hier niet !! */
    //~ WinPostMsg(WinWindowFromID(hdlgDebugDialog, ID_LB_1), LM_INSERTITEM, (MPARAM) LIT_END, (MPARAM) "=====> ZIP-thread ended");
    __debug(NULL, "=====> ZIP-thread **ENDING**", DBG_AUX);

    /* Destroy the message-queue */
    brc = WinDestroyMsgQueue(hmq_thread);

    /* Terminate PM usage */
    brc = WinTerminate(hab_thread);

    _endthread();
}

///
//!: ---------------------------------------------------- [ START THE THREADS ]
///


/**
 * Here all the working threads are started.
 * You can set start_threads to FALSE to disable them for debugging purposes.
 * Note that the EjectThread does not loop and is a one-shot thread.
 * It is activated on eject, so it is not present here.
 */
void    StartThreads() {
    BOOL    start_threads = TRUE;
    BOOL    start_threads_ex = FALSE;
    char    buf[356] = "\0";
    APIRET  ulrc = -1;
    BOOL    brc = FALSE;

    __debug(__FUNCTION__, "BEGIN", DBG_MLE | DBG_AUX);

#ifndef     _OMIT_TEST_THREADS_
    /* Start the C++ test-threads */
    if (start_threads_ex) {
        TestThread = new MonitorThread();
        TestThread->setDebug(true);
        TestThread->start();
        //~ TestThread->fire();

        //~ DosBlock(2000);

        TestThread2 = new DrivesThread();
        TestThread2->setDebug(true);
        TestThread2->start();
    }
#endif

    /* Start the normal threads */
    if (start_threads) {
        __debug(NULL, "Starting Threads...", DBG_MLE | DBG_AUX);


        //!: -------------------------------------- [ Start WmQuitMonitor ]
        /**
         * This thread monitors WM_QUIT messages.
         * This solves the EStyler ShutDown.
         */

        wmquit_monitor_thread_working = TRUE;
        tid_wmquit_monitor = _beginthread(
            WmQuitMonitorThread,
            NULL,
            10000,
            NULL
        );
        ulrc = DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 10, tid_wmquit_monitor);
        sprintf(buf, "ulrc: %d", ulrc);
        __debug("DosSetPriority(WmQuitMonitor-thread)[TC]", buf, DBG_MLE | DBG_AUX);

        //!: ----------------------------------------- [ Start LVM-thread ]
        /**
         * This thread monitors the LVM-system.
         */

        /* Enable the loop */
        lvm_thread_working = TRUE;

        /* Start the LVM-thread */
        tid_lvm = _beginthread(
            LVMthread,
            NULL,
            40000,                  // Note the large stack !
            NULL
        );

        ulrc = DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 10, tid_lvm);
        sprintf(buf, "ulrc: %d", ulrc);
        __debug("DosSetPriority(LVM-thread)[TC]", buf, DBG_MLE | DBG_AUX);

        //~ rc = DosCreateThread(&tid_lvm,          /* Thread ID (returned by function) */
                            //~ LVMthread,          /* Address of thread program        */
                            //~ NULL,               /* Parameter passed to ThreadProc   */
                            //~ CREATE_READY |      /* Thread is ready when created     */
                            //~ STACK_COMMITTED,    /* Do pre-commit stack pages        */
                            //~ 40000L);            /* Stack size, rounded to page bdy  */

        //~ sprintf(buf, "tid_lvm: %08lX, rc: %d", tid_lvm, rc);
        //~ __debug("DosCreateThread->LVM-thread", buf, DBG_MLE | DBG_AUX);


        //!: ----------------------------------------- [ Start USB-thread ]
        /**
         * This thread monitors USB events.
         * It saves the current drive-map and then blocks on a mutex
         * sempahore that is posted by the USB-system on either insertion
         * or removal of an usb-device. When woken-up it saves a new
         * drive-map and firgures-out any changes. If new or deleted
         * drive-letters are related to an usb-device, action is
         * undertaken. If not, the change is ignored.
         * (like network or other drives)
         */


        /* Enable the loop */
        monitor_thread_working = TRUE;

        /* Start the USB-thread */
        tid_monitor = _beginthread(
            MonitorUSBDevices,
            NULL,
            //!: ** 40000 too small when using FixProblemDialog ??? **
            //!: ** 70000 bytes stack does not work ??? **
            60000,                                // Note the large stack !
            NULL
        );


        /* Start the USB-thread */
        //~ ulrc = DosCreateThread(
            //~ &tid_monitor,
            //~ (PFNTHREAD)MonitorUSBDevices,
            //~ NULL,
            //~ CREATE_READY | STACK_SPARSE,
            //~ 60000
        //~ );
        ulrc = DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 10, tid_monitor);
        sprintf(buf, "ulrc: %d", ulrc);
        __debug("DosSetPriority(USB-MONITOR-thread)[TC]", buf, DBG_MLE | DBG_AUX);


        //!: -------------------------------------- [ Start SLAYER-thread ]
        /**
         * This thread purges phantom drives.
         * When clicking on the Widget Button, this thread is always
         * activated, because refreshing LVM brings phantom drives back.
         * It can also run periodically if phantom_Slayer_periodic is set.
         */

        /* Enable the working loop */
        phantom_slayer_thread_working = TRUE;

        /* Start the SLAYER-thread */
        tid_phantom_slayer = _beginthread(
            PhantomSlayerThread,
            NULL,
            40000,                                // Note the large stack !
            NULL
        );

        ulrc = DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 10, tid_phantom_slayer);
        sprintf(buf, "ulrc: %d", ulrc);
        __debug("DosSetPriority(PHANTOM-SLAYER-thread)[TC]", buf, DBG_MLE | DBG_AUX);

        //!: -------------------------------------- [ Start DRIVES-thread ]
        /**
         * This thread monitors drive-letter changes.
         * It was added later to support foreign eject, like from the
         * command-line or the WPS. It's functionality should be used by
         * the USB-thread but that requires special timing to be able to
         * relate drive-letters to usb-devices. For now it's purpose is to
         * support foreign eject only. Therefore it only acts when
         * drive-latters vanish because of the foreign eject.
         */

        /* Enable the working loop */
        drive_monitor_thread_working = TRUE;

        /* Start the DRIVEMON-thread */
        tid_drive_monitor = _beginthread(
            DriveMonitorThread,
            NULL,
            40000,                                // Note the large stack !
            NULL
        );

        ulrc = DosSetPriority(PRTYS_THREAD, PRTYC_TIMECRITICAL, 10, tid_drive_monitor);
        sprintf(buf, "ulrc: %d", ulrc);
        __debug("DosSetPriority(DRIVE-MONITOR-thread)[TC]", buf, DBG_MLE | DBG_AUX);


        //!: ----------------------------------------- [ Start ZIP-thread ]
        /**
         * This thread monitors ZIP-drives.
         * It is currently disabled.
         * ZIP-drives are special in that their medium can change without
         * generating insert or remove events.
         */

        /* Disable the loop so the thread exits immediately */
        zipdrive_thread_working = FALSE;                     // DISABLED !!

        /* Start the ZIP-thread */
        tid_zipdrive = _beginthread(
            ZipDriveMonitor,
            NULL,
            40000,                                // Note the large stack !
            NULL
        );


        sprintf(buf, "tid_lvm_monitor=%08lX, tid_usb_monitor=%08lX, tid_phantom_slayer=%08lX, tid_drive_monitor=%08lX",
                    tid_lvm,
                    tid_monitor,
                    tid_phantom_slayer,
                    tid_drive_monitor);

        __debug(NULL, buf, DBG_MLE | DBG_AUX);

    }

    __debug(__FUNCTION__, "END", DBG_MLE | DBG_AUX);

}



#define     ___debug(x,y,z) if (ldbg) __debug(x,y,z);


///
//!: ----------------------------------------------------- [ STOP THE THREADS ]
///


/**
 * Here all the working threads are stopped.
 * Note that the EjectThread does not loop and is a one-shot thread.
 */
void    StopThreads() {
    BOOL    stop_threads = TRUE;
    BOOL    stop_threads_ex = FALSE;
    APIRET  ulrc        = 0;
    BOOL    cleanup     = TRUE;
    char    buf[256]    = "\0";
    int     i           = 0;
    BOOL    ldbg        = TRUE;

    __debug(__FUNCTION__, "BEGIN", DBG_MLE | DBG_AUX);

#ifndef     _OMIT_TEST_THREADS_
    /* Stop the C++ test-threads */
    if (stop_threads_ex) {

        if (TestThread) {
            TestThread->stop();
            delete TestThread;
            TestThread = null;
        }

        //~ DosBlock(2000);

        if (TestThread2) {
            TestThread2->stop();
            delete TestThread2;
            TestThread2 = null;
        }
    }
#endif

    /* Stop the normal threads */
    if (stop_threads) {

        //!: ------------------------------------------ [ Stop LVM-thread ]
        /**
         * This stops the LVM-thread by clearing it's working loop and posting
         * it's semaphore. When the thread wakes up it inspects the working loop
         * variable and if cleared the thread ends.
         */

        if (tid_lvm != -1 ) {

             /* Break loop */
            lvm_thread_working = FALSE;

            /* Unblock thread */
            ulrc = DosPostEventSem(hevPushLVMthread);

            sprintf(buf, "-- waiting for lvm_thread: tid_lvm=%08X", tid_lvm);
            ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            /* Wait for thread to end */
            {
                for (int i=0; i<50; i++) {
                    ulrc = DosWaitThread(&tid_lvm, DCWW_NOWAIT);
                    sprintf(buf, "    ... %d (%d)", i, ulrc);
                    ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                    if (ulrc == NO_ERROR || ulrc == ERROR_INVALID_THREADID)
                        break;
                    //~ DosBlock(10);
                    MyDosSleep(10);
                }
            }


            /* Kill the thread if waiting for it to end was unsuccessful */
            //~ if (ulrc == NO_ERROR || ERROR_INVALID_THREADID) {
            if (ulrc == NO_ERROR || ulrc == ERROR_INVALID_THREADID) {
                sprintf(buf, "-- lvm_thread stopped: ulrc=%ld, tid_lvm=%08X", ulrc, tid_lvm);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }
            else {
                sprintf(buf, "-- lvm_thread not stopped in time, gonna kill it: ulrc=%ld, tid_lvm=%08X", ulrc, tid_lvm);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                /* Kill the thread */
                ulrc = DosKillThread(tid_lvm);
                sprintf(buf, "-- lvm_thread   killed: ulrc=%ld, tid_lvm=%08X", ulrc, tid_lvm);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);
            }

            /* Close the event-semaphore and reset its global handle */
            DosCloseEventSem(hevPushLVMthread);
            hevPushLVMthread = NULL;

            /* Reset thread-id to not running */
            tid_lvm =-1;

            ___debug(NULL, "", DBG_AUX);
        }


        //!: ------------------------------------------ [ Stop USB-thread ]
        /**
         * This stops the USB-thread by clearing it's working loop and posting
         * it's semaphore. When the thread wakes up it inspects the working loop
         * variable and if cleared the thread ends.
         */

        if (tid_monitor != -1 ) {

            /* Break loop */
            monitor_thread_working = FALSE;

            /* Unblock thread */
            ulrc = DosPostEventSem(hevEventInserted);
            ulrc = DosPostEventSem(hevEventRemoved);

            sprintf(buf, "-- waiting for monitor_thread: tid_monitor=%08X", tid_monitor);
            ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            /* Wait for thread to end */
            {
                for (int i=0; i<50; i++) {
                    ulrc = DosWaitThread(&tid_monitor, DCWW_NOWAIT);
                    sprintf(buf, "    ... %d (%d)", i, ulrc);
                    ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                    if (ulrc == NO_ERROR || ulrc == ERROR_INVALID_THREADID)
                        break;
                    //~ DosBlock(10);
                    MyDosSleep(10);
                }
            }

            /* Kill the thread if waiting for it to end was unsuccessful */
            if (ulrc == NO_ERROR || ERROR_INVALID_THREADID) {
                sprintf(buf, "-- monitor_thread stopped: ulrc=%ld, tid_monitor=%08X", ulrc, tid_monitor);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }
            else {
                sprintf(buf, "-- monitor_thread not stopped in time, gonna kill it: ulrc=%ld, tid_lvm=%08X", ulrc, tid_lvm);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                /* Kill the thread */
                ulrc = DosKillThread(tid_monitor);
                sprintf(buf, "-- monitor_thread  killed: ulrc=%ld, tid_monitor=%08X", ulrc, tid_monitor);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }


            //DosSleep(100);


            /* Close the event-semaphores and reset their global handle */
            DosCloseEventSem(hevEventInserted);
            hevEventInserted = NULL;
            DosCloseEventSem(hevEventRemoved);
            hevEventRemoved = NULL;
            // MUTEX !!

            /* Reset thread-id to not running */
            tid_monitor = -1;

            ___debug(NULL, "", DBG_AUX);
        }




        //!: ------------------------------------------ [ Stop ZIP-thread ]
        /**
         * This stops the ZIP-thread by clearing it's working loop and posting
         * it's semaphore. When the thread wakes up it inspects the working loop
         * variable and if cleared the thread ends.
         */

        if (tid_zipdrive != -1 ) {

            /* Break loop */
            zipdrive_thread_working = FALSE;

            sprintf(buf, "-- waiting for zipdrive_thread: tid_zipdrive=%08X", tid_zipdrive);
            ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            /* Wait for thread to end */
            {
                for (int i=0; i<50; i++) {
                    /*
                    // This thread is not running.
                    // So DosWaitThread would return no thread ended, causing the
                    // complete loop to run
                    // DWT also disabled here.
                    */
                    //~ ulrc = DosWaitThread(&tid_zipdrive, DCWW_NOWAIT);
                    sprintf(buf, "    ... %d (%d)", i, ulrc);
                    ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                    if (ulrc == NO_ERROR || ulrc == ERROR_INVALID_THREADID)
                        break;
                    //~ DosBlock(10);
                    MyDosSleep(10);
                }
            }

            /* Kill the thread if waiting for it to end was unsuccessful */
            if (ulrc == NO_ERROR || ERROR_INVALID_THREADID) {
                sprintf(buf, "-- zipdrive_thread stopped: ulrc=%ld, tid_zipdrive=%08X", ulrc, tid_zipdrive);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }
            else {
                sprintf(buf, "-- zipdrive_thread not stopped in time, gonna kill it: ulrc=%ld, tid_lvm=%08X", ulrc, tid_zipdrive);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                ulrc = DosKillThread(tid_zipdrive);
                sprintf(buf, "-- zipdrive_thread killed: ulrc=%ld, tid_zipdrive=%08X", ulrc, tid_zipdrive);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }


            /* Reset thread-id to not running */
            tid_zipdrive = -1;

            ___debug(NULL, "", DBG_AUX);
        }


        //!: --------------------------------------- [ Stop SLAYER-thread ]
        /**
         * This stops the SLAYER-thread by clearing it's working loop and posting
         * it's semaphore. When the thread wakes up it inspects the working loop
         * variable and if cleared the thread ends.
         */

        if (tid_phantom_slayer != -1 ) {

            /* Break loop */
            phantom_slayer_thread_working = FALSE;
            ulrc = DosPostEventSem(hevPSthread);

            sprintf(buf, "-- waiting for phantom_slayer_thread: tid_phantom_slayer=%08X", tid_phantom_slayer);
            ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            //~ DosSleep(1000);

            /* Wait for thread to end */
            {
                for (int i=0; i<50; i++) {
                    ulrc = DosWaitThread(&tid_phantom_slayer, DCWW_NOWAIT);
                    sprintf(buf, "    ... %d (%d)", i, ulrc);
                    ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                    if (ulrc == NO_ERROR || ulrc == ERROR_INVALID_THREADID)
                        break;
                    //~ DosBlock(10);
                    MyDosSleep(10);
                }
            }

            /* Kill the thread if waiting for it to end was unsuccessful */
            if (ulrc == NO_ERROR || ERROR_INVALID_THREADID) {
                sprintf(buf, "-- phantom_slayer_thread stopped: ulrc=%ld, tid_phantom_slayer=%08X", ulrc, tid_phantom_slayer);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }
            else {
                sprintf(buf, "-- phantom_slayer_thread not stopped in time, gonna kill it: ulrc=%ld, tid_phantom_slayer=%08X", ulrc, tid_phantom_slayer);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                ulrc = DosKillThread(tid_phantom_slayer);
                sprintf(buf, "-- phantom_slayer_thread killed: ulrc=%ld, tid_phantom_slayer=%08X", ulrc, tid_phantom_slayer);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }


            /* Close the event-semaphore and reset its global handle */
            DosCloseEventSem(hevPSthread);
            hevPSthread = NULL;

            /* Reset thread-id to not running */
            tid_phantom_slayer = -1;

            ___debug(NULL, "", DBG_AUX);
        }



        //!: --------------------------------------- [ Stop DRIVES-thread ]
        /**
         * This stops the DRIVES-thread by clearing it's working loop and posting
         * it's semaphore. When the thread wakes up it inspects the working loop
         * variable and if cleared the thread ends.
         */

        if (tid_drive_monitor != -1 ) {

            /* Break loop */
            drive_monitor_thread_working = FALSE;
            //~ DosSleep(1000);
            ulrc = DosPostEventSem(hevDMthread);
            //~ DosSleep(1000);
            sprintf(buf, "-- waiting for drive_monitor_thread: tid_drive_monitor=%08X, pes-ulrc=%d", tid_drive_monitor, ulrc);
            ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            //~ DosSleep(1000);

            /* Wait for thread to end */
            {
                for (int i=0; i<50; i++) {
                    /*
                    // Waiting for the Drives Monitor thread to end actually keeps
                    // the thread from ending. When setting working to FALSE and
                    // then posting the semaphore should cause the thread to break
                    // out of it's working loop and end. But it does not even post
                    // the message it breaking out of the loop. So some deadlock
                    // occurs here. Other threads behave as expected when ended
                    // this way. So waiting for it disabled for now.
                    */
                    //~ ulrc = DosWaitThread(&tid_drive_monitor, DCWW_NOWAIT);
                    sprintf(buf, "    ... %d (%d)", i, ulrc);
                    ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                    if (ulrc == NO_ERROR || ulrc == ERROR_INVALID_THREADID)
                        break;
                    //~ DosBlock(10);
                    MyDosSleep(10);
                }
            }

            /* Kill the thread if waiting for it to end was unsuccessful */
            if (ulrc == NO_ERROR || ERROR_INVALID_THREADID) {
                sprintf(buf, "-- drive_monitor_thread stopped: ulrc=%ld, tid_drive_monitor=%08X", ulrc, tid_drive_monitor);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }
            else {
                sprintf(buf, "-- drive_monitor_thread not stopped in time, gonna kill it: ulrc=%ld, tid_lvm=%08X", ulrc, tid_drive_monitor);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                ulrc = DosKillThread(tid_drive_monitor);
                sprintf(buf, "-- drive_monitor_thread killed: ulrc=%ld, tid_drive_monitor=%08X", ulrc, tid_drive_monitor);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }


            /* Close the event-semaphore and reset its global handle */
            DosCloseEventSem(hevDMthread);
            hevDMthread = NULL;

            /* Reset thread-id to not running */
            tid_drive_monitor = -1;

            ___debug(NULL, "", DBG_AUX);
        }



        //!: ---------------------------------------- [ Stop EJECT-thread ]
        /**
         * While the EJECT-thread is a one-shot thread, we must make sure it's not
         * running anymore when we terminate.
         */

        if (tid_eject != -1 ) {

            sprintf(buf, "-- waiting for eject_thread: tid_eject=%08X", tid_eject);
            ___debug(NULL, buf, DBG_MLE|DBG_AUX);


            /* Wait for thread to end */
            {
                for (int i=0; i<50; i++) {
                    ulrc = DosWaitThread(&tid_eject, DCWW_NOWAIT);
                    sprintf(buf, "    ... %d (%d)", i, ulrc);
                    ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                    if (ulrc == NO_ERROR || ulrc == ERROR_INVALID_THREADID)
                        break;
                    //~ DosBlock(10);
                    MyDosSleep(10);
                }
            }

            /* Kill the thread if waiting for it to end was unsuccessful */
            if (ulrc == NO_ERROR || ERROR_INVALID_THREADID) {
                sprintf(buf, "-- eject_thread stopped: ulrc=%ld, tid_eject=%08X", ulrc, tid_eject);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                tid_eject = -1;
            }
            else {
                sprintf(buf, "-- eject_thread not stopped in time, gonna kill it: ulrc=%ld, tid_lvm=%08X", ulrc, tid_eject);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                ulrc = DosKillThread(tid_eject);
                sprintf(buf, "-- eject_eject    killed: ulrc=%ld, tid_eject=%08X", ulrc, tid_eject);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }

            //DosSleep(100);

            /* Reset thread-id to not running */
            tid_eject = -1;

            ___debug(NULL, "", DBG_AUX);
        }


        //!: -------------------------------- [ Stop WmQuitMonitor-thread ]
        /**
         * This stops the WmQuit Monitr thread.
         */

        if (tid_wmquit_monitor != -1 ) {

            /* Break loop */
            wmquit_monitor_thread_working = FALSE;

            /* Unblock thread */
            //~ ulrc = DosPostEventSem(hevPushLVMthread);

            sprintf(buf, "-- waiting for wmquit_monitor_thread: tid_lvm=%08X", tid_wmquit_monitor);
            ___debug(NULL, buf, DBG_MLE|DBG_AUX);


            /* Wait for thread to end */
            {
                for (int i=0; i<50; i++) {
                    ulrc = DosWaitThread(&tid_wmquit_monitor, DCWW_NOWAIT);
                    sprintf(buf, "    ... %d (%d)", i, ulrc);
                    ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                    if (ulrc == NO_ERROR || ulrc == ERROR_INVALID_THREADID)
                        break;
                    //~ DosBlock(10);
                    MyDosSleep(10);
                }
            }


            /* Kill the thread if waiting for it to end was unsuccessful */
            if (ulrc == NO_ERROR || ERROR_INVALID_THREADID) {
                sprintf(buf, "-- wmquit-monitor_thread stopped: ulrc=%ld, tid_lvm=%08X", ulrc, tid_wmquit_monitor);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

            }
            else {
                sprintf(buf, "-- wmquit-monitor_thread not stopped in time, gonna kill it: ulrc=%ld, tid_lvm=%08X", ulrc, tid_wmquit_monitor);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);

                /* Kill the thread */
                ulrc = DosKillThread(tid_wmquit_monitor);
                sprintf(buf, "-- wmquit-monitor   killed: ulrc=%ld, tid_lvm=%08X", buf, ulrc, tid_wmquit_monitor);
                ___debug(NULL, buf, DBG_MLE|DBG_AUX);
            }

            /* Close the event-semaphore and reset its global handle */
            //~ DosCloseEventSem(hevPushLVMthread);
            //~ hevPushLVMthread = NULL;

            /* Reset thread-id to not running */
            tid_wmquit_monitor =-1;

            ___debug(NULL, "", DBG_AUX);
        }

    }

    __debug(__FUNCTION__, "END", DBG_MLE | DBG_AUX);

}



///
//!: EXPERIMENTAL -- Like the whole usbwidget :-)
///

#ifndef     _OMIT_TEST_THREADS_

#define     __tdebug(a, b, c)   if (this_thread && this_thread->getDebug()) __debug(a, b, c)
//~ #define     __tdebug(a, b, c)   if (this_thread && this_thread->getDebug()) {DosEnterCritSec(); __debug(a, b, c); DosExitCritSec();}

Thread::Thread() {
    this->debug     = false;
    this->tid       = -1;
    this->stopped   = true;
    this->flags     = null;
    this->semaphore = -1;
    this->looping   = false;
    this->hab       = -1;
    this->hmq       = -1;
    //~ this->qmsg;
}

Thread::~Thread() {
}

int     Thread::fire() {
    this->stopped = false;
    this->looping = false;
    this->tid = _beginthread(
                    Thread::threadOneshotProc,
                    NULL,
                    40000,
                    this
                );
    return this->tid;
}

int     Thread::start() {
    this->stopped = false;
    this->looping = true;
    DosCreateEventSem(NULL, (PHEV)&this->semaphore, DC_SEM_SHARED, FALSE);
    this->tid = _beginthread(
                    Thread::threadLoopingProc,
                    NULL,
                    40000,
                    this
                );
    return this->tid;
}

int     Thread::stop() {
    char    buf[256];
    for (int i=0; i<50; i++) {
        this->looping = false;
        this->unblock();
        sprintf(buf, "TID:%08lX, stopcount:%d, stopped:%d", this->getTid(), i, this->stopped);
        __debug(__FUNCTION__, buf, DBG_AUX);
        //~ if (this->stopped) break; else MyDosSleep(50);
        if (this->stopped) break; else DosBlock(500);
    }
    DosCloseEventSem(this->semaphore);
    this->semaphore = -1;
    return this->stopped;
}

int     Thread::block() {
    int     postcount;
    DosResetEventSem(this->semaphore, (PULONG)&postcount);
    return null;
}

int     Thread::unblock() {
    DosPostEventSem(this->semaphore);
    return null;
}

int     Thread::suspend() {
    return null;
}

int     Thread::resume() {
    return null;
}

int     Thread::wait(int timeout) {
    APIRET  ulrc;
    ulrc = DosWaitEventSem(this->semaphore, timeout);
    return ulrc;
}

int     Thread::kill() {
    return null;
}

/* Getters and Setters */
int     Thread::getDebug()                  {return this->debug;}
int     Thread::setDebug(int debug)         {return this->debug = debug;}
int     Thread::getTid()                    {return this->tid;}
int     Thread::setTid(int tid)             {return this->tid = tid;}
int     Thread::getStopped()                {return this->stopped;}
int     Thread::setStopped(int stopped)     {return this->stopped = stopped;}
int     Thread::getFlags()                  {return this->flags;}
int     Thread::setFlags(int flags)         {return this->flags = flags;}
int     Thread::getSemaphore()              {return this->semaphore;}
int     Thread::setSemaphore(int semaphore) {return this->semaphore = semaphore;}
int     Thread::getLooping()                {return this->looping;}
int     Thread::setLooping(int looping)     {return this->looping = looping;}
int     Thread::getHab()                    {return this->hab;}
int     Thread::setHab(int hab)             {return this->hab = hab;}
int     Thread::getHmq()                    {return this->hmq;}
int     Thread::setHmq(int hmq)             {return this->hmq = hmq;}




void    Thread::threadProc() {
    char        buf[256]    = "\0";

    sprintf(buf, "--THREADPROC-- TID:%08lX", this->getTid());
    __debug(__FUNCTION__, buf, DBG_AUX);
}


void    Thread::threadOneshotProc(void* toi) {
    BOOL        brc         = FALSE;
    APIRET      ulrc        = -1;
    HAB         hab_thread  = NULL;
    HMQ         hmq_thread  = NULL;
    QMSG        qmsg_thread;
    CHAR        buf[256]    = "\0";

    /* Get the pointer to the Thread Object Instance */
    Thread*     this_thread = (Thread*) toi;

    this_thread->threadProc();
    this_thread->stopped = true;

    _endthread();

}


/* Thread Looping Procedure */
void    Thread::threadLoopingProc(void* toi) {
    BOOL        brc         = FALSE;
    APIRET      ulrc        = -1;
    HAB         hab_thread  = NULL;
    HMQ         hmq_thread  = NULL;
    QMSG        qmsg_thread;
    CHAR        buf[256]    = "\0";
    int         count       = 0;


    /* Get the pointer to the Thread Object Instance */
    Thread*     this_thread = (Thread*) toi;

    __tdebug(__FUNCTION__, "BEGIN", DBG_AUX);

    /* Code Block that can be broken out of 'On Error' */
    do {

        /* Break out of the block if the Thread Object Instance is null */
        if (!this_thread) break;

        /* Break out of the block if we're not going to loop anyway */
        if (!this_thread->getLooping()) break;

        /* Get anchor-block and create a message-queue */
        this_thread->setHab(WinInitialize(NULL));
        this_thread->setHmq(WinCreateMsgQueue(this_thread->getHab(), NULL));

        /*
        // Don't put WM_QUIT messages on this queue.
        // This solves EStyler Extended Shutdown, which otherwise would wait
        // indefinately. Or, rather the system in progressing to the next queue.
        */
        brc = WinCancelShutdown(this_thread->getHmq(), TRUE);


        __tdebug(__FUNCTION__, "Entering Loop...", DBG_AUX);

        /*
        // This is the main thread loop.
        */
        while (this_thread->getLooping()) {
            sprintf(
                buf,
                "Looping(%04d)... -- tid:%08lX, hab:%08lX, hmq:%08lX, sem:%08lX, brc:%d, ulrc:%d",
                count++,
                this_thread->getTid(),
                this_thread->getHab(),
                this_thread->getHmq(),
                this_thread->getSemaphore(),
                brc,
                ulrc
            );
            __tdebug(__FUNCTION__, buf, DBG_AUX);


            /* Invoke the true work for this thread */
            this_thread->threadProc();

            //~ MyDosSleep(2000);
            ulrc = this_thread->wait(2000);
        }

        //~ __tdebug(__FUNCTION__, "Exited Loop...", DBG_MLE | DBG_USE_POST | DBG_AUX);    // MISSES THIS ONE BUT SENDS "END" TO MLE ??!!
        __tdebug(__FUNCTION__, "Exited Loop...", DBG_AUX);    // BLOCKS THREAD ENDING !!

        __debug("BDMQ", NULL, DBG_AUX);
        if (this_thread->getHmq()) WinDestroyMsgQueue(this_thread->getHmq());
        __debug("BWT", NULL, DBG_AUX);
        if (this_thread->getHab()) WinTerminate(this_thread->getHab());

    }
    while (0);



    this_thread->setStopped(true);

    __tdebug(__FUNCTION__, "END", DBG_AUX);

    /* Formally end the thread */
    _endthread();

}




void    MonitorThread::threadProc() {
    char        buf[256]    = "\0";

    sprintf(buf, "TID:%08lX", this->getTid());
    __debug(__FUNCTION__, buf, DBG_AUX);
}



void    DrivesThread::threadProc() {
    char        buf[256]    = "\0";

    sprintf(buf, "TID:%08lX", this->getTid());
    __debug(__FUNCTION__, buf, DBG_AUX);
}


#endif
