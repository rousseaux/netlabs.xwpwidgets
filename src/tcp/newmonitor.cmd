/* TCP/IP Monitor */

if RxFuncLoad('TcpLoadFuncs') then do
  call RxFuncAdd 'TcpLoadFuncs','RxTCPMon','TcpLoadFuncs'
  call TcpLoadFuncs
end

numeric digits 15

parse value TcpQueryInterface(10) with . type . . . . . newI newO .
newT = DosQuerySysInfo(14)

if type = 0 then do
  /* no network .. cleanup old stats */
  gauge.user = ''
  gauge.text = '(no network)'
  exit
end

if gauge.user == '' then do
  /* first run with network, initialize data and exit, since no time available */
  gauge.user = newT newI newO 0 /* uptime in out max */
  gauge.text = 'initializing'
  exit
end

parse var gauge.user oldT oldI oldO oldM

deltaT = getDelta(oldT, newT)
deltaI = getDelta(oldI, newI)
deltaO = getDelta(oldO, newO)

dispI = format(deltaI * 1000 / deltaT,,0)
dispO = format(deltaO * 1000 / deltaT,,0)

newM = max(oldM, dispI + dispO)

gauge.user = newT newI newO newM

gauge.text = dispO || d2c(24) dispI || d2c(25)

if newM \= 0 then do
  gauge.1 = (dispO * 100 ) % newM
  gauge.2 = min(gauge.1 + (dispI * 100) % newM, 100)
end

gauge.tooltip = 'In:' newI'0d0a'x'Out:' newO'0d0a'x'Max throughput:' newM'b/s'

exit

getDelta:
  if arg(1)  > arg(2) then
    return 4294967296 - arg(1) + arg(2)
  else
    return arg(2) - arg(1)

syntax:
  call RxMessageBox 'Error' rc 'on line' sigl ':' errortext(rc)
  exit
