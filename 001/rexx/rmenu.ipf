.* EBOOKIE (IPFTAGS.DEF)
.* Copyright (c) 2001 Martin Lafaix. All Rights Reserved.
:userdoc.
:h1 res=1001.XCenter: REXX Menu Widget
:lm margin=1.
:p.The XCenter :hp2.REXX menu:ehp2. widget displays a button which
leads to a menu.  If you press that button, a menu will opens.

:p.A menu can contain as many items and subitems as needed.
The tooltip can be modified by the script to give
additional information.

:p.There can be as many REXX menu widgets as you wish in an
XCenter.

:p.To configure a REXX menu widget, right click inside the widget
and select :hp2.Properties:ehp2..  A dialog will pop up that allows you
to define the script to run, the widget's title, and so on.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10001.More on REXX menu scripts:elink.
:li.:link reftype=hd res=10002.More on REXX command scripts:elink.
:eul.

:h1 res=1000.XCenter: REXX Menu Widget Settings
:lm margin=1.
:p.In this dialog you define the properties of the REXX menu
widget.

:p.The :hp2.Title:ehp2. field is what is displayed in the
tooltip that appears when the mouse pointer remains over the widget for
a few seconds, if it is not redefined by the script.  You can
specify anything you want.  The length of the title cannot exceed 250
characters though.

:p.The :hp2.Icon:ehp2. field is the icon that is used to decorate the
widget in the XCenter.  Drop an icon file onto this area to set the
icon.  If no icon is specified, the widget will use a question mark.

:p.The :hp2.Menu:ehp2. field is the actual REXX program
that will build the menu.  If no script is defined, nothing happens
on left clicks.

:p.The :hp2.Script:ehp2. field is the actual REXX program that will
run whenever you select an item in the opened menu.  By default, the
script is run in the XCenter thread.  This is adequate for many
scripts, but, as this locks the message queue, you may want to
transfer time-consuming scripts onto a separate thread.  To do that,
just select the :hp2.Run in a separate thread:ehp2.  checkbox.  This
is the recommended practice if you expect your script to take more
that 1/10 of a second to execute.  &lbrk.This script is referred to as
the command script in this document to prevent confusion.&rbrk.

.* V0.5.2 (2001-06-21) [lafaix]
:p.The :hp2.OK:ehp2. button saves the changes and closes the dialog.
The :hp2.Apply:ehp2. button saves the changes but does not close the
dialog.  The :hp2.Reset:ehp2. button restores the values to the last
saved ones.  The :hp2.Cancel:ehp2. button restores the values to what
they were before the dialog was opened, and close the dialog.

:p.See also:
:ul compact.
:li.:link reftype=hd res=10001.More on REXX menu scripts:elink.
:li.:link reftype=hd res=10002.More on REXX command scripts:elink.
:eul.

:h1 res=10001.More on REXX menu scripts
:lm margin=1.
:p.A menu script can be any valid REXX script, but its length is
limited to 5000 characters.  This is due to the fact that REXX menus
are intended for small scripts and that the script remains in memory
as long as the XCenter is open.

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
:p.The menu script is expected to set the menu appearance
through the :hp2.MENU:ehp2. stem variable.

:p.The number of toplevel menu items is set in :hp2.MENU.0:ehp2..  The
first menu item is defined in :hp2.MENU.1:ehp2., the second
in :hp2.MENU.2:ehp2., and so on.

:p.Each menu item is defined by a string that follows this format:

:xmp.
  &lbrk.modifier&rbrk.command=label
or
  -
:exmp.
:p.where :hp2.modifier:ehp2. is optional and is one of '&xclm.', '?',
or '*'.  :hp2.command:ehp2. is any text string containing no '='
character.  :hp2.label:ehp2. is any text string.

:p.The optional :hp2.modifier:ehp2. part specifies whether the menu
item is normal (in which case the modifier is empty), disabled (in
which case the modifier is '*'), checked (in which case the modifier
is '&xclm.'), or checkable (in which case the modifier is '?').

:p.The mandatory :hp2.command:ehp2. part is what is passed to the
second script when the item is selected.  It usually consists of just
one word, but it may be any string, as long as it contains no
'=' character and starts with a letter.

:p.If :hp2.command :ehp2. starts with an '&atsign.', then is is
considered a submenu.  In this case, the rest of :hp2.command:ehp2. is
expected to be a stem name defining the submenu items.

:p.The mandatory :hp2.label:ehp2. part is what is displayed in the
menu.  It can contains any character, including special ones as
defined for Presentation Manager (&tilde. and '09'x, mostly).

:p.If the menu item is defined as '-', then a menu item separator
is included in the menu.  Such items cannot be selected, and hence
have no commands associated.  Similarly, as they are displayed as a
beveled line, no label is associated either.

:p.A menu can contains any number of submenus, and submenus can also
contain any number of items and submenus.

:p.Here are some examples of menu item definitions:

:xmp.
  menu.1 = 'fr=&tilde.France'
  menu.2 = 'us=&tilde.United states'
  menu.3 = '-'
  menu.4 = '&atsign.menu.4=Other'
  menu.0 = 4

  menu.4.1 = '*misc1=A Disabled item'
  menu.4.2 = '&xclm.misc2=A Checked item'
  menu.4.3 = '?misc3=A Checkable item'
  menu.4.4 = 'misc4=An item with an accelerator' || '09'x || 'F2'
:exmp.

.* V0.5.1 (2001-06-07) [lafaix]
:p.Additionally, the script may access the widget's user data area.
This area contains up to one hundred bytes.  Its use is not
constrained in any way.  The current user data area is stored in
:hp2.MENU.USER:ehp2..  The value of this field is preserved between
run of the script (but not between sessions).  It initially contains
an empty string.

:p.Note that the user data area is shared between the menu script and
the command script.  You can use it to pass information
between the two.

:p.Here are two examples of menu scripts:

:xmp.
/* An XButton-like menu */

menu.1 = '&atsign.dummy=Desktop'
menu.2 = '-'
menu.3 = 'run=&tilde.Run...'
menu.4 = '-'
menu.5 = 'lockup=&tilde.Lockup now'
menu.6 = 'restart=&tilde.Restart WPS'
menu.7 = 'shutdown=& Shut down'
menu.0 = 7

dummy.1 = '*foo=Not implemented yet &colon.)'
dummy.0 = 1
:exmp.
.*
:p.This menu script simply builds a menu that mimics the XButton one.
Adding folder content submenus is not possible yet, so a submenu with
a disabled entry is defined instead.

:xmp.
/* A silly dynamic menu */

hour = time('h')

if hour >= 9 and hour <= 17 then do
  menu.1 = 'x=Drink caffe'
  menu.2 = 'y=Check mail'
  menu.3 = 'x=Drink caffe'
  menu.4 = '&atsign.a=If boss is there'
  menu.5 = '&atsign.b=If boss is out'
  menu.0 = 5

  a.1 = 'a1=Look busy'
  a.2 = 'a2=Look even more busier'
  a.0 = 2

  b.1 = 'b1=Read news'
  b.2 = 'b2=Browse the web'
  b.3 = 'b3=Plan for the next weekend'
  b.4 = 'b4=Work'
  b.0 = 4
end
else do
  menu.1 = 'u=Do shopping'
  menu.2 = 'v=Watch TV'
  menu.3 = 'w=<censored>'
  menu.0 = 3
end
:exmp.
:p.This menu is a fictious example of a dynamic menu.  Depending on
the current time, the displayed menu differs.

:h1 res=10002.More on REXX command scripts
:lm margin=1.
:p.A command script can be any valid REXX script, but its
length is limited to 5000 characters.  This is due to the fact that
REXX double click actions are intended for small scripts and that the
script remains in memory as long as the XCenter is open.

:p.The command script is run whenever an item is selected in the
menu.  It is not run if the menu is closed without selecting any
items in it.

:p.When the script is run, three arguments are provided:
:dl compact.
:dt.Arg(1)
:dd.contains the selected command
:dt.Arg(2)
:dd.
contains the modifiers in action when the mouse
button was depressed.  This is a number that combines the status of
the :hp2.Shift:ehp2., :hp2.Control:ehp2., and :hp2.Alt:ehp2. keys.
:hp2.Shift:ehp2. is represented by the number 8, :hp2.Control:ehp2.
by 16 and :hp2.Alt:ehp2. by 32.  These numbers are added if
more that one modifier was depressed.  For example, a value of 24
means that both :hp2.Shift:ehp2. and :hp2.Control:ehp2. were
depressed.
:dt.Arg(3)
:dd.
is the window handle of the widget (in hex).
:edl.
.*
:p.Additionally, the script may access the widget's user data area.
This area contains up to one hundred bytes.  Its use is not
constrained in any way.  The current user data area is stored in
:hp2.MENU.USER:ehp2..  The value of this field is preserved between
run of the script (but not between sessions).  It initially contains
an empty string.

:p.Note that the user data area is shared between the menu script
and the command script.  You can use it to pass information between
the two.

:p.Finally, the script may define any of the following three
values&colon.
:dl compact break=all.
:dt.:hp2.MENU.BACKGROUND:ehp2.
:dd.is the button's background color.  Its format is of the
'RRGGBB' form (for example, pure green is 00FF00).
:dt.:hp2.MENU.ICON:ehp2.
:dd.is the absolute path to the new icon to use.
:dt.:hp2.MENU.TOOLTIP:ehp2.
:dd.is the text to use as the tooltip.
:edl.
.*
:p.Defining a value overrides its definition as specified in the
settings dialog.  If a value is not defined in the script, then
its default value is used.  Changes are not persistent.

:p.Here are two examples of command scripts:

:xmp.
/* An XButton-like command script */

if RxFuncQuery('SysLoadFuncs') then do
     call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
     call SysLoadFuncs
     end

select
  when arg(1) = 'run' then
    call SysSetObjectData '<WP&us.DESKTOP>', 'SHOWRUNDLG=DEFAULT'
  when arg(2) = 'restart' then
    'resetwps' /* from WPTOOLS */
  when arg(3) = 'shutdown' then
    call SysSetObjectData '<WP&us.DESKTOP>', 'MENUITEMSELECTED=704'
    /* or 'XSHUTDOWNNOW=DEFAULT' or something if you prefer to use
     * XWorkplace's shutdown features
     */
otherwise
  nop
end
:exmp.
.*
:p.This command script matches the first menu script given in the
menu script section.

:p.The second script just displays the parameters as received by
the script:

:xmp.
/* sample */
arg command, modifiers, hwnd

if RxFuncQuery('SysLoadFuncs') then do
     call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
     call SysLoadFuncs
     end

call RxMessageBox 'command =' command', modifiers =' modifiers,
                  ', hwnd=' hwnd
:exmp.
.*
:p.Note that you cannot use the REXX :hp2.SAY:ehp2.
or :hp2.PULL:ehp2.
instructions as the script has no standard input and output
streams that you can see.

:p.Also note that only one instance of a given command script may be
running at any time.  More than one command script may be running at
any given time though.

:h1 res=1002.Script already running
.*
:p.The script to be run on item selection is already running.

.* V0.5.2 (2001-07-08) [lafaix]
:p.This situation normally cannot happen.  If it does, it means there
is a bug in the XWorkplace widget library.  Please report it with as
much details as possible.  Thank you.

:h1 res=1003.Interpreter error
.*
:p.An interpreter error occurred while interpreting the command script.

:p.To find out what the error code means, consult the :hp1.REXX
Information:ehp1. or :hp1.Object REXX Reference:ehp1. online guides.

:p.To find out where the script failed, check the SIGNAL ON ERROR and
SIGNAL ON SYNTAX instructions in the aforementioned manuals.

:p.REXX scripts can include error checking routines.  But keep in mind
that they cannot use :hp2.SAY:ehp2. instructions.  To interact with the
user, use for example the RxMessageBox function.

.* V0.5.2 (2001-07-08) [lafaix]
:p.For example, if you want to catch errors in your script, you
can add this statement at the beginning of the script (but after the
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
:p.An interpreter error occurred while interpreting the menu script.
To restart the gauge, open its settings notebook, possibly fix the
script, and select OK.

:p.To find out what the error code means, consult the :hp1.REXX
Information:ehp1. or :hp1.Object REXX Reference:ehp1. online guides.

:p.To find out where the script failed, check the SIGNAL ON ERROR and
SIGNAL ON SYNTAX instructions in the aforementioned manuals.

:p.REXX scripts can include error checking routines.  But keep in mind
that they cannot use :hp2.SAY:ehp2. instructions.  To interact with the
user, use for example the RxMessageBox function.

.* V0.5.2 (2001-07-08) [lafaix]
:p.For example, if you want to catch syntax errors in your script, you
can add this statement at the beginning of the script (but after the
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
is a trick used to stop the gauge script.  If it wasn't there, the
error message box would pop up again and again, each time the script
would be run.  It cause an extra error (error #35, "Invalid
expression") that you can safely ignore.

:euserdoc.
