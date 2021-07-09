// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_3DC2222AAC033386A4E665F6E947EE26
#define AU_COCOA_VIEW_NAME AUCocoaView_3DC2222AAC033386A4E665F6E947EE26

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.volume.bundleID";
const char* kAUBundleID = "developer.au.volume.bundleID";
const char* kVST3BundleID = "developer.vst3.volume.bundleID";

// --- Plugin Names 
const char* kPluginName = "Volume";
const char* kShortPluginName = "Volume";
const char* kAUBundleName = "Volume";

// --- Plugin Type 
const pluginType kPluginType = pluginType::kFXPlugin;

// --- VST3 UUID 
const char* kVSTFUID = "{3dc2222a-ac03-3386-a4e6-65f6e947ee26}";

// --- 4-char codes 
const int32_t kFourCharCode = 'VOL1';
const int32_t kAAXProductID = 'VOL1';
const int32_t kManufacturerID = 'DOOM';

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
const uint32_t kAAXCategory = 0;

#endif
