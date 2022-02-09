#!/bin/zsh

archivefile=$1
keychain_profile=$(cat keychain-profile.txt)

echo Notarizing artefacts present in ${archivefile}...
xcrun notarytool submit $archivefile --keychain-profile "$keychain_profile" --wait

pushd $(dirname ${archivefile})

zipdirname=$(basename $archivefile)
zipdirname="${zipdirname%.*}"

unzip -q ${archivefile} -d $zipdirname

for file in "${zipdirname}"/*
do
  xcrun stapler staple $file
done

echo Rebuilding $archivefile with stapled and validated artefacts...
pushd $zipdirname
zip -rq $archivefile *
popd
rm -Rf $zipdirname

echo "Repackaged file: $(ls -lad $archivefile)"

popd
