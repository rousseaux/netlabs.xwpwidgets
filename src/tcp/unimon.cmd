/* TCP/IP Monitor */

threshold = 2
interface = 0

/* nothing to change below this line */
if RxFuncQuery('TcpLoadFuncs') then do
  call RxFuncAdd 'TcpLoadFuncs','RxTCPMon','TcpLoadFuncs'
  call TcpLoadFuncs
end
if RxFuncQuery('SysLoadFuncs') then do
  call RxFuncAdd 'SysLoadFuncs','RexxUtil','SysLoadFuncs'
  call SysLoadFuncs
end

numeric digits 15
gauge.user = ''
restart:
say gauge.text
parse value TcpQueryInterface(interface),
      with . type . . . . . newI newO . . . . . . . . . desc
newT = DosQuerySysInfo(14)
parse var gauge.user oldT oldI oldO oldM

if type = 0 then do
  /* no network, cleanup old stats */
  gauge.user = ''
  gauge.text = '(no network)'
  call syssleep 1
  signal restart
end

if gauge.user == '' then do
  /* first run, initialize data and exit, since no time available */
  gauge.user = newT newI newO 0 /* uptime in out max */
  gauge.text = 'initializing'
  call syssleep 1
  signal restart
end

deltaT = getDelta(oldT, newT)
dispI = getDelta(oldI, newI) * 1000 % deltaT
dispO = getDelta(oldO, newO) * 1000 % deltaT
newM = max(oldM, dispI + dispO)

gauge.user = newT newI newO newM
gauge.text = dispO || d2c(24) dispI || d2c(25)
gauge.foreground = 0
gauge.tooltip = desc'0d0a'x'In:' newI'0d0a'x'Out:' newO'0d0a'x'Max throughput:' newM'b/s'

if newM \= 0 then do
  /* some traffic has been recorded, let us draw a gauge. */
  /* (we exagerate the traffic so that if there's some, something is shown) */
  gauge.1 = (dispO * 100 ) % newM
  if dispO \= 0 then
    gauge.1 = max(gauge.1, threshold)
  if dispI \= 0 then do
    gauge.2 = min(gauge.1 + max((dispI * 100) % newM, threshold), 100)
    if gauge.2 = 100 then
      gauge.1 = min(gauge.1, 100-threshold)
  end
end

call syssleep 1
signal restart

getDelta:
  if arg(1)  > arg(2) then
    return 4294967296 - arg(1) + arg(2)
  else
    return arg(2) - arg(1)
