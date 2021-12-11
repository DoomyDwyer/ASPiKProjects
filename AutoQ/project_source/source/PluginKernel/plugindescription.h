// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_DCBB5084F0E63773857B29883484D0F6
#define AU_COCOA_VIEW_NAME AUCocoaView_DCBB5084F0E63773857B29883484D0F6

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.autoq.bundleID";
const char* kAUBundleID = "developer.au.autoq.bundleID";
const char* kVST3BundleID = "developer.vst3.autoq.bundleID";

// --- Plugin Names 
const char* kPluginName = "AutoQ";
const char* kShortPluginName = "AutoQ";
const char* kAUBundleName = "AutoQ";
const char* kAAXBundleName = "AutoQ";
const char* kVSTBundleName = "AutoQ";

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
const char* kVSTFUID = "{dcbb5084-f0e6-3773-857b-29883484d0f6}";

// --- 4-char codes 
const int32_t kFourCharCode = 'AUTQ';
const int32_t kAAXProductID = 'AUTQ';
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
