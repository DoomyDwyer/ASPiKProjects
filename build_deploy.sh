#!/bin/zsh

echo Building in $(pwd)

if ls *.xcodeproj 1> /dev/null 2>&1; then
else
  cmake -GXcode ../
fi

project=$(basename $(dirname $(pwd)))
projectName=$project
projectFile=${projectName}.xcodeproj

if [[ ! -e $projectFile ]] then
  projectName=${project}_UNIVERSAL
  projectFile=${projectName}.xcodeproj
fi

plugintype=aufx
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

# Can pass in extra args for xcodebuild, such as -xcconfig filename
extra_args=""
for arg in "$@"; do extra_args=($extra_args $arg) ; done

vst3_targetdir=/Library/Audio/Plug-Ins/VST3/Doomsville/$arch/
if [[ ! -d $vst3_targetdir ]] then
  sudo mkdir -p $vst3_targetdir
fi

au_targetdir=~/Library/Audio/Plug-Ins/Components/
if [[ ! -d $au_targetdir ]] then
  sudo mkdir -p $au_targetdir
fi

au_distrodir=~/Documents/Doomsville/Releases/MacOS/AU/$arch/
# Only create distribution directory for Release builds
if [[ "$configuration" == "Release" ]] then
  if [[ ! -d $au_distrodir ]] then
    sudo mkdir -p $au_distrodir
  fi
fi

vst3file=$project.vst3
vst3_targetfile=${vst3_targetdir}$vst3file

if [[ -e $vst3_targetfile ]] then
  echo Deleting previous version of $vst3file from $vst3_targetdir 
  sudo rm -R $vst3_targetfile
fi

au_file=${project}.component
au_targetfile=${au_targetdir}$au_file
au_distrofile=${au_distrodir}$au_file

if [[ -e $au_targetfile ]] then
  echo Deleting previous version of $au_file from $au_targetdir 
  sudo rm -R $au_targetfile
fi

xcodebuild -target ${project}_VST -configuration $configuration -arch $arch $extra_args
xcodebuild -target ${project}_AU_CocoaUI -configuration $configuration -arch $arch $extra_args
xcodebuild -target ${project}_AU -configuration $configuration -arch $arch $extra_args

vst3_buildfile=VST3/$configuration/$vst3file
au_buildfile=AU/$configuration/$au_file
au_bundlefile=AU/$configuration/${project}.bundle

# Sign the AU bundle prior to copying it - only bother with Release builds
if [[ "$configuration" == "Release" ]] then
  developer_id=$(cat ../../developer_id.txt)
  echo "Signing $au_bundlefile as $developer_id"
  codesign --deep --force --verify --verbose --options runtime --sign "$developer_id" --arch $arch $au_bundlefile --timestamp
  codesign -d --deep -vvv $au_bundlefile
fi

# Copy bundle into component directory
echo Adding bundle file $au_bundlefile to Component $au_buildfile
cp -Rp $au_bundlefile $au_buildfile/Contents/Resources

# Only bother signing Release builds
if [[ "$configuration" == "Release" ]] then
  developer_id=$(cat ../../developer_id.txt)
  echo "Signing $vst3_buildfile as $developer_id"
  codesign --deep --force --verify --verbose --options runtime --sign "$developer_id" --arch $arch $vst3_buildfile --timestamp
  codesign -d --deep -vvv $vst3_buildfile

  echo "Signing $au_buildfile as $developer_id"
  codesign --deep --force --verify --verbose --options runtime --sign "$developer_id" --arch $arch $au_buildfile --timestamp
  codesign -d --deep -vvv $au_buildfile
fi

echo Copying $vst3_buildfile to $vst3_targetdir
sudo cp -Rp $vst3_buildfile $vst3_targetdir

# Delete symbolic link to VST3 created during build
vst3_linkdir=~/Library/Audio/Plug-Ins/VST3/
vst3_linkfile=${vst3_linkdir}$vst3file
echo "Deleting symbolic link $vst3_linkfile"
rm -R $vst3_linkfile

echo Copying $au_buildfile to $au_targetdir
sudo cp -Rp $au_buildfile $au_targetdir

# Only copy to distribution directory for Release builds
if [[ "$configuration" == "Release" ]] then
  echo Copying $au_buildfile to $au_distrodir
  sudo cp -Rp $au_buildfile $au_distrodir
fi

echo "Waiting for 6 seconds for file copy to complete prior to AU validation..."
read -t 6

auval -v $plugintype $pluginsubtype $pluginmanufacturer

echo "VST3 build completed $(date)"
echo "New file: $(ls -lad $vst3_targetfile)"

echo "AU build completed $(date)"
echo "New file: $(ls -lad $au_targetfile)"

# Only copy to distribution directory for Release builds
if [[ "$configuration" == "Release" ]] then
  echo "File (for distribution): $(ls -lad $au_distrofile)"
fi
