@echo off

set plugins_dir=C:\Program Files\Common Files\VST3\Doomsville
IF NOT EXIST "%plugins_dir%" mkdir "%plugins_dir%"

set configuration=%1
IF %1.==. set configuration=Release

for /f "tokens=*" %%p in (vst3_releases.txt) do (
  pushd %%p\win_build
  call ..\..\build_deploy.cmd %configuration%
  popd
)

set targetdir=C:\Program Files\Common Files\VST3\Doomsville

if "%configuration%" == "Release" (
  set /p version=<version.txt
  for /f "tokens=*" %%p in (vst3_releases.txt) do (
    pushd %targetdir%
    zip %userprofile%\Documents\Doomsville\Releases\Win64\Doomsville_VST3_x86_64_win_%version%.zip %%p.vst3
    popd
  )
)
