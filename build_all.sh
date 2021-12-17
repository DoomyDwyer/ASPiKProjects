#!/bin/zsh

plugins_dir=/Library/Audio/Plug-Ins/VST3/Doomsville
x86_64_dir=x86_64/
if [[ -d $plugins_dir/_$x86_64_dir ]] then
  # Move _x86_64 to x86_64
  sudo mv $plugins_dir/_$x86_64_dir $plugins_dir/$x86_64_dir 
fi

for dir in $(cat vst3_releases.txt); 
do 
  pushd $dir/mac_build; 
  ../../build_deploy.sh Release x86_64; 
  ../../build_deploy.sh Release; 
  popd ; 
done

if [[ -d $plugins_dir/$x86_64_dir ]] then
  # Move _x86_64 to x86_64 (so Reaper loads the arm64 versions last
  sudo mv $plugins_dir/$x86_64_dir $plugins_dir/_$x86_64_dir 
fi

