// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.h
//
/**
    \file   plugincore.h
    \author Will Pirkle
    \date   17-September-2018
    \brief  base class interface file for ASPiK plugincore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#ifndef __pluginCore_h__
#define __pluginCore_h__

#include "pluginbase.h"
#include "../../../../ASPiKCommon/dsp/customfxobjects.h"

// **--0x7F1F--**

// --- Plugin Variables controlID Enumeration 

enum controlID {
	lfoRate_Hz = 0,
	lfoDepth_Pct = 1,
	feedback_Pct = 2,
	fx_On = 3,
	fx_OnOff_Toggle = 4
};

	// **--0x0F1F--**

/**
\class PluginCore
\ingroup ASPiK-Core
\brief
The PluginCore object is the default PluginBase derived object for ASPiK projects.
Note that you are fre to change the name of this object (as long as you change it in the compiler settings, etc...)


PluginCore Operations:
- overrides the main processing functions from the base class
- performs reset operation on sub-modules
- processes audio
- processes messages for custom views
- performs pre and post processing functions on parameters and audio (if needed)

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class PluginCore : public PluginBase
{
public:
    PluginCore();

	/** Destructor: empty in default version */
    ~PluginCore() override = default;

    // Suppress generation of copy constructor and copy assignment operator
    PluginCore(const PluginCore&) = delete;
    virtual PluginCore& operator=(const PluginCore&) = delete;

    // Suppress generation of move constructor and move assignment operator
    PluginCore(const PluginCore&&) = delete;
    virtual PluginCore& operator=(const PluginCore&&) = delete;

	// --- PluginBase Overrides ---
	//
	/** this is the creation function for all plugin parameters */
	bool initPluginParameters();

	/** called when plugin is loaded, a new audio file is playing or sample rate changes */
	bool reset(ResetInfo& resetInfo) override;

	/** one-time post creation init function; pluginInfo contains path to this plugin */
	bool initialize(PluginInfo& _pluginInfo) override;

	/** preProcess: sync GUI parameters here; override if you don't want to use automatic variable-binding */
	bool preProcessAudioBuffers(ProcessBufferInfo& processInfo) override;

    /**
     * \brief
     * Bypass - renders FX pass-through
     *
     \param processFrameInfo structure of information about *frame* processing

     */
    virtual void renderFXPassThrough(ProcessFrameInfo& processFrameInfo);

	/** process frames of data (DEFAULT MODE) */
	bool processAudioFrame(ProcessFrameInfo& processFrameInfo) override;

	/** Pre-process the block with: MIDI events for the block and parametet smoothing */
	bool preProcessAudioBlock(IMidiEventQueue* midiEventQueue = nullptr) override;

	/** process sub-blocks of data (OPTIONAL MODE) */
	bool processAudioBlock(ProcessBlockInfo& processBlockInfo) override;

	/** This is the native buffer processing function; you may override and implement
	     it if you want to roll your own buffer or block procssing code */
	// virtual bool processAudioBuffers(ProcessBufferInfo& processBufferInfo);

	/** preProcess: do any post-buffer processing required; default operation is to send metering data to GUI  */
	bool postProcessAudioBuffers(ProcessBufferInfo& processInfo) override;

	/** called by host plugin at top of buffer proccess; this alters parameters prior to variable binding operation  */
	bool updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo) override;

	/** called by host plugin at top of buffer proccess; this alters parameters prior to variable binding operation  */
	bool updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo) override;

	/** this can be called: 1) after bound variable has been updated or 2) after smoothing occurs  */
	bool postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo) override;

	/** this is ony called when the user makes a GUI control change */
	bool guiParameterChanged(int32_t controlID, double actualValue) override;

	/** processMessage: messaging system; currently used for custom/special GUI operations */
	bool processMessage(MessageInfo& messageInfo) override;

	/** processMIDIEvent: MIDI event processing */
	bool processMIDIEvent(midiEvent& event) override;

	/** specialized joystick servicing (currently not used) */
	bool setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData) override;

	/** create the presets */
	bool initPluginPresets();

	// --- example block processing template functions (OPTIONAL)
	//
	/** FX EXAMPLE: process audio by passing through */
	bool renderFXPassThrough(ProcessBlockInfo& blockInfo);

	/** SYNTH EXAMPLE: render a block of silence */
	bool renderSynthSilence(ProcessBlockInfo& blockInfo);

	// --- BEGIN USER VARIABLES AND FUNCTIONS -------------------------------------- //
	//	   Add your variables and methods here



	// --- END USER VARIABLES AND FUNCTIONS -------------------------------------- //

protected:
    SimpleChorus chorus;
    void updateParameters();

private:
	//  **--0x07FD--**

	// --- Continuous Plugin Variables 
	double lfoRate_Hz = 0.0;
	double lfoDepth_Pct = 0.0;
	double feedback_Pct = 0.0;

	// --- Discrete Plugin Variables 
	int fx_On = 0;
	enum class fx_OnEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(fx_OnEnum::SWITCH_OFF, fx_On)) etc... 

	int fx_OnOff_Toggle = 0;
	enum class fx_OnOff_ToggleEnum { SWITCH_OFF,SWITCH_ON };	// to compare: if(compareEnumToInt(fx_OnOff_ToggleEnum::SWITCH_OFF, fx_OnOff_Toggle)) etc... 

	// **--0x1A7F--**
    // --- end member variables

public:
    /** static description: bundle folder name

	\return bundle folder name as a const char*
	*/
    static const char* getPluginBundleName();

    /** static description: name

	\return name as a const char*
	*/
    static const char* getPluginName();

	/** static description: short name

	\return short name as a const char*
	*/
	static const char* getShortPluginName();

	/** static description: vendor name

	\return vendor name as a const char*
	*/
	static const char* getVendorName();

	/** static description: URL

	\return URL as a const char*
	*/
	static const char* getVendorURL();

	/** static description: email

	\return email address as a const char*
	*/
	static const char* getVendorEmail();

	/** static description: Cocoa View Factory Name

	\return Cocoa View Factory Name as a const char*
	*/
	static const char* getAUCocoaViewFactoryName();

	/** static description: plugin type

	\return type (FX or Synth)
	*/
	static pluginType getPluginType();

	/** static description: VST3 GUID

	\return VST3 GUID as a const char*
	*/
	static const char* getVSTFUID();

	/** static description: 4-char code

	\return 4-char code as int
	*/
	static int32_t getFourCharCode();

	/** initalizer */
	bool initPluginDescriptors();

    /** Status Window Messages for hosts that can show it */
    void sendHostTextMessage(std::string messageString)
    {
        HostMessageInfo hostMessageInfo;
        hostMessageInfo.hostMessage = sendRAFXStatusWndText;
        hostMessageInfo.rafxStatusWndText.assign(messageString);
        if(pluginHostConnector)
            pluginHostConnector->sendHostMessage(hostMessageInfo);
    }

};




#endif /* defined(__pluginCore_h__) */
