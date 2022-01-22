#!/bin/zsh

echo Building in $(pwd)

project=$(basename $(dirname $(pwd)))
projectFile=${project}.xcodeproj

plugintype=aumf
pluginsubtype=$1
pluginmanufacturer=DOOM
shift

# Can be Debug or Release
configuration=$1
if [[ -z $configuration ]] ; then configuration=Release fi
shift

# Can be arm64 or x86_64
arch=$1
if [[ -z $arch ]] ; then arch=arm64 fi
shift

targetdir=/Library/Audio/Plug-Ins/VST3/Doomsville/$arch/
if [[ ! -d $targetdir ]] then
  sudo mkdir -p $targetdir
fi

# Can pass in extra args for xcodebuild, such as -xcconfig filename
extra_args=""
for arg in "$@"; do extra_args=($extra_args $arg) ; done

vst3file=$project.vst3
targetfile=${targetdir}$vst3file

if [[ -e $targetfile ]] then
  echo Deleting previous version of $vst3file from $targetdir 
  sudo rm -R $targetfile
fi

if [[ ! -e $projectFile ]] then
  cmake -GXcode ../
fi

xcodebuild -configuration $configuration -arch $arch $extra_args

auval -v $plugintype $pluginsubtype $pluginmanufacturer

buildfile=VST3/$configuration/$vst3file

# Only bother signing Release builds
if [[ "$configuration" == "Release" ]] then
  developer_id=$(cat ../../developer_id.txt)
  echo "Signing $buildfile as $developer_id"
  codesign --deep --force --verify --verbose --options runtime --sign "$developer_id" --arch $arch $buildfile --timestamp
  codesign -d --deep -vvv $buildfile
fi

echo Copying $buildfile to $targetdir
sudo cp -R $buildfile $targetdir

echo "Build completed $(date)"
echo "New file: $(ls -lad $targetfile)"
