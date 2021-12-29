// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_D5797A6F84AC36BE96196DB5E8CB2BDB
#define AU_COCOA_VIEW_NAME AUCocoaView_D5797A6F84AC36BE96196DB5E8CB2BDB

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.democustomviews.bundleID";
const char* kAUBundleID = "developer.au.democustomviews.bundleID";
const char* kVST3BundleID = "developer.vst3.democustomviews.bundleID";

// --- Plugin Names 
const char* kPluginName = "DemoCustomViews";
const char* kShortPluginName = "DemoCustomViews";
const char* kAUBundleName = "DemoCustomViews";
const char* kAAXBundleName = "DemoCustomViews";
const char* kVSTBundleName = "DemoCustomViews";

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
const char* kVSTFUID = "{d5797a6f-84ac-36be-9619-6db5e8cb2bdb}";

// --- 4-char codes 
const int32_t kFourCharCode = 'dcv2';
const int32_t kAAXProductID = 'dcv2';
const int32_t kManufacturerID = 'ASPK';

// --- Vendor information 
const char* kVendorName = "ASPiK'";
const char* kVendorURL = "www.myplugins.com";
const char* kVendorEmail = "support@myplugins.com";

// --- Plugin Options 
const bool kProcessFrames = true;
const uint32_t kBlockSize = DEFAULT_AUDIO_BLOCK_SIZE;
const bool kWantSidechain = false;
const uint32_t kLatencyInSamples = 0;
const double kTailTimeMsec = 0;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = 2;

#endif
