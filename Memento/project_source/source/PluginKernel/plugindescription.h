// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_2DF920D015D53FF79F39CCFD2834D8BD
#define AU_COCOA_VIEW_NAME AUCocoaView_2DF920D015D53FF79F39CCFD2834D8BD

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.memento.bundleID";
const char* kAUBundleID = "developer.au.memento.bundleID";
const char* kVST3BundleID = "developer.vst3.memento.bundleID";

// --- Plugin Names 
const char* kPluginName = "Memento";
const char* kShortPluginName = "Memento";
const char* kAUBundleName = "Memento";
const char* kAAXBundleName = "Memento";
const char* kVSTBundleName = "Memento";

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
const char* kVSTFUID = "{2df920d0-15d5-3ff7-9f39-ccfd2834d8bd}";

// --- 4-char codes 
const int32_t kFourCharCode = 'MEMO';
const int32_t kAAXProductID = 'MEMO';
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