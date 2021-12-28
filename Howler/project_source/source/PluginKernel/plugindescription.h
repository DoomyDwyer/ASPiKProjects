// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_E5C3B1BED51934909EB76A5435A3F073
#define AU_COCOA_VIEW_NAME AUCocoaView_E5C3B1BED51934909EB76A5435A3F073

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.howler.bundleID";
const char* kAUBundleID = "developer.au.howler.bundleID";
const char* kVST3BundleID = "developer.vst3.howler.bundleID";

// --- Plugin Names 
const char* kPluginName = "Howler";
const char* kShortPluginName = "Howler";
const char* kAUBundleName = "Howler";
const char* kAAXBundleName = "Howler";
const char* kVSTBundleName = "Howler";

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
const char* kVSTFUID = "{e5c3b1be-d519-3490-9eb7-6a5435a3f073}";

// --- 4-char codes 
const int32_t kFourCharCode = 'HOWL';
const int32_t kAAXProductID = 'HOWL';
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
const double kTailTimeMsec = 0.000000;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = aaxPlugInCategory_None;

#endif
