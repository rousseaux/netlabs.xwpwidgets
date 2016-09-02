/***********************************************************************\
 *                                APM/2                                *
 *  Copyright (C) by Stangl Roman (rstangl@vnet.ibm.com), 1997, 1998   *
 * This Code may be freely distributed, provided the Copyright isn't   *
 * removed, under the conditions indicated in the documentation.       *
 *                                                                     *
\***********************************************************************/

#ifndef __APM_H__
#define __APM_H__

                                        /* APM IOCTL */
#ifndef IOCTL_POWER
#define IOCTL_POWER                         0x000C
#endif  /* IOCTL_POWER */

#pragma pack(1)
/*---------------------------------------------------------------------*\
 * IOCTL_POWER: Data packet                                            *
\*---------------------------------------------------------------------*/
                                        /* Data packet for all APM device I/Os */
typedef struct  _POWERRETURNCODE
{
USHORT  usReturnCode;
} POWERRETURNCODE, *PPOWERRETURNCODE;

                                        /* Return codes */
#define POWER_NOERROR                       0x0000
#define POWER_BADSUBID                      0x0001
#define POWER_BADRESERVED                   0x0002
#define POWER_BADDEVID                      0x0003
#define POWER_BADPWRSTATE                   0x0004
#define POWER_SELALREADYSETUP               0x0005
#define POWER_BADFLAGS                      0x0006
#define POWER_BADSEMHANDLE                  0x0007
#define POWER_BADLENGTH                     0x0008
#define POWER_DISABLED                      0x0009
#define POWER_NOEVENTQUEUE                  0x000A
#define POWER_TOOMANYQUEUES                 0x000B
#define POWER_BIOSERROR                     0x000C
#define POWER_BADSEMAPHORE                  0x000D
#define POWER_QUEUEOVERFLOW                 0x000E
#define POWER_STATECHANGEREJECT             0x000F
#define POWER_NOTSUPPORTED                  0x0010
#define POWER_DISENGAGED                    0x0011

                                        /* Some APM data structures not defined in Warp Toolkit yet.
                                           For more information please look into the Physical
                                           Device Driver Reference available from the DDK (device
                                           driver kit) homepage for free! */

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: SENDPOWEREVENT parameter packet                        *
\*---------------------------------------------------------------------*/
#ifndef POWER_SENDPOWEREVENT
#define POWER_SENDPOWEREVENT                0x0040
#endif  /* POWER_SENDPOWEREVENT */

#ifndef SENDPOWEREVENT
                                        /* Parameter packet for POWER_SENDPOWEREVENT */
struct  SENDPOWEREVENT
{
    USHORT  usSubID;
    USHORT  usReserved;                 /* Always 0 */
    USHORT  usData1;
    USHORT  usData2;
};

                                        /* 0:0 (usData1:usData2) */
#define SUBID_ENABLE_POWER_MANAGEMENT       0x0003
#define SUBID_DISABLE_POWER_MANAGEMENT      0x0004
#define SUBID_RESTORE_BIOS_DEFAULTS         0x0005
                                        /* DevId:PwrState (usData1:usData2) */
#define SUBID_SET_POWER_STATE               0x0006
                                        /* 0:0 (usData1:usData2) */
#define SUBID_BATTERY_LOW_EVENT             0x0007
#define SUBID_NORMAL_RESUME_EVENT           0x0008
#define SUBID_CRITICAL_RESUME_EVENT         0x0009
#define SUBID_STANDBY_RESUME_EVENT          0x000A
                                        /* DevId (usData1) */
#define SUBID_ENGAGE_POWER_MANAGEMENT       0x000B
#define SUBID_DISENGAGE_POWER_MANAGEMENT    0x000C
                                        /* 0:0 (usData1:usData2) */
#define SUBID_POWER_STATUS_CHANGE_EVENT     0x000D
#define SUBID_UPDATE_TIME_EVENT             0x000E
                                        /* EvtCode (usData1) */
#define SUBID_OEM_DEFINED_APM_EVENT         0x000F
                                        /* DevId:PwrStata (usData1:usData2) */
#define SUBID_QUERY_POWER_STATE             0x0010

                                        /* DevID, for 0xabb bb is the unit, and
                                           0xFF is all units */
#define DEVID_APMBIOS                       0x0000
#define DEVID_ALL_DEVICES                   0x0001
#define DEVID_DISPLAY                       0x0100
#define DEVID_SECONDARY_STORAGE             0x0200
#define DEVID_PARALLEL_PORTS                0x0300
#define DEVID_SERIAL_PORTS                  0x0400
#define DEVID_NETWORK_ADAPTERS              0x0500
#define DEVID_PCMCIA_SOCKETS                0x0600
#define DEVID_BATTERIES                     0x8000

                                        /* PwrState */
#define POWERSTATE_READY                    0x0000
#define POWERSTATE_STANDBY                  0x0001
#define POWERSTATE_SUSPEND                  0x0002
#define POWERSTATE_OFF                      0x0003
                                        /* OEM defined 0x0020 - 0x007F) */
#define POWERSTATE_OEMDEFINED               0x0020
#endif  /* SENDPOWEREVENT */

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: SETPOWEREVENTRES parameter packet                      *
\*---------------------------------------------------------------------*/
#ifndef POWER_SETPOWEREVENTRES
#define POWER_SETPOWEREVENTRES              0x0041
#endif  /* POWER_SETPOWEREVENTRES */


#ifndef SETPOWEREVENTRES
                                        /* Parameter packet for POWER_SETPOWEREVENTRES */
struct  SETPOWEREVENTRES
{
    USHORT  usRequestFlags;
    ULONG   ulSemaphoreHandle;          /* !6-bit system semaphore or 32-bit shared event semaphore */
    ULONG   ulEventMask;
};

#define REQUESTFLAG_SEMAPHORE_SETUP         0x0001
#define REQUESTFLAG_SEMAPHORE_REMOVAL       0x0002
#define REQUESTFLAG_16BIT_SEMAPHORE         0x0004
#define REQUESTFLAG_32BIT_SEMAPHORE         0x0008
#define REQUESTFLAG_NO_SEMAPHORE            0x0010
                                        /* All other bits must be 0 */
#define REQUESTFLAG_RESERVED_BITS           0xFFE0

#define EVENTMASK_ENABLE_POWER_MANAGEMENT   0x0008
#define EVENTMASK_DISABLE_POWER_MANAGEMENT  0x0010
#define EVENTMASK_RESTORE_BIOS_DEFAULTS     0x0020
#define EVENTMASK_SET_POWER_STATE           0x0040
#define EVENTMASK_BATTERY_LOW_EVENT         0x0080
#define EVENTMASK_NORMAL_RESUME_EVENT       0x0100
#define EVENTMASK_CRITICAL_RESUME_EVENT     0x0200
                                        /* All other bits must be 0 */
#define EVENTMASK_RESERVED_BITS             0xFD00
#endif  /* POWEREVENTRES */

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: POWEROEMFUNCTION parameter packet                      *
\*---------------------------------------------------------------------*/
#ifndef POWER_OEMFUNCTION
#define POWER_OEMFUNCTION                   0x0045
#endif  /* POWER_OEMFUNCTION */

#ifndef POWEROEMFUNCTION
                                        /* Parameter packet for POWER_OEMFUNCTION. The Physical Device
                                           Driver Reference states "The low order word of EAX is set to the
                                           value 5380H, regardless of the parameter field value." However
                                           with the APM 1.2 driver released on the device driver online
                                           site this is simply not true, one can move e.g. 5307H into EAX
                                           to power off by APM and it will work! I'm not sure if the
                                           documentation if incorrect or a bug in APM.SYS, as this DevIOCtl
                                           interface is based on the Ring-0 IDC interface where no limitation
                                           is documented in the Input/Output Device Driver Reference I think
                                           the documentation is incorrect. */
struct  POWEROEMFUNCTION
{
    USHORT  usParmLength;
    USHORT  usFlags;                    /* Contains CPU flags upon APM BIOS return (using LAHF
                                           instruction):
                                           Bit 7 ... Sign flag
                                           Bit 6 ... Zero flag
                                           Bit 5
                                           Bit 4 ... Auxiliary Carry flag
                                           Bit 3
                                           Bit 2 ... Parity flag
                                           Bit 1
                                           Bit 0 ... Carry flag */
    ULONG   ulEAX;                      /* AX is set to 0x5380 before calling APM BIOS, but
                                           for some unknown reasons we! must set 0x5380 also */
    ULONG   ulEBX;
    ULONG   ulECX;
    ULONG   ulEDX;
    ULONG   ulESI;
    ULONG   ulEDI;
    USHORT  usDS;
    USHORT  usES;
    USHORT  usFS;
    USHORT  usGS;
};

                                        /* Access APM BIOS OEM functions */
#define POWEROEMFUNCTION_EAX_OEMFUNCTION    0x5380

                                        /* The selector registers (DS, ..., GS) may or may not be
                                           included in that call */
#define POWEROEMFUNCTION_PARMLENTH_NOSELREG 0x001C
#define POWEROEMFUNCTION_PARMLENTH_SELREG   0x0024
#endif  /* POWEROEMFUNCTION */

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: GETPOWERSTATUS parameter packet                        *
\*---------------------------------------------------------------------*/
#ifndef POWER_GETPOWERSTATUS
#define POWER_GETPOWERSTATUS                0x0060
#endif  /* POWER_GETPOWERSTATUS */

#ifndef GETPOWERSTATUS
                                        /* Parameter packet for POWER_GETPOWERSTATUS */
typedef struct  _GETPOWERSTATUS
{
    USHORT  usParmLength;               /* Depends on APM 1.0 or 1.1+ */
    USHORT  usPowerFlags;
    UCHAR   ucACStatus;
    UCHAR   ucBatteryStatus;
    UCHAR   ucBatteryLife;              /* 0...100 or unknown */
    UCHAR   ucBatteryTimeForm;
    USHORT  usBatteryTime;
    UCHAR   ucBatteryFlags;
} GETPOWERSTATUS, *PGETPOWERSTATUS;

#define GETPOWERSTATUS_PARMLENGTH_APM10     0x0007
#define GETPOWERSTATUS_PARMLENGTH_APM11     0x000B

#define FLAGS_POWER_MANAGEMENT_ENABLED      0x0001
#define FLAGS_POWER_MANAGEMENT_ENGAGED      0x0002
#define FLAGS_POWER_MANAGEMENT_CONNECTED    0x0004
                                        /* All other bits must be 0 */
#define FLAGS_RESERVED_BITS                 0xFFF8

#define ACSTATUS_ACOFFLINE                  0x0000
#define ACSTATUS_ACONLINE                   0x0001
#define ACSTATUS_ON_BACKUP_POWER            0x0002
#define ACSTATUS_UNKNOWN                    0x00FF

#define DCSTATUS_HIGH                       0x0000
#define DCSTATUS_LOW                        0x0001
#define DCSTATUS_CRITICAL                   0x0002
#define DCSTATUS_CHARGING                   0x0003
#define DCSTATUS_UNKNOWN                    0x00FF

#define LIFE_UNKNOWN                        0x00FF

#define TIMEFORM_SECONDS                    0x0000
#define TIMEFORM_MINUTES                    0x0001
#define TIMEFORM_UNKNOWN                    0x00FF

#define BATTERYFLAGS_HIGH                   0x0001
#define BATTERYFLAGS_LOW                    0x0002
#define BATTERYFLAGS_CRITICAL               0x0004
#define BATTERYFLAGS_CHARGING               0x0008
#define BATTERYFLAGS_NO_SYSTEM_BATTERY      0x0080
#define BATTERYFLAGS_UNKNOWN                0x00FF
#endif  /* GETPOWERSTATUS */

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: GETPOWEREVENT parameter packet                         *
\*---------------------------------------------------------------------*/
#ifndef POWER_GETPOWEREVENT
#define POWER_GETPOWEREVENT                 0x0061
#endif  /* POWER_GETPOWEREVENT */

#ifndef GETPOWEREVENT
                                        /* Parameter packet for POWER_GETPOWEREVENT */
struct  GETPOWEREVENT
{
    USHORT  usParmLength;
    USHORT  usMessageCount;             /* Number of remaining queue elements */
    ULONG   ulParm1;                    /* Defined by Power Management event message */
    ULONG   ulParm2;                    /* Defined by Power Management event message */
};
#endif  /* GETPOWEREVENT */

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: GETPOWERINFO parameter packet                          *
\*---------------------------------------------------------------------*/
#ifndef POWER_GETPOWERINFO
#define POWER_GETPOWERINFO                  0x0062
#endif  /* POWER_GETPOWERINFO */

#ifndef GETPOWERINFO
                                        /* Parameter packet for POWER_GETPOWERINFO */
struct  GETPOWERINFO
{
    USHORT  usParmLength;               /* Depends on compatible or new calling convention */
    USHORT  usBIOSFlags;                /* Defined in APM BIOS technical reference */
    USHORT  usBIOSVersion;              /* MSB:LSB (major:minor APM BIOS revision) */
    USHORT  usSubsysVersion;            /* MSB:LSB (major:minor OS/2 APM subsystem revision) */
    USHORT  usAPMVersion;               /* MSB:LSB (major:minor current APM support level)
                                           This value is the lowest value of either the
                                           APM BIOS revision, OS/2 APM subsystem revision and
                                           version set to APM.SYS by /V commandline parameter */
};

                                        /* For use with APM.SYS version 1.0, 1.1, 1.2 */
#define GETPOWERINFO_PARMLENGTH_COMPATIBLE 0x0008
                                        /* For use with APM.SYS version 1.2 */
#define GETPOWERINFO_PARMLENGTH_NEW        0x000A
#endif  /* GETPOWERINFO */

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: GETPOWERSTATE parameter packet                         *
\*---------------------------------------------------------------------*/
#ifndef POWER_GETPOWERSTATE
#define POWER_GETPOWERSTATE                 0x0063
#endif  /* POWER_GETPOWERSTATE */

#ifndef GETPOWERSTATE
                                        /* Parameter packet for POWER_GETPOWERSTATE */
struct  GETPOWERSTATE
{
    USHORT  usParmLength;
    USHORT  usDeviceID;
    USHORT  usPowerState;
};
#endif  /* GETPOWERSTATE */

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: Input/Output Device Driver Reference excerpt           *
\*---------------------------------------------------------------------*/
/***********************************************************************\
 Some information from the Input/Output Device Driver Reference about event messages.
 Event SubIds 000Ah through 0010h are only available on systems running APM version
 1.1 or higher. They are not available under APM version 1.0.  An attempt to send an
 event which is not supported under the current APM version will return a
 PowerNotSupported error.

 ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
 ³OS/2 Power Management      ³                 ³                 ³
 ³Event Messages             ³     ulParm1     ³    ulParm2      ³
 ³                           ³ SubId  ³Reserved³ (msg-specific)  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄ´
 ³Enable Pwr Mgt Functions   ³ 0003h  ³ 0000h  ³ DevId  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Disable Pwr Mgt Functions  ³ 0004h  ³ 0000h  ³ DevId  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Restore BIOS Defaults      ³ 0005h  ³ 0000h  ³  - - - ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Set Power State            ³ 0006h  ³ 0000h  ³ DevId  ³PwrState³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Battery Low Event          ³ 0007h  ³ 0000h  ³ - - -  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Normal Resume Event        ³ 0008h  ³ 0000h  ³ - - -  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Critical Resume Event      ³ 0009h  ³ 0000h  ³ - - -  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Standby Resume Event       ³ 000Ah  ³ 0000h  ³ - - -  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Engage Pwr Mgt Functions   ³ 000Bh  ³ 0000h  ³ DevId  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Disengage Pwr Mgt Functions³ 000Ch  ³ 0000h  ³ DevId  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Power Status Change Event  ³ 000Dh  ³ 0000h  ³ - - -  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Update Time Event          ³ 000Eh  ³ 0000h  ³ - - -  ³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³OEM Defined APM Event      ³ 000Fh  ³ 0000h  ³ EvtCode³ - - -  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄ´
 ³Query Power State          ³ 0010h  ³ 0000h  ³ DevId  ³PwrState³ (Ring-0 only)
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄ´
 ³ Note:  All other values are reserved.                         ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

 The device types (DevId) supported by the Advanced Power Management Specification are:

      System BIOS (0000h)
       Device ID for Advanced Power Management BIOS

      System BIOS-managed devices (0001h)
       Generic device ID for all devices managed by Advanced Power Management BIOS.

      Display devices (01xxh)
       Generic device ID for display devices

      Secondary storage devices (02xxh)
       Generic device ID for secondary storage devices

      Parallel ports (03xxh)
       Generic device ID for parallel ports

      Serial ports (04xxh)
       Generic device ID for serial ports

      Network adapters (05xxh)
       Generic device ID for network adapters

      PCMCIA sockets (06xxh)
       Generic device ID for PCMCIA sockets

      Reserved (0700h - 7FFF)

      Batteries (80xx)

      Reserved (8100h - DFFF)

      OEM defined power device IDs (E000h - EFFFh)
       Range of IDs to be used by OEM devices

      Reserved (F000 - FFFF)

  Note:

    1. XX indicates the physical device number (0-based)

    2. FF indicates all devices in that class

    3. All other device-type values are reserved

    4. Devices powered with external system unit power supplies are not supported

    5. Device IDs 05xxh and higher are only supported by version 1.1 or higher of APM
\***********************************************************************/

/*---------------------------------------------------------------------*\
 * IOCTL_POWER: Ralf Brown's Interrupt List excerpt                    *
\*---------------------------------------------------------------------*/
/***********************************************************************\
 The following is an excerpt from Ralf Brown's Interrupt List, included
 to allow calling the POWER_OEMFUNCTION DevIOCtl

 Copyright (c) 1989,1990,1991,1992,1993,1994,1995,1996,1997 Ralf Brown


 --------p-155300-----------------------------
 INT 15 - Advanced Power Management v1.0+ - INSTALLATION CHECK
        AX = 5300h
        BX = device ID of system BIOS (0000h)
 Return: CF clear if successful
            AH = major version (BCD)
            AL = minor version (BCD)
            BX = 504Dh ("PM")
            CX = flags (see #0398)
        CF set on error
            AH = error code (06h,09h,86h) (see #0399)
 BUG:   early versions of the Award Modular BIOS with built-in APM support
          reportedly do not set BX on return

 Bitfields for APM flags:
 Bit(s) Description     (Table 0398)
  0     16-bit protected mode interface supported
  1     32-bit protected mode interface supported
  2     CPU idle call reduces processor speed
  3     BIOS power management disabled
  4     BIOS power management disengaged (APM v1.1)
  5-7   reserved

 (Table 0399)
 Values for APM error code:
  01h   power management functionality disabled
  02h   interface connection already in effect
  03h   interface not connected
  04h   real-mode interface not connected
  05h   16-bit protected-mode interface already connected
  06h   16-bit protected-mode interface not supported
  07h   32-bit protected-mode interface already connected
  08h   32-bit protected-mode interface not supported
  09h   unrecognized device ID
  0Ah   invalid parameter value in CX
  0Bh   (APM v1.1) interface not engaged
  0Ch   (APM v1.2) function not supported
  0Dh   (APM v1.2) Resume Timer disabled
  0Eh-1Fh reserved for other interface and general errors
  20h-3Fh reserved for CPU errors
  40h-5Fh reserved for device errors
  60h   can't enter requested state
  61h-7Fh reserved for other system errors
  80h   no power management events pending
  81h-85h reserved for other power management event errors
  86h   APM not present
  87h-9Fh reserved for other power management event errors
  A0h-FEh reserved
  FFh   undefined
 --------p-155301-----------------------------
 INT 15 - Advanced Power Management v1.0+ - CONNECT REAL-MODE INTERFACE
        AX = 5301h
        BX = device ID of system BIOS (0000h)
 Return: CF clear if successful
        CF set on error
            AH = error code (02h,05h,07h,09h) (see #0399)
 Note:  on connection, an APM v1.1 or v1.2 BIOS switches to APM v1.0
          compatibility mode until it is informed that the user supports a
          newer version of APM (see AX=530Eh)
 SeeAlso: AX=5302h,AX=5303h,AX=5304h
 --------p-155302-----------------------------
 INT 15 R - Advanced Power Management v1.0+ - CONNECT 16-BIT PROTMODE INTERFACE
        AX = 5302h
        BX = device ID of system BIOS (0000h)
 Return: CF clear if successful
            AX = real-mode segment base address of protected-mode 16-bit code
                segment
            BX = offset of entry point
            CX = real-mode segment base address of protected-mode 16-bit data
                segment
            ---APM v1.1---
            SI = APM BIOS code segment length
            DI = APM BIOS data segment length
        CF set on error
            AH = error code (02h,05h,06h,07h,09h) (see #0399)
 Notes: the caller must initialize two consecutive descriptors with the
          returned segment base addresses; these descriptors must be valid
          whenever the protected-mode interface is called, and will have
          their limits arbitrarily set to 64K.
        the protected mode interface is invoked by making a far call with the
          same register values as for INT 15; it must be invoked while CPL=0,
          the code segment descriptor must have a DPL of 0, the stack must be
          in a 16-bit segment and have enough room for BIOS use and possible
          interrupts, and the current I/O permission bit map must allow access
          to the I/O ports used for power management.
        functions 00h-03h are not available from protected mode
        on connection, an APM v1.1 or v1.2 BIOS switches to APM v1.0
          compatibility mode until it is informed that the user supports a
          newer version of APM (see AX=530Eh)
 SeeAlso: AX=5301h,AX=5303h,AX=5304h
 --------p-155303-----------------------------
 INT 15 - Advanced Power Management v1.0+ - CONNECT 32-BIT PROTMODE INTERFACE
        AX = 5303h
        BX = device ID of system BIOS (0000h)
 Return: CF clear if successful
            AX = real-mode segment base address of protected-mode 32-bit code
                segment
            EBX = offset of entry point
            CX = real-mode segment base address of protected-mode 16-bit code
                segment
            DX = real-mode segment base address of protected-mode 16-bit data
                segment
            ---APM v1.1---
            SI = APM BIOS code segment length
            DI = APM BIOS data segment length
        CF set on error
            AH = error code (02h,05h,07h,08h,09h) (see #0399)
 Notes: the caller must initialize three consecutive descriptors with the
          returned segment base addresses for 32-bit code, 16-bit code, and
          16-bit data, respectively; these descriptors must be valid whenever
          the protected-mode interface is called, and will have their limits
          arbitrarily set to 64K.
        the protected mode interface is invoked by making a far call to the
          32-bit code segment with the same register values as for INT 15; it
          must be invoked while CPL=0, the code segment descriptor must have a
          DPL of 0, the stack must be in a 32-bit segment and have enough room
          for BIOS use and possible interrupts, and the current I/O permission
          bit map must allow access to the I/O ports used for power management.
        functions 00h-03h are not available from protected mode
        on connection, an APM v1.1 or v1.2 BIOS switches to APM v1.0
          compatibility mode until it is informed that the user supports a
          newer version of APM (see AX=530Eh)
 SeeAlso: AX=5301h,AX=5302h,AX=5304h
 --------p-155304-----------------------------
 INT 15 - Advanced Power Management v1.0+ - DISCONNECT INTERFACE
        AX = 5304h
        BX = device ID of system BIOS (0000h)
 Return: CF clear if successful
        CF set on error
            AH = error code (03h,09h) (see #0399)
 SeeAlso: AX=5301h,AX=5302h,AX=5303h
 --------p-155305-----------------------------
 INT 15 - Advanced Power Management v1.0+ - CPU IDLE
        AX = 5305h
 Return: CF clear if successful (after system leaves idle state)
        CF set on error
            AH = error code (03h,0Bh) (see #0399)
 Notes: call when the system is idle and should be suspended until the next
          system event or interrupt
        should not be called from within a hardware interrupt handler to avoid
          reentrance problems
        if an interrupt causes the system to resume normal processing, the
          interrupt may or may not have been handled when the BIOS returns
          from this call; thus, the caller should allow interrupts on return
        interrupt handlers may not retain control if the BIOS allows
          interrupts while in idle mode even if they are able to determine
          that they were called from idle mode
        the caller should issue this call continuously in a loop until it needs
          to perform some processing of its own
 SeeAlso: AX=1000h,AX=5306h,INT 2F/AX=1680h
 --------p-155306-----------------------------
 INT 15 - Advanced Power Management v1.0+ - CPU BUSY
        AX = 5306h
 Return: CF clear if successful
        CF set on error
            AH = error code (03h,0Bh) (see #0399)
 Notes: called to ensure that the system runs at full speed even on systems
          where the BIOS is unable to recognize increased activity (especially
          if interrupts are hooked by other programs and not chained to the
          BIOS)
        this call may be made even when the system is already running at full
          speed, but it will create unnecessary overhead
        should not be called from within a hardware interrupt handler to avoid
          reentrance problems
 SeeAlso: AX=5305h
 --------p-155307-----------------------------
 INT 15 - Advanced Power Management v1.0+ - SET POWER STATE
        AX = 5307h
        BX = device ID (see #0400)
        CX = system state ID (see #0401)
 Return: CF clear if successful
        CF set on error
            AH = error code (01h,03h,09h,0Ah,0Bh,60h) (see #0399)
 Note:  should not be called from within a hardware interrupt handler to avoid
          reentrance problems
 SeeAlso: AX=530Ch

 (Table 0400)
 Values for APM device IDs:
  0000h system BIOS
  0001h all devices for which the system BIOS manages power
  01xxh display (01FFh for all attached display devices)
  02xxh secondary storage (02FFh for all attached secondary storage devices)
  03xxh parallel ports (03FFh for all attached parallel ports)
  04xxh serial ports (04FFh for all attached serial ports)
 ---APM v1.1+ ---
  05xxh network adapters (05FFh for all attached network adapters)
  06xxh PCMCIA sockets (06FFh for all)
  0700h-7FFFh reserved
  80xxh system battery devices (APM v1.2)
  8100h-DFFFh reserved
  Exxxh OEM-defined power device IDs
  F000h-FFFFh reserved

 (Table 0401)
 Values for system state ID:
  0000h ready (not supported for device ID 0001h)
  0001h stand-by
  0002h suspend
  0003h off (not supported for device ID 0001h)
 ---APM v1.1---
  0004h last request processing notification (only for device ID 0001h)
  0005h last request rejected (only for device ID 0001h)
  0006h-001Fh reserved system states
  0020h-003Fh OEM-defined system states
  0040h-007Fh OEM-defined device states
  0080h-FFFFh reserved device states
 --------p-155307CX0001-----------------------
 INT 15 - Advanced Power Management v1.0+ - SYSTEM STAND-BY
        AX = 5307h
        CX = 0001h
        BX = 0001h (device ID for all power-managed devices)
 Return: CF clear
 Notes: puts the entire system into stand-by mode; normally called in response
          to a System Stand-by Request notification after any necessary
          processing, but may also be invoked at the caller's discretion
        should not be called from within a hardware interrupt handler to avoid
          reentrance problems
        the stand-by state is typically exited on an interrupt
 SeeAlso: AX=4280h,AX=5307h/CX=0002h"SUSPEND",AX=530Bh
 --------p-155307CX0002-----------------------
 INT 15 - Advanced Power Management v1.0+ - SUSPEND SYSTEM
        AX = 5307h
        CX = 0002h
        BX = 0001h (device ID for all power-managed devices)
 Return: after system is resumed
        CF clear
 Notes: puts the entire system into a low-power suspended state; normally
          called in response to a Suspend System Request notification after
          any necessary processing, but may also be invoked at the caller's
          discretion
        should not be called from within a hardware interrupt handler to avoid
          reentrance problems
        the caller may need to update its date and time values because the
          system could have been suspended for a long period of time
 SeeAlso: AX=5307h/CX=0001h"STAND-BY",AX=530Bh
 --------p-155308-----------------------------
 INT 15 - Advanced Power Management v1.0+ - ENABLE/DISABLE POWER MANAGEMENT
        AX = 5308h
        BX = device ID for all devices power-managed by APM
            0001h (APM v1.1+)
            FFFFh (APM v1.0)
        CX = new state
            0000h disabled
            0001h enabled
 Return: CF clear if successful
        CF set on error
            AH = error code (01h,03h,09h,0Ah,0Bh) (see #0399)
 Notes: when power management is disabled, the system BIOS will not
          automatically power down devices, enter stand-by or suspended mode,
          or perform any power-saving actions in response to AX=5305h calls
        should not be called from within a hardware interrupt handler to avoid
          reentrance problems
        the APM BIOS should never be both disabled and disengaged at the same
          time
 SeeAlso: AX=5309h,AX=530Dh,AX=530Fh
 --------p-155309-----------------------------
 INT 15 - Advanced Power Management v1.0+ - RESTORE POWER-ON DEFAULTS
        AX = 5309h
        BX = device ID for all devices power-managed by APM
            0001h (APM v1.1)
            FFFFh (APM v1.0)
 Return: CF clear if successful
        CF set on error
            AH = error code (03h,09h,0Bh) (see #0399)
 Note:  should not be called from within a hardware interrupt handler to avoid
          reentrance problems
 SeeAlso: AX=5308h
 --------p-15530A-----------------------------
 INT 15 - Advanced Power Management v1.0+ - GET POWER STATUS
        AX = 530Ah
        BX = device ID
            0001h all devices power-managed by APM
            80xxh specific battery unit number XXh (01h-FFh) (APM v1.2)
 Return: CF clear if successful
            BH = AC line status
                00h off-line
                01h on-line
                02h on backup power (APM v1.1)
                FFh unknown
                other reserved
            BL = battery status (see #0402)
            CH = battery flag (APM v1.1+) (see #0403)
            CL = remaining battery life, percentage
                00h-64h (0-100) percentage of full charge
                FFh unknown
            DX = remaining battery life, time (APM v1.1) (see #0404)
            ---if specific battery unit specified---
            SI = number of battery units currently installed
        CF set on error
            AH = error code (09h,0Ah) (see #0399)
 Notes: should not be called from within a hardware interrupt handler to avoid
          reentrance problems
        supported in real mode (INT 15) and both 16-bit and 32-bit protected
          mode

 (Table 0402)
 Values for APM v1.0+ battery status:
  00h   high
  01h   low
  02h   critical
  03h   charging
  FFh   unknown
  other reserved
 SeeAlso: #0403,#0404

 Bitfields for APM v1.1+ battery flag:
 Bit(s) Description     (Table 0403)
  0     high
  1     low
  2     critical
  3     charging
  4     selected battery not present (APM v1.2)
  5-6   reserved (0)
  7     no system battery
 Note:  all bits set (FFh) if unknown
 SeeAlso: #0402,#0404

 Bitfields for APM v1.1+ remaining battery life:
 Bit(s) Description     (Table 0404)
  15    time units: 0=seconds, 1=minutes
  14-0  battery life in minutes or seconds
 Note:  all bits set (FFFFh) if unknown
 SeeAlso: #0402,#0403
 --------p-15530B-----------------------------
 INT 15 - Advanced Power Management v1.0+ - GET POWER MANAGEMENT EVENT
        AX = 530Bh
 Return: CF clear if successful
            BX = event code (see #0405)
            CX = event information (APM v1.2) if BX=0003h or BX=0004h
                bit 0: PCMCIA socket was powered down in suspend state
        CF set on error
            AH = error code (03h,0Bh,80h) (see #0399)
 Notes: although power management events are often asynchronous, notification
          will not be made until polled via this call to permit software to
          only receive event notification when it is prepared to process
          power management events; since these events are not very time-
          critical, it should be sufficient to poll once or twice per second
        the critical resume notification is made after the system resumes
          from an emergency suspension; normally, the system BIOS only notifies
          its partner that it wishes to suspend and relies on the partner to
          actually request the suspension, but no notification is made on an
          emergency suspension
        should not be called from within a hardware interrupt handler to avoid
          reentrance problems
 SeeAlso: AX=5307h,AX=5307h/CX=0001h"STAND-BY",AX=5307h/CX=0002h"SUSPEND"

 (Table 0405)
 Values for APM event code:
  0001h system stand-by request
  0002h system suspend request
  0003h normal resume system notification
  0004h critical resume system notification
  0005h battery low notification
 ---APM v1.1---
  0006h power status change notification
  0007h update time notification
  0008h critical system suspend notification
  0009h user system standby request notification
  000Ah user system suspend request notification
  000Bh system standby resume notification
 ---APM v1.2---
  000Ch capabilities change notification (see AX=5310h)
 ------
  000Dh-00FFh reserved system events
  01xxh reserved device events
  02xxh OEM-defined APM events
  0300h-FFFFh reserved
 --------p-15530C-----------------------------
 INT 15 - Advanced Power Management v1.1+ - GET POWER STATE
        AX = 530Ch
        BX = device ID (see #0400)
 Return: CF clear if successful
            CX = system state ID (see #0401)
        CF set on error
            AH = error code (01h,09h) (see #0399)
 SeeAlso: AX=5307h
 --------p-15530D-----------------------------
 INT 15 - Advanced Power Management v1.1+ - EN/DISABLE DEVICE POWER MANAGEMENT
        AX = 530Dh
        BX = device ID (see #0400)
        CX = function
            0000h disable power management
            0001h enable power management
 Return: CF clear if successful
        CF set on error
            AH = error code (01h,03h,09h,0Ah,0Bh) (see #0399)
 Desc:  specify whether automatic power management should be active for a
          given device
 SeeAlso: AX=5308h,AX=530Fh
 --------p-15530E-----------------------------
 INT 15 - Advanced Power Management v1.1+ - DRIVER VERSION
        AX = 530Eh
        BX = device ID of system BIOS (0000h)
        CH = APM driver major version (BCD)
        CL = APM driver minor version (BCD) (02h for APM v1.2)
 Return: CF clear if successful
            AH = APM connection major version (BCD)
            AL = APM connection minor version (BCD)
        CF set on error
            AH = error code (03h,09h,0Bh) (see #0399)
 SeeAlso: AX=5300h,AX=5303h
 --------p-15530F-----------------------------
 INT 15 - Advanced Power Management v1.1+ - ENGAGE/DISENGAGE POWER MANAGEMENT
        AX = 530Fh
        BX = device ID (see #0400)
        CX = function
            0000h disengage power management
            0001h engage power management
 Return: CF clear if successful
        CF set on error
            AH = error code (01h,09h) (see #0399)
 Notes: unlike AX=5308h, this call does not affect the functioning of the APM
          BIOS
        when cooperative power management is disengaged, the APM BIOS performs
          automatic power management of the system or device
 SeeAlso: AX=5308h,AX=530Dh
 --------p-155310-----------------------------
 INT 15 - Advanced Power Management v1.2 - GET CAPABILITIES
        AX = 5310h
        BX = device ID (see #0400)
            0000h (APM BIOS)
            other reserved
 Return: CF clear if successful
            BL = number of battery units supported (00h if no system batteries)
            CX = capabilities flags (see #0406)
        CF set on error
            AH = error code (01h,09h,86h) (see #0399)
 Notes: this function is supported via the INT 15, 16-bit protected mode, and
          32-bit protected mode interfaces; it does not require that a
          connection be established prior to use
        this function will return the capabilities currently in effect, not
          any new settings which have been made but do not take effect until
          a system restart
 SeeAlso: AX=5300h,AX=530Fh,AX=5311h,AX=5312h,AX=5313h

 Bitfields for APM v1.2 capabilities flags:
 Bit(s) Description     (Table 0406)
  0     can enter global standby state
  1     can enter global suspend state
  2     resume timer will wake up system from standby mode
  3     resume timer will wake up system from suspend mode
  4     Resume on Ring Indicator will wake up system from standby mode
  5     Resume on Ring Indicator will wake up system from suspend mode
  6     PCMCIA Ring Indicator will wake up system from standby mode
  7     PCMCIA Ring Indicator will wake up system from suspend mode
 --------p-155311-----------------------------
 INT 15 - Advanced Power Management v1.2 - GET/SET/DISABLE RESUME TIMER
        AX = 5311h
        BX = device ID (see #0400)
            0000h (APM BIOS)
            other reserved
        CL = function
            00h disable Resume Timer
            01h get Resume Timer
            02h set Resume Timer
                CH = resume time, seconds (BCD)
                DL = resume time, minutes (BCD)
                DH = resume time, hours (BCD)
                SI = resume date (BCD), high byte = month, low byte = day
                DI = resume date, year (BCD)
 Return: CF clear if successful
            ---if getting timer---
            CH = resume time, seconds (BCD)
            DL = resume time, minutes (BCD)
            DH = resume time, hours (BCD)
            SI = resume date (BCD), high byte = month, low byte = day
            DI = resume date, year (BCD)
        CF set on error
            AH = error code (03h,09h,0Ah,0Bh,0Ch,0Dh,86h) (see #0399)
 Notes: this function is supported via the INT 15, 16-bit protected mode, and
          32-bit protected mode interfaces
 SeeAlso: AX=5300h,AX=5310h,AX=5312h,AX=5313h
 --------p-155312-----------------------------
 INT 15 - Advanced Power Management v1.2 - ENABLE/DISABLE RESUME ON RING
        AX = 5312h
        BX = device ID (see #0400)
            0000h (APM BIOS)
            other reserved
        CL = function
            00h disable Resume on Ring Indicator
            01h enable Resume on Ring Indicator
            02h get Resume on Ring Indicator status
 Return: CF clear if successful
            CX = resume status (0000h disabled, 0001h enabled)
        CF set on error
            AH = error code (03h,09h,0Ah,0Bh,0Ch,86h) (see #0399)
 Notes: this function is supported via the INT 15, 16-bit protected mode, and
          32-bit protected mode interfaces
 SeeAlso: AX=5300h,AX=5310h,AX=5311h,AX=5313h
 --------p-155313-----------------------------
 INT 15 - Advanced Power Management v1.2 - ENABLE/DISABLE TIMER-BASED REQUESTS
        AX = 5313h
        BX = device ID (see #0400)
            0000h (APM BIOS)
            other reserved
        CL = function
            00h disable timer-based requests
            01h enable timer-based requests
            02h get timer-based requests status
 Return: CF clear if successful
            CX = timer-based requests status (0000h disabled, 0001h enabled)
        CF set on error
            AH = error code (03h,09h,0Ah,0Bh,86h) (see #0399)
 Notes: this function is supported via the INT 15, 16-bit protected mode, and
          32-bit protected mode interfaces
 SeeAlso: AX=5300h,AX=5310h,AX=5311h,AX=5312h
 --------p-155380BH00-------------------------
 INT 15 - APM SL Enhanced v1.0 - GET SUSPEND/GLOBAL STANDBY MODE
        AX = 5380h
        BH = 00h
 Return: CF clear if successful
            AL = 82360SL Auto Power Off Timer High Count (APWR_TMRH)
            BL = sustdbymode (see #0407)
 SeeAlso: AX=5380h/BH=01h,AX=5380h/BH=02h,AX=5380h/BH=7Fh

 Bitfields for APM SL sustdbymode:
 Bit(s) Description     (Table 0407)
  2     ???
  1     Auto Power Off Timer Enable (APWR_TMR_EN)
  0     ???
 --------p-155380BH01-------------------------
 INT 15 - APM SL Enhanced v1.0 - SET SUSPEND/GLOBAL STANDBY MODE
        AX = 5380h
        BH = 01h
        BL = sustdbymode (see #0407)
 Return: CF clear if successful
 SeeAlso: AX=5380h/BH=00h,AX=5380h/BH=7Fh
 --------p-155380BH02-------------------------
 INT 15 - APM SL Enhanced v1.0 - GET GLOBAL STANDBY TIMER
        AX = 5380h
        BH = 02h
 Return: CF clear if successful
        SI:DI = timer count in seconds (actually 1.024 seconds)
 Desc:  reads the value of 82360SL GSTDBY_TMRH & GSTDBY_TMRL registers
 SeeAlso: AX=5380h/BH=00h,AX=5380h/BH=03h,AX=5380h/BH=04h,AX=5380h/BH=7Fh
 --------p-155380BH03-------------------------
 INT 15 - APM SL Enhanced v1.0 - SET GLOBAL STANDBY TIMER
        AX = 5380h
        BH = 03h
        SI:DI = timer count in seconds (actually 1.024 seconds)
 Return: CF clear if successful
 Desc:  sets the value of 82360SL GSTDBY_TMRH & GSTDBY_TMRL registers
 Note:  the maximum timer count is 268431 seconds
 SeeAlso: AX=5380h/BH=02h,AX=5380h/BH=7Fh
 --------p-155380BH04-------------------------
 INT 15 - APM SL Enhanced v1.0 - GET AUTO POWER OFF TIMER
        AX = 5380h
        BH = 04h
 Return: CF clear if successful
        SI:DI = timer count in seconds (actually 1.024 seconds)
 Desc:  reads the value of 82360SL APWR_TMRH & APWR_TMRL registers
 SeeAlso: AX=5380h/BH=02h,AX=5380h/BH=05h,AX=5380h/BH=06h,AX=5380h/BH=7Fh
 --------p-155380BH05-------------------------
 INT 15 - APM SL Enhanced v1.0 - SET AUTO POWER OFF TIMER
        AX = 5380h
        BH = 05h
        SI:DI = timer count in seconds (actually 1.024 seconds)
 Return: CF clear if successful
 Desc:  sets the value of 82360SL APWR_TMRH & APWR_TMRL registers
 Note:  the maximum timer count is 134213 seconds
 SeeAlso: AX=5380h/BH=04h,AX=5380h/BH=7Fh
 --------p-155380BH06-------------------------
 INT 15 - APM SL Enhanced v1.0 - GET RESUME CONDITION
        AX = 5380h
        BH = 06h
 Return: CF clear if successful
            BL = resume condition (see #0408)
 Desc:  reads the value of 82360SL RESUME_MASK register
 SeeAlso: AX=5380h/BH=04h,AX=5380h/BH=07h,AX=5380h/BH=08h,AX=5380h/BH=7Fh

 Bitfields for APM SL resume condition:
 Bit(s) Description     (Table 0408)
  7-2   reserved (0)
  1     alarm enabled (resume on CMOS alarm)
  0     ring enabled
 --------p-155380BH07-------------------------
 INT 15 - APM SL Enhanced v1.0 - SET RESUME CONDITION
        AX = 5380h
        BH = 07h
        BL = resume condition (see #0408)
 Return: CF clear if successful
 Desc:  sets the value of 82360SL RESUME_MASK register
 SeeAlso: AX=5380h/BH=06h,AX=5380h/BH=7Fh
 --------p-155380BH08-------------------------
 INT 15 - APM SL Enhanced v1.0 - GET CALENDAR EVENT TIME
        AX = 5380h
        BH = 08h
 Return: CF clear if successful
            CH = hours
            CL = minutes
            SI = seconds
        CF set on error
            AH = error code (see #0409)
 Desc:  gets calendar event time from CMOS ram
 SeeAlso: AX=5380h/BH=06h,AX=5380h/BH=09h,AX=5380h/BH=0Ah,AX=5380h/BH=7Fh

 (Table 0409)
 Values for APM SL error code:
  02h   no alarm set
  03h   no battery
 --------p-155380BH09-------------------------
 INT 15 - APM SL Enhanced v1.0 - SET CALENDAR EVENT TIME
        AX = 5380h
        BH = 09h
        CH = hours
        CL = minutes
        SI = seconds
 Return: CF clear if successful
        CF set on error
            AH = error code (see #0409)
 Desc:  sets calendar event time in CMOS ram, enables Alarm resume
 SeeAlso: AX=5380h/BH=08h,AX=5380h/BH=7Fh
 --------p-155380BH0A-------------------------
 INT 15 - APM SL Enhanced v1.0 - GET CALENDAR EVENT DATE
        AX = 5380h
        BH = 0Ah
 Return: CF clear if successful
            SI = century
            DI = year
            CH = month
            CL = day
        CF set on error
            AH = error code (see #0409)
 Desc:  reads calendar event date from Extended CMOS ram
 SeeAlso: AX=5380h/BH=08h,AX=5380h/BH=0Bh,AX=5380h/BH=0Ch,AX=5380h/BH=7Fh
 --------p-155380BH0B-------------------------
 INT 15 - APM SL Enhanced v1.0 - SET CALENDAR EVENT DATE
        AX = 5380h
        BH = 0Bh
        SI = century
        DI = year
        CH = month
        CL = day
 Return: CF clear if successful
        CF set on error
            AH = error code (see #0409)
 Desc:  sets calendar event date in Extended CMOS ram
 SeeAlso: AX=5380h/BH=0Ah,AX=5380h/BH=7Fh
 --------p-155380BH0C-------------------------
 INT 15 - APM SL Enhanced v1.0 - GET CPU SPEED MODE
        AX = 5380h
        BH = 0Ch
 Return: CF clear if successful
        CL = CPU clock divider (1,2,4 or 8)
        BL = autocpumode ???
 Desc:  reads bits 4-5 of CPUPWRMODE register
 SeeAlso: AX=5380h/BH=0Ah,AX=5380h/BH=0Dh,AX=5380h/BH=7Fh
 --------p-155380BH0D-------------------------
 INT 15 - APM SL Enhanced v1.0 - SET CPU SPEED MODE
        AX = 5380h
        BH = 0Dh
        CL = CPU clock divider (1,2,4 or 8)
        BL = autocpumode ???
 Return: CF clear if successful
 Desc:  writes bits 4-5 of CPUPWRMODE register
 SeeAlso: AX=5380h/BH=0Ch,AX=5380h/BH=7Eh,AX=5380h/BH=7Fh
 --------p-155380BH7E-------------------------
 INT 15 - APM SL Enhanced v1.0 - SL HW PARAMETER
        AX = 5380h
        BH = 7Eh
 Return: AL = ???
            03h on A-Step 386SL BIOSes
            12h on later steps
        BX = Control port (00B0h)
 SeeAlso: AX=5380h/BH=00h,AX=5380h/BH=7Fh
 --------p-155380BH7F-------------------------
 INT 15 - Advanced Power Management v1.1 - OEM APM INSTALLATION CHECK
        AX = 5380h
        BH = 7Fh
 Return: CF clear if successful
            BX = OEM identifier
            all other registers OEM-defined
            ---Intel SL Enhanced Option BIOS---
            BX = 534Ch ('SL')
            CL = 4Fh ('O')
            AL = version (10h = 1.0)
            ---HP APM BIOS---
            BX = 4850h ('HP')
            CX = version (0001h)
        CF set on error
            AH = error code (03h) (see #0399)
 SeeAlso: AX=5380h/BH=00h
 --------p-155380-----------------------------
 INT 15 - Advanced Power Management v1.1 - OEM APM FUNCTIONS
        AX = 5380h
        BH <> 7Fh
        all other registers OEM-defined
 Return: OEM-defined
 SeeAlso: AX=5380h/BH=7Fh
\***********************************************************************/

#pragma pack()

#endif  /* __APM_H__ */
