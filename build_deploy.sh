#!/bin/zsh

echo Building in $(pwd)

project=$(basename $(dirname $(pwd)))
projectFile=${project}.xcodeproj

targetdir=~/Library/Audio/Plug-Ins/VST3/
vst3file=$project.vst3
targetfile=${targetdir}$vst3file

if [[ -e $targetfile ]] then
  echo Deleting previous version of $vst3file from $targetdir 
  rm -R $targetfile
fi

if [[ ! -e $projectFile ]] then
  cmake -GXcode ../
fi

configuration=$1
if [[ -z $configuration ]] ; then configuration=Release fi

xcodebuild -configuration $configuration

buildfile=VST3/$configuration/$vst3file

if [[ -e $targetfile ]] then
  echo "Deleting symbolic link $vst3file from $targetdir (created by CMake PostBuild Rules)"
  rm $targetfile
fi

echo Copying $buildfile to $targetdir

cp -R $buildfile $targetdir

