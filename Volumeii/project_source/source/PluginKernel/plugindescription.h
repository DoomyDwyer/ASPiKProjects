// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_6E5110D66B8544F2B2B02243C48E2430
#define AU_COCOA_VIEW_NAME AUCocoaView_6E5110D66B8544F2B2B02243C48E2430

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "doomsvillesoundscapes.aax.Volume2.bundleID";
const char* kAUBundleID = "doomsvillesoundscapes.au.Volume2.bundleID";
const char* kVST3BundleID = "doomsvillesoundscapes.vst3.Volume2.bundleID";

// --- Plugin Names 
const char* kPluginName = "Volume2";
const char* kShortPluginName = "Volume2";
const char* kAUBundleName = "Volume2";

// --- Plugin Type 
const pluginType kPluginType = pluginType::kFXPlugin;

// --- VST3 UUID 
const char* kVSTFUID = "{6E5110D6-6B85-44F2-B2B0-2243C48E2430}";

// --- 4-char codes 
const int32_t kFourCharCode = 'VOL2';
const int32_t kAAXProductID = 'VOL2';
const int32_t kManufacturerID = 'DOOM';

// --- Vendor information 
const char* kVendorName = "Doomsville Soundscapes";
const char* kVendorURL = "https://github.com/DoomyDwyer";
const char* kVendorEmail = "doomy@puscii.nl";

// --- Plugin Options 
const bool kWantSidechain = false;
const uint32_t kLatencyInSamples = 0;
const double kTailTimeMsec = 0.000;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = 0;

#endif

