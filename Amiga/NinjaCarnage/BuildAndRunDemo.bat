@set DemoName=NinjaCarnage

@echo off
@set path=%path%;..\..\LDOS\vasm\

echo.
echo BUILDING %DemoName%
del %DemoName%.bin
del %DemoName%.adf

vc -O2 -notmpfile -nostdlib -o %DemoName%.bin src\%DemoName%.asm
..\..\LDOS\install script.txt %DemoName%.adf

REM if error do not launch winuae
if not exist %DemoName%.adf goto FINISHED
if "%~1"=="" goto RUNEMU
goto FINISHED

:RUNEMU
"..\..\ToolsForFramework\winuae\winuae_ckdebug.exe" -config="configs\a500.uae" -s floppy0="%~dp0%DemoName%.adf"

:FINISHED
pause


echo Done!
