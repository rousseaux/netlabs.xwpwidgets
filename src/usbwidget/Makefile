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

BLDENV=vac3
#BLDENV=icc36
#BLDENV=vac4    # Not supported anymore

#
# ------------------------------------------------------------------ [ MACROS ]
#
ARROW=------*
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

#CCFLAGS=-L+ $(CCFLAGS)

# Vac3 barks on _Optlink on static member function; disable test-threads when
# compiling with vac3.
!if "$(BLDENV)"=="vac3"
CCFLAGS=$(CCFLAGS) -D_OMIT_TEST_THREADS_
RCINCLUDE=$(CPPMAIN)\INCLUDE;$(CPPMAIN)\INCLUDE\OS2;$(RCINCLUDE)
!endif

#
# ----------------------------------------------------------------- [ SOURCES ]
#
SOURCES=\
	Debug.cpp\
	AllocMem.cpp\
	ProblemFixer.cpp\
	Threads.cpp\
	CList.cpp\
	USBHelpers.cpp\
	LVMHelpers.cpp\
	APIHelpers.cpp\
	GUIHelpers.cpp\
	ModuleGlobals.cpp\
	Testing123.cpp\
	WidgetSettings.cpp\
	USBWidget.cpp\
	usbshold.lvl\
	usbshold.cpp\

#
# ------------------------------------------------------------ [ DEPENDENCIES ]
#
DEPS=\
	Bldvars.h\
	Debug.h\
	Dialogids.h\
	Notebookids.h\
	Master.hpp\
	Debug.hpp\
	AllocMem.hpp\
	ProblemFixer.hpp\
	Threads.hpp\
	USBHelpers.hpp\
	LVMHelpers.hpp\
	APIHelpers.hpp\
	GUIHelpers.hpp\
	ModuleGlobals.hpp\
	Testing123.hpp\
	WidgetSettings.hpp\
	USBWidget.hpp\

#
# --------------------------------------------------- [ RESOURCE DEPENDENCIES ]
#
RESDEPS=\
	Debug.h\
	Debug.hpp\
	Dialogs.rc\
	Dialogs.dlg\
	Dialogids.h\
	Notebooks.rc\
	Notebooks.dlg\
	Notebookids.h\
	usbshold.rc\
	USBWidget.rc\
	Images.rc\

#
# ------------------------------------------------- [ OBJECT MODULES TO BUILD ]
#
OBJECTS=\
	Debug.obj\
	AllocMem.obj\
	ProblemFixer.obj\
	Threads.obj\
	CList.obj\
	ModuleGlobals.obj\
	Testing123.obj\
	WidgetSettings.obj\
	USBWidget.obj\
	usbshold.obj\

#
# -----------------------------------------------[ LIBRARIES TO BUILD AND USE ]
#
LIBRARIES=\
	APIHelpers.lib\
	LVMHelpers.lib\
	USBHelpers.lib\
	GUIHelpers.lib\

#
# --------------------------------------------------------[ PSEUDO ALL TARGET ]
#
all: Bldvars.h
	@echo ##############################################################
	@echo ## Building the NeoWPS * USB Widget and it's support Daemon ##
	@echo ##############################################################
	@$(MAKE) -nologo dump-bldenv
	@$(MAKE) -nologo support-daemon
	@$(MAKE) -nologo usb-widget
#	@$(MAKE) clean
#	@$(MAKE) map
#	@$(MAKE) rename
#	@$(MAKE) dist
#	@$(MAKE) status

#
# ------------------------------------------------ [ GENERATE BUILD VARIABLES ]
#
Bldvars.h: Bldvars.mif
	@echo $(ARROW) [ Generating Build Variables ]
	@type nul > $@
	@echo /* $@ -- Build Variables generated from $? */ >> $@
	@echo #define VENDOR_STR "$(VENDOR)" >> $@
	@echo #define VERSION_MAJOR $(VERSION_MAJOR) >> $@
	@echo #define VERSION_MAJOR_STR "$(VERSION_MAJOR)" >> $@
	@echo #define VERSION_MIDDLE $(VERSION_MIDDLE) >> $@
	@echo #define VERSION_MIDDLE_STR "$(VERSION_MIDDLE)" >> $@
	@echo #define VERSION_MINOR $(VERSION_MINOR) >> $@
	@echo #define VERSION_MINOR_STR "$(VERSION_MINOR)" >> $@
	@echo #define BLD_YEAR $(BLD_YEAR) >> $@
	@echo #define BLD_YEAR_STR "$(BLD_YEAR)" >> $@
	@echo #define BLD_MONTH $(BLD_MONTH) >> $@
	@echo #define BLD_MONTH_STR "$(BLD_MONTH)" >> $@
	@echo #define BLD_DAY $(BLD_DAY) >> $@
	@echo #define BLD_DAY_STR "$(BLD_DAY)" >> $@
	@echo #define BLD_HOURS $(BLD_HOURS) >> $@
	@echo #define BLD_HOURS_STR "$(BLD_HOURS)" >> $@
	@echo #define BLD_MINUTES $(BLD_MINUTES) >> $@
	@echo #define BLD_MINUTES_STR "$(BLD_MINUTES)" >> $@
	@echo #define BLD_SECONDS $(BLD_SECONDS) >> $@
	@echo #define BLD_SECONDS_STR "$(BLD_SECONDS)" >> $@
	@echo #define BLD_MACHINE_STR "$(BLD_MACHINE)" >> $@
	@echo #define BLD_LANGUAGE_STR "$(BLD_LANGUAGE)" >> $@
	@echo #define BLD_VERSION_STR "v$(VERSION_MAJOR).$(VERSION_MIDDLE).$(VERSION_MINOR)" >> $@
	@echo #define BLD_INFO_STR "testbuild-$(BLD_YEAR)$(BLD_MONTH)$(BLD_DAY)" >> $@
	@echo #define DAEMON_DESCRIPTION_STR "$(DAEMON_DESCRIPTION)" >> $@
	@echo #define WIDGET_DESCRIPTION_STR "$(WIDGET_DESCRIPTION)" >> $@
	@type nul > $*.rexx
	@echo /* $*.rexx -- Build Info generated from $? */ >> $*.rexx
	@echo return "testbuild-$(BLD_YEAR)$(BLD_MONTH)$(BLD_DAY)" >> $*.rexx

#
# -------------------------------------------------- [ DUMP BUILD ENVIRONMENT ]
#
dump-bldenv: Bldvars.h
	@echo $(ARROW) [ Dumping Build Environment ]
	@if exist Makefile.bldenv del Makefile.bldenv
	@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! >> Makefile.bldenv
	@echo !! This file is generated by the Make utility !! >> Makefile.bldenv
	@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! >> Makefile.bldenv
#	@echo. | date >> Makefile.bldenv
#	@echo. | sed "s/.*/\n/"|date|sed "/Enter/d;s/Current date is/Generated at/" >> Makefile.bldenv
	@if exist Makefile.bldenv.err del Makefile.bldenv.err
	@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! >> Makefile.bldenv.err
	@echo !! This file is generated by the Make utility !! >> Makefile.bldenv.err
	@echo !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! >> Makefile.bldenv.err
#	@echo. | date >> Makefile.bldenv.err
	$(MAKE) -nologo -p dummy 1>> Makefile.bldenv 2>> Makefile.bldenv.err
	@echo.

#
# Used by dump-bldenv to dump the Make environment
#
dummy:

#
# ----------------------------------------------- [ SUPPORT DAEMON EXECUTABLE ]
#
support-daemon: Bldvars.h
	@echo $(ARROW) [ Building the Support Daemon ]
	@$(MAKE) -nologo usbshlpr.exe
	@echo.

#
# -------------------------------------------------------------- [ WIDGET DLL ]
#
usb-widget: Bldvars.h
	@echo $(ARROW) [ Building the NeoWPS * USB Widget ]
	@$(MAKE) -nologo usbshold.dll
	@echo.

#
# ----------------------------------------- [ GENERATE MODULE DEFINITION FILE ]
#
usbshlpr.def: Bldvars.h
	@echo.
	@echo $(ARROW) [ Generating $@ ]
	@type nul> $@
	@echo ;##############################################################################>> $@
	@echo ;# usbshlpr.def -- Module Definition generated from Bldvars.mif               #>> $@
	@echo ;# -------------------------------------------------------------------------- #>> $@
	@echo ;# This is the Module Definition file for the USBSHLPR.EXE DAEMON.            #>> $@
	@echo ;# The DAEMON provides named shared memory to the Widget.                     #>> $@
	@echo ;# It's .EXE extension is renamed to .DMN to prevent simple user execution.   #>> $@
	@echo ;##############################################################################>> $@
	@echo.>> $@
	@echo NAME $* WINDOWCOMPAT>> $@
	@echo Description '$(DAEMON_DESCRIPTION) [testbuild-$(BLD_YEAR)$(BLD_MONTH)$(BLD_DAY)]'>> $@
	@echo PROTMODE>> $@
	@echo.>> $@
	@echo DATA    MULTIPLE READWRITE LOADONCALL NONSHARED>> $@
	@echo CODE    LOADONCALL>> $@


#
# This is the DAEMON target.
# It supplies the named shared memory for the widget.
#
usbshlpr.exe: $*.obj Debug.obj AllocMem.obj $*.def
	@echo.
	@echo $(ARROW) [ Linking modules ]
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

usbshlpr.obj: usbshlpr.cpp usbshlpr.hpp ModuleGlobals.hpp Master.hpp Debug.hpp AllocMem.hpp usbshlpr.lvl Bldvars.h
	$(CC) $(CCFLAGS) -Ge+ -Fo$*.obj $*.cpp

#
# ----------------------------------------- [ GENERATE MODULE DEFINITION FILE ]
#
usbshold.def: Bldvars.h
	@echo.
	@echo $(ARROW) [ Generating $@ ]
	@type nul> $@
	@echo ;##############################################################################>> $@
	@echo ;# usbshold.def -- Module Definition generated from Bldvars.mif               #>> $@
	@echo ;# -------------------------------------------------------------------------- #>> $@
	@echo ;# This is the Module Definition file for the USBSHOLD.DLL target.            #>> $@
	@echo ;# It declares the entrypoints called by eCenter.                             #>> $@
	@echo ;# Note that this module is loaded, and these entrypoints called, even when   #>> $@
	@echo ;# the Widget is not active in the eCenter. Adding and removing the Widget    #>> $@
	@echo ;# causes Widget Window creation and destruction, not DLL loading / unloading.#>> $@
	@echo ;##############################################################################>> $@
	@echo.>> $@
	@echo LIBRARY $* INITGLOBAL TERMGLOBAL>> $@
	@echo Description '$(WIDGET_DESCRIPTION) [testbuild-$(BLD_YEAR)$(BLD_MONTH)$(BLD_DAY)]'>> $@
	@echo PROTMODE>> $@
	@echo.>> $@
	@echo DATA    MULTIPLE READWRITE LOADONCALL NONSHARED>> $@
	@echo ;       Doesn't really matter, because the widget is only ever used>> $@
	@echo ;       in the WPS process; we use MULTIPLE NONSHARED here to reduce>> $@
	@echo ;       the amount of shared memory that is needed for the DLL.>> $@
	@echo.>> $@
	@echo CODE    LOADONCALL>> $@
	@echo.>> $@
	@echo EXPORTS>> $@
	@echo ; ALIAS             =   REALNAME                ORDINAL>> $@
	@echo ; -----                 --------                ------->> $@
	@echo.>> $@
	@echo ; These three are are invoked by the eCenter.>> $@
	@echo ; They actually forward to the functions that do the actual work, so to>> $@
	@echo ; activate a different code-path, the forwarding can be changed.>> $@
	@echo.>> $@
	@echo                         UsbWgtInitModule        @1>> $@
	@echo                         UsbWgtUnInitModule      @2>> $@
	@echo                         UsbWgtQueryVersion      @3>> $@
	@echo.>> $@
	@echo ; This is some testing with aliases>> $@
	@echo ;                        DummyExport             @20>> $@
	@echo ;Alias_01            =   WgtInitModule           @1000>> $@
	@echo ;Alias_02            =   WgtUnInitModule         @2000>> $@
	@echo ;Alias_03            =   WgtQueryVersion         @65533>> $@

#
# This is the WIDGET target.
# It is gets loaded by eCenter.
#
usbshold.dll: $(OBJECTS) $(LIBRARIES) $*.res $*.def
	@echo.
	@echo $(ARROW) [ Linking modules ]
	$(LNK) $(LNKFLAGS) -dll -out:$@ $*.def $(OBJECTS) $(LIBRARIES) $(LIBS)
	@mapsym $*.map > nul
	@echo.
	@echo $(ARROW) [ Embedding Resources ]
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

usbshold.obj: usbshold.cpp usbshold.hpp $(DEPS) usbshold.lvl Bldvars.h
	$(CC) $(CCFLAGS) -Ge- -Fo$*.obj $*.cpp
	@echo.

usbshold.res: $(RESDEPS)
	@echo $(ARROW) [ Compiling Resources ]
!if "$(BLDENV)"=="vac3"
	@set INCLUDE=$(RCINCLUDE)
!endif
	$(RC) -r Dialogs.rc
	$(RC) -r Notebooks.rc
	$(RC) -r $*.rc $@

#
# The Widget code interfacing with eCenter
#
#USBWidget.obj: USBWidget.c USBWidget.h USBWidget.ih $(DEPS)
#	$(CC) $(CCFLAGS) -Fo$*.obj $*.c

USBWidget.obj: USBWidget.cpp USBWidget.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ USBWidget.cpp

#
# USB SubSystem Interface
#
USBHelpers.lib: USBHelpers.obj
	@echo $(ARROW) [ USBHelpers.lib ]
!if "$(BLDENV)"=="vac3"
	$(LIBTOOL) $(LIBFLAGS) $*.lib -+$*.obj; 2>nul
!else
!if "$(BLDENV)"=="icc36"
	$(LIBTOOL) $(LIBFLAGS) -out:$*.lib $*.obj
!else
!error **** INVALID BUILD ENVIRONMENT ****
!endif
!endif
	@echo.

USBHelpers.obj: USBHelpers.cpp USBHelpers.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ USBHelpers.cpp

#
# LVM SubSystem Interface
#
LVMHelpers.lib: LVMHelpers.obj
	@echo $(ARROW) [ LVMHelpers.lib ]
!if "$(BLDENV)"=="vac3"
	$(LIBTOOL) $(LIBFLAGS) $*.lib -+$*.obj; 2>nul
!else
!if "$(BLDENV)"=="icc36"
	$(LIBTOOL) $(LIBFLAGS) -out:$*.lib $*.obj
!else
!error **** INVALID BUILD ENVIRONMENT ****
!endif
!endif
	@echo.

LVMHelpers.obj: LVMHelpers.cpp LVMHelpers.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ LVMHelpers.cpp

#
# API SubSystem Interface
#
APIHelpers.lib: APIHelpers.obj
	@echo $(ARROW) [ APIHelpers.lib ]
!if "$(BLDENV)"=="vac3"
	$(LIBTOOL) $(LIBFLAGS) $*.lib -+$*.obj; 2>nul
!else
!if "$(BLDENV)"=="icc36"
	$(LIBTOOL) $(LIBFLAGS) -out:$*.lib $*.obj
!else
!error **** INVALID BUILD ENVIRONMENT ****
!endif
!endif
	@echo.

APIHelpers.obj: APIHelpers.cpp APIHelpers.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ APIHelpers.cpp

#
# GUI SubSystem Interface
#
GUIHelpers.lib: GUIHelpers.obj
	@echo $(ARROW) [ GUIHelpers.lib ]
!if "$(BLDENV)"=="vac3"
	$(LIBTOOL) $(LIBFLAGS) $*.lib -+$*.obj; 2>nul
!else
!if "$(BLDENV)"=="icc36"
	$(LIBTOOL) $(LIBFLAGS) -out:$*.lib $*.obj
!else
!error **** INVALID BUILD ENVIRONMENT ****
!endif
!endif
	@echo.

GUIHelpers.obj: GUIHelpers.cpp GUIHelpers.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ GUIHelpers.cpp

#
# Module with globals
#
ModuleGlobals.obj: ModuleGlobals.cpp ModuleGlobals.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

#
# Module containing debug functions
#
Debug.obj: $*.cpp $*.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

#
# Module containing memory allocation overrides
#
AllocMem.obj: $*.cpp $*.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

#
# Module containing stuff to fix LVM hostile disk lauouts
#
ProblemFixer.obj: $*.cpp $*.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

#
# Module containing the new Widget Setting NoteBook
#
WidgetSettings.obj: $*.cpp $*.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

#
# Module containing memory allocation overrides
#
Threads.obj: $*.cpp $*.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

#
# List to store attached usb devices
#
CList.obj: CList.cpp CList.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

#
# Module with testing-stuff
#
Testing123.obj: Testing123.cpp Testing123.hpp $(DEPS)
	$(CC) $(CCFLAGS) -Fo$@ $*.cpp

#
# Distribute targets to system
#
dist:
	@echo.
	@echo ## Distributing Widget and Daemon to System ##
	@call install-usb-widget.cmd

#
# Cleanup
#
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
