// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_A0D7BCD689B2393C97922734BC6884A6
#define AU_COCOA_VIEW_NAME AUCocoaView_A0D7BCD689B2393C97922734BC6884A6

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.memento.bundleID";
const char* kAUBundleID = "developer.au.memento.bundleID";
const char* kVST3BundleID = "developer.vst3.memento.bundleID";

// --- Plugin Names 
const char* kPluginName = "Memento - Ducking Delay";
const char* kShortPluginName = "Memento - Ducki";
const char* kAUBundleName = "Memento_AU";
const char* kAAXBundleName = "Memento_AAX";
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
const char* kVSTFUID = "{a0d7bcd6-89b2-393c-9792-2734bc6884a6}";

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
