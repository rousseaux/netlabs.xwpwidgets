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
EXTLIBS=ecolange.lib lvm.lib usbcalls.lib ecomedia.lib          # External libraries to use
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
# Global Dependencies
#
GLOBDEPS=\
Bldvars.h\
Debug.h\
Dialogids.h\
Notebookids.h\
Master.hpp\
ModuleGlobals.hpp\

#
# Daemon Modules
#
DEAMON_MODULES=

#
# Class Modules
#
CLASS_MODULES=\
Classes@\
Button@\
Dialog@\
DebugDialog@\
ProblemFixerDialog@\
Monitor@\
DriveMonitor@\
UsbMonitor@\
WidgetSettingsDialog@\
Object@\
Notebook@\
NotebookPage@\
Notifier@\
Window@\
Widget@\
UsbWidget@\

#
# Widget Modules
#
WIDGET_MODULES=\
AllocMem@\
CList@\
Debug@\
ModuleGlobals@\
ProblemFixer@\
Testing123@\
Threads@\

#
# Widget Legacy Implementation
#
WIDGET_IMPL=\
usbsimpl@\

#
# Resource Dependencies
#
RESDEPS=\
Dialogs.rc\
Dialogs.dlg\
Images.rc\
Notebooks.rc\
Notebooks.dlg\
usbshold.rc\
usbsimpl.rc\

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
	@echo.
	@echo ## Building the Support Daemon ##
	@$(MAKE) -nologo usbshlpr.exe

# =============================================================================
#                                                                [ WIDGET DLL ]
# =============================================================================
usb-widget:
	@echo.
	@echo ## Building the NeoWPS * USB Widget ##
	@$(MAKE) -nologo usbshold.dll

# =============================================================================
#                                                          [ DAEMON EXECUTABLE]
# =============================================================================
usbshlpr.exe: $*.obj $(DAEMON_MODULES:@=.obj) $*.def
	@echo.
	@echo [$@]
	@echo Reason: $?
	@echo		## Linking modules ##
	$(LNK) $(LNKFLAGS) -exe -out:$@ $*.obj $(DAEMON_MODULES:@=.obj) $*.def
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
usbshlpr.obj: $*.cpp $*.hpp $*.lvl $(GLOBDEPS)
	@echo.
	@echo [$@]
	@echo Reason: $?
	$(CC) $(CCFLAGS) -Ge+ -Fo$*.obj $*.cpp

# =============================================================================
#                                                                [ WIDGET DLL ]
# =============================================================================
usbshold.dll: $*.obj $(WIDGET_IMPL:@=.obj) $*.res $*.def
	@echo.
	@echo [$@]
	@echo Reason: $?
	@echo		## Linking modules ##
	$(LNK) $(LNKFLAGS) -dll -out:$@ $*.obj $(WIDGET_IMPL:@=.obj) $(WIDGET_MODULES:@=.obj) $(LIBRARIES) $(EXTLIBS) $*.def
	@mapsym $*.map > nul
	@echo		## Embedding Resources ##
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
#                      [ LEGACY IMPLEMENTATION TO BUILD USING INFERENCE RULES ]
# =============================================================================
usbsimpl.obj: $*.cpp $*.hpp $(WIDGET_MODULES:@=.obj) $(LIBRARIES) $(GLOBDEPS)

# =============================================================================
#                                                   [ MAIN MODULE OBJECT FILE ]
# =============================================================================
usbshold.obj: $*.cpp $*.hpp $*.lvl $(WIDGET_MODULES:@=.obj) $(LIBRARIES) $(GLOBDEPS)
	@echo.
	@echo [$@]
	@echo Reason: $?
	$(CC) $(CCFLAGS) -Ge- -Fo$*.obj $*.cpp

# =============================================================================
#                                                 [ MAIN MODULE RESOURCE FILE ]
# =============================================================================
usbshold.res: $(RESDEPS) $(GLOBDEPS)
	@echo ## Compiling Resources ##
!if "$(BLDENV)"=="vac3"
	@set INCLUDE=$(RCINCLUDE)
!endif
	$(RC) -r Dialogs.rc
	$(RC) -r Notebooks.rc
	$(RC) -r $*.rc $@

# =============================================================================
#                              [ CLASS OBJECTS TO BUILD USING INFERENCE RULES ]
# =============================================================================
Classes.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Button.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Dialog.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
DebugDialog.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
ProblemFixerDialog.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Monitor.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
DriveMonitor.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
UsbMonitor.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Notebook.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
NotebookPage.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Notifier.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
WidgetSettingsDialog.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Object.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Window.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Widget.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
UsbWidget.obj: $*.cpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)

# =============================================================================
#                             [ COMMON OBJECTS TO BUILD USING INFERENCE RULES ]
# =============================================================================
AllocMem.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
APIHelpers.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
CList.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Debug.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
GUIHelpers.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
LVMHelpers.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
ModuleGlobals.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
ProblemFixer.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Testing123.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
Threads.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)
USBHelpers.obj: $*.cpp $*.hpp $(CLASS_MODULES:@=.hpp) $(GLOBDEPS)

# =============================================================================
#                                  [ LIBRARIES TO BUILD USING INFERENCE RULES ]
# =============================================================================
Classes.lib: $(CLASS_MODULES:@=.obj)
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

# Start Daemon (first build if needed)
daemon-start: support-daemon
	@echo.
	@echo [$@]
	usbshlpr.exe

# Stop Daemon
daemon-stop:
	@echo.
	@echo [$@]
	usbshlpr.exe /stop

# =============================================================================
#                                                [ CPP TO OBJ INFERENCE RULES ]
# =============================================================================
.cpp.obj:
	@echo.
	@echo [$@]
	@echo Reason: $?
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

# =============================================================================
#                                                [ OBJ TO LIB INFERENCE RULES ]
# =============================================================================
.obj.lib:
	@echo.
	@echo [$@]
	@echo Reason: $?
!if "$(BLDENV)"=="vac3"
	!$(LIBTOOL) $(LIBFLAGS) $@ -+$?; 2>nul
!else
!if "$(BLDENV)"=="icc36"
	$(LIBTOOL) $(LIBFLAGS) -out:$@ $?
!else
!error **** INVALID BUILD ENVIRONMENT ****
!endif
!endif

# Include targets that generate files
!include GenFiles.mif
