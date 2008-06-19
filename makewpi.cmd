/**/
wpistem = 'xwpwidgets-0-7-3'
filetime = '00:07:30'
wisfile = 'xwpwidgets.wis'
reldir = 'bin\widgets'
outdir = 'bin'

packages.0 = 8
packages.1 = 'spacer.*'
packages.2 = 'rbutton.*'
packages.3 = 'rgauge.*'
packages.4 = 'rscrlr.*'
packages.5 = 'rmenu.*'
packages.6 = 'rmonitor.*'
packages.7 = 'popper.*'
packages.8 = 'irmon.*'
/**/
call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs
parse arg xwpdir .
if xwpdir = '' then do
  say 'Usage: makewpi <XWP base directory>'
  return
end

makedir = directory()
reldir = directory(reldir)
call directory makedir
outdir = directory(outdir)
warpindir = SysIni(USER, 'WarpIN', 'Path')
if warpindir = 'ERROR:' | warpindir = '' then do
  say 'WarpIN is not installed correctly'
  return
end
call directory warpindir
wic1 = '@wic' outdir'\'wpistem '-a'
wic2 = ''
do i = 1 to packages.0
  wic2 = wic2 i '-c'reldir packages.i
  call setfiletime reldir'\'packages.i
end
wic3 = '-s' makedir'\'wisfile
call SysFileDelete outdir'\'wpistem'.wpi'
wic1 wic2 wic3
call directory makedir
return

setfiletime: procedure expose filetime makedir xwpdir
  parse arg pkgdir
  call SysFileTree pkgdir'\*', 'stem', 'FOS'
  filedate = date('S')
  filedate = left(filedate,4)'-'substr(filedate,5,2)'-'substr(filedate,7,2)
  do i = 1 to stem.0
    call SysSetFileDateTime stem.i, filedate, filetime
    '@'xwpdir'\tools\setftime' stem.i
  end
  return
