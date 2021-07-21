// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_63A0D13B42E14E06A9B43D160346F96E
#define AU_COCOA_VIEW_NAME AUCocoaView_63A0D13B42E14E06A9B43D160346F96E

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "doomsvillesoundscapes.aax.IIRFilters.bundleID";
const char* kAUBundleID = "doomsvillesoundscapes.au.IIRFilters.bundleID";
const char* kVST3BundleID = "doomsvillesoundscapes.vst3.IIRFilters.bundleID";

// --- Plugin Names 
const char* kPluginName = "IIRFilters";
const char* kShortPluginName = "IIRFilters";
const char* kAUBundleName = "IIRFilters";

// --- Plugin Type 
const pluginType kPluginType = pluginType::kFXPlugin;

// --- VST3 UUID 
const char* kVSTFUID = "{63A0D13B-42E1-4E06-A9B4-3D160346F96E}";

// --- 4-char codes 
const int32_t kFourCharCode = 'IIRF';
const int32_t kAAXProductID = 'IIRF';
const int32_t kManufacturerID = 'DOOM';

// --- Vendor information 
const char* kVendorName = "Doomsville Soundscapes";
const char* kVendorURL = "https://github.com/DoomyDwyer";
const char* kVendorEmail = "doomy@puscii.nl";

// --- Plugin Options 
const bool kWantSidechain = true;
const uint32_t kLatencyInSamples = 0;
const double kTailTimeMsec = 0.000;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = 0;

#endif

