/* REXX :: Add the UsbWidget to the XCenter */

/* FIXME: Prohibit multiple instances of the UsbWidget */

/*
// This immediately adds the UsbWidget to the XCenter.
// If it was already present, an _additional_ instance will be added, which
// is _not_ good. So, first the remove script must be run, which will remove
// _all_ instances from the XCenter.
*/
rc = SysSetObjectData("<XWP_XCENTER>", "ADDWIDGETS=USBMonibiWidget;")
say 'rc:'||rc;
