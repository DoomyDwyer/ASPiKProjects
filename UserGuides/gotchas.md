# Gotchas #

## Exotic behaviours encountered so far on MacOS (Big Sur Silicon): ##

- FL Studio 20 will require the Plug-Ins to be copied to the system-wide Library, e.g. `/Library/Audio/Plug-Ins/VST3` - it won't load them from your personal Library
- Ableton Live 11 on MacOS Silicon requires the Intel x86_64 VST3s - it won't load the Silicon arm64 versions
- FL Studio 20 on MacOS Silicon *won't* load the Intel x86_64 versions - it requires the Silicon arm64 versions
- Reaper (ARM) 6.42 seems happy with either, it'll load x86_64 VST3s as "Bridged plug-ins"
- Cubase 11 refuses to recognise the Silicon arm64 VST3s (it says 32-Bit Plug-ins are no longer supported)

Both the Intel x86\_64 *and* Silicon arm64 VST3s can be installed onto a single Silicon Mac - simply create a subdirectory for each set in `[~]/Library/Audio/Plug-Ins/VST3`, and ensure the directory name for the Intel x86\_64 VST3s is alphabetically less than the directory for the Silicon arm64 plugins, since Reaper will load the latest discovered plugin with any given name (I name my directories `Doomsville/_x86_64` and `Doomsville/arm64`, since '\_' sorts before 'a'). This way I can successfully load the plugins in Reaper-ARM 6.42 (arm64), Ableton Live 11 (x86_64), FL Studio 20 (arm64) and Cubase 11 (x86_64).

- FL Studio 20 displays the plugins as 'tiny' by default. Each plugin has a 'Scale UI' drop-down (see User Guides) where the GUI can be scaled.

## Presets ##
Neither Cubase, Ableton Live, nor PreSonus Studio One load the Factory Presets from the Plugins themselves. I've therefore released two Zip files containing the Factory Presets as `.aupreset` and `.vstpreset` files, with the correct directory structure. For Cubase, Ableton Live and Studio One users, these need to be copied into the appropriate `Presets` directory:

See [Presets](workaround_factory_presets.md) for more details.

## Invitation for feedback ##

If you encounter any issues running these plugins, either on Windows or MacOS, feel free to raise an [issue](https://github.com/DoomyDwyer/ASPiKProjects/issues). The MacOS plugins should work on all versions of MacOS >= 10.9. The Windows VST3s should work on any 64-bit Windows system.