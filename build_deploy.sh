#!/bin/zsh

echo Building in $(pwd)

cmake -GXcode ../

configuration=$1
if [[ -z $configuration ]] ; then configuration=Release fi

project=$(basename $(dirname $(pwd)))

xcodebuild -configuration $configuration

linkfile=~/Library/Audio/Plug-Ins/VST3/$project.vst3

if [[ -e $linkfile ]] then
  echo Deleting symbolic link $linkfile
  rm $linkfile
fi

buildfile=VST3/$configuration/$project.vst3

echo Creating symbolic link $linkfile to $buildfile

ln -s $buildfile $linkfile

