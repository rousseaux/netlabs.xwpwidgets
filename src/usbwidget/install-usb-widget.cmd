/* REXX */

/*
// ############################################################################
// # Install / Update the eCenter Widget Plugin and Daemon                    #
// ############################################################################
*/


/* Some variables */
build="testbuild-20140713";
widget="usbshold";
daemon="usbshlpr";
files.0=2;
files.1=widget||'.dll';
files.2=daemon||'.dmn';
osdir=Value("OSDIR",,"ENVIRONMENT");
dllpath=osdir||"\System\eWPS\Plugins\XCenter";

'@echo:';
'@echo :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::';
'@echo :: Installing new eCenter USB Widget -- ('||build||')               ::';
'@echo :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::';

/* Stop the Daemon */
'@echo:';
'@echo Stopping the Widget Daemon...';
'@'||dllpath||'\'||daemon||'.dmn /stop > nul';
'@echo OK';


/* Make backup of old files, unlock daemon and dll and overwrite with new version */
do i = 1 to files.0
    '@echo:';
    '@echo Updating "'dllpath||'\'||files.i||'"';
    '@if not exist 'dllpath||'\'||files.i||'.bu copy 'dllpath||'\'||files.i||' 'dllpath||'\'||files.i||'.bu';
    '@unlock /v '||dllpath||'\'||files.i;
    '@md5sum '||dllpath||'\'||files.i;
    '@copy '||files.i||' '||dllpath||'\'||files.i||' > nul';
    '@md5sum '||dllpath||'\'||files.i;
    '@copy '||files.i||' '||dllpath||'\'||files.i||'.'build||' > nul';
end

/* Copy DFSVOS2.BIN executable */
'@copy dfsvos2.bin '||dllpath||' > nul';

/* Copy Language Resource */
'@copy '||widget||'.nls '||osdir||'\lang > nul';

/* Copy symbol files */
'@copy '||widget||'.sym '||dllpath||' > nul';
'@copy '||daemon||'.sym '||dllpath||'> nul';


/* Done */
'@echo:';
'@echo Done !';
'@echo Please restart the Workplace Shell to activate the new Widget version.';
'@echo Then right click on the Widget Button and choose Properties to verify the build info.';
'@echo:';
'@echo Please report problems with the Widget to: rousseau.ecsdev@gmail.com or use the bug-tracker.';
