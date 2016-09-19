/*****************************************************************************\
* Dialogids.h -- NeoWPS * USB Widget                                          *
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
 * This is a public header file that defines all dialog id's.
 * It is used by other resource files.
 */


/* Main Icon */
#define     ID_ICO_USB_WIDGET                   1


/* Png embedded images */
#ifndef     RT_PNG
#define     RT_PNG                              400     // ecomedia value
#endif

#define     ID_PNG_BASE                         30
#define     ID_PNG_USBDEV                       31
#define     ID_PNG_USBDEV2                      32
#define     ID_PNG_INFORMATION                  33
#define     ID_PNG_WARNING                      34
#define     ID_PNG_WIRELESS                     35
#define     ID_PNG_BLUETOOTH                    36
#define     ID_PNG_ATTENTIONS                   37
#define     ID_PNG_TRANSPORTATION               38
#define     ID_PNG_EJECT_BUTTON                 39


/* Icons */
#define     ID_POP_MENU                         21
#define     ID_ICO_EJECT                        22
#define     ID_ICO_CD                           23
#define     ID_ICO_HDD                          24
#define     ID_ICO_HDDBAD                       25
#define     ID_ICO_HDDPHANTOM                   26
#define     ID_ICO_ZIP                          27


/* Dialogs */
#define     ID_USB_WIDGET_DIALOG                100
#define     ID_NOTIFICATION_DIALOG              101
#define     ID_DEBUG_DIALOG                     102
#define     ID_FORCE_REMOVE_DIALOG              103

#define     ID_WIDGET_UUID                      200
#define     ID_WIDGET_VERSION                   201

/* Extended Debug Menu */
#define     ID_SUBMENU_DEBUG_CONNECT_PIPE       251
#define     ID_SUBMENU_DEBUG_DISCONNECT_PIPE    252

#define     ID_SHOW_INSERT_NOTIFICATION         261
#define     ID_HIDE_INSERT_NOTIFICATION         262
#define     ID_SHOW_EJECT_NOTIFICATION          263
#define     ID_HIDE_EJECT_NOTIFICATION          264

#define     ID_SUBMENU_DEBUG                    271
#define     ID_SUBMENU_DEBUG_SHOW_DRIVE_MAP     272
#define     ID_SUBMENU_DEBUG_SHOW_DRIVE_MAP2    273
#define     ID_SUBMENU_DEBUG_SHOW_VOLUMES       274
#define     ID_SUBMENU_DEBUG_SHOW_VOLUMES2      275

#define     ID_SUBMENU_DEBUG_SHOW_DIALOGS       281
#define     ID_SUBMENU_DEBUG_HIDE_DIALOGS       282
#define     ID_SUBMENU_DEBUG_SHOW_WINDOWS       283
#define     ID_SUBMENU_DEBUG_HIDE_WINDOWS       284

#define     ID_SUBMENU_WIDGET_SETTINGS_SHOW_NOTEBOOK    291
#define     ID_SUBMENU_DEBUG_SHOW_DEBUG_DIALOG          299




/* Test Dialogs */
#define     ID_DIALOG_BASE                      5000
#define     ID_DIALOG_1                         5001
#define     ID_DIALOG_2                         5002
#define     ID_DIALOG_3                         5003
#define     ID_DIALOG_4                         5004
#define     ID_DIALOG_NEXT                      5005



/* Test Windows */
#define     ID_WINDOW_BASE                      5500
#define     ID_WINDOW_1                         5501
#define     ID_WINDOW_2                         5502
#define     ID_WINDOW_3                         5503
#define     ID_WINDOW_4                         5504
#define     ID_WINDOW_NEXT                      5505


/* Controls */
#define     ID_CONTROL_BASE                     5700

#define     ID_LB_1                             5701
#define     ID_LB_2                             5702
#define     ID_LB_3                             5703
#define     ID_LB_4                             5704

#define     ID_HWND_DLG                         5709
#define     ID_BLDLEVEL                         5710

#define     ID_MLE_1                            5711
#define     ID_MLE_2                            5712
#define     ID_MLE_3                            5713
#define     ID_MLE_4                            5714

#define     ID_DRIVE_LETTERS                    5801
#define     ID_UNIT_NUMBERS                     5802

#define     MSGSIZE                             128

#define     ID_MEDEDELING                       401

#define     ID_MENU_REDISCOVER                  500



#define     UNDO_BUTTON                         120
#define     DEFAULT_BUTTON                      121
#define     CLOSE_BUTTON                        122

#define     ENTRY_TITLE                         123
#define     ENTRY_NEOWPS                        124
#define     ID_MAXSTRING                        125


#define     ID_ICON_AND_TEXT                    1001
#define     ID_ICON_ONLY                        1002
#define     ID_TEXT_ONLY                        1003

#define     ID_SHOW_DEBUG_INFO                  1010
#define     ID_TIME_DIVIDER                     1011
#define     ID_VERSION_TEXT                     1012

#define     ID_LIFE_PERCENTAGE                  1021
#define     ID_LIFE_TIME                        1022
#define     ID_LIFE_TIME_PERCENTAGE             1023

#define     ID_LIFE_MINUTES                     1031
#define     ID_LIFE_HOURSMINUTES                1032

#define     ID_FF_NEOWPS                        1501
#define     ID_FF_BETAZONE                      1502
#define     ID_FF_BENSBITS                      1503

#define     ID_POWER_AC_ICO                     2000
#define     ID_POWER_BATTERY_ICO                2001
#define     ID_POWER_NO_BATTERY_ICO             2002
#define     ID_POWER_BATTERY020_ICO             2003
#define     ID_POWER_BATTERY025_ICO             2004
#define     ID_POWER_BATTERY050_ICO             2005
#define     ID_POWER_BATTERY075_ICO             2006
#define     ID_POWER_BATTERY100_ICO             2007

#define     ID_COMBO_CHARGED_TITLE              3001
#define     ID_COMBO_CHARGED                    3002
#define     ID_COMBO_CHARGING_TITLE             3003
#define     ID_COMBO_CHARGING                   3004
#define     ID_COMBO_DISCHARGING_TITLE          3005
#define     ID_COMBO_DISCHARGING                3006

#define     ID_CHKDSK                           3007
#define     ID_BEEPS_CONNECT                    3008
#define     ID_MONITOR_ZIP                      3009

#define     ID_WIDGET_PROBLEMS                  4001
#define     ID_WIDGET_UPDATES                   4002
#define     ID_WIDGET_EMAIL                     4003

#define     ID_LOGO                             7001
#define     USB_THUMB                           7002
#define     ICON_INFO                           7003
#define     ICON_WARNING                        7004


/* EcoSoft */
#define     WIDGET_NAME                         "NeoWPS * USB widget"
#define     WIDGET_URL                          "NeoWPS: http://ecomstation.ru/neowps"



/* Actions */
#define     ID_ACTION_1                         12301
#define     ID_ACTION_2                         12302
#define     ID_ACTION_3                         12303
#define     ID_ACTION_4                         12304
#define     ID_ACTION_5                         12305
#define     ID_ACTION_6                         12306
#define     ID_ACTION_7                         12307
#define     ID_ACTION_8                         12308
#define     ID_ACTION_9                         12309
#define     ID_ACTION_10                        12310
#define     ID_ACTION_11                        12311
#define     ID_ACTION_12                        12312   // TermLVMthread
#define     ID_ACTION_13                        12313
#define     ID_ACTION_14                        12314
#define     ID_ACTION_15                        12315
#define     ID_ACTION_16                        12316
#define     ID_ACTION_17                        12317   // QueryDrive (old StartFireFoxAsSession)
#define     ID_ACTION_18                        12318   // PutgeDrive (old StartFireFoxAsProgram)
#define     ID_ACTION_19                        12319   // PhysOpen
#define     ID_ACTION_20                        12320   // PhysClose
#define     ID_ACTION_21                        12321   // DasdOpen
#define     ID_ACTION_22                        12322   // DasdClose
#define     ID_ACTION_23                        12323   // Create
#define     ID_ACTION_24                        12324   // Show
#define     ID_ACTION_25                        12325   // Hide
#define     ID_ACTION_26                        12326   // Destroy

#define     ID_ACTION_27                        12327   // DfsVOS2
#define     ID_ACTION_28                        12328   // FixDialog
#define     ID_ACTION_29                        12329



#define     ID_CLEAR                            12330
#define     ID_CLEAR1                           12331
#define     ID_CLEAR2                           12332
#define     ID_CLEAR3                           12333

#define     ID_FOCUS_ME                         0

/* Newly Added */
#define DID_CLOSE                               89
#define ID_PROBLEMFIXER_DIALOG                  19648
#define ID_DRIVE_LETTER                         19649
#define DID_FIX                                 19650
#define ID_FIX_WARNING                          19651
#define ID_CLEAR_LOG_WINDOW                     19652
#define ID_DIALOG_TITLE                         19653
#define ID_DISK_NUMBER                          19654
#define ID_GROUP_ACTIONS                        19655
#define ID_GROUP_BACKUP_RESTORE                 19656
#define ID_IMAGE_BACKUP                         19657
#define ID_IMAGE_RESTORE                        19658
#define ID_GROUP_SHOW_DISK_INFO                 19659
#define DID_HIDE                                19660
#define ID_SHOW_PROBLEMS                        19661
#define ID_GROUP_PROBLEMS_AND_FIXING            19662
#define ID_TEST_1                               19701
#define ID_TEST_2                               19702
#define ID_TEST_3                               19703
#define ID_TEST_4                               19704
#define ID_TEST_5                               19705
#define ID_TEST_6                               19706
#define ID_TEST_7                               19707

