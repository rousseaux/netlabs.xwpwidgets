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


/******************************************************************************
* This module implements a DAEMON that keeps shared memory from being released
* -----------------------------------------------------------------------------
* It is not directly possible to identify which usb-device holds the mass
* storage device where one or more drives are mounted on. These relations are
* held in a list which is stored in the shared memory. If the WPS is restarted,
* the Widget can re-use the list to know which drives are mounted on which
* usb-devices.
*
* The shared memory is initialized at the start with a custom structure which
* also understood by the Widget. The rest is initialized by DosSetSubMem to
* enable the use of DosSubAllocMem by the Widget. The Widget attaches to this
*  named shared memory and has it's C malloc and free functions overridden to
* use DosSubAllocMem. Also, the C++ new and delete operators are overloaded
* to use the overridden malloc and free functions. This enables the Widget to
*  retain it's state between WPS restarts.
*
*/

/*
// Phantom Drives
// --------------
// When an usb-device with a drive mounted on it is removed without properly
// ejecting it, the drive does not disappear. Ejecting the drive will make it
// disappear, but only until the removable media are rescanned, after which it
// will reappear. Such a drive is called a *phantom* drive. Because the backing
// medium is missing, such a drive is of course not accessible. However,
// reinserting the _exact_ same usb-device will make the drive functional
// again. But, insering a _different_ usb-device will most probably cause a
// crash in LVM. The widget is capable of recovering phantom drives when the
// correct usb-device is interted again.
*/


//! ----------------------------------------------------------------- [includes]

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

//! ----------------------------------------------------------------- [globals]

/* Pointer to the named shared memory */
//~ void*   shmem = NULL;
//~ int     deamon = FALSE;

//! ------------------------------------------------------------------ [Daemon]

Daemon::Daemon() {
    __ctorb();
    this->clear();
    __ctore();
}

//~ Daemon::Daemon(int argc, char* argv[]) {
    //~ __ctorb();
    //~ this->clear();
    //~ this->argc = argc;
    //~ this->argv = argv;
    //~ __ctore();
//~ }

Daemon::~Daemon() {
    __dtorb();
    this->clear();
    __dtore();
}

void    Daemon::clear() {
    __mthd();
    this->argc          = 0;
    this->argv          = NULL;
    this->wait4daemon   = NULL;
    this->wait4stop     = NULL;
}

int     Daemon::doAction() {
    __mthd();
    printf("argc      : %d\n", this->argc);
    printf("argv[0]   : %s\n", this->argv[0]);
    printf("wait4stop : %08X\n", this->wait4stop);

    do {
        int     rc      = 0;
        BOOL    stop    = FALSE;
        BOOL    start   = FALSE;
        int     action  = 0;

        /* Args present see if stop is the first (no check for other args) */
        if (this->argc > 1) {
            stop    = !strcmpi(argv[1], "/stop");
            action  = stop;
        }

        /* Only 1 stop is recognized, the rest is start */
        switch (action)
            case    1: {
                rc = this->stop();
                break;
            default:
                rc = this->start();
                break;
        }

    } while (0);

    return 0;
}

int     Daemon::start() {
    __mthd();
    return 0;
}

int     Daemon::stop() {
    __mthd();
    return 0;
}

//! --------------------------------------------------------------- [CliDaemon]

CliDaemon::CliDaemon(int argc, char* argv[]) {
    __ctorb();
    this->clear();
    this->argc = argc;
    this->argv = argv;
    __ctore();
}

CliDaemon::~CliDaemon() {
    __dtorb();
    this->clear();
    __dtore();
}

void    CliDaemon::clear() {
    __mthd();
    this->pmmp          = NULL;
    //~ this->Daemon::clear();
}

int     CliDaemon::start() {
    int     rc      = 0;
    APIRET  ulrc    = -1;
    __mthd();

    /* Try to allocate the named shared memory */
    ulrc =  DosAllocSharedMem(
                (PPVOID) &this->pmmp,
                SHMEM_NAME,
                SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL),
                PAG_COMMIT | PAG_READ | PAG_WRITE
            );

    if (ulrc) {
        switch (ulrc) {
            case ERROR_ALREADY_EXISTS: {
                PVOID   p = NULL;
                ulrc =  DosGetNamedSharedMem(
                            &p,
                            SHMEM_NAME,
                            PAG_READ | PAG_WRITE
                        );
                printf("The Shared Memory (%s) already exists at: 0x%08x.\n", SHMEM_NAME, p);
                ulrc = -1;
                return 0;
                break;
            }
            default: {
                printf("Error %d occured, aborting.\n", ulrc);
                return 0;
                break;
            }
        }
    }

    printf(
        "DosAllocSharedMem: size: %d, ulrc: %d\n",
        SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL),
        ulrc
    );

    /* Open the semaphore that the Widget created */
    ulrc = DosOpenEventSem(EVSEM_WAIT4DAEMON, &this->wait4daemon);
    printf("DosOpenEventSem: ulrc: %d\n", ulrc);

    /* First try to allocate the Master Pool */
    if (!ulrc) {
        //~ this->pmmp = (PMASTERMEMPOOL) shmem;          // Cast pointer to access master structure
        this->pmmp->signature     = SHMEM_SIGNATURE;  // Set signature
        this->pmmp->poolsize      = SHMEM_SIZE+sizeof(MASTERMEMPOOL)+sizeof(SUBMEMPOOL);
        this->pmmp->pooltype      = PT_USE_NAMED;     // Set the pooltype
        this->pmmp->submempool    = &this->pmmp[1];         // Sub Memory Pool starts here
        this->pmmp->pwidgetstate  = NULL;             // Pointer to the Widget State


        printf(
            "Master Pool: address:%08lX, signature:%08lX, poolsize:%d, pooltype:%d, submempool:%08lX, widgetstate: %08lX\n",
            this->pmmp,
            this->pmmp->signature,
            this->pmmp->poolsize,
            this->pmmp->pooltype,
            this->pmmp->submempool,
            this->pmmp->pwidgetstate);

        /* Now try to create the sub-allocation system */
        ulrc = DosSubSetMem(
                    this->pmmp->submempool,
                    DOSSUB_INIT,
                    SHMEM_SIZE+sizeof(SUBMEMPOOL)
                );

        printf("DosSubSetMem: size: %d, ulrc: %d\n", SHMEM_SIZE+sizeof(SUBMEMPOOL), ulrc);
        if (!ulrc) {
            //~ printf("Press a key to deallocate the shared memory...\n");
            //~ _getch();                               // Wait for a key press
            //!: SubSetMem Beep
            //~ DosBeep(3000,200);
            ulrc = DosPostEventSem(this->wait4daemon);
            printf("DosPostEventSem: ulrc: %d\n", ulrc);
            ulrc = DosCloseEventSem(this->wait4daemon);
            printf("DosCloseEventSem: ulrc: %d\n", ulrc);

            ulrc = DosCreateEventSem(EVSEM_WAIT4STOP, &this->wait4stop, DC_SEM_SHARED, FALSE);
            printf("DosCreateEventSem: ulrc: %d\n", ulrc);

            //!: BOOKMARK: Block on semaphore
            printf("DosWaitEventSem: blocking...\n");
            fflush(stdout);
            ulrc = DosWaitEventSem(this->wait4stop, SEM_INDEFINITE_WAIT);
            printf("DosWaitEventSem: ulrc: %d\n", ulrc);

            ulrc = DosCloseEventSem(this->wait4stop);
            printf("DosCloseEventSem: ulrc: %d\n", ulrc);

            //~ DosSleep(120000);
            //!: SubSetMem Beep 2
            //~ DosBeep(1000,200);
            ulrc = DosSubUnsetMem(this->pmmp->submempool); // Cleanup sub-allocation system
            printf("DosSubUnsetMem: ulrc: %d\n", ulrc);

            ulrc = DosFreeMem(this->pmmp);               // Free the Master Pool
            printf("DosFreeMem: ulrc: %d\n", ulrc);

            this->pmmp = NULL;
            rc = ulrc;

        }
        /* Setting up the sub-allocation system failed */
        else {
            DosFreeMem(this->pmmp);          // Free the Master Pool
            printf("DosSubSetMem: ulrc: %d\n", ulrc);

            this->pmmp = NULL;
            rc = ulrc;
        }
    }
    /* Allocating the Master Pool failed */
    else {
        this->pmmp = NULL;
        rc = ulrc;
    }

    return rc;
}

int     CliDaemon::stop() {
    APIRET  ulrc    = -1;
    __mthd();

    ulrc = DosOpenEventSem(EVSEM_WAIT4STOP, &this->wait4stop);
    printf("DosOpenEventSem  : ulrc: %d\n", ulrc);
    ulrc = DosPostEventSem(this->wait4stop);
    printf("DosPostEventSem  : ulrc: %d\n", ulrc);
    ulrc = DosCloseEventSem(this->wait4stop);
    printf("DosCloseEventSem : ulrc: %d\n", ulrc);

    return 0;
}

//! -------------------------------------------------------------------- [main]

/*
// Main entrypoint
*/
int     main(int argc, char* argv[]) {
    int     rc  = 0;

    /* Create a Daemon Instance and give it control */
    do {
        Daemon*         daemon      = NULL;
        daemon = new CliDaemon(argc, argv);
        //~ daemon = new CliDaemon();
        if (daemon) {
            printf("-------------------------------------------------------------------------------\n");
            rc = daemon->doAction();
            printf("-------------------------------------------------------------------------------\n");
            delete daemon;
        }
    } while (0);
    return rc;

    /* Return to system */
    printf("usbshlpr.dmn: rc: %d\n", rc);
    return rc;
}
