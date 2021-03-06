#
# makefile:
#       makefile for src/rexx directory.
#       For use with IBM NMAKE, which comes with the IBM compilers,
#       the Developer's Toolkit, and the DDK.
#
#       This compiles the widget source files into the bin\widgets
#       object files directory. Note that we use the VAC subsystem
#       libraries here to reduce DLL sizes. As a result, the
#       widgets must be single-threaded.
#
#       Called from:    main makefile
#
#       Input:		./*.c
#
#       Output:		../bin/*.obj
#
#       Edit "setup.in" to set up the make process.
#

# Say hello to yourself.
!if [@echo +++++ Entering $(MAKEDIR)]
!endif

PROJECT_BASE_DIR = $(CVS_WORK_ROOT)\xwpwidgets

# include setup (compiler options etc.)
!include ..\..\setup.in

# FIXED MACROS
# ------------
#
# You probably need not change the following.
#

# Define the suffixes for files which NMAKE will work on.
# .SUFFIXES is a reserved NMAKE keyword ("pseudotarget") for
# defining file extensions that NMAKE will recognize in inference
# rules.

.SUFFIXES: .c .obj .dll .h .ih .rc .res

# OUTPUTDIR specifies the directory where all the output .OBJ
# files will be created in.
BASE_OUTPUTDIR = $(XWP_OUTPUT_ROOT)
OUTPUTDIR = $(BASE_OUTPUTDIR)\widgets
!if [@echo       OUTPUTDIR is $(OUTPUTDIR)]
!endif

# create output directory
!if [@md $(OUTPUTDIR) 2> NUL]
!endif

# The OBJS macro contains all the .OBJ files which need to be
# created from the files in this directory.
# These will be put into BIN\.
# If you add a new source, add the corresponding .OBJ file here.
OBJS = \
    $(OUTPUTDIR)\w_rbutton.obj \
    $(OUTPUTDIR)\w_rgauge.obj \
    $(OUTPUTDIR)\w_rmenu.obj \
    $(OUTPUTDIR)\w_rmonitor.obj \
    $(OUTPUTDIR)\w_rscrlr.obj \
    $(OUTPUTDIR)\rbutton.res \
    $(OUTPUTDIR)\rgauge.res \
    $(OUTPUTDIR)\rmenu.res \
    $(OUTPUTDIR)\rmonitor.res \
    $(OUTPUTDIR)\rscrlr.res

# The main target:
# If we're called from the main makefile, MAINMAKERUNNING is defined,
# and we'll set $(OBJS) as our targets (which will go on).
# Otherwise, we call the main makefile, which will again call ourselves later.
all:   \
!ifndef MAINMAKERUNNING
    # we're not being called from main makefile: start main makefile
    callmainmake
    @echo ----- Leaving $(MAKEDIR)
!else
    $(OBJS) $(OUTPUTDIR)\rbutton.res $(OUTPUTDIR)\rgauge.res
    @echo ----- Leaving $(MAKEDIR)
!endif

callmainmake:
    @echo $(MAKEDIR)\makefile: Recursing to main makefile.
    @cd ..\..
    @nmake
    @echo $(MAKEDIR)\makefile: Returned from main makefile. Done.

# Now define inference rules for what to do with certain file
# types, based on their file extension.
# The syntax we need here is ".fromext.toext".
# So whenever NMAKE encounters a .toext file, it
# executes what we specify here.
# The ugly {} brackets are some awkward syntax for specifying
# files in other directories.

# Special macros used here: $(@B) is the current target w/out ext.

# -- compile C files to .OBJ files, using the CC macro above.
#    The output will be placed in the directory specified by
#    the OUTPUTDIR variable (set above).

.c.{$(OUTPUTDIR)}.obj:
    @echo $(MAKEDIR)\makefile: Compiling $(@B).c
!ifndef PRECH
    # precompiled headers disabled:
    $(CC_DLL_SUBSYS) -I$(INCLUDE) /Fo$(OUTPUTDIR)\$(@B).obj $(@B).c
!else
    $(CC_DLL_SUBSYS) /fi"$(PRECH)\$(@B).pch" /si"$(PRECH)\$(@B).pch" -I$(INCLUDE) /Fo$(OUTPUTDIR)\$(@B).obj $(@B).c
!endif

# main include path
INC = ..\..\include
# helpers include path
HLPINC = $(HELPERS_BASE)\include\helpers
# XWP include path
XWPINC = $(XWP_BASE)\include

# NLS location
NLS = $(PROJECT_BASE_DIR)\$(XWP_LANG_CODE)\rexx

# Now define dependencies for all our .OBJ files by telling NMAKE
# which .OBJ file depends on what. For each .OBJ we define what files
# will enforce a recompilation when they've been changed.
# This uses the inference rules defined above.

# Note that include\setup.h is specified with _all_ files, so
# you can enforce a complete recompile of XWorkplace if you update
# that file (just save it once).

# Here follow the .OBJs for the SOM/WPS classes. Each class needs to be
# recompiled when its SOM header files change (due to SOM recompilation)
# or when other headers (in MAIN or HELPERS) have changed.

# XWorkplace "library" .OBJ files used by several classes.
# Source code is here in SRC\MAIN.

$(OUTPUTDIR)\w_rbutton.obj: \
    $(XWPINC)\setup.h \
    $(HLPINC)\comctl.h $(HLPINC)\dosh.h $(HLPINC)\gpih.h $(HLPINC)\threads.h \
	$(HLPINC)\prfh.h $(HLPINC)\except.h $(HLPINC)\winh.h $(HLPINC)\xstring.h \
	$(HLPINC)\linklist.h \
    $(XWPINC)\shared\center.h $(XWPINC)\shared\common.h \
    $(INC)\rexx\w_rbutton.h \
    $(@B).c

$(OUTPUTDIR)\w_rgauge.obj: \
    $(XWPINC)\setup.h \
    $(HLPINC)\comctl.h $(HLPINC)\dosh.h $(HLPINC)\gpih.h \
	$(HLPINC)\prfh.h $(HLPINC)\except.h $(HLPINC)\winh.h $(HLPINC)\xstring.h \
	$(HLPINC)\timer.h \
    $(XWPINC)\shared\center.h $(XWPINC)\shared\common.h \
    $(INC)\rexx\w_rgauge.h \
    $(@B).c

$(OUTPUTDIR)\w_rmonitor.obj: \
    $(XWPINC)\setup.h \
    $(HLPINC)\comctl.h $(HLPINC)\dosh.h $(HLPINC)\gpih.h \
	$(HLPINC)\prfh.h $(HLPINC)\except.h $(HLPINC)\winh.h $(HLPINC)\xstring.h \
	$(HLPINC)\timer.h \
    $(XWPINC)\shared\center.h $(XWPINC)\shared\common.h \
    $(INC)\rexx\w_rmonitor.h \
    $(@B).c

$(OUTPUTDIR)\w_rmenu.obj: \
    $(XWPINC)\setup.h \
    $(HLPINC)\comctl.h $(HLPINC)\dosh.h $(HLPINC)\gpih.h \
	$(HLPINC)\prfh.h $(HLPINC)\except.h $(HLPINC)\winh.h $(HLPINC)\xstring.h \
	$(HLPINC)\timer.h \
    $(XWPINC)\shared\center.h $(XWPINC)\shared\common.h \
    $(INC)\rexx\w_rmenu.h \
    $(@B).c

$(OUTPUTDIR)\w_rscrlr.obj: \
    $(XWPINC)\setup.h \
    $(HLPINC)\comctl.h $(HLPINC)\dosh.h $(HLPINC)\gpih.h \
	$(HLPINC)\prfh.h $(HLPINC)\except.h $(HLPINC)\winh.h $(HLPINC)\xstring.h \
	$(HLPINC)\timer.h \
    $(XWPINC)\shared\center.h $(XWPINC)\shared\common.h \
    $(INC)\rexx\w_rscrlr.h \
    $(@B).c

$(OUTPUTDIR)\rbutton.res: \
    $(NLS)\rbutton.rc $(NLS)\rbutton.dlg $(NLS)\default.ico \
    $(INC)\rexx\w_rbutton.h \
    # RCINCLUDE is stupid in that it requires its arg to be in the current dir
    @cd $(NLS)
    $(RC) -r $(NLS)\rbutton.rc $(OUTPUTDIR)\$(@B).res
    @cd $(MAKEDIR)

$(OUTPUTDIR)\rgauge.res: \
    $(NLS)\rgauge.rc $(NLS)\rgauge.dlg \
    $(INC)\rexx\w_rgauge.h \
    # RCINCLUDE is stupid in that it requires its arg to be in the current dir
    @cd $(NLS)
    $(RC) -r $(NLS)\rgauge.rc $(OUTPUTDIR)\$(@B).res
    @cd $(MAKEDIR)

$(OUTPUTDIR)\rmonitor.res: \
    $(NLS)\rmonitor.rc $(NLS)\rmonitor.dlg \
    $(INC)\rexx\w_rmonitor.h \
    # RCINCLUDE is stupid in that it requires its arg to be in the current dir
    @cd $(NLS)
    $(RC) -r $(NLS)\rmonitor.rc $(OUTPUTDIR)\$(@B).res
    @cd $(MAKEDIR)

$(OUTPUTDIR)\rmenu.res: \
    $(NLS)\rmenu.rc $(NLS)\rmenu.dlg \
    $(INC)\rexx\w_rmenu.h \
    # RCINCLUDE is stupid in that it requires its arg to be in the current dir
    @cd $(NLS)
    $(RC) -r $(NLS)\rmenu.rc $(OUTPUTDIR)\$(@B).res
    @cd $(MAKEDIR)

$(OUTPUTDIR)\rscrlr.res: \
    $(NLS)\rscrlr.rc $(NLS)\rscrlr.dlg \
    $(INC)\rexx\w_rscrlr.h \
    # RCINCLUDE is stupid in that it requires its arg to be in the current dir
    @cd $(NLS)
    $(RC) -r $(NLS)\rscrlr.rc $(OUTPUTDIR)\$(@B).res
    @cd $(MAKEDIR)

