// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace)
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_F7FE70FE4F474C7A898BEC0F0644AC9C
#define AU_COCOA_VIEW_NAME AUCocoaView_F7FE70FE4F474C7A898BEC0F0644AC9C

// --- BUNDLE IDs (MacOS Only)
const char* kAAXBundleID = "doomsvillesoundscapes.aax.gunslinger.bundleID";
const char* kAUBundleID = "doomsvillesoundscapes.au.gunslinger.bundleID";
const char* kVST3BundleID = "doomsvillesoundscapes.vst3.gunslinger.bundleID";

// --- Plugin Names
const char* kPluginName = "Gunslinger";
const char* kShortPluginName = "Gunslinger";
const char* kAUBundleName = "Gunslinger";
const char* kAAXBundleName = "Gunslinger";
const char* kVSTBundleName = "Gunslinger";

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
const char* kVSTFUID = "{F7FE70FE-4F47-4C7A-898B-EC0F0644AC9C}";

// --- 4-char codes
const int32_t kFourCharCode = 'GUNS';
const int32_t kAAXProductID = 'GUNS';
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


