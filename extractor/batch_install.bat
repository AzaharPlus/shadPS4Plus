@echo off

set dir=%1%

FOR %%i IN (%dir%\*.pkg) DO call %0\..\install_pkg.bat %%i --batch

echo Batch install done

pause
