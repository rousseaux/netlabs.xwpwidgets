/**********************************************************************
*   RXTCPMON.C                                                        *
*                                                                     *
*   This program extends the REXX language by providing some          *
*   REXX external functions relating to TCP.                          *
*   These functions are:                                              *
*       DosQuerySysInfo             --                                *
*       TCPQueryRecCount            --                                *
*       TCPQuerySndCount            --                                *
*       TCPQueryStats               --                                *
*       TCPQueryAvailableInterfaces --                                *
*       TCPQueryInterface           --                                *
*       TCPLoadFuncs                --                                *
*       TCPDropFuncs                --                                *
*                                                                     *
*   To compile:    nmake -f rxtcpmon.mak                              *
*                                                                     *
**********************************************************************/
/* Include files */

#define INCL_DOSPROCESS
#define INCL_DOSPROFILE
#define INCL_DOSMISC

#define INCL_DOSDEVIOCTL
#define INCL_DOSDEVICES

#define INCL_DOSERRORS
#define INCL_REXXSAA
#define INCL_RXFUNC

#define TCPIPCV4        // all magic is here :)

#include <os2.h>
#include <rexxsaa.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// TCP/IP stuff starts here
//
#include <sys/types.h>       // socket related
#include <sys/socket.h>
#include <sys/ioctl.h>   // ioctl related
#include <net/if.h>
#include <netinet/tcp.h>
//#include <netinet/tcp_var.h>
//

/*********************************************************************/
/* Numeric Return calls                                              */
/*********************************************************************/

#define RETURN_INVALID                   40
#define RETURN_OK                        0

/*********************************************************************/
/* RxFncTable                                                        */
/*   Array of names of the RXTCPMON functions.                       */
/*   This list is used for registration and deregistration.          */
/*********************************************************************/

static PSZ  RxFncTable[] =
   {
      "DosQuerySysInfo",
      "TCPQueryRecCount",
      "TCPQuerySndCount",
      "TCPQueryStats",
      "TCPQueryAvailableInterfaces",
      "TCPQueryInterface",
      "TCPLoadFuncs",
      "TCPDropFuncs"
   };

/*********************************************************************/
/*  Declare all exported functions as REXX functions.                */
/*********************************************************************/

RexxFunctionHandler RxTCPRecCount;
RexxFunctionHandler RxTCPSndCount;
RexxFunctionHandler RxLoadTCPMon;
RexxFunctionHandler RxDropTCPMon;
RexxFunctionHandler RxTCPSRCount;
RexxFunctionHandler RxTCPQueryAvailableInterfaces;
RexxFunctionHandler RxTCPQueryInterface;
RexxFunctionHandler RxDosQuerySysInfo;

/*********************************************************************/
/*  Various definitions used by various functions.                   */
/*********************************************************************/

#define  MAX_DIGITS     9          /* maximum digits in numeric arg  */


/********************************************************************
* Function:  string2long(string, number)                            *
*                                                                   *
* Purpose:   Validates and converts an ASCII-Z string from string   *
*            form to an signed long.  Returns FALSE if the number   *
*            is not valid, TRUE if the number was successfully      *
*            converted.                                             *
*                                                                   *
* RC:        TRUE - Good number converted                           *
*            FALSE - Invalid number supplied.                       *
*********************************************************************/

BOOL string2long(PSZ string, LONG *number)
{
  ULONG    accumulator;                /* converted number           */
  INT      length;                     /* length of number           */
  INT      sign;                       /* sign of number             */

  sign = 1;                            /* set default sign           */
  if (*string == '-') {                /* negative?                  */
    sign = -1;                         /* change sign                */
    string++;                          /* step past sign             */
  }

  length = strlen(string);             /* get length of string       */
  if (length == 0 ||                   /* if null string             */
      length > MAX_DIGITS)             /* or too long                */
    return FALSE;                      /* not valid                  */

  accumulator = 0;                     /* start with zero            */

  while (length) {                     /* while more digits          */
    if (!isdigit(*string))             /* not a digit?               */
      return FALSE;                    /* tell caller                */
                                       /* add to accumulator         */
    accumulator = accumulator *10 + (*string - '0');
    length--;                          /* reduce length              */
    string++;                          /* step pointer               */
  }

  *number = accumulator * sign;        /* return the value           */
  return TRUE;                         /* good number                */
}


/*************************************************************************
* Function:  RxTCPRecCount                                               *
*                                                                        *
* Syntax:    received = TCPQueryRecCount()                               *
*                                                                        *
* Return:    -1 if an error occured while querying TCP data, or the      *
*            number of bytes received since initialization.              *
*************************************************************************/

ULONG RxTCPRecCount(CHAR *name,
                    ULONG numargs,
                    RXSTRING args[],
                    CHAR *qjunk,
                    RXSTRING *rstring)
{
    int stat;
    struct tcpstat mystat;
    APIRET rc;
    
    if (numargs > 0)
        return RETURN_INVALID;

    stat = socket(PF_INET, SOCK_RAW, 0); // i need a SOCKET
    rc = ioctl(stat, SIOSTATTCP, (caddr_t) &mystat, sizeof(struct tcpstat));
    soclose(stat); // closing socket

    if (rc != NO_ERROR)
    {
        strcpy(rstring->strptr, "-1");
        rstring->strlength = 2;
    }
    else
    {
        sprintf(rstring->strptr, "%u", mystat.tcps_rcvbyte);
        rstring->strlength = strlen(rstring->strptr);
    }

    return RETURN_OK;
}


/*************************************************************************
* Function:  RxTCPSndCount                                               *
*                                                                        *
* Syntax:    interfaces = TCPQuerySndCount()                             *
*                                                                        *
* Return:    -1 if an error occured while querying TCP data, or the      *
*            number of bytes sent since initialization.                  *
*************************************************************************/

ULONG RxTCPSndCount(CHAR *name,
                    ULONG numargs,
                    RXSTRING args[],
                    CHAR *qjunk,
                    RXSTRING *rstring)
{
    int stat;
    struct tcpstat mystat;
    APIRET rc;
    
    if (numargs > 0)
        return RETURN_INVALID;

    stat = socket(PF_INET, SOCK_RAW, 0); // i need a SOCKET
    rc = ioctl(stat, SIOSTATTCP, (caddr_t) &mystat, sizeof(struct tcpstat));
    soclose(stat); // closing socket

    if (rc!=NO_ERROR)
    {
        strcpy(rstring->strptr, "-1");
        rstring->strlength = 2;
    }
    else
    {
        sprintf(rstring->strptr, "%u", mystat.tcps_sndbyte);
        rstring->strlength = strlen(rstring->strptr);
    }
    
    return RETURN_OK;
}


/*************************************************************************
* Function:  RxTCPSRCount                                                *
*                                                                        *
* Syntax:    parse value TCPQueryStats() with received send .            *
*                                                                        *
* Return:    -1 if an error occured while querying TCP stats, or a       *
*            space-separated list starting with the number of bytes      *
*            received and sent since initialization.  Other data         *
*            may follow.                                                 *
*************************************************************************/

ULONG RxTCPSRCount(CHAR *name,
                   ULONG numargs,
                   RXSTRING args[],
                   CHAR *queuename,
                   RXSTRING *retstr)
{
    int stat;
    struct tcpstat mystat;
    APIRET rc;
    
    if (numargs > 0)
        return RETURN_INVALID;

    stat = socket(PF_INET, SOCK_RAW, 0); // i need a SOCKET
    rc = ioctl(stat, SIOSTATTCP, (caddr_t) &mystat, sizeof(struct tcpstat));
    soclose(stat); // closing socket

    if (rc != NO_ERROR)
    {
        strcpy(retstr->strptr, "-1");
        retstr->strlength = 2;
    }
    else
    {
        // the default retstr buffer is big enough (256 bytes), so
        // no extra allocation is needed
        sprintf(retstr->strptr, "%u %u",
                                mystat.tcps_rcvbyte,
                                mystat.tcps_sndbyte);
        retstr->strlength = strlen(retstr->strptr);
    }

    return RETURN_OK;
}


/*************************************************************************
* Function:  RxTCPQueryAvailableInterfaces                               *
*                                                                        *
* Syntax:    interfaces = TCPQueryAvailableInterfaces()                  *
*                                                                        *
* Return:    -1 if an error occured while querying available interfaces, *
*            or a space-separated list of interface ID.                  *
*************************************************************************/

ULONG RxTCPQueryAvailableInterfaces(CHAR *name,
                                    ULONG numargs,
                                    RXSTRING args[],
                                    CHAR *queuename,
                                    RXSTRING *retstr)
{
    int stat;
    #pragma pack(1)
    struct interfaces 
    {
        USHORT usCount;
        struct buff 
        {
            ULONG  ulIPAddr;
            USHORT usInterfaceIndex;
            ULONG  ulNetMask;
            ULONG  ulBroadcaseAddr;
        } aBuff[IFMIB_ENTRIES];
    } interf;
    #pragma pack()
    APIRET rc;
    
    if (numargs > 0)
        return RETURN_INVALID;

    stat = socket(PF_INET, SOCK_RAW, 0); // i need a SOCKET
    rc = ioctl(stat, SIOSTATAT, (caddr_t) &interf, sizeof(interf));
    soclose(stat); // closing socket

    if (rc != NO_ERROR)
    {
        strcpy(retstr->strptr, "-1");
        retstr->strlength = 2;
    }
    else
    {
        int i;
        
        // the default retstr buffer is big enough (256 bytes), so
        // no extra allocation is needed

        retstr->strptr[0] = 0;
        for (i = 0; i < interf.usCount; i++)
        {
            CHAR ach[10];
            sprintf(ach, "%u ", interf.aBuff[i].usInterfaceIndex);
            strcat(retstr->strptr, ach);
        }
        retstr->strlength = strlen(retstr->strptr);
    }

    return RETURN_OK;
}


/*************************************************************************
* Function:  RxTCPQueryInterface                                         *
*                                                                        *
* Syntax:    info = TCPQueryInterface(interface)                         *
*                                                                        *
* Return:    a space-separated list of interface data.  Namely, an in    *
*            this order:                                                 *
*                                                                        *
*            - index (same as parameter if valid)                        *
*            - type                                                      *
*            - mtu size                                                  *
*            - physical address (12 hex digits)                          *
*            - OperStatus                                                *
*            - Speed                                                     *
*            - LastChange                                                *
*            - InOctets                                                  *
*            - OutOctets                                                 *
*            - OutDiscards                                               *
*            - InDiscards                                                *
*            - InErrors                                                  *
*            - OutErrors                                                 *
*            - InUnknownProtos                                           *
*            - InUcastPkts                                               *
*            - OutUcastPkts                                              *
*            - InNUcastPkts                                              *
*            - OutNUcastPkts                                             *
*            - description (up to the end of the returned string)        *
*                                                                        *
*            All elements are numbers, except for description, which     *
*            can be any string (including the null string).              *
*************************************************************************/

ULONG RxTCPQueryInterface(CHAR *name,
                          ULONG numargs,
                          RXSTRING args[],
                          CHAR *queuename,
                          RXSTRING *retstr)
{
    int stat;
    struct ifmib mystat;
    APIRET rc;
    LONG lInterface;

    if (numargs != 1)
      return RETURN_INVALID;
      
    if (   (RXVALIDSTRING(args[0]))
        && (string2long(args[0].strptr, &lInterface))
        && (lInterface >= 0)
        && (lInterface < IFMIB_ENTRIES)
       )
    {
        stat = socket(PF_INET, SOCK_RAW, 0); // i need a SOCKET
        rc = ioctl(stat, SIOSTATIF42, (caddr_t) &mystat, sizeof(mystat));
        soclose(stat); // closing socket

        if (rc != NO_ERROR)
        {
            strcpy(retstr->strptr, "-1");
            retstr->strlength = 2;
        }
        else
        {
            // the default retstr buffer is big enough (256 bytes), so
            // no extra allocation is needed
            sprintf(retstr->strptr, 
                    "%d %d %d %02X%02X%02X%02X%02X%02X %d %u %u %u %u %u %u %u %u %u %u %u %u %u %s",
                    mystat.iftable[lInterface].ifIndex,
                    mystat.iftable[lInterface].ifType,
                    mystat.iftable[lInterface].ifMtu,
                    mystat.iftable[lInterface].ifPhysAddr[0],
                    mystat.iftable[lInterface].ifPhysAddr[1],
                    mystat.iftable[lInterface].ifPhysAddr[2],
                    mystat.iftable[lInterface].ifPhysAddr[3],
                    mystat.iftable[lInterface].ifPhysAddr[4],
                    mystat.iftable[lInterface].ifPhysAddr[5],
                    mystat.iftable[lInterface].ifOperStatus,
                    mystat.iftable[lInterface].ifSpeed,
                    mystat.iftable[lInterface].ifLastChange,
                    mystat.iftable[lInterface].ifInOctets,
                    mystat.iftable[lInterface].ifOutOctets,
                    mystat.iftable[lInterface].ifOutDiscards,
                    mystat.iftable[lInterface].ifInDiscards,
                    mystat.iftable[lInterface].ifInErrors,
                    mystat.iftable[lInterface].ifOutErrors,
                    mystat.iftable[lInterface].ifInUnknownProtos,
                    mystat.iftable[lInterface].ifInUcastPkts,
                    mystat.iftable[lInterface].ifOutUcastPkts,
                    mystat.iftable[lInterface].ifInNUcastPkts,
                    mystat.iftable[lInterface].ifOutNUcastPkts,
                    mystat.iftable[lInterface].ifDescr);

            retstr->strlength = strlen(retstr->strptr);
        }

        return RETURN_OK;
    }
    else
        return RETURN_INVALID;
}


/*************************************************************************
* Function:  RxDosQuerySysInfo                                           *
*                                                                        *
* Syntax:    info = DosQuerySysInfo(sysvar)                              *
*                                                                        *
* Return:    the value of the system variable (a unsigned number)        *
*************************************************************************/

ULONG RxDosQuerySysInfo(CHAR *name,
                        ULONG numargs,
                        RXSTRING args[],
                        CHAR *queuename,
                        RXSTRING *retstr)
{
    APIRET rc;
    LONG lVar;

    if (numargs != 1)
      return RETURN_INVALID;
      
    if (   (RXVALIDSTRING(args[0]))
        && (string2long(args[0].strptr, &lVar))
        && (lVar >= 1)
        && (lVar < QSV_MAX)
       )
    {
        ULONG ulValue;
        
        rc = DosQuerySysInfo(lVar, lVar, &ulValue, sizeof(ULONG));

        if (rc != NO_ERROR)
        {
            return RETURN_INVALID;
        }
        else
        {
            // the default retstr buffer is big enough (256 bytes), so
            // no extra allocation is needed
            sprintf(retstr->strptr, 
                    "%d",
                    ulValue);

            retstr->strlength = strlen(retstr->strptr);
            
            return RETURN_OK;
        }
    }
    else
        return RETURN_INVALID;
}


/*************************************************************************
* Function:  RxLoadTCPMon                                                *
*                                                                        *
* Syntax:    call TcpLoadFuncs                                           *
*                                                                        *
* Return:    null string                                                 *
*************************************************************************/

ULONG RxLoadTCPMon(CHAR *name,
                   ULONG numargs,
                   RXSTRING args[],
                   CHAR *queuename,
                   RXSTRING *retstr)
{
    INT    entries;                      /* Num of entries             */
    INT    j;                            /* Counter                    */

                                         /* check arguments            */
    if (numargs > 0)
        return RETURN_INVALID;

    entries = sizeof(RxFncTable)/sizeof(PSZ);

    for (j = 0; j < entries; j++)
    {
        RexxRegisterFunctionDll(RxFncTable[j], "RXTCPMON", RxFncTable[j]);
    }
    retstr->strlength = 0;               /* set return value           */
    return RETURN_OK;
}


/*************************************************************************
* Function:  RxDropTCPMon                                                *
*                                                                        *
* Syntax:    call TcpDropFuncs                                           *
*                                                                        *
* Return:    NO_UTIL_ERROR - Successful.                                 *
*************************************************************************/

ULONG RxDropTCPMon(CHAR *name,
                   ULONG numargs,
                   RXSTRING args[],
                   CHAR *queuename,
                   RXSTRING *retstr)
{
    INT     entries;                     /* Num of entries             */
    INT     j;                           /* Counter                    */

    if (numargs != 0)                    /* no arguments for this      */
        return RETURN_INVALID;           /* raise an error             */


    entries = sizeof(RxFncTable)/sizeof(PSZ);

    for (j = 0; j < entries; j++)
    {
        RexxDeregisterFunction(RxFncTable[j]);
    }

    retstr->strlength = 0;               /* return a null string result*/
    return RETURN_OK;                    /* no error on call           */
}

