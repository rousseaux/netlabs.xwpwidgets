//////////////////////////////////////
/// (c) 1998-2000 Christian Wonschina
//////////////////////////////////////
#ifndef _SOCKET_H
#define _SOCKET_H

#include <netinet\in.h>
#include <sys\socket.h>
#include <sys\types.h>
#include <netdb.h>
#include <utils.h>
#include <stdarg.h>
#include <sys\ioctl.h>


int sockOpen(PSZ            pszHostname,
             unsigned short usPort);

void sockClose(int sok);




int sockWrite(int   sok,
              PSZ   pszBuf,
              ULONG ulLen);

int sockWriteln(int sok,
                PSZ pszBuf,
                ...);



int sockReadln(int   sok,
               PSZ   pszBuf,
               ULONG ulLen);

#endif
