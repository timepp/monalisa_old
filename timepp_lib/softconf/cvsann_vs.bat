cvs -q annotate %1 > %temp%\cvsann_%1
start /MAX e:\greensoft\vim\vim72\gvim.exe -c "setf cvsann" %temp%\cvsann_%1
