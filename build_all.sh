#!/bin/zsh

x86_64_dir=x86_64/

vst3_plugins_dir=/Library/Audio/Plug-Ins/VST3/Doomsville
if [[ -d $vst3_plugins_dir/_$x86_64_dir ]] then
  # Move _x86_64 to x86_64
  sudo mv $vst3_plugins_dir/_$x86_64_dir $vst3_plugins_dir/$x86_64_dir 
fi

au_distrodir=~/Documents/Doomsville/Releases/MacOS/AU/$arch/
# Only create distribution directory for Release builds
if [[ "$configuration" == "Release" ]] then
  if [[ ! -d $au_distrodir ]] then
    sudo mkdir -p $au_distrodir
  fi
fi

# Can be Debug or Release
configuration=$1
if [[ -z $configuration ]] ; then configuration=Release fi
shift

for entry in $(cat plugin_releases.txt); 
do 
  IFS=';'
  read -rA arr <<< "$entry"
  dir=${arr[1]}
  pluginsubtype=${arr[2]}
  pushd $dir/mac_build
  ../../build_deploy.sh $pluginsubtype $configuration x86_64 -xcconfig ../../x86_64_xcodebuild_config.txt
  ../../build_deploy.sh $pluginsubtype $configuration 
  popd
done

if [[ -d $vst3_plugins_dir/$x86_64_dir ]] then
  # Move _x86_64 to x86_64 (so Reaper loads the arm64 versions last
  sudo mv $vst3_plugins_dir/$x86_64_dir $vst3_plugins_dir/_$x86_64_dir 
fi

# Only create zip file for Release builds
if [[ "$configuration" == "Release" ]] then
  version=$(cat version.txt)
  pushd $vst3_plugins_dir/_$x86_64_dir/
  zip -r ~/Documents/Doomsville/Releases/MacOS/Doomsville_VST3_MacOS_x86_64_$version.zip *
  popd
  pushd $vst3_plugins_dir/arm64/
  zip -r ~/Documents/Doomsville/Releases/MacOS/Doomsville_VST3_MacOS_arm64_$version.zip *
  popd

  pushd $au_distrodir/$x86_64_dir/
  zip -r ~/Documents/Doomsville/Releases/MacOS/Doomsville_AU_MacOS_x86_64_$version.zip *
  popd
  pushd $au_distrodir/arm64/
  zip -r ~/Documents/Doomsville/Releases/MacOS/Doomsville_AU_MacOS_arm64_$version.zip *
  popd
fi
