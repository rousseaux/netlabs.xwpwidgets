
#
# makefile:
#       makefile for NLS directory.
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
#			Use "nmake -a [<targets>] to _re_build the targets,
#			even if they are up to date.
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
!include ..\setup.in

# MODULESDIR is used for helpfiles (HLP) output.
# PROJECT_OUTPUT_DIR has been set by setup.in based on the environment.
MODULESDIR=$(PROJECT_OUTPUT_DIR)\widgets
!if [@echo       MODULESDIR is $(MODULESDIR)]
!endif

# Define the suffixes for files which NMAKE will work on.
# .SUFFIXES is a reserved NMAKE keyword ("pseudotarget") for
# defining file extensions that NMAKE will recognize in inference
# rules.

.SUFFIXES: .ipf .hlp

# some variable strings to pass to sub-nmakes
SUBMAKE_PASS_STRING = "PROJECT_BASE_DIR=$(PROJECT_BASE_DIR)" "PROJECT_INCLUDE=$(PROJECT_INCLUDE)"

# store current directory so we can change back later
CURRENT_DIR = $(MAKEDIR)


# PSEUDOTARGETS
# -------------

all: cpl_main link
    @echo ----- Leaving $(MAKEDIR)

# "really_all" references "all".
really_all: all
    @echo ----- Leaving $(MAKEDIR)

# If you add a subdirectory to SRC\, add a target to
# "cpl_main" also to have automatic recompiles.
cpl_main: help

# COMPILER PSEUDOTARGETS
# ----------------------

help:

# LINKER PSEUDOTARGETS
# --------------------

link: $(XWPRUNNING)\plugins\xcenter\rbutton.hlp \
      $(XWPRUNNING)\plugins\xcenter\rgauge.hlp \
      $(XWPRUNNING)\plugins\xcenter\spacer.hlp \

# Finally, define rules for linking the target DLLs and EXEs
# This uses the $OBJS and $HLPOBJS macros defined at the top.
#
# The actual targets are the DLLs and EXEs in the XWorkplace
# installation directory. We create the target in bin\ first
# and then copy it thereto.

#
# Making RBUTTON.HLP
#
$(XWPRUNNING)\plugins\xcenter\rbutton.hlp: $(MODULESDIR)\$(@B).hlp
!ifdef XWP_UNLOCK_MODULES
	unlock $@
!endif
	cmd.exe /c copy $(MODULESDIR)\$(@B).hlp $(XWPRUNNING)\plugins\xcenter

$(MODULESDIR)\rbutton.hlp: rexx\$(@B).ipf
    @cd rexx
    @ipfc $(@B).ipf $(MODULESDIR)\$(@B).hlp
    @cd ..

#
# Making RGAUGE.HLP
#
$(XWPRUNNING)\plugins\xcenter\rgauge.hlp: $(MODULESDIR)\$(@B).hlp
!ifdef XWP_UNLOCK_MODULES
	unlock $@
!endif
	cmd.exe /c copy $(MODULESDIR)\$(@B).hlp $(XWPRUNNING)\plugins\xcenter

$(MODULESDIR)\rgauge.hlp: rexx\$(@B).ipf
    @cd rexx
    @ipfc $(@B).ipf $(MODULESDIR)\$(@B).hlp
    @cd ..

#
# Making SPACER.HLP
#
$(XWPRUNNING)\plugins\xcenter\spacer.hlp: $(MODULESDIR)\$(@B).hlp
!ifdef XWP_UNLOCK_MODULES
	unlock $@
!endif
	cmd.exe /c copy $(MODULESDIR)\$(@B).hlp $(XWPRUNNING)\plugins\xcenter

$(MODULESDIR)\spacer.hlp: spacer\$(@B).ipf
    @cd spacer
    @ipfc $(@B).ipf $(MODULESDIR)\$(@B).hlp
    @cd ..

