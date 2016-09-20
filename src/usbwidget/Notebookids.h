/*****************************************************************************\
* Notebookids.h -- NeoWPS * USB Widget                                        *
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
* This file defines the IDs for Notebook Dialogs and their controls.
* While the ID of a child-window is in the scope of its parent, and other
* can have child-windows with the same ID, for the Dialog Editor the IDs here
* must be unique. This is because multiple dialogs with their controls can be
* defined and duplicate IDs makes it impossible for the Dialog Editor to map
* them to their corresponding #defines.
*/

#define DLG_ID_WIDGETSETTINGS               10000   // Notebook Dialog

#define DLG_ID_WIDGETSETTINGS_CLOSEBUTTON   10101   // Close Button
#define DLG_ID_WIDGETSETTINGS_NOTEBOOK      10900   // Notebook Control

#define NB_PAGE_DEFAULT                 10999   // Default Page Dialog

#define NB_PAGE_1                       10901   // Page #1 Page Dialog
#define NB_PAGE_1_LB_1                      1   // Listbox #1
#define NB_PAGE_1_PB_1                     11   // Button #1
#define NB_PAGE_1_PB_2                     12   // Button #2
#define NB_PAGE_1_PB_3                     13   // Button #3
#define NB_PAGE_1_PB_4                     14   // Button #4
#define NB_PAGE_1_PB_5                     15   // Button #5
#define NB_PAGE_1_PB_6                     16   // Button #6
#define NB_PAGE_1_PB_7                     17   // Button #7

#define NB_PAGE_2                       10902   // Page #2 Page Dialog
#define NB_PAGE_2_LB_1                      1   // Listbox #1

#define NB_PAGE_3                       10903   // Page #3 Page Dialog
#define NB_PAGE_3_LB_1                      1   // Listbox #1
