.* EBOOKIE (IPFTAGS.DEF)
:userdoc.
:docprof toc=123.
:title.XWorkplace Widget Library
:h1.Notices
.*
:p.:hp2.April 2001:ehp2.
.*
:p.The following paragraph does not apply to the United Kingdom or any
country where such provisions are inconsistent with local law:  THIS
PUBLICATION IS PROVIDED "AS IS," INCLUDING ANY SAMPLE APPLICATION
PROGRAMS, WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  Some states do
not allow disclaimer of express or implied warranties in certain
transactions, therefore, this statement may not apply to you.
.*
:p.This publication could include technical inaccuracies or
typographical errors.  Changes are periodically made to the
information herein; these changes will be incorporated in new editions
of the publication.  The XWorkplace Widget Library development team
may make improvements and/or changes in the product(s) and/or the
program(s) described in this publication at any time.
.*
:h2.Year 2000 Compliance
.*
:p.The date-sensitive widgets contained in the XWorkplace Widget 
Library are Year 2000 Compliant.
.*
:h2.Comments and suggestions
.*
:p.The XWorkplace Widget Library is an open source project with a
development team.
.*
:p.If you have any comments or suggestions regarding the XWorkplace 
Widget Library, please send them to one of the :hp2.XWorkplace mailing
lists:ehp2..
.*
:ul compact.
:li.xworkplace-user@yahoogroups.com is for XWorkplace users.  Use this
address to submit bug reports if you don't understand the source code.
Also, this is intended for public discussions about XWorkplace (e.g.
suggesting future features).
:li.xworkplace-dev@yahoogroups.com is for messages related to the
source code.  This is used by the developers to communicate and can
also be used by anyone else.
:eul.
.*
:p.http&colon.//xworkplace.netlabs.org is XWorkplace's home page on 
the
World Wide Web.  Watch this space for news, updates, and public
downloads.
.*
:p.A public bug list is also maintained on that homepage.
.*
:p.Reporting bugs.  Please include the following information (the
more, the better):
.*
:ul compact.
:li.Please describe the situation in which the error occurred and
whether it is reproducible.
:li.Include the OS/2 version you are running (Warp 3 or 4, including
the fixpak level).
:li.Include a list of other PM or WPS enhancers you are running. 
:li.Look in the root directory of your boot drive for a file named
XFLDTRAP.LOG.  This file logs XWorkplace's exceptions which might have
occurred on your system.  If it exists, please include it in your
message.
:eul.
.*
:h2.Copyright Notices
.*
:p.:hp2.(c) Copyright Martin Lafaix, 2001.:ehp2.
.*
:h2.Trademarks
:p.The following terms are trademarks of the IBM Corporation in the
United States or other countries or both:
.*
:p.IBM 
.br
Operating System/2 
.br
OS/2 
.br
OS/2 Warp 
.br
Presentation Manager 
.br
SOM 
.br
System Object Model 
.br
VisualAge 
.br
WarpCenter 
.br
Win-OS/2 
.br
Workplace Shell 
.*
:p.:hp2.Open Source:ehp2.
is now a Registered Certification Mark of Software in
the Public Interest.  Check www.opensource.org for details.
.*
:p.:hp2.Java:ehp2.
and all Java-based trademarks and logos are trademarks of Sun
Microsystems, Inc. 
.*
:p.:hp2.C++:ehp2.  and :hp2.UNIX:ehp2. are trademarks of the American
Telephone and Telegraph Company (AT&amp.T).
.*
:p.Other company, product, or service names may be trademarks or
service marks of these companies or others.  They are not specifically
marked.
.*
:p.Any mention of a product in this publication does not constitute an
endorsement or affiliation.
.* 
:h1.Why a XWorkplace Widget Library?
.*
:p.The :hp2.XWorkplace Widget Library:ehp2. complements
:hp2.XWorkplace:ehp2..  It is a collection of :hp2.XCenter:ehp2.
widgets.  If you want to use the XWorkplace Widget Library, you need
XWorkplace.
.*
:p.The library is not part of the standard XWorkplace distribution, so
that the two products can evolve at their own pace.  A minimum 
XWorkplace version level is required to run a given library release.  
(For example, as of this writing, the library requires XWorkplace 
V0.9.9 or higher.)
.*
:h1.How is this library organized
.*
:p.The source code repository consists of three main directories:
.*
:dl compact.
:dt.001
:dd.the NLS parts for English (resources, help files, etc.)
:dt.include
:dd.the headers for the files
:dt.src
:dd.the source code
:edl.
.*
:p.In those main directories, there is one subdirectory per widget 
library.  For example, as of this writing, you will see two 
sub-directories, rexx and spacer.  As the library evolves, more 
subdirectories will appear.
.*
:h1.How do I ...
.*
:p.How do I...
.*
:ul compact.
:li.build the library
:li.add a widget to the library
:eul.
.*
:h2.How do I build the library
.*
:p.In order to be able to build the library, you need some 
prerequisites:
.*
:ul compact.
:li.:hp2.IBM VisualAge for C++ 3.0:ehp2. is the compiler used to 
develop the library.  For it, :hp2.fixpak 8:ehp2. is strongly 
recommended.  (You may be able to build the library with another 
compiler, but this has not been tested.)
:li.An :hp2.OS/2 developer's toolkit:ehp2. has to be installed (or at 
least the header files and the libraries)
:li.:hp2.IBM NMAKE:ehp2. is the make utility used.
:li.And the :hp2.xworkplace:ehp2. and :hp2.xwphelpers:ehp2. 
repositories have to be checked out.  (You don't have to build those 
two repositories, but the XWorkplace Widget Library requires many 
header files included in them.)
:eul.
.*
:p.You also need to setup some variables.  Except for 
:hp2.PROJECT&us.BASE&us.DIR:ehp2., they are all compatible with those
needed for XWorkplace:
.*
:ul compact.
:li.:hp2.PROJECT&us.BASE&us.DIR:ehp2. points to where the library code
is on your disk.  For example, it is defined as 
PROJECT&us.BASE&us.DIR=f&colon.&bsl.work&bsl.xwpwidgets here.
:li.:hp2.CVS&us.WORK&us.ROOT:ehp2. points to the where the CVS work 
directory is on your disk.  It is used to locate the xworkplace and 
xwphelpers repositories.  For example, it is defined as 
CVS&us.WORK&us.ROOT=f&colon.&bsl.work here.
:li.:hp2.XWP&us.RUNNING:ehp2. is where your running copy of XWorkplace
is installed.  The library elements are updated in this location if 
the build is a success.
:li.:hp2.XWP&us.OUTPUT&us.ROOT:ehp2. is where the temporary copies of 
the library elements are to be copied.
:li.:hp2.XWP&us.DEBUG:ehp2. can be defined if you want to link the 
modules with the debug library (PMPRINTF).
:eul.
.*
:p.(If you use the make.cmd script as explained below, you can define 
those environment variables in envicc.cmd and enviproject.cmd.  Those 
two batch files being defined somewhere along your PATH.  For 
example, here, envicc.cmd is globally available-and hence shared with 
XWorkplace-and defines all variables except PROJECT&us.BASE&us.DIR, 
which is defined in enviproject.cmd, and is located at the xwpwidgets 
library directory.)
.*
:p.When the prerequisites are fulfilled and the environment variables 
are set, you can invoke the make.cmd script.  It will build the 
library and the English NLS help files.
.*
:p.You can also use nmake (or nmake -a if you want a complete 
rebuild), but in this case the English NLS help files are not updated. 
Similarly, if you only want to update the NLS help files, you can 
switch to the 001 directory and invoke nmake from there.

.*
:h2.How do I add a widget to the library
.*
:p.If you want to add a new widget to the repository, the simplest way
to proceed is as follow:
.*
:ol compact.
:li.create a new subdirectory in the three main directories.  For 
example, create '001/foo', 'include/foo', and 'src/foo'.
:li.update the main makefile so that it recognize the new widget. 
Usually, you will have to add an entry in the widgets target and in 
the link target.
:li.update the NLS makefile (001/makefile) to also recognize the new 
widget.
:li.happy programming
:eol.
.*
:euserdoc.
