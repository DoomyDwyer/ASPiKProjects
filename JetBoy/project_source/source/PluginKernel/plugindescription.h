// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace)
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_7CB29BEE5D484D09A37554BEA10DD7E8
#define AU_COCOA_VIEW_NAME AUCocoaView_7CB29BEE5D484D09A37554BEA10DD7E8

// --- BUNDLE IDs (MacOS Only)
const char* kAAXBundleID = "doomsvillesoundscapes.aax.jetboy.bundleID";
const char* kAUBundleID = "doomsvillesoundscapes.au.jetboy.bundleID";
const char* kVST3BundleID = "doomsvillesoundscapes.vst3.jetboy.bundleID";

// --- Plugin Names
const char* kPluginName = "JetBoy";
const char* kShortPluginName = "JetBoy";
const char* kAUBundleName = "JetBoy";
const char* kAAXBundleName = "JetBoy";
const char* kVSTBundleName = "JetBoy";

// --- bundle name helper
inline static const char* getPluginDescBundleName()
{
#ifdef AUPLUGIN
	return kAUBundleName;
#endif

#ifdef AAXPLUGIN
	return kAAXBundleName;
#endif

#ifdef VSTPLUGIN
	return kVSTBundleName;
#endif

	// --- should never get here
	return kPluginName;
}

// --- Plugin Type
const pluginType kPluginType = pluginType::kFXPlugin;

// --- VST3 UUID
const char* kVSTFUID = "{7CB29BEE-5D48-4D09-A375-54BEA10DD7E8}";

// --- 4-char codes
const int32_t kFourCharCode = 'JBOY';
const int32_t kAAXProductID = 'JBOY';
const int32_t kManufacturerID = 'DOOM';

// --- Vendor information
const char* kVendorName = "Doomsville Soundscapes";
const char* kVendorURL = "github.com/DoomyDwyer";
const char* kVendorEmail = "doomy@puscii.nl";

// --- Plugin Options
const bool kProcessFrames = true;
const uint32_t kBlockSize = DEFAULT_AUDIO_BLOCK_SIZE;
const bool kWantSidechain = true;
const uint32_t kLatencyInSamples = 0;
const double kTailTimeMsec = 0.000;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = 0;

#endif


