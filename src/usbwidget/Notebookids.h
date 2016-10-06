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



#define NB_PAGE_DEFAULT                 10999   // Default Page Dialog


