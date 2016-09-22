/*****************************************************************************\
* usbshlpr.hpp -- Support DAEMON for the NeoWPS * USB Widget                  *
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


#ifndef     __USBSHLPR_HPP__
#define     __USBSHLPR_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif

#include    <conio.h>
#include    "Master.hpp"

/* Prototypes */
//~ int     startd();
//~ int     stopd();

/* Class Declarations */
class   Daemon;
class   CliDaemon;

// ----------------------------------------------------------------------------
// Daemon :: Kinda provides the interface with no real implementation
// ----------------------------------------------------------------------------
class   Daemon {

    public:

    /* Constructor(s) and Destructor */
    Daemon();
    virtual ~Daemon();

    /* Public Methods */
    virtual int     doAction();     // Determines the action to take
    virtual int     start();        // Start the Daemon
    virtual int     stop();         // Stop the Daemon

    /* Public Attributes */
    int             argc;           // Argument count passed from main()
    char**          argv;           // Array of argument pointers passed from main()
    HEV             wait4daemon;    // UsbWidget blocks on this until Daemon ready
    HEV             wait4stop;      // Daemon blocks on this (/stop posts it)

    protected:
    virtual void    clear();        // Clear instance attributes

    private:
};

// ----------------------------------------------------------------------------
// CliDaemon :: Provides the implementation for the Daemon
// ----------------------------------------------------------------------------
// This class is used when the daemon is started from the command-line,
// wether it is detached or not.
// ----------------------------------------------------------------------------
class   CliDaemon : public Daemon {
    public:
    CliDaemon(int argc, char* argv[]);
    virtual ~CliDaemon();
    virtual int     start();        // Override default implementation
    virtual int     stop();         // Override default implemenation
    PMASTERMEMPOOL  pmmp;           // Pointer to shared memory
    protected:
    virtual void    clear();        // Clear instance attributes
    private:
    char    buf[512];               // Work buffer
};


#ifdef      __cplusplus
    }
#endif

#endif // __USBSHLPR_HPP__
