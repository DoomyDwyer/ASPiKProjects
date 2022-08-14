#!/bin/zsh

echo "Validating plugin..."

vst3_targetfile=$1
au_targetfile=$2
plugintype=$3
pluginsubtype=$4
pluginmanufacturer=$5

# Extract paths to the various plugin validation executables
pluginval=$(cat ../../config/MacOS/pluginval.txt)
vst3_validator=$(cat ../../config/MacOS/vst3_validator.txt)

pluginval_available=true
if [[ -z $pluginval || ! -e $pluginval ]] ; then
  echo "pluginval not available, using default validators"
  pluginval_available=false
fi

vst3_validator_available=true
if [[ -z $vst3_validator || ! -e $vst3_validator ]] ; then
  echo "VST3 validator not available, skipping VST3-specific validation"
  vst3_validator_available=false
fi

if $pluginval_available ; then
  source ../../config/pluginval/config.txt

  if [[ -z $output_dir ]] ; then
    output_dir="logs"
  fi

  if [[ ! -e $output_dir ]] ; then
    mkdir -p $output_dir
  fi

  pluginval_command="$pluginval --strictness-level $strictness_level"
  if [[ $verbose ]] ; then
    pluginval_command="$pluginval_command --verbose"
  fi

  if [[ -n $output_dir ]] ; then
    pluginval_command="$pluginval_command --output-dir $output_dir"
  fi

  eval "$pluginval_command $au_targetfile"

  if [[ $? -ne 0 ]] ; then
    echo "AU PLUGIN VALIDATION FAILED! (pluginval) View logs for further details."
  fi

  if $vst3_validator_available ; then
    pluginval_command="$pluginval_command --vst3validator $vst3_validator"
  fi

  eval "$pluginval_command $vst3_targetfile"

  if [[ $? -ne 0 ]] ; then
    echo "VST3 PLUGIN VALIDATION FAILED! (pluginval) View logs for further details."
  fi
else
  if $vst3_validator_available ; then
    # Configure command to run VST3 validator
    vst3_validator_command="$vst3_validator $vst3_targetfile"

    eval "$vst3_validator_command"

    if [[ $? -ne 0 ]] ; then
      echo "PLUGIN VALIDATION FAILED! (vst3validator)"
    fi
  fi

  # Always run auval, since we know it'll always be installed
  auval -v $plugintype $pluginsubtype $pluginmanufacturer

  if [[ $? -ne 0 ]] ; then
    echo "PLUGIN VALIDATION FAILED! (auval)"
  fi
fi

