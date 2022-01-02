#!/bin/zsh

plugins_dir=/Library/Audio/Plug-Ins/VST3/Doomsville
x86_64_dir=x86_64/
if [[ -d $plugins_dir/_$x86_64_dir ]] then
  # Move _x86_64 to x86_64
  sudo mv $plugins_dir/_$x86_64_dir $plugins_dir/$x86_64_dir 
fi

for dir in $(cat vst3_releases.txt); 
do 
  pushd $dir/mac_build
  ../../build_deploy.sh Release x86_64 -xcconfig ../../x86_64_xcodebuild_config.txt
  ../../build_deploy.sh Release
  popd
done

if [[ -d $plugins_dir/$x86_64_dir ]] then
  # Move _x86_64 to x86_64 (so Reaper loads the arm64 versions last
  sudo mv $plugins_dir/$x86_64_dir $plugins_dir/_$x86_64_dir 
fi

version=$(cat version.txt)
pushd $plugins_dir/_$x86_64_dir/
zip -r ~/Documents/Doomsville/Releases/MacOS/Doomsville_MacOS_x86_64_$version.zip *
popd
pushd $plugins_dir/arm64/
zip -r ~/Documents/Doomsville/Releases/MacOS/Doomsville_MacOS_arm64_$version.zip *
popd
