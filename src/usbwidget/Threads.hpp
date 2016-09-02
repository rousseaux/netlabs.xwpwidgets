/*****************************************************************************\
* Threads.hpp -- NeoWPS * USB Widget                                          *
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
 * This is the public header for it's corresponding source.
 */


#ifndef     __THREADS_HPP__
#define     __THREADS_HPP__
#ifdef      __cplusplus
    extern "C" {
#endif


#include    "ModuleGlobals.hpp"

/* Prototypes */
void    _Optlink WmQuitMonitorThread(void* p);
void    _Optlink DriveMonitorThread(void* p);
void    _Optlink LVMthread (void* p);
void    _Optlink PhantomSlayerThread(void* p);
void    _Optlink MonitorUSBDevices (void* p);
void    _Optlink EjectThread(void FAR  *pParam);




#define     FL_THREAD_USE_WMQUIT    0x00000001  // WM_QUIT can be posted
#define     FL_THREAD_USE_MSGQUEUE  0x00000002  // Create a Message queue
#define     FL_THREAD_USE_SEMAPHORE 0x00000004  // Create a Semaphore
#define     FL_THREAD_USE_SYSCT     0x00000008  // Use DosCreateThread
#define     FL_THREAD_USE_SYSSLEEP  0x00000010  // Use DosSleep


#ifndef     _OMIT_TEST_THREADS_

class   Thread {
    private:
    int     debug;
    int     tid;
    int     stopped;
    int     flags;
    int     semaphore;
    int     looping;
    int     hab;
    int     hmq;
    QMSG    qmsg;

    protected:

    public:
    Thread();
    virtual ~Thread();
    virtual int     fire();
    virtual int     start();
    virtual int     stop();
    virtual int     block();
    virtual int     unblock();
    virtual int     suspend();
    virtual int     resume();
    virtual int     wait(int timeout);
    virtual int     kill();

    virtual int     getDebug();
    virtual int     setDebug(int debug);
    virtual int     getTid();
    virtual int     setTid(int tid);
    virtual int     getStopped();
    virtual int     setStopped(int stopped);
    virtual int     getFlags();
    virtual int     setFlags(int flags);
    virtual int     getSemaphore();
    virtual int     setSemaphore(int semaphore);
    virtual int     getLooping();
    virtual int     setLooping(int looping);
    virtual int     getHab();
    virtual int     setHab(int hab);
    virtual int     getHmq();
    virtual int     setHmq(int hmq);

    virtual void    threadProc();
    // icc36 needs _Optlink
    static  void    _Optlink   threadOneshotProc(void* toi);
    static  void    _Optlink   threadLoopingProc(void* toi);
    // icc308 barks on _Optlink -- cannot be appied to void() ???
    //~ static  void    threadOneshotProc(void* toi);
    //~ static  void    threadLoopingProc(void* toi);

};


class   MonitorThread : public virtual Thread {
    public:
    virtual void    threadProc();
};

class   DrivesThread : public virtual Thread {
    public:
    virtual void    threadProc();
};


#endif




// -- stashed --
void    _Optlink ZipDriveMonitor(void* p);



void    StartThreads();
void    StopThreads();

#ifdef      __cplusplus
    }
#endif

#endif // __THREADS_HPP__
