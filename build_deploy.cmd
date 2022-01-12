@echo off

echo "IMPORTANT! This batch file will try to copy files to C:\Program Files\Common Files\VST3, and MUST be run as Administrator"

net session >nul 2>&1
if %errorLevel% == 0 (
    echo Success: Administrative permissions confirmed.
) else (
    echo Failure: Current permissions inadequate.
    EXIT /b 1
)

set configuration=%1
IF %1.==. set configuration=Release

for %%I in (..) do set project=%%~nxI
echo Building project %project% in %cd%...

set projectFile=%project%.sln
IF NOT EXIST %projectFile% cmake -G"Visual Studio 16 2019" ../

msbuild %projectFile% /p:Configuration=%configuration%

set targetdir=C:\Program Files\Common Files\VST3\Doomsville
IF NOT EXIST "%targetdir%" mkdir "%targetdir%"

copy /y VST3\%configuration%\%project%.vst3\Contents\x86_64-win\%project%.vst3 "%targetdir%"

echo Build completed %date% %time%
echo New file:
dir "%targetdir%\%project%.vst3"

