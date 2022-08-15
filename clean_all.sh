#!/bin/zsh

echo "Cleaning logs directory..."$
rm -Rf logs

for entry in $(cat plugin_releases.txt);
do
  IFS=';'
  read -rA arr <<< "$entry"
  dir=${arr[1]}
  
  echo "Cleaning ${dir} build directory..."
  find $dir/mac_build -mindepth 1 -maxdepth 1 -type d -exec rm -Rf {} \;
  find $dir/mac_build -maxdepth 1 -type f -not \( -path "$dir/mac_build/readme.txt"  -prune \) -exec rm -f {} \;
done

