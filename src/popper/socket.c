#include <stdio.h>
#include <stdlib.h>
#include <os2.h>
#include <string.h>
#include <netinet\in.h>
#include <sys\socket.h>
#include <sys\types.h>
#include <netdb.h>
#include <utils.h>
#include <stdarg.h>
#include <sys\ioctl.h>


// open a stream socket
int sockOpen(PSZ            pszHostname,
             unsigned short usPort)
{
 struct sockaddr_in sa;
 struct hostent     *hp;
 int    sok;


 if((hp=gethostbyname(pszHostname))==NULL)
   return(-1);

 bzero(&sa, 0);
 sa.sin_family      = AF_INET;
 sa.sin_port        = htons(usPort);
 sa.sin_addr.s_addr = *((unsigned long *)hp->h_addr);
 if((sok = socket(PF_INET, SOCK_STREAM, 0)) < 0)
   return(-2);

 if(connect(sok, (struct sockaddr *)&sa, sizeof(sa)) < 0)
  {
   soclose(sok);
   return(-3);
  }

 return(sok);
}

// close's a given socket
void sockClose(int sok)
{
 soclose(sok);
}



// write to the socket
// returns 0 if write was ok
int sockWrite(int   sok,
              PSZ   pszBuf,
              ULONG ulLen)
{
 int n;


 while(ulLen)
  {
   n = send(sok, pszBuf, ulLen, 0);
   if(n <= 0)
     return -1;
   ulLen  -= n;
   pszBuf += n;
  }

 return(0);
}

int sockWriteln(int sok,
                PSZ pszBuf,
                ...)
{
 va_list       arg_ptr;
 char          txt[1024];


 va_start(arg_ptr, pszBuf);
   vsprintf(txt, pszBuf, arg_ptr);
 va_end(arg_ptr);


 if(sockWrite(sok, txt, strlen(txt))<0)
   return(-1);

 if(sockWrite(sok, "\r\n", 2))
   return(-2);

 return(0);
}

// reads a line from our socket
int sockReadln(int   sok,
               PSZ   pszBuf,
               ULONG ulLen)
{
 memset(pszBuf, 0, ulLen);


 while(--ulLen)
  {
   if(recv(sok, pszBuf, 1, 0) != 1)
     return -1;
   if(*pszBuf == '\n')
     break;
   if(*pszBuf != '\r') /* remove all CRs */
     pszBuf++;
  }
 *pszBuf = 0;

 return(0);
}

