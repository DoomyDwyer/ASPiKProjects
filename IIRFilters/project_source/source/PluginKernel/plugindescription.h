// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_EAC2B84472063026ACA02051D1699876
#define AU_COCOA_VIEW_NAME AUCocoaView_EAC2B84472063026ACA02051D1699876

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.iirfilters.bundleID";
const char* kAUBundleID = "developer.au.iirfilters.bundleID";
const char* kVST3BundleID = "developer.vst3.iirfilters.bundleID";

// --- Plugin Names 
const char* kPluginName = "IIRFilters";
const char* kShortPluginName = "IIRFilters";
const char* kAUBundleName = "IIRFilters";
const char* kAAXBundleName = "IIRFilters";
const char* kVSTBundleName = "IIRFilters";

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
const char* kVSTFUID = "{eac2b844-7206-3026-aca0-2051d1699876}";

// --- 4-char codes 
const int32_t kFourCharCode = 'IIRF';
const int32_t kAAXProductID = 'IIRF';
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
