/*****************************************************************************\
* usbshlpr.cpp -- Support DAEMON for the NeoWPS * USB Widget                  *
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
 * This module implements a DAEMON to supply named shared memory to the
 * USB Widget. It allocates a memory pool and initializes the start with a
 * custom structure understood by the Widget. The rest is initialized by
 * DosSetSubMem to enable the use of DosSubAllocMem by the Widget.
 *
 * The Widget attaches to this named shared memory and has it's C malloc and
 * free functions overridden to use DosSubAllocMem. Also, the C++ new and
 * delete operators are overloaded to use the overridden malloc and free
 * functions. This enables the Widget to retain it's state between WPS
 * restarts, especially the drive <-> usb-device relations.
 */

#include    "AllocMem.hpp"

/* The public include file for this module */
#include    "usbshlpr.hpp"

/*
// This includes the BLDLEVEL information.
// It defines a it defines a global character string so it is a source file
// and not an include file. The character string holds the BLDLEVEL information
// and is thus embedded in the resulting object file.
*/
#include    "usbshlpr.lvl"

/* Pointer to the named shared memory */
void*   shmem = NULL;
int     deamon = FALSE;


/*
// Start the Daemon; this is the default action.
*/
int     startd() {
    int             rc          = 0;
    APIRET          ulrc        = -1;
    PMASTERMEMPOOL  pmmp        = NULL;
    HEV             wait4daemon = NULL;
    HEV             wait4stop   = NULL;

    /* Try to allocate the named shared memory */
    ulrc = DosAllocSharedMem(
                &shmem,
                SHMEM_NAME,
                SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL),
                PAG_COMMIT | PAG_READ | PAG_WRITE
            );


    if (ulrc) {
        switch (ulrc) {
            case ERROR_ALREADY_EXISTS:
                printf("The Shared Memory (%s) already exists.\n", SHMEM_NAME);
                return 0;
            break;
            default:
                printf("Error %d occured, aborting.\n", ulrc);
                return 0;
            break;
        }
    }


    printf(
        "DosAllocSharedMem: size: %d, ulrc: %d\n",
        SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL),
        ulrc
    );

    /* Open the semaphore that the Widget created */
    ulrc = DosOpenEventSem(EVSEM_WAIT4DAEMON, &wait4daemon);
    printf("DosOpenEventSem: ulrc: %d\n", ulrc);

    /* First try to allocate the Master Pool */
    if (!ulrc) {
        pmmp = (PMASTERMEMPOOL) shmem;          // Cast pointer to access master structure
        pmmp->signature     = SHMEM_SIGNATURE;  // Set signature
        pmmp->poolsize      = SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL);
        pmmp->pooltype      = PT_USE_NAMED;     // Set the pooltype
        pmmp->submempool    = &pmmp[1];         // Sub Memory Pool starts here
        pmmp->pwidgetstate  = NULL;             // Pointer to the Widget State


        printf(
            "Master Pool: address:%08lX, signature:%08lX, poolsize:%d, pooltype:%d, submempool:%08lX, widgetstate: %08lX\n",
            pmmp,
            pmmp->signature,
            pmmp->poolsize,
            pmmp->pooltype,
            pmmp->submempool,
            pmmp->pwidgetstate);

        /* Now try to create the sub-allocation system */
        ulrc = DosSubSetMem(
                    pmmp->submempool,
                    DOSSUB_INIT,
                    SHMEM_SIZE+sizeof(SUBMEMPOOL)
                );

        printf("DosSubSetMem: size: %d, ulrc: %d\n", SHMEM_SIZE+sizeof(SUBMEMPOOL), ulrc);
        if (!ulrc) {
            //~ printf("Press a key to deallocate the shared memory...\n");
            //~ _getch();                               // Wait for a key press
            //!: SubSetMem Beep
            //~ DosBeep(3000,200);
            ulrc = DosPostEventSem(wait4daemon);
            printf("DosPostEventSem: ulrc: %d\n", ulrc);
            ulrc = DosCloseEventSem(wait4daemon);
            printf("DosCloseEventSem: ulrc: %d\n", ulrc);

            ulrc = DosCreateEventSem(EVSEM_WAIT4STOP, &wait4stop, DC_SEM_SHARED, FALSE);
            printf("DosCreateEventSem: ulrc: %d\n", ulrc);

            //!: BOOKMARK: Block on semaphore
            printf("DosWaitEventSem: blocking...\n");
            fflush(stdout);
            ulrc = DosWaitEventSem(wait4stop, SEM_INDEFINITE_WAIT);
            printf("DosWaitEventSem: ulrc: %d\n", ulrc);

            ulrc = DosCloseEventSem(wait4stop);
            printf("DosCloseEventSem: ulrc: %d\n", ulrc);





            //~ DosSleep(120000);
            //!: SubSetMem Beep 2
            //~ DosBeep(1000,200);
            ulrc = DosSubUnsetMem(pmmp->submempool); // Cleanup sub-allocation system
            printf("DosSubUnsetMem: ulrc: %d\n", ulrc);

            ulrc = DosFreeMem(shmem);               // Free the Master Pool
            printf("DosFreeMem: ulrc: %d\n", ulrc);

            shmem = NULL;
            rc = ulrc;


        }
        /* Setting up the sub-allocation system failed */
        else {
            DosFreeMem(shmem);          // Free the Master Pool
            printf("DosSubSetMem: ulrc: %d\n", ulrc);

            shmem = NULL;
            rc = ulrc;

        }
    }
    /* Allocating the Master Pool failed */
    else {
        shmem = NULL;
        rc = ulrc;
    }

    return rc;
}

/*
// Stop the Daemon; when /stop argument found.
*/
int     stopd() {
    int             rc          = 0;
    APIRET          ulrc        = -1;
    PMASTERMEMPOOL  pmmp        = NULL;
    HEV             wait4daemon = NULL;
    HEV             wait4stop   = NULL;

    ulrc = DosOpenEventSem(EVSEM_WAIT4STOP, &wait4stop);
    printf("DosOpenEventSem  : ulrc: %d\n", ulrc);
    ulrc = DosPostEventSem(wait4stop);
    printf("DosPostEventSem  : ulrc: %d\n", ulrc);
    ulrc = DosCloseEventSem(wait4stop);
    printf("DosCloseEventSem : ulrc: %d\n", ulrc);

    //~ rc = ulrc;

    return 0;
}



/*
// Allocate the named shared memory and wait for a key-press.
*/
int     main(int argc, char* argv[]) {
    int             rc          = 0;
    APIRET          ulrc        = -1;
    PMASTERMEMPOOL  pmmp        = NULL;
    HEV             wait4daemon = NULL;
    BOOL            stop        = FALSE;
    BOOL            start       = FALSE;
    int             action      = 0;

    /* Args present see if stop is the first (no check for other args) */
    if (argc > 1) {
        stop    = !strcmpi(argv[1], "/stop");
        action  = stop;
    }

    /* Only 1 stop is recognized, the rest is start */
    switch (action)
        case    1: {
            rc = stopd();
            break;
        default:
            rc = startd();
            break;
    }

    /* Return to system */
    printf("usbshlpr.dmn: rc: %d\n", rc);
    return rc;
}
