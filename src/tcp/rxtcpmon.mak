# NMAKE-compatible MAKE file for the REXX sample program RXTCPMON.DLL.

rxtcpmon.dll:     rxtcpmon.obj  rxtcpmon.def
	 ILINK /NOFREE rxtcpmon.obj,rxtcpmon.dll,,REXX SO32DLL TCP32DLL,rxtcpmon.def;

rxtcpmon.obj:     rxtcpmon.c
	 icc -ss -c -Ge- rxtcpmon.c


