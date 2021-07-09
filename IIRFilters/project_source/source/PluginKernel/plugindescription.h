// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace) 
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_D90FD75058753F18B388FC9353868A00
#define AU_COCOA_VIEW_NAME AUCocoaView_D90FD75058753F18B388FC9353868A00

// --- BUNDLE IDs (MacOS Only) 
const char* kAAXBundleID = "developer.aax.iirfilters.bundleID";
const char* kAUBundleID = "developer.au.iirfilters.bundleID";
const char* kVST3BundleID = "developer.vst3.iirfilters.bundleID";

// --- Plugin Names 
const char* kPluginName = "Filterizer";
const char* kShortPluginName = "Filterizer";
const char* kAUBundleName = "IIRFilters";

// --- Plugin Type 
const pluginType kPluginType = pluginType::kFXPlugin;

// --- VST3 UUID 
const char* kVSTFUID = "{d90fd750-5875-3f18-b388-fc9353868a00}";

// --- 4-char codes 
const int32_t kFourCharCode = 'DFDS';
const int32_t kAAXProductID = 'DFDS';
const int32_t kManufacturerID = 'ASPK';

// --- Vendor information 
const char* kVendorName = "My Company";
const char* kVendorURL = "www.myplugins.com";
const char* kVendorEmail = "support@myplugins.com";

// --- Plugin Options 
const bool kWantSidechain = false;
const uint32_t kLatencyInSamples = 0;
const double kTailTimeMsec = 0.000000;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = 0;

#endif
