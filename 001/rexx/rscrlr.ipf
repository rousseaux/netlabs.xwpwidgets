.* EBOOKIE (IPFTAGS.DEF)
.* Copyright (c) 2002 Cristiano Guadagnino. All Rights Reserved.
:userdoc.
:h1 res=1001.XCenter: REXX Scroller Widget
:lm margin=1.
:p.The XCenter :hp2.REXX scroller:ehp2. widget displays a graph whose
appearance is defined by a REXX script.  The script is run at
regular intervals, and the scroller is updated accordingly.

:p.In the following screenshot you can see a TCPI/IP Monitor widget
built upon a Rexx scroller (it is the top left widget).
.*
:p.:artwork runin name='scrlr001.bmp'.

:p.A scroller can contain up to three graphs, and may also display
text.  The tooltip can be modified by the script to give
additional information.

:p.A bakground image can be added to enhance the look and/or readability.
The bitmap is controlled by the script.

:p.On some REXX scrollers, you can double click with mouse button 1.
When such an action occurs, a script is run too.

:p.There can be as many REXX scroller widgets as you wish in an
XCenter.

:p.To configure a REXX scroller widget, right click inside the widget
and select :hp2.Properties:ehp2..  A dialog will pop up that allows you
to define the script to run, the widget's title, and so on.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10001.More on REXX scroller scripts:elink.
:li.:link reftype=hd res=10002.More on double click action
scripts:elink.
:eul.

:h1 res=1000.XCenter: REXX Scroller Widget Settings
:lm margin=1.
:p.In this dialog you define the properties of the REXX scroller
widget.

:p.The :hp2.Default title:ehp2. field is what is displayed in the
tooltip that appears when the mouse pointer remains over the widget for
a few seconds, if it is not redefined by the scroller script.  You can
specify anything you want.  The length of the title cannot exceed 250
characters though.

:p.The :hp2.Scroller script:ehp2. field is the actual REXX program
that will be run at regular intervals.  If no script is defined,
nothing happens.

:p.The :hp2.Double click action:ehp2. field is the actual REXX
program that will run whenever you double click on the widget.  The
script is run in the XCenter thread.

:p.The :hp2.Refresh rate:ehp2. defines how often the scroller script should
be run.  The value is in milliseconds, and cannot be lower than 500ms.

:p.The :hp2.Scroller colors:ehp2. area defines the colors to use for the
graph.  To change a color, drop a color over the corresponding area.

:p.The :hp2.Scroller width:ehp2. area defines the width of the scroller.
The scroller is either of a fixed width or resizeable, interactively.

:p.The :hp2.Scroller graph:ehp2. defines the look of the graphs in the
widget. If :hp2.Line:ehp2. is selected, the graph will look like a thin
line. If :hp2.Filled:ehp2. is selected, it will look like a filled curve.
If :hp2.Averaging:ehp2. is checked, the last two values will be averaged
to give a smoother curve (not yet implemented).

:p.Finally, if :hp2.Baseline Mode:ehp2. is checked, the scroller will be split
vertically in two. You can have graphs extending from the baseline up
(positive values), or from the baseline down (negative values). Note that
if you input negative values in the scroller while Baseline Mode is :hp1.not:ehp1.
enabled, they will be changed to positive. See the screenshot below:
:p.:artwork runin name='scrlr003.bmp'.

:p.The :hp2.OK:ehp2. button saves the changes and closes the dialog.
The :hp2.Apply:ehp2. button saves the changes but does not close the
dialog.  The :hp2.Reset:ehp2. button restores the values to the last
saved ones.  The :hp2.Cancel:ehp2. button restores the values to what
they were before the dialog was opened, and close the dialog.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10001.More on REXX scroller scripts:elink.
:li.:link reftype=hd res=10002.More on double click action
scripts:elink.
:eul.

:h1 res=10001.More on REXX scroller scripts
:lm margin=1.
:p.A script can be any valid REXX script, but its length is limited
to 5000 characters.  This is due to the fact that REXX scrollers are
intended for small scripts and that the script remains in memory as
long as the XCenter is open.

:p.When the script is run, one argument is provided:
:dl compact.
:dt.Arg(1)
:dd.contains the window handle of the widget (in hex).
:edl.
.*
:p.The script is expected to set the scroller state through the :hp2.SCRLR:ehp2.
stem variable.  The state is defined by three values, a text string, and
possibly a tooltip and a bitmap.  The three values must be integers.
They are stored in :hp2.SCRLR.1:ehp2., :hp2.SCRLR.2:ehp2.,
and :hp2.SCRLR.3:ehp2..  The text and tooltip are stored in
:hp2.SCRLR.TEXT:ehp2. and :hp2.SCRLR.TOOLTIP:ehp2. respectively.
The full path to the bitmap is stored in :hp2.SCRLR.BITMAP:ehp2..

:p.The bottom of the scroller area corresponds to 0 and the top to
the current maximum value.  The scroller will dynamically rescale
the graph as the maximum value changes.

:p.The scroller will draw one to three graphs based on the values you supply.
If :hp2.Filled:ehp2. is selected in the scroller properties, the graphs are
drawn on top of each other, with graph 1 being the :hp1.topmost:ehp1., then
graph 2 and then graph 3.  Every time the script is run, the previous values
are scrolled to the left, and the new value is plotted.

:p.If a value is null, it is ignored.

:p.The text is painted using text color, and is centered over the
scroller.  If no text is defined, no text is painted over the scroller.

:p.The tooltip is painted the usual way for tooltips.  If no tooltip
is defined by the script, the default tooltip (if any) is used.

:p.The bitmap is painted as backround, and it is :hp1.not:ehp1. scrolled
with the graph.  If it is smaller/bigger than the widget window, it is
stretched to exactly fit in the window.

:p.Additionally, the script may access the widget's user data area.
This area contains up to one hundred bytes.  Its use is not
constrained in any way.  The current user data area is stored in
:hp2.SCRLR.USER:ehp2..  The value of this field is preserved between
run of the script (but not between sessions).  It initially contains
an empty string.

:p.Note that the user data area is shared between the scroller script and
the double click action script.  You can use it to pass informations
between the two.

:p.Finally, the script may define any of the following five
values&colon.
:dl compact break=all.
:dt.:hp2.SCRLR.BACKGROUND:ehp2.
:dt.:hp2.SCRLR.FOREGROUND:ehp2.
:dt.:hp2.SCRLR.COLOR1:ehp2.
:dt.:hp2.SCRLR.COLOR2:ehp2.
:dt.:hp2.SCRLR.COLOR3:ehp2.
:dd.are the colors of the respective elements.  The format is of the
'RRGGBB' form (for example, pure blue is 0000FF).
:edl.
.*
:p.If a color is not defined in the script run, then its
default value is used.  Changes are not persistent.

:p.Here are two examples of scripts:

:p.This first script is a disk free space monitor.  It uses only one
line (filled) for the graph:
:p.:artwork runin name='scrlr002.bmp'.

:xmp.
/* Disk Free Monitor */

if RxFuncQuery('SysLoadFuncs') then do
     call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
     call SysLoadFuncs
end

parse value SysDriveInfo('E&colon.') with dsk free total label

t = 100 * (total - free) % total

scrlr.1 = t

/* Something like "E: (SCSI1) 43%" */
scrlr.text = dsk '('strip(label)')' t'%'
:exmp.
.*
:p.The script starts with a required comment, then loads the Sys*
functions if they are not already loaded, queries the disk
settings, sets the graph value, and finally sets the tooltip.

:nt.
Remember that scroller scripts are run at regular intervals.  Be
sure not to consume too many CPU cycles.  Also, choose the refresh
interval wisely.  For the example above, 5000ms seems just fine.
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
:p.Additionally, the script may access the widget's user data area.
This area contains up to one hundred bytes.  Its use is not
constrained in any way.  The current user data area is stored in
:hp2.SCRLR.USER:ehp2..  The value of this field is preserved between
run of the script (but not between sessions).  It initially contains
an empty string.

:p.Note that the user data area is shared between the scroller script and
the double click action script.  You can use it to pass informations
between the two.

:p.Here is an example of script:

:p.This script just displays the parameters as received:

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

:p.This situation normally cannot happen.  If it does, it means there
is a bug in the XWorkplace widget library.  Please report it with as
much details as possible.  Thank you.

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

:p.For example, if you want to catch errors in your script, you
can add this statement at the begining of the script (but after the
mandatory initial comment):

:xmp.
signal on syntax
:exmp.
.*
:p.and add the following method at the end of your script:

:xmp.
exit /* just in case so that preceding code does not enter
        the error catching routine */
syntax:
  call RxMessageBox 'Error' RC 'on line' SIGL '&colon.' errortext(RC)
  exit
:exmp.

:h1 res=1004.Interpreter error
.*
:p.An interpreter error occurred while interpreting the scroller script.  To
restart the scroller, open its settings notebook, possibly fix the script,
and select OK.

:p.To find out what the error code means, consult the :hp1.REXX
Information:ehp1. or :hp1.Object REXX Reference:ehp1. online guides.

:p.To find out where the script failed, check the SIGNAL ON ERROR and
SIGNAL ON SYNTAX instructions in the aforementioned manuals.

:p.REXX scripts can include error checking routines.  But keep in mind
that they cannot use :hp2.SAY:ehp2. instructions.  To interact with the
user, use for example the RxMessageBox function.

:p.For example, if you want to catch syntax errors in your script, you
can add this statement at the begining of the script (but after the
mandatory initial comment):

:xmp.
signal on syntax
:exmp.
.*
:p.and add the following method at the end of your script:

:xmp.
exit /* just in case so that preceding code does not enter
        the error catching routine */
syntax:
  call RxMessageBox 'Error' RC 'on line' SIGL '&colon.' errortext(RC)
  / /* an intentional error again */
:exmp.
.*
:p.Note the last line.  It contains an intentional syntax error.  It
is a trick used to stop the scroller script.  If it wasn't there, the
error message box would pop up again and again, each time the script
would be run.  It cause an extra error (error #35, "Invalid
expression") that you can safely ignore.

:euserdoc.
