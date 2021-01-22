@echo off
set UV=C:\Keil\UV4\UV4.exe
set UV_PRO_PATH=stc.uvproj
echo Init building ...
echo .>build_log.txt
%UV% -j0 -r %UV_PRO_PATH% -o %cd%\build_log.txt
type build_log.txt
echo Done.
rem pause