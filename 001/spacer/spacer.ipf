.* EBOOKIE (IPFTAGS.DEF)
.* Copyright (c) 2001 Martin Lafaix. All Rights Reserved.
:userdoc.
:h1 res=1001.XCenter: Spacer Widget
:lm margin=1.
:p.The XCenter :hp2.spacer:ehp2. widget adds an empty space on the 
XCenter.  This empty space does nothing special.

:p.Despite doing nothing on their own, spacers are quite
useful.  For example, they can enhance the usability of an XCenter, 
just like empty space can enhance the readability of text.

:p.Also, spacers allows you to organize the layout of an XCenter.  You
can, for example, center some other widgets between two spacers, or put
a spacer between two groups of widgets so that one group is flushed
on the left and the other on the right.

:p.Spacers are either of a specified size, or they can grab all 
available space (or, more precisely, the available space is
evenly distributed between all spacers and widgets that require
it).

:p.To configure a spacer widget, right click inside the widget and
select :hp2.Properties:ehp2..  A dialog will pop up that allows you to
define the widget's width and height.

:h1 res=1000.XCenter: Spacer Widget Settings
:lm margin=1.
:p.In this dialog you define the properties of the spacer
widget.

:p.The width and the height of the spacer are set independently.  For 
the two dimensions, you can specify that the widget uses either all 
the available space (i.e. the spacer is "greedy"), or that it uses a 
specified number of pixels.

:p.The most common uses of spacers are either to separate two groups
of widgets, in which case a fully-specified spacer is often used, or
to flush a group of widgets to the right of the XCenter, in which case
a spacer that uses all the available space for its width is
recommended.

:nt.
The current XCenter implementation does not allow the definition
of widgets that require all the available space for their height. 
The spacer widget fakes it by using a height of five pixels in that 
case.  Also, for similar reasons, a width of zero cannot be specified. 
(If you were to specify a width of zero, you wouldn't be able to access 
the widget's settings dialog any more.)
:ent.
:euserdoc.
