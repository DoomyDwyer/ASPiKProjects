// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_04FC30BA54B23A3B9FE41454227A81E0
#define AU_COCOA_VIEW_NAME AUCocoaView_04FC30BA54B23A3B9FE41454227A81E0

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.volumeii.bundleID";
const char* kAUBundleID = "developer.au.volumeii.bundleID";
const char* kVST3BundleID = "developer.vst3.volumeii.bundleID";

// --- Plugin Names 
const char* kPluginName = "Volume2";
const char* kShortPluginName = "Volume2";
const char* kAUBundleName = "Volumeii";

// --- Plugin Type 
const pluginType kPluginType = pluginType::kFXPlugin;

// --- VST3 UUID 
const char* kVSTFUID = "{04fc30ba-54b2-3a3b-9fe4-1454227a81e0}";

// --- 4-char codes 
const int32_t kFourCharCode = 'VOL2';
const int32_t kAAXProductID = 'VOL2';
const int32_t kManufacturerID = 'VOL2';

// --- Vendor information 
const char* kVendorName = "Doomsville Soundscapes";
const char* kVendorURL = "https://github.com/DoomyDwyer";
const char* kVendorEmail = "doomy@puscii.nl";

// --- Plugin Options 
const bool kWantSidechain = false;
const uint32_t kLatencyInSamples = 0;
const double kTailTimeMsec = 0.000000;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = aaxPlugInCategory_None;

#endif
