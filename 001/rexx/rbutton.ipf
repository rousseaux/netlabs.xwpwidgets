.* EBOOKIE (IPFTAGS.DEF)
.* Copyright (c) 2001 Martin Lafaix. All Rights Reserved.
:userdoc.
:h1 res=1001.XCenter: REXX Button Widget
:lm margin=1.
:p.The XCenter :hp2.REXX button:ehp2. widget displays a button which
represents a REXX script.  If you press that button, the script it
contains will run.

:p.On some REXX buttons, you can also drop files or objects.  When
such an action occurs, the script is run too.

:p.REXX buttons differs from object buttons linking to an external
REXX script in that no separate process is created when the script
is run.

:p.REXX buttons are handy to implement small functions that do not
need to be run in their own process.

:p.There can be as many REXX button widgets as you wish in an
XCenter.

:p.To configure a REXX button widget, right click inside the widget
and select :hp2.Properties:ehp2..  A dialog will pop up that allows
you to define the script to run, the widget's title, as well as its
icon.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10001.More on REXX scripts:elink.
:li.:link reftype=hd res=10002.More on direct manipulation:elink.
:eul.

:h1 res=1000.XCenter: REXX Button Widget Settings
:lm margin=1.
:p.In this dialog you define the properties of the REXX button
widget.

:p.The :hp2.Title:ehp2. field is what is displayed in the tooltip
that appears when the mouse pointer remains over the widget for a few
seconds.  You can specify anything you want.  The length of the title
cannot exceed 250 characters though.

:p.The :hp2.Icon:ehp2. field is the icon that is used to decorate the
widget in the XCenter.  Drop an icon file onto this area to set the
icon.  If no icon is specified, the widget will use a question mark.

:note.If for whatever reason the widget cannot find the icon (say if
you delete the file you specified, or if you move it), it will revert
to the default question mark.  It just records where the icon is, not
its content.

:p.The :hp2.Script:ehp2. field is the actual REXX program that will
run whenever you press the widget.  By default, the script is run in
the XCenter thread.  This is adequate for many scripts, but, as this
locks the message queue, you may want to transfer time-consuming scripts
onto a separate thread.  To do that, just select the :hp2.Run in a
separate thread:ehp2. checkbox.  This is the recommended practice if
you expect your script to take more that 1/10 of a second to execute.

:p.A REXX button can react to object or file drops.  You specify the
kind of direct manipulation your button accepts in the list box.

.* V0.5.2 (2001-06-21) [lafaix] 
:p.The :hp2.OK:ehp2. button saves the changes and closes the dialog. 
The :hp2.Apply:ehp2. button saves the changes but does not close the
dialog.  The :hp2.Reset:ehp2. button restores the values to the last
saved ones.  The :hp2.Cancel:ehp2. button restores the values to what
they were before the dialog was opened, and close the dialog.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10001.More on REXX scripts:elink.
:li.:link reftype=hd res=10002.More on direct manipulation:elink.
:eul.

:h1 res=10001.More on REXX scripts
:p.A script can be any valid REXX script, but its length is limited
to 5000 characters.  This is due to the fact that REXX buttons are
intended for small scripts and that the script remains in memory as
long as the XCenter is open.

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
:p.If the script accepts dropped elements, the :hp2.DRAGITEM:ehp2.
stem variable is defined.  :hp2.DRAGITEM.0:ehp2. contains the number of
dropped elements, and :hp2.DRAGITEM.1:ehp2., :hp2.DRAGITEM.2:ehp2. and so
on contain the fully qualified names of the dropped elements.

.* V0.5.1 (2001-06-07) [lafaix] 
:p.Additionally, the script may access the widget's user data area.
This area contains up to one hundred bytes.  Its use is not
constrained in any way.  The current user data area is stored in
:hp2.BUTTON.USER:ehp2..  The value of this field is preserved between
run of the script (but not between sessions).  It initially contains
an empty string.

.* V0.5.1 (2001-06-07) [lafaix]
:p.Finally, the script may define any of the following three
values&colon.
:dl compact break=all.
:dt.:hp2.BUTTON.BACKGROUND:ehp2.
:dd.is the button's background color.  Its format is of the
'RRGGBB' form (for example, pure green is 00FF00).
:dt.:hp2.BUTTON.ICON:ehp2.
:dd.is the absolute path to the new icon to use.
:dt.:hp2.BUTTON.TOOLTIP:ehp2.   
:dd.is the text to use as the tooltip.
:edl.
.*
:p.Defining a value overrides its definition as specified in the
settings dialog.  If a value is not defined in the script, then
its default value is used.  Changes are not persistent.

:p.Here are three examples of scripts:

:p.This first script opens the "Find" dialog:

:xmp.
/* Find */

if RxFuncQuery('SysLoadFuncs') then do
     call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
     call SysLoadFuncs
     end

call SysSetObjectData '<WP_DESKTOP>', 'MENUITEMSELECTED=8'
:exmp.
.*
:p.The script starts with a required comment, then loads the Sys*
functions if they are not already loaded, and finally opens the Find
dialog.  (You can find a list of other useful menu item IDs in the
:hp1.XWorkplace Users Guide:ehp1. in the Appendices.)

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
or :hp2.PULL:ehp2. instructions as the script has no standard input
and output streams that you can see.  Hence the use of RxMessageBox
to display results.

:p.The third script switches between a primary keyboard layout and a
secondary layout.  The button's icon is updated to reflect the
currently active layout.

:xmp.
/* keyboard layout switcher */

primary = 'fr'
secondary = 'ux'
iconspath = 'e&colon.&bsl.local&bsl.icons'

if button.user = '' &splitvbar. button.user = primary then
  target = secondary
else
  target = primary

'keyb' target

button.icon = iconspath'&bsl.'target'.ico'
button.user = target
:exmp.
.*
:p.Here you see an example of the BUTTON.USER usage.  It is used to 
store the currently selected layout.

:p.Note that only one instance of a given script may be running at any
time.  More than one script may be running at any given time though.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10002.More on direct manipulation:elink.
:eul.
                                         
:h1 res=10002.More on direct manipulation
:p.A REXX button handles direct manipulation in one of the following
ways:

:ul compact.
:li.It refuses dropped elements
:li.It accepts at most one dropped element, which must be a file
:li.It accepts any number of dropped elements, each of them being a
file
:li.It accepts at most one dropped element, which must be an
object
:li.It accepts any number of dropped elements, each of them being an
object
:eul.
.*
:p.Other forms of direct manipulation (say dynamic data exchange or
printing) are not handled.

:p.A REXX button that accepts dropped element(s) can still be
activated by a simple mouse button 1 click.  In this case, and in this
case only, :hp2.DRAGITEM.0:ehp2. is equal to 0.

:p.The other elements in the :hp2.DRAGITEM:ehp2. stem variable contain the
fully qualified names of the dropped elements. (Shadows are
dereferenced.)

:p.When elements are dropped on a REXX button, the initial four
arguments are passed as usual.  Note that the modifiers can only be a
possibly null combination of :hp2.Shift:ehp2. and :hp2.Control:ehp2..
:hp2.Alt:ehp2. is never present, as it corresponds to no valid direct
manipulation operation for files or objects.

:p.If a REXX button refuses drops, the :hp2.DRAGITEM:ehp2. stem
remains undefined when the script is activated.
                              
:h1 res=1002.Interpreter error
.*
:p.An interpreter error occurred while interpreting the button script.

:p.To find out what the error code means, consult the :hp1.REXX
Information:ehp1. or :hp1.Object REXX Reference:ehp1. online guides.

:p.To find out where the script failed, check the :hp2.SIGNAL ON ERROR:ehp2.
and :hp2.SIGNAL ON SYNTAX:ehp2. instructions in the aforementioned
manuals.

:p.REXX scripts can include error checking routines.  But keep in mind
that they cannot use :hp2.SAY:ehp2. instructions.  To interact with
the user, use for example the RxMessageBox function.

.* V0.5.2 (2001-07-08) [lafaix]
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
  exit
:exmp.

.* V0.5.2 (2001-06-13) [lafaix]
:h1 res=1003.Script already running
.*
:p.An instance of the button script is already running.  This can 
happen if the script is defined to be run in a background thread.

:p.As running more than one instance of a script at any given time is 
not allowed, you may get this message if the previous instance has
not completed yet.  You will have to wait for its completion before 
you can start it again.

.* V0.5.2 (2001-06-13) [lafaix]
:h1 res=1004.Thread creation error
.*
:p.This is an internal XWorkplace error.  It means that you have 
exceeded the number of possible threads.  To check that, open the
:hp2.Scheduler:ehp2. page in the :hp2.OS/2 Kernel:ehp2. object.
If the current thread count is close to the maximum thread count, then
you can try increasing it and restarting your system, or you can
close some applications to release some threads.

:p.If that is not the case, i.e., the maximum thread count is way 
above the current thread count, then you possibly maybe uncovered a
bug in XWorkplace or in the XWorkplace widget library.  Please report 
is with as much details as possible. Thank you.

:euserdoc.
