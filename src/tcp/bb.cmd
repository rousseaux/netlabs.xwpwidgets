/* IP MON Rexx Guage
 Set refresh rate to 500 ms and resizable for variable number of interfaces,
low to high.
*/
call RxFuncAdd 'TcpLoadFuncs','RxTCPMon','TcpLoadFuncs'
call TcpLoadFuncs
 /* Ask for current byte counts */
stats=''
interfaces = TcpQueryAvailableInterfaces()
do i = 1 to words(interfaces)
   data = TcpQueryInterface(word(interfaces, i))
   parse var data index type mtu physaddr . . .  in out .
   stats = in || ' ' || out || ' ' || stats || ' '
end /* end do */
say stats
last_stats=VALUE('LAST_REC_RATE',stats,'OS2ENVIRONMENT')
/*say last_stats*/
gaugetext=''
do i = 1 to words(interfaces)*2
 rate.i = word(stats,i) - word(last_stats,i)
 gaugetext=gaugetext || FORMAT(rate.i/512,5,1)||'K '||D2C(24) || ' '
 i=i+1
 rate.i = word(stats,i) - word(last_stats,i)
 gaugetext=gaugetext || FORMAT(rate.i/512,5,1)||'K '||D2C(25) || ' '
end
gauge.text = gaugetext
say gauge.text
