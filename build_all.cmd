@echo off

set plugins_dir=C:\Program Files\Common Files\VST3\Doomsville
IF NOT EXIST "%plugins_dir%" mkdir "%plugins_dir%"

for /f "tokens=*" %%p in (vst3_releases.txt) do (
  pushd %%p\win_build
  ..\..\build_deploy.cmd
  popd
)

