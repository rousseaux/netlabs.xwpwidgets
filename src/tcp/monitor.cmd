/* TCP/IP Monitor */

numeric digits 10

  gauge.user = 0 0 0
restart:
parse var gauge.user oldT oldI oldO

newT = DosQuerySysInfo(14)

parse value TcpQueryInterface(10) with . type . . . . . newI newO .

gauge.user = newT newI newO

deltaT = getDelta(oldT, newT)
deltaI = getDelta(oldI, newI)
deltaO = getDelta(oldO, newO)

gauge.text = 'In:' deltaI * 1000 / deltaT 'Out:' deltaO * 1000 / deltaT
say gauge.text
call syssleep 2
signal restart
exit

getDelta:
say '1:'arg(1) '2:'arg(2)
  if arg(1)  > arg(2) then
    return 4294967296 - arg(1) + arg(2)
  else
    return arg(2) - arg(1)
