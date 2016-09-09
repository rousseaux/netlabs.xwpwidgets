/*****************************************************************************\
* WidgetSettings.cpp -- NeoWPS * USB Widget                                   *
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

// From GUIHelpers.cpp

/**
 * This module contains the new WidgetSettings Notebook.
 */


#include    "WidgetSettings.hpp"


void    WidgetSettings(void) {
}

/*
// WidgetSettings
*/
WidgetSettings::WidgetSettings() {
    MessageBox("WidgetSettings","CONSTRUCTOR");
}

WidgetSettings::~WidgetSettings() {
    MessageBox("WidgetSettings","DESTRUCTOR");
}

int WidgetSettings::create() {
    MessageBox("WidgetSettings","CREATE");
    return NULL;
}

//~ int WidgetSettings::show() {
    //~ MessageBox("Widget Settings #1","Widget Settings #2");
    //~ return NULL;
//~ }

int WidgetSettings::process() {
    MessageBox("WidgetSettings","PROCESS");
    return NULL;
}

int WidgetSettings::destroy() {
    MessageBox("WidgetSettings","DESTROY");
    return NULL;
}
