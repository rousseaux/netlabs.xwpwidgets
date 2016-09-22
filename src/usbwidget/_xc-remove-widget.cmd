/* REXX :: Remove the UsbWidget from the XCenter */

/* FIXME: Prohibit multiple instances of the UsbWidget */

/*
// This removes all UsbWidget instances from the XCenter.
// Multiple instances can only be present when the install script is run
// multiple times. The UsbWidget should only be present _once_, since multiple
// instances will interfere with eachother.
*/
rc = SysSetObjectData("<XWP_XCENTER>", "DELETEWIDGETS=USBMonibiWidget;")
say 'rc:'||rc;
