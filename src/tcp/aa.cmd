/* Example of RxTCPMon functions */
call RxFuncAdd 'TcpLoadFuncs','RxTCPMon','TcpLoadFuncs'
call TcpLoadFuncs

numeric digits 10

say TcpQueryRecCount()
say TcpQuerySndCount()
say TcpQueryStats()

interfaces = TcpQueryAvailableInterfaces()
do i = 1 to words(interfaces)
  data = TcpQueryInterface(word(interfaces, i))
  parse var data index type mtu physaddr . . . in out .
  say 'interface #'index 'type' type 'mtu size' mtu 'phys addr' physaddr '['subword(data, 19)']'
  say ' in' in 'out' out
end /* end do*/
