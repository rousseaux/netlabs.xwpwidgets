###############################################################################
# Makefile -- NeoWPS * USB Widget                                             #
#                                                                             #
# Copyright (c) RDP Engineering                                               #
#                                                                             #
# Author: Ben Rietbroek <rousseau.os2dev@gmx.com>                             #
#                                                                             #
#   This program is free software; you can redistribute it and/or modify      #
#   it under the terms of the GNU General Public License as published by      #
#   the Free Software Foundation; either version 2 of the License, or         #
#   (at your option) any later version.                                       #
#                                                                             #
#   This program is distributed in the hope that it will be useful,           #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of            #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
#   GNU General Public License for more details.                              #
#                                                                             #
#   You should have received a copy of the GNU General Public License         #
#   along with this program; if not, write to the Free Software               #
#   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,                #
#   MA 02110-1301, USA.                                                       #
#                                                                             #
###############################################################################

#
# This is the main Makefile.
# It builds the USB Widget and it accompanying DAEMON.
#

# Include Build Variables
!include Bldvars.mif

BLDENV=vac3     # Current active toolchain
#~ BLDENV=icc36    # Supported toolchain but not re-verified yet

#
# Macros for toolchains
#
DLLPATH=$(OSDIR)\SYSTEM\eWPS\PLUGINS\XCENTER                    # Target path for the widget-dll
RCINCLUDE=.;h;xworkplace\include;xwphelpers\include             # Dirs to search for rc includes
INCLUDE=$(RCINCLUDE);$(INCLUDE)                                 # Dirs to search for headers
LIB=.;lib;$(LIB)                                                # Dirs to search for libraries
LIBS=ecolange.lib lvm.lib usbcalls.lib ecomedia.lib             # External libraries to use
CCFLAGS=-Q+ -C+ -Gm+ -Ss -Sp1 -Su4 -Gd- -Ge+ -Tdp               # Compiler flags
LIBFLAGS=/nologo                                                # Lib flags
LNKFLAGS=-nologo -noe -map:$*.map                               # Linker flags
RCFLAGS=-n                                                      # Resource Compiler flags
CC=icc                                                          # Compiler to use
LIBTOOL=ilib                                                    # Librarian to use
LNK=ilink                                                       # Linker to use
RC=rc                                                           # Resource compiler to use

# Vac3 barks on _Optlink on static member function; disable test-threads when
# compiling with vac3.
!if "$(BLDENV)"=="vac3"
CCFLAGS=$(CCFLAGS) -D_OMIT_TEST_THREADS_
RCINCLUDE=$(CPPMAIN)\INCLUDE;$(CPPMAIN)\INCLUDE\OS2;$(RCINCLUDE)
!endif

#
# Debug dependencies
#
DBGDEPS=\
	Bldvars.h\
	Debug.h\
	Debug.hpp\

#
# General dependencies
#
INCDEPS=\
	Bldvars.h\
	Dialogids.h\
	Notebookids.h\
	Master.hpp\
	ModuleGlobals.hpp\

#
# Daemon dependencies
#
DMNDEPS=\
	$(DBGDEPS)\
	$(INCDEPS)\
	AllocMem.hpp\
	Master.hpp\

#
# Class dependencies
#
CLASSDEPS=\
	Classes.hpp\
	Dialog.hpp\
	DebugDialog.hpp\
	ProblemFixerDialog.hpp\
	Monitor.hpp\
	DriveMonitor.hpp\
	UsbMonitor.hpp\
	WidgetSettingsDialog.hpp\
	Object.hpp\
	Notebook.hpp\
	NotebookPage.hpp\
	Notifier.hpp\
	Window.hpp\
	Widget.hpp\

#
# Widget dependencies
#
WGTDEPS=\
	$(DBGDEPS)\
	$(INCDEPS)\
	$(CLASSDEPS)\
	AllocMem.hpp\
	ProblemFixer.hpp\
	Threads.hpp\
	USBHelpers.hpp\
	LVMHelpers.hpp\
	APIHelpers.hpp\
	GUIHelpers.hpp\
	Testing123.hpp\
	USBWidget.hpp\

#
# Resource dependencies
#
RESDEPS=\
	$(DBGDEPS)\
	$(INCDEPS)\
	Dialogs.rc\
	Dialogs.dlg\
	Notebooks.rc\
	Notebooks.dlg\
	usbshold.rc\
	USBWidget.rc\
	Images.rc\

#
# Objects to build the Daemon from
#
DMNOBJECTS=\
	Debug.obj\
	AllocMem.obj\

#
# Class Objects
#
CLASSOBJECTS=\
	Classes.obj\
	Dialog.obj\
	DebugDialog.obj\
	ProblemFixerDialog.obj\
	Monitor.obj\
	DriveMonitor.obj\
	UsbMonitor.obj\
	WidgetSettingsDialog.obj\
	Object.obj\
	Notebook.obj\
	NotebookPage.obj\
	Notifier.obj\
	Window.obj\
	Widget.obj\

#
# Objects to build the Widget from
#
WGTOBJECTS=\
	$(CLASSOBJECTS)\
	Debug.obj\
	AllocMem.obj\
	ProblemFixer.obj\
	Threads.obj\
	CList.obj\
	ModuleGlobals.obj\
	Testing123.obj\
	USBWidget.obj\

#
# Libraries to cluster some stuff
#
LIBRARIES=\
	Classes.lib\
	APIHelpers.lib\
	LVMHelpers.lib\
	USBHelpers.lib\
	GUIHelpers.lib\

# =============================================================================
#                                                [ CPP TO OBJ INFERENCE RULES ]
# =============================================================================
.cpp.obj:
	@echo [$@]
	@echo Reason: $?
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

# =============================================================================
#                                                [ OBJ TO LIB INFERENCE RULES ]
# =============================================================================
.obj.lib:
	@echo [$@]
	@echo Reason: $?
!if "$(BLDENV)"=="vac3"
	$(LIBTOOL) $(LIBFLAGS) $@ -+$?; 2>nul
!else
!if "$(BLDENV)"=="icc36"
	$(LIBTOOL) $(LIBFLAGS) -out:$@ $?
!else
!error **** INVALID BUILD ENVIRONMENT ****
!endif
!endif

# =============================================================================
#                                                         [ PSEUDO ALL TARGET ]
# =============================================================================
all:
	@echo ##############################################################
	@echo ## Building the NeoWPS * USB Widget and it's support Daemon ##
	@echo ##############################################################
	@$(MAKE) -nologo dump-bldenv
	@$(MAKE) -nologo support-daemon
	@$(MAKE) -nologo usb-widget

# =============================================================================
#                                                 [ SUPPORT DAEMON EXECUTABLE ]
# =============================================================================
support-daemon:
	@echo ## Building the Support Daemon ##
	@$(MAKE) -nologo usbshlpr.exe
	@echo.

# =============================================================================
#                                                                [ WIDGET DLL ]
# =============================================================================
usb-widget:
	@echo ## Building the NeoWPS * USB Widget ##
	@$(MAKE) -nologo usbshold.dll

# =============================================================================
#                                                          [ DAEMON EXECUTABLE]
# =============================================================================
usbshlpr.exe: $*.obj $(DMNOBJECTS) $*.def
	@echo.
	@echo ## Linking modules ##
	$(LNK) $(LNKFLAGS) -exe -out:$@ $*.def $*.obj
	@mapsym $*.map > nul
	@exehdr $*.exe > $*.exh
# 2013-03-18 SHL
# Suppress incorrect return value of EXEHDR -V (Version 4.01.003 Dec 11 2003)
	@-exehdr -v $*.exe > $*.exh-v
	@rem exemap $*.exe > $*.exm
	@echo.
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo @ TARGET BUILT: $@ @
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@exehdr -nologo $@
	@echo.
	@bldlevel $@
	@bldlevel $@ > $*.bldlevel
	@echo.
	@type $*.md5
	@md5sum $@
	@md5sum $@ > $*.md5
	@copy $@ $*.dmn

# =============================================================================
#                                                   [ MAIN MODULE OBJECT FILE ]
# =============================================================================
usbshlpr.obj: $*.cpp $*.hpp $*.lvl $(DMNDEPS)
	@echo [$@]
	@echo Reason: $?
	$(CC) $(CCFLAGS) -Ge+ -Fo$*.obj $*.cpp

# =============================================================================
#                                                                [ WIDGET DLL ]
# =============================================================================
usbshold.dll: $*.obj $(WGTOBJECTS) $(LIBRARIES) $*.res $*.def
	@echo.
	@echo ## Linking modules ##
	$(LNK) $(LNKFLAGS) -dll -out:$@ $*.def $*.obj $(WGTOBJECTS) $(LIBRARIES) $(LIBS)
	@mapsym $*.map > nul
	@echo.
	@echo ## Embedding Resources ##
	$(RC) $(RCFLAGS) $*.res $*.dll
	@exehdr $*.DLL > $*.exh
# 2013-03-18 SHL
# Suppress incorrect return value of EXEHDR -V (Version 4.01.003 Dec 11 2003)
	@-exehdr -V $*.DLL > $*.exh-v
	@rem exemap $*.DLL > $*.exm
	@echo.
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@echo @ TARGET BUILT: $@ @
	@echo @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@exehdr -NOlogo $@
	@echo.
	@bldlevel $@
	@bldlevel $@ > $*.bldlevel
	@echo.
	@type $*.md5
	@md5sum $@
	@md5sum $@ > $*.md5

# =============================================================================
#                                                   [ MAIN MODULE OBJECT FILE ]
# =============================================================================
usbshold.obj: $*.cpp $*.hpp $*.lvl $(WGTDEPS)
	@echo [$@]
	@echo Reason: $?
	$(CC) $(CCFLAGS) -Ge- -Fo$*.obj $*.cpp

# =============================================================================
#                                                 [ MAIN MODULE RESOURCE FILE ]
# =============================================================================
usbshold.res: $(RESDEPS)
	@echo ## Compiling Resources ##
!if "$(BLDENV)"=="vac3"
	@set INCLUDE=$(RCINCLUDE)
!endif
	$(RC) -r Dialogs.rc
	$(RC) -r Notebooks.rc
	$(RC) -r $*.rc $@

# =============================================================================
#                                    [ OBJECTS TO BUILD USING INFERENCE RULES ]
# =============================================================================
# Class Objects
Classes.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Dialog.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
DebugDialog.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
ProblemFixerDialog.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Monitor.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
DriveMonitor.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
UsbMonitor.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Notebook.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
NotebookPage.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Notifier.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
WidgetSettingsDialog.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Object.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Window.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Widget.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
# Common Objects
AllocMem.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
APIHelpers.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
CList.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Debug.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
GUIHelpers.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
LVMHelpers.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
ModuleGlobals.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
ProblemFixer.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Testing123.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
Threads.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
USBHelpers.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)
USBWidget.obj: $*.cpp $*.hpp $(INCDEPS) $(DBGDEPS)

# =============================================================================
#                                  [ LIBRARIES TO BUILD USING INFERENCE RULES ]
# =============================================================================
Classes.lib: $(CLASSOBJECTS)
APIHelpers.lib: $*.obj
GUIHelpers.lib: $*.obj
LVMHelpers.lib: $*.obj
USBHelpers.lib: $*.obj

# =============================================================================
#                                                  [ DEPLOY TARGETS TO SYSTEM ]
# =============================================================================
dist:
	@echo.
	@echo ## Distributing Widget and Daemon to System ##
	@call install-usb-widget.cmd

# =============================================================================
#                                                [ CLEANUP INTERMEDIATE FILES ]
# =============================================================================
# Extensions to remove
CLEANEXTS=obj lib dll exe dmn res map sym exh exh-v exm lst err bak
clean:
	@echo.
	@echo ## Cleaning up... ##
	@for %%i in ($(CLEANEXTS)) do @if exist *.%%i dir /b *.%%i && del *.%%i
	@if exist Makefile.bldenv del Makefile.bldenv
#	@echo Cleaning code-store
#	vacbld -CL "$(BASENAME).icc"
	@touch Bldvars.mif

# Include targets that generate files
!include GenFiles.mif
