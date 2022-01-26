// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_A904AB83F9E633FCABFBAF547FE6CB77
#define AU_COCOA_VIEW_NAME AUCocoaView_A904AB83F9E633FCABFBAF547FE6CB77

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.reliq.bundleID";
const char* kAUBundleID = "developer.au.reliq.bundleID";
const char* kVST3BundleID = "developer.vst3.reliq.bundleID";

// --- Plugin Names 
const char* kPluginName = "Reliq";
const char* kShortPluginName = "Reliq";
const char* kAUBundleName = "Reliq_AU";
const char* kAAXBundleName = "Reliq_AAX";
const char* kVSTBundleName = "Reliq";

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
const char* kVSTFUID = "{a904ab83-f9e6-33fc-abfb-af547fe6cb77}";

// --- 4-char codes 
const int32_t kFourCharCode = 'RELQ';
const int32_t kAAXProductID = 'RELQ';
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
