@echo off

set plugins_dir=C:\Program Files\Common Files\VST3\Doomsville
IF NOT EXIST "%plugins_dir%" mkdir "%plugins_dir%"

set configuration=%1
IF %1.==. set configuration=Release

for /f "tokens=*" %%p in (plugin_releases.txt) do (
  for /f "tokens=1,2 delims=;" %%a in ("%%p") do (
    pushd %%a\win_build
    call ..\..\build_deploy.cmd %configuration%
    popd
  )
)

set targetdir=C:\Program Files\Common Files\VST3\Doomsville

if "%configuration%" == "Release" (
  set /p version=<version.txt
  for /f "tokens=*" %%p in (plugin_releases.txt) do (
    for /f "tokens=1,2 delims=;" %%a in ("%%p") do (
      pushd %targetdir%
      zip %userprofile%\Documents\Doomsville\Releases\Win64\Doomsville_VST3_x86_64_win_%version%.zip %%a.vst3
      popd
    )
  )
)

echo.
echo *** BUILD COMPLETED %date% %time% ***
