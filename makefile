
#
# makefile:
#       makefile for main directory.
#       For use with IBM NMAKE, which comes with the IBM compilers,
#       the Developer's Toolkit, and the DDK.
#
#       Called from:    nowhere, maybe MAKE.CMD. Main makefile.
#			This recurses into the subdirectories.
#
#       Input:		specify the target(s) to be made, which can be:
#
#			--  "all" (default): build the widgets.
#
#			The following subtargets exist (which get called
#			by the "all" or "really_all" targets):
#
#			--  nls: compile $(XWP_LANG_CODE)\ directory
#			--  cpl_main: compile *.c files for DLLs, no link
#			--  link: link bin\*.obj to DLLs and copy to XWorkplace
#				  install directory
#
#			Use "nmake -a [<targets>] to _re_build the targets,
#			even if they are up to date.
#
#			Other special targets not used by "all" or "really_all"
#			(these must be specified explicitly on the nmake
#			command line):
#
#			--  dlgedit: invoke dialog editor on NLS DLL
#			--  release: create/update release tree in directory
#			    specified by XWPRELEASE; this invokes "really_all"
#			    in turn.
#
#       Environment:    You MUST set a number of environment variables before
#			compiling. See PROGREF.INF.
#
#       Output:		All XWorkplace Files code files. This calls the other
#			makefiles in the subdirectories.
#
#			Output files are first created in bin\ (which is
#			created if it doesn't exist), then copied to
#			$(XWPRUNNING), which must be defined externally or
#			thru "setup.in".
#
#       Edit "setup.in" to set up the make process (compilation flags etc.).
#

# Say hello to yourself.
!if [@echo +++++ Entering $(MAKEDIR)]
!endif

# PROJECT_BASE_DIR is used by "setup.in"
# to identify the root of the source
# tree. This is passed to the sub-makefiles.
!ifndef PROJECT_BASE_DIR
PROJECT_BASE_DIR = $(CVS_WORK_ROOT)\xwpwidgets
!endif

# include setup (compiler options etc.)
!include setup.in

# MODULESDIR is used for mapfiles and final module (DLL, EXE) output.
# PROJECT_OUTPUT_DIR has been set by setup.in based on the environment.
MODULESDIR=$(PROJECT_OUTPUT_DIR)\widgets
!if [@echo       MODULESDIR is $(MODULESDIR)]
!endif

# create output directory
!if [@md $(PROJECT_OUTPUT_DIR) 2> NUL]
!endif
!if [@md $(MODULESDIR) 2> NUL]
!endif

# VARIABLES
# ---------

# The following macros contains the .OBJ files for the XCenter plugins.
RBUTTONOBJS = $(XWP_OUTPUT_ROOT)\widgets\w_rbutton.obj $(PMPRINTF_LIB)
RGAUGEOBJS = $(XWP_OUTPUT_ROOT)\widgets\w_rgauge.obj $(PMPRINTF_LIB)
SPACEROBJS = $(XWP_OUTPUT_ROOT)\widgets\w_spacer.obj $(PMPRINTF_LIB)

# Define the suffixes for files which NMAKE will work on.
# .SUFFIXES is a reserved NMAKE keyword ("pseudotarget") for
# defining file extensions that NMAKE will recognize in inference
# rules.

.SUFFIXES: .obj .dll .exe .h .rc .res

# The LIBS macro contains all the .LIB files, either from the compiler or
# others, which are needed for this project:
#   somtk       is the SOM toolkit lib
#   pmprintf    is for debugging
# The other OS/2 libraries are used by default.
LIBS = somtk.lib $(PMPRINTF_LIB)

# some variable strings to pass to sub-nmakes
SUBMAKE_PASS_STRING = "PROJECT_BASE_DIR=$(PROJECT_BASE_DIR)" "PROJECT_INCLUDE=$(PROJECT_INCLUDE)"

# store current directory so we can change back later
CURRENT_DIR = $(MAKEDIR)


# PSEUDOTARGETS
# -------------

all: cpl_main link
    @echo ----- Leaving $(MAKEDIR)

# "really_all" references "all".
really_all: all nls
    @echo ----- Leaving $(MAKEDIR)

# If you add a subdirectory to SRC\, add a target to
# "cpl_main" also to have automatic recompiles.
cpl_main: widgets

# COMPILER PSEUDOTARGETS
# ----------------------

widgets:
    @echo $(MAKEDIR)\makefile: Going for subdir src\rexx
    @cd src\rexx
    @nmake -nologo all "MAINMAKERUNNING=YES" $(SUBMAKE_PASS_STRING)
    @cd ..\..
    @echo $(MAKEDIR)\makefile: Going for subdir src\spacer
    @cd src\spacer
    @nmake -nologo all "MAINMAKERUNNING=YES" $(SUBMAKE_PASS_STRING)
    @cd ..\..

# LINKER PSEUDOTARGETS
# --------------------

link: $(XWPRUNNING)\plugins\xcenter\rbutton.dll \
      $(XWPRUNNING)\plugins\xcenter\rgauge.dll \
      $(XWPRUNNING)\plugins\xcenter\spacer.dll \

# Finally, define rules for linking the target DLLs and EXEs
# This uses the $OBJS and $HLPOBJS macros defined at the top.
#
# The actual targets are the DLLs and EXEs in the XWorkplace
# installation directory. We create the target in bin\ first
# and then copy it thereto.

#
# Linking RBUTTON.DLL
#
$(XWPRUNNING)\plugins\xcenter\rbutton.dll: $(MODULESDIR)\$(@B).dll
!ifdef XWP_UNLOCK_MODULES
	unlock $@
!endif
	cmd.exe /c copy $(MODULESDIR)\$(@B).dll $(XWPRUNNING)\plugins\xcenter
	cmd.exe /c copy $(MODULESDIR)\$(@B).sym $(XWPRUNNING)\plugins\xcenter

# update DEF file if buildlevel has changed
src\rexx\rbutton.def: include\bldlevel.h
	cmd.exe /c BuildLevel.cmd $@ include\bldlevel.h "Rexx button plugin DLL"

$(MODULESDIR)\rbutton.dll: $(RBUTTONOBJS) src\rexx\$(@B).def $(XWP_OUTPUT_ROOT)\widgets\$(@B).res
	@echo $(MAKEDIR)\makefile: Linking $@
	$(LINK) /OUT:$@ src\rexx\$(@B).def $(RBUTTONOBJS) REXX.LIB
	@cd $(MODULESDIR)
   $(RC) $(@B).res $(@B).dll
	mapsym /n $(@B).map > NUL
	@cd $(CURRENT_DIR)

#
# Linking RGAUGE.DLL
#
$(XWPRUNNING)\plugins\xcenter\rgauge.dll: $(MODULESDIR)\$(@B).dll
!ifdef XWP_UNLOCK_MODULES
	unlock $@
!endif
	cmd.exe /c copy $(MODULESDIR)\$(@B).dll $(XWPRUNNING)\plugins\xcenter
	cmd.exe /c copy $(MODULESDIR)\$(@B).sym $(XWPRUNNING)\plugins\xcenter

# update DEF file if buildlevel has changed
src\rexx\rgauge.def: include\bldlevel.h
	cmd.exe /c BuildLevel.cmd $@ include\bldlevel.h "Rexx gauge plugin DLL"

$(MODULESDIR)\rgauge.dll: $(RGAUGEOBJS) src\rexx\$(@B).def $(XWP_OUTPUT_ROOT)\widgets\$(@B).res
	@echo $(MAKEDIR)\makefile: Linking $@
	$(LINK) /OUT:$@ src\rexx\$(@B).def $(RGAUGEOBJS) REXX.LIB
	@cd $(MODULESDIR)
   $(RC) $(@B).res $(@B).dll
	mapsym /n $(@B).map > NUL
	@cd $(CURRENT_DIR)

#
# Linking SPACER.DLL
#
$(XWPRUNNING)\plugins\xcenter\spacer.dll: $(MODULESDIR)\$(@B).dll
!ifdef XWP_UNLOCK_MODULES
	unlock $@
!endif
	cmd.exe /c copy $(MODULESDIR)\$(@B).dll $(XWPRUNNING)\plugins\xcenter
	cmd.exe /c copy $(MODULESDIR)\$(@B).sym $(XWPRUNNING)\plugins\xcenter

# update DEF file if buildlevel has changed
src\spacer\spacer.def: include\bldlevel.h
	cmd.exe /c BuildLevel.cmd $@ include\bldlevel.h "Spacer plugin DLL"

$(MODULESDIR)\spacer.dll: $(SPACEROBJS) src\spacer\$(@B).def $(XWP_OUTPUT_ROOT)\widgets\$(@B).res
	@echo $(MAKEDIR)\makefile: Linking $@
	$(LINK) /OUT:$@ src\spacer\$(@B).def $(SPACEROBJS)
	@cd $(MODULESDIR)
   $(RC) $(@B).res $(@B).dll
	mapsym /n $(@B).map > NUL
	@cd $(CURRENT_DIR)
