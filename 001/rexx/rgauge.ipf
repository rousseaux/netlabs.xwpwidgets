.* EBOOKIE (IPFTAGS.DEF)
.* Copyright (c) 2001 Martin Lafaix. All Rights Reserved.
:userdoc.
:h1 res=1001.XCenter: REXX Gauge Widget 
:lm margin=1.
:p.The XCenter :hp2.REXX gauge:ehp2. widget displays a gauge whose
appearance is defined by a REXX script.  The script is run at 
regular intervals, and the gauge is updated accordingly.
.*
:p.:artwork runin name='gauge000.bmp'. :artwork runin
name='gauge002.bmp'.

:p.A gauge can contain up to four segments, and may also display
text.  The tooltip can be modified by the script to give 
additional information.

:p.On some REXX gauges, you can double click with mouse button 1.
When such an action occurs, a script is run too.

:p.There can be as many REXX gauge widgets as you wish in an
XCenter.

:p.To configure a REXX gauge widget, right click inside the widget
and select :hp2.Properties:ehp2..  A dialog will pop up that allows you
to define the script to run, the widget's title, and so on.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10001.More on REXX gauge scripts:elink.
:li.:link reftype=hd res=10002.More on double click action 
scripts:elink.                          
:eul.

:h1 res=1000.XCenter: REXX Gauge Widget Settings
:lm margin=1.
:p.In this dialog you define the properties of the REXX gauge
widget.

:p.The :hp2.Default title:ehp2. field is what is displayed in the
tooltip that appears when the mouse pointer remains over the widget for
a few seconds, if it is not redefined by the gauge script.  You can
specify anything you want.  The length of the title cannot exceed 250
characters though.

:p.The :hp2.Gauge script:ehp2. field is the actual REXX program 
that will be run at regular intervals.  If no script is defined, 
nothing happens.

:p.The :hp2.Double click action:ehp2. field is the actual REXX 
program that will run whenever you double click on the widget.  The
script is run in the XCenter thread.

:p.The :hp2.Refresh rate:ehp2. defines how often the gauge script should
be run.  The value is in milliseconds, and cannot be lower than 500ms.

:p.The :hp2.Gauge colors:ehp2. area defines the colors to use for the 
gauge.  To change a color, drop a color over the corresponding area.

:p.The :hp2.Gauge width:ehp2. area defines the width of the gauge. 
The gauge is either of a fixed width or resizeable, interactively.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10001.More on REXX gauge scripts:elink.
:li.:link reftype=hd res=10002.More on double click action 
scripts:elink.                          
:eul.

:h1 res=10001.More on REXX gauge scripts
:lm margin=1.
:p.A script can be any valid REXX script, but its length is limited
to 5000 characters.  This is due to the fact that REXX gauges are
intended for small scripts and that the script remains in memory as
long as the XCenter is open.
     
:p.When the script is run, one argument is provided:
:dl compact.                      
:dt.Arg(1)
:dd.contains the window handle of the widget (in hex).
:edl.
.*
:p.The script is expected to set the gauge state through the :hp2.GAUGE:ehp2.
stem variable.  The state is defined by three values, a text string, and
possibly a tooltip.  The three values must be integers in the 0 to 100 range,
inclusive.  They are stored in :hp2.GAUGE.1:ehp2., :hp2.GAUGE.2:ehp2.,
and :hp2.GAUGE.3:ehp2..  The text and tooltip are stored in
:hp2.GAUGE.TEXT:ehp2. and :hp2.GAUGE.TOOLTIP:ehp2. respectively.

:p.The left side of the gauge corresponds to 0 and the right side to 
100.  From 0 to value 1, the gauge is painted using color 1; from 
value 1 to value 2, the gauge is painted using color 2; and from value
2 to value 3, the gauge is painted using color 3.  The rest of the 
gauge, if applicable, is painted using background color.

:p.If a value is null, or lower than the preceding values, it is 
ignored.

:p.The text is painted using text color, and is centered over the 
gauge.  If no text is defined, no text is painted over the gauge.

:p.The tooltip is painted the usual way for tooltips.  If no tooltip 
is defined by the script, the default tooltip (if any) is used.

:p.Here are two examples of scripts:

:p.This first script is a disk free space monitor.  The color changes 
at 75% and 90% thresholds:
:p.:artwork runin name='gauge000.bmp'. :artwork runin 
name='gauge003.bmp'.

:xmp.
/* Disk monitor */
  
if RxFuncQuery('SysLoadFuncs') then do
     call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
     call SysLoadFuncs
     end

parse value SysDriveInfo('E&colon.') with dsk free total label

t = 100 * (total - free) % total

select
  when t < 75 then
    gauge.1 = t
  when t < 90 then do
    gauge.1 = 74
    gauge.2 = t
  end
otherwise
  gauge.1 = 74
  gauge.2 = 89
  gauge.3 = t
end

/* Something like "E: (SCSI1) 43%" */
gauge.text = dsk '('strip(label)')' t'%'
:exmp.
.*
:p.The script starts with a required comment, then loads the Sys*
functions if they are not already loaded, queries the disk 
settings, and finally sets the gauge.

:p.The second script is a simple clock.  The date is returned for the
tooltip, and the color of the background changes to reflect the
time of day:
:p.:artwork runin name='gauge001.bmp'. :artwork runin
name='gauge002.bmp'.

:xmp.
/* clock */

gauge.tooltip = date()
gauge.text = time()

/*
 * Hour  0         1         2
 *       012345678901234567890123
 * Color 333333322111111112233333
 *
 */
select                             
  when time('H') < 7 &splitvbar. time('H') > 19 then
    gauge.3 = 100
  when time('H') < 9 &splitvbar. time('H') > 17 then
    gauge.2 = 100             
otherwise
  gauge.1 = 100
end
:exmp.                        
.*
:p.The first two lines (the ones starting with 'gauge.') are enough to
define a clock widget.  The rest of the script is used to change the 
background's color.

:nt.
Remember that gauge scripts are run at regular intervals.  Be 
sure not to consume too many CPU cycles.  Also, choose the refresh 
interval wisely.  For the two examples above, 5000ms seems just fine 
for the disk monitor, and 1000ms for the clock if you want the seconds
to elapse fluidly.
:p.Finally, you should not rely on the timer script running regularly. 
There is no guarantee that the script is run if the widget is not visible. 
Do not use scripts such as CRON-like ones.
:ent.
  
:h1 res=10002.More on double click action scripts
:lm margin=1.
:p.A double click action script can be any valid REXX script, but its
length is limited to 5000 characters.  This is due to the fact that
REXX double click actions are intended for small scripts and that the
script remains in memory as long as the XCenter is open.

:p.When the script is run, four arguments are provided:
:dl compact.
:dt.Arg(1)
:dd.contains the normalized X position of the mouse
pointer over the widget when the mouse click occurred.  A normalized position
means that it is 0 when the mouse is on the left side of the widget and
100 when on the right side.
:dt.Arg(2)
:dd.
contains the normalized Y position of the mouse
pointer.
0 is the bottom of the widget and 100 its top.
:dt.Arg(3)
:dd.
contains the modifiers in action when the mouse
button was depressed.  This is a number that combines the status of
the :hp2.Shift:ehp2., :hp2.Control:ehp2., and :hp2.Alt:ehp2. keys. 
:hp2.Shift:ehp2. is represented by the number 8, :hp2.Control:ehp2.
by 16 and :hp2.Alt:ehp2. by 32.  These numbers are added if
more that one modifier was depressed.  For example, a value of 24
means that both :hp2.Shift:ehp2. and :hp2.Control:ehp2. were
depressed.
:dt.Arg(4)
:dd.
is the window handle of the widget (in hex).
:edl.
.*
:p.Here are two examples of scripts:

:p.This first script opens the :hp2.Clock:ehp2. settings notebook:

:xmp.
/* Clock settings notebook */

if RxFuncQuery('SysLoadFuncs') then do
     call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
     call SysLoadFuncs
     end

call SysSetObjectData '<WP_CLOCK>', 'OPEN=SETTINGS'
:exmp.
.*
:p.The script starts with a required comment, then loads the Sys*
functions if they are not already loaded, and finally opens the
clock's settings notebook.  (You can find a list of other useful
setup strings in the :hp1.XWorkplace Users Guide:ehp1. in the
Appendices.)

:p.The second script just displays the parameters as received by
the script:

:xmp.
/* sample */
arg x, y, modifiers, hwnd

if RxFuncQuery('SysLoadFuncs') then do
     call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
     call SysLoadFuncs
     end

call RxMessageBox 'x =' x', y =' y', modifiers =' modifiers,
                  ', hwnd=' hwnd
:exmp.
.*
:p.Note that you cannot use the REXX :hp2.SAY:ehp2.
or :hp2.PULL:ehp2.
instructions as the script has no standard input and output
streams that you can see.

:p.Also note that only one instance of a given script may be running
at any time.  More than one script may be running at any given time
though.

:h1 res=1002.Script already running
.*
:p.The script to be run on double click is already running.

:h1 res=1003.Interpreter error
.*
:p.An interpreter error occurred while interpreting the double click script.

:p.To find out what the error code means, consult the :hp1.REXX
Information:ehp1. or :hp1.Object REXX Reference:ehp1. online guides.

:p.To find out where the script failed, check the SIGNAL ON ERROR and 
SIGNAL ON SYNTAX instructions in the aforementioned manuals.

:p.REXX scripts can include error checking routines.  But keep in mind
that they cannot use :hp2.SAY:ehp2. instructions.  To interact with the
user, use for example the RxMessageBox function.

:h1 res=1004.Interpreter error
.*
:p.An interpreter error occurred while interpreting the gauge script.  To
restart the gauge, open its settings notebook, possibly fix the script,
and select OK.

:p.To find out what the error code means, consult the :hp1.REXX
Information:ehp1. or :hp1.Object REXX Reference:ehp1. online guides.

:p.To find out where the script failed, check the SIGNAL ON ERROR and 
SIGNAL ON SYNTAX instructions in the aforementioned manuals.

:p.REXX scripts can include error checking routines.  But keep in mind
that they cannot use :hp2.SAY:ehp2. instructions.  To interact with the
user, use for example the RxMessageBox function.

:euserdoc.
