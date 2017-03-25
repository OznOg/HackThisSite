@shift 1
@echo off
echo Enter Password:
set pass=
set /p pass=
if "%pass%"=="speech" goto pass2
Echo wrong password!
pause
exit
:pass2
Echo Congrats! HTS password is freedom.
pause
exit
