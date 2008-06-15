/* Make XWorkplace Widget Library */
"@echo off"

/*
    This will call nmake to (re)build XWP Widgets Library completely.

    Note that there are some directory checks below which might
    not apply to your system, because I am also using this script
    to update my "private" XWorkplace parts which are not included
    in the source distribution. This applies mostly to all the
    different NLS sources.

    However, this script is smart enough though to check for the
    existence of those directories.

    Please check the below variables and adjust them to your
    system before using this file.

    Also check the "setup.in" file, which has a few more options.
*/

/*
    Change the following if you need some cmd file
    which sets compiler environent variables; otherwise
    comment this out. This is only because I don't like
    all the VAC++ settings in CONFIG.SYS.

    If you have a standard VAC and toolkit installation
    with all the variables set up in CONFIG.SYS, you
    can comment these lines out.
*/

"call envicc.cmd"
"call envproject.cmd"

/*  Set other required environment variables for the built
    process. YOU MUST SET THESE, or building will fail.

    See PROGREF.INF for details. */

/* CVS_WORK_ROOT must point to the root of your CVS tree. */
/* SET CVS_WORK_ROOT= */

/* XWPRUNNING (current XFolder/XWorkplace installation
   from where WPS classes are registered; the executables
   in there will be unlocked by the makefiles) */
/* SET XWPRUNNING= */

/* *** go! */

/* reset timer */
call time("E")
mydir = directory();

Say "***********************************************"
Say "*  Making XWP Widget Library     (./MAIN/)... *"
Say "***********************************************"
"nmake -nologo all"

Say "***********************************************"
Say "*  Making NLS files...                        *"
Say "***********************************************"
"cd 001"
"nmake -nologo all"
"cd .."

/* show elapsed time */
seconds = time("e"); /* in seconds */
minutes = trunc(seconds/60);
seconds2 = trunc(seconds - (minutes*60));
Say;
Say "Done!"
Say "Elapsed time: "minutes" minutes, "seconds2" seconds."
"pause"


