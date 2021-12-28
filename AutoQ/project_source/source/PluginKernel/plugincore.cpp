// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.cpp
//
/**
    \file   plugincore.cpp
    \author Will Pirkle
    \date   17-September-2018
    \brief  Implementation file for PluginCore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#include "plugincore.h"
#include "plugindescription.h"
#pragma warning (disable : 4244)

/**
\brief PluginCore constructor is launching pad for object initialization

Operations:
- initialize the plugin description (strings, codes, numbers, see initPluginDescriptors())
- setup the plugin's audio I/O channel support
- create the PluginParameter objects that represent the plugin parameters (see FX book if needed)
- create the presets
*/
PluginCore::PluginCore()
{
    // --- describe the plugin; call the helper to init the static parts you setup in plugindescription.h
    initPluginDescriptors();

    // --- default I/O combinations
	// --- for FX plugins
	if (getPluginType() == kFXPlugin)
	{
		addSupportedIOCombination({ kCFMono, kCFMono });
		addSupportedIOCombination({ kCFMono, kCFStereo });
		addSupportedIOCombination({ kCFStereo, kCFStereo });
	}
	else // --- synth plugins have no input, only output
	{
		addSupportedIOCombination({ kCFNone, kCFMono });
		addSupportedIOCombination({ kCFNone, kCFStereo });
	}

	// --- for sidechaining, we support mono and stereo inputs; auxOutputs reserved for future use
	addSupportedAuxIOCombination({ kCFMono, kCFNone });
	addSupportedAuxIOCombination({ kCFStereo, kCFNone });

	// --- create the parameters
    initPluginParameters();

    // --- create the presets
    initPluginPresets();
}

void PluginCore::updateParameters()
{
	AutoQEnvelopeFollowerParameters params = envFollowers[0].getParameters();

	// --- Filter Variables 
	params.filterAlgorithm = convertIntToEnum(filterType + FILTER_ENUM_OFFSET, vaFilterAlgorithm);
	params.fc = filterFc_Hz;
	params.Q = filterQ;
	params.filterOutputGain_dB = filterOutputGain_dB;
	params.enableGainComp = enableGainComp;
	params.matchAnalogNyquistLPF = matchAnalogNyquistLPF;
	params.selfOscillate = selfOscillate;
	params.enableNLP = enableNLP;

	// Detector variables
	params.attackTime_mSec = attackTime_mSec;
	params.releaseTime_mSec = releaseTime_mSec;
	params.threshold_dB = threshold_dB;
	params.sensitivity = sensitivity;

    for (auto& envFollower : envFollowers)
    {
	    envFollower.setParameters(params);
    }
}

/**
\brief initialize object for a new run of audio; called just before audio streams

Operation:
- store sample rate and bit depth on audioProcDescriptor - this information is globally available to all core functions
- reset your member objects here

\param resetInfo structure of information about current audio format

\return true if operation succeeds, false otherwise
*/
bool PluginCore::reset(ResetInfo& resetInfo)
{
    // --- save for audio processing
    audioProcDescriptor.sampleRate = resetInfo.sampleRate;
    audioProcDescriptor.bitDepth = resetInfo.bitDepth;

	// --- Reset the filters
    for (auto& envFollower : envFollowers)
    {
	    envFollower.reset(resetInfo.sampleRate);
    }
    // --- other reset inits
    return PluginBase::reset(resetInfo);
}

/**
\brief one-time initialize function called after object creation and before the first reset( ) call

Operation:
- saves structure for the plugin to use; you can also load WAV files or state information here
*/
bool PluginCore::initialize(PluginInfo& pluginInfo)
{
	// --- add one-time init stuff here

	return true;
}

/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- syncInBoundVariables when preProcessAudioBuffers is called, it is *guaranteed* that all GUI control change information
  has been applied to plugin parameters; this binds parameter changes to your underlying variables
- NOTE: postUpdatePluginParameter( ) will be called for all bound variables that are acutally updated; if you need to process
  them individually, do so in that function
- use this function to bulk-transfer the bound variable data into your plugin's member object variables

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::preProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
    // --- sync internal variables to GUI parameters; you can also do this manually if you don't
    //     want to use the auto-variable-binding
    syncInBoundVariables();

    return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--- FRAME PROCESSING FUNCTION --- //
/*
	This is where you do your DSP processing on a PER-FRAME basis; this means that you are
	processing one sample per channel at a time, and is the default mechanism in ASPiK.

	You will get better performance is you process buffers or blocks instead, HOWEVER for
	some kinds of processing (e.g. ping pong delay) you must process on a per-sample
	basis anyway. This is also easier to understand for most newbies.

	NOTE:
	You can enable and disable frame/buffer procssing in the constructor of this object:

	// --- to process audio frames call:
	processAudioByFrames();

	// --- to process complete DAW buffers call:
	processAudioByBlocks(WANT_WHOLE_BUFFER);

	// --- to process sub-blocks of the incoming DAW buffer in 64 sample blocks call:
	processAudioByBlocks(DEFAULT_AUDIO_BLOCK_SIZE);

	// --- to process sub-blocks of size N, call:
	processAudioByBlocks(N);
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
\brief frame-processing method

Operation:
- decode the plugin type - for synth plugins, fill in the rendering code; for FX plugins, delete the if(synth) portion and add your processing code
- note that MIDI events are fired for each sample interval so that MIDI is tightly sunk with audio
- doSampleAccurateParameterUpdates will perform per-sample interval smoothing

\param processFrameInfo structure of information about *frame* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processAudioFrame(ProcessFrameInfo& processFrameInfo)
{
    // --- fire any MIDI events for this sample interval
    processFrameInfo.midiEventQueue->fireMidiEvents(processFrameInfo.currentFrame);

	// --- do per-frame smoothing
	doParameterSmoothing();

	// --- call your GUI update/cooking function here, now that smoothing has occurred
	//
	//     NOTE:
	//     updateParameters is the name used in Will Pirkle's books for the GUI update function
	//     you may name it what you like - this is where GUI control values are cooked
	//     for the DSP algorithm at hand
	updateParameters();


    // --- decode the channelIOConfiguration and process accordingly
    //
	// --- Synth Plugin:
	// --- Synth Plugin --- remove for FX plugins
	if (getPluginType() == kSynthPlugin)
	{
		// --- output silence: change this with your signal render code
		processFrameInfo.audioOutputFrame[0] = 0.0;
		if (processFrameInfo.channelIOConfig.outputChannelFormat == kCFStereo)
			processFrameInfo.audioOutputFrame[1] = 0.0;

		return true;	/// processed
	}

    // --- FX Plugin:
	vuMeter = 0.f;

	for (unsigned int i = 0; i < NUM_CHANNELS; i++)
    {
	    if (fx_On)
	    {
		    // --- Read input
		    double xn = processFrameInfo.audioInputFrame[i];

		    // --- Process the audio to produce output
		    double yn = envFollowers[i].processAudioSample(xn);

		    // --- Write output
		    processFrameInfo.audioOutputFrame[i] = yn;

		    // � sum all channels into 1 single value for Vu Meter
		    vuMeter += yn / NUM_CHANNELS;
	    }
	    else
	    {
		    // Bypass
		    processFrameInfo.audioOutputFrame[i] = processFrameInfo.audioInputFrame[i];
	    }
    }

	return true; /// processed
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--- BLOCK/BUFFER PRE-PROCESSING FUNCTION --- //
//      Only used when BLOCKS or BUFFERS are processed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
\brief pre-process the audio block

Operation:
- fire MIDI events for the audio block; see processMIDIEvent( ) for the code that loads
  the vector on the ProcessBlockInfo structure

\param IMidiEventQueue ASPIK event queue of MIDI events for the entire buffer; this
       function only fires the MIDI events for this audio block

\return true if operation succeeds, false otherwise
*/
bool PluginCore::preProcessAudioBlock(IMidiEventQueue* midiEventQueue)
{
	// --- pre-process the block
	processBlockInfo.clearMidiEvents();

	// --- sample accurate parameter updates
	for (uint32_t sample = processBlockInfo.blockStartIndex;
		sample < processBlockInfo.blockStartIndex + processBlockInfo.blockSize;
		sample++)
	{
		// --- the MIDI handler will load up the vector in processBlockInfo
		if (midiEventQueue)
			midiEventQueue->fireMidiEvents(sample);
	}

	// --- this will do parameter smoothing ONLY ONCE AT THE TOP OF THE BLOCK PROCESSING
	//
	// --- to perform per-sample parameter smoothing, move this line of code, AND your updating
	//     functions (see updateParameters( ) in comment below) into the for( ) loop above
	//     NOTE: smoothing only once per block usually SAVES CPU cycles
	//           smoothing once per sample period usually EATS CPU cycles, potentially unnecessarily
	doParameterSmoothing();

	// --- call your GUI update/cooking function here, now that smoothing has occurred
	//
	//     NOTE:
	//     updateParameters is the name used in Will Pirkle's books for the GUI update function
	//     you may name it what you like - this is where GUI control values are cooked
	//     for the DSP algorithm at hand
	//     NOTE: updating (cooking) only once per block usually SAVES CPU cycles
	//           updating (cooking) once per sample period usually EATS CPU cycles, potentially unnecessarily
	// updateParameters();

	return true;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--- BLOCK/BUFFER PROCESSING FUNCTION --- //
/*
This is where you do your DSP processing on a PER-BLOCK basis; this means that you are
processing entire blocks at a time -- each audio channel has its own block to process.

A BUFFER is simply a single block of audio that is the size of the incoming buffer from the
DAW. A BLOCK is a sub-block portion of that buffer.

In the event that the incoming buffer is SMALLER than your requested audio block, the
entire buffer will be sent to this block processing function. This is also true when your
block size is not a divisor of the actual incoming buffer, OR when an incoming buffer
is partially filled (which is rare, but may happen under certain circumstances), resulting
in a "partial block" of data that is smaller than your requested block size.

NOTE:
You can enable and disable frame/buffer procssing in the constructor of this object:

// --- to process audio frames call:
processAudioByFrames();

// --- to process complete DAW buffers call:
processAudioByBlocks(WANT_WHOLE_BUFFER);

// --- to process sub-blocks of the incoming DAW buffer in 64 sample blocks call:
processAudioByBlocks(DEFAULT_AUDIO_BLOCK_SIZE);

// --- to process sub-blocks of size N, call:
processAudioByBlocks(N);
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
\brief block or buffer-processing method

Operation:
- process one block of audio data; see example functions for template code
- renderSynthSilence: render a block of 0.0 values (synth, silence when no notes are rendered)
- renderFXPassThrough: pass audio from input to output (FX)

\param processBlockInfo structure of information about *block* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processAudioBlock(ProcessBlockInfo& processBlockInfo)
{
	// --- FX or Synth Render
	//     call your block processing function here
	// --- Synth
	if (getPluginType() == kSynthPlugin)
		renderSynthSilence(processBlockInfo);

	// --- or FX
	else if (getPluginType() == kFXPlugin)
		renderFXPassThrough(processBlockInfo);

	return true;
}


/**
\brief
renders a block of silence (all 0.0 values) as an example
your synth code would render the synth using the MIDI messages and output buffers

Operation:
- process all MIDI events for the block
- perform render into block's audio buffers

\param blockInfo structure of information about *block* processing
\return true if operation succeeds, false otherwise
*/
bool PluginCore::renderSynthSilence(ProcessBlockInfo& blockInfo)
{
	// --- process all MIDI events in this block (same as SynthLab)
	uint32_t midiEvents = blockInfo.getMidiEventCount();
	for (uint32_t i = 0; i < midiEvents; i++)
	{
		// --- get the event
		midiEvent event = *blockInfo.getMidiEvent(i);

		// --- do something with it...
		// myMIDIMessageHandler(event); // <-- you write this
	}

	// --- render a block of audio; here it is silence but in your synth
	//     it will likely be dependent on the MIDI processing you just did above
	for (uint32_t sample = blockInfo.blockStartIndex, i = 0;
		 sample < blockInfo.blockStartIndex + blockInfo.blockSize;
		 sample++, i++)
	{
		// --- write outputs
		for (uint32_t channel = 0; channel < blockInfo.numAudioOutChannels; channel++)
		{
			// --- silence (or, your synthesized block of samples)
			blockInfo.outputs[channel][sample] = 0.0;
		}
	}
	return true;
}

/**
\brief
Renders pass-through code as an example; replace with meaningful DSP for audio goodness

Operation:
- loop over samples in block
- write inputs to outputs, per channel basis

\param blockInfo structure of information about *block* processing
\return true if operation succeeds, false otherwise
*/
bool PluginCore::renderFXPassThrough(ProcessBlockInfo& blockInfo)
{
	// --- block processing -- write to outputs
	for (uint32_t sample = blockInfo.blockStartIndex, i = 0;
		sample < blockInfo.blockStartIndex + blockInfo.blockSize;
		sample++, i++)
	{
		// --- handles multiple channels, but up to you for bookkeeping
		for (uint32_t channel = 0; channel < blockInfo.numAudioOutChannels; channel++)
		{
			// --- pass through code, or your processed FX version
			blockInfo.outputs[channel][sample] = blockInfo.inputs[channel][sample];
		}
	}
	return true;
}


/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- updateOutBoundVariables sends metering data to the GUI meters

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
	// --- update outbound variables; currently this is meter data only, but could be extended
	//     in the future
	updateOutBoundVariables();

    return true;
}

/**
\brief update the PluginParameter's value based on GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- use base class helper
    setPIParamValue(controlID, controlValue);

    // --- do any post-processing
    postUpdatePluginParameter(controlID, controlValue, paramInfo);

    return true; /// handled
}

/**
\brief update the PluginParameter's value based on *normlaized* GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param normalizedValue the new control value in normalized form
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo)
{
	// --- use base class helper, returns actual value
	double controlValue = setPIParamValueNormalized(controlID, normalizedValue, paramInfo.applyTaper);

	// --- do any post-processing
	postUpdatePluginParameter(controlID, controlValue, paramInfo);

	return true; /// handled
}

/**
\brief perform any operations after the plugin parameter has been updated; this is one paradigm for
	   transferring control information into vital plugin variables or member objects. If you use this
	   method you can decode the control ID and then do any cooking that is needed. NOTE: do not
	   overwrite bound variables here - this is ONLY for any extra cooking that is required to convert
	   the GUI data to meaninful coefficients or other specific modifiers.

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- now do any post update cooking; be careful with VST Sample Accurate automation
    //     If enabled, then make sure the cooking functions are short and efficient otherwise disable it
    //     for the Parameter involved
    /*switch(controlID)
    {
        case 0:
        {
            return true;    /// handled
        }

        default:
            return false;   /// not handled
    }*/

    return false;
}

/**
\brief has nothing to do with actual variable or updated variable (binding)

CAUTION:
- DO NOT update underlying variables here - this is only for sending GUI updates or letting you
  know that a parameter was changed; it should not change the state of your plugin.

WARNING:
- THIS IS NOT THE PREFERRED WAY TO LINK OR COMBINE CONTROLS TOGETHER. THE PROPER METHOD IS
  TO USE A CUSTOM SUB-CONTROLLER THAT IS PART OF THE GUI OBJECT AND CODE.
  SEE http://www.willpirkle.com for more information

\param controlID the control ID value of the parameter being updated
\param actualValue the new control value

\return true if operation succeeds, false otherwise
*/
bool PluginCore::guiParameterChanged(int32_t controlID, double actualValue)
{
	/*
	switch (controlID)
	{
		case controlID::<your control here>
		{

			return true; // handled
		}

		default:
			break;
	}*/

	return false; /// not handled
}

/**
\brief For Custom View and Custom Sub-Controller Operations

NOTES:
- this is for advanced users only to implement custom view and custom sub-controllers
- see the SDK for examples of use

\param messageInfo a structure containing information about the incoming message

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMessage(MessageInfo& messageInfo)
{
	// --- decode message
	switch (messageInfo.message)
	{
		// --- add customization appearance here
	case PLUGINGUI_DIDOPEN:
	{
		return false;
	}

	// --- NULL pointers so that we don't accidentally use them
	case PLUGINGUI_WILLCLOSE:
	{
		return false;
	}

	// --- update view; this will only be called if the GUI is actually open
	case PLUGINGUI_TIMERPING:
	{
		return false;
	}

	// --- register the custom view, grab the ICustomView interface
	case PLUGINGUI_REGISTER_CUSTOMVIEW:
	{

		return false;
	}

	case PLUGINGUI_REGISTER_SUBCONTROLLER:
	case PLUGINGUI_QUERY_HASUSERCUSTOM:
	case PLUGINGUI_USER_CUSTOMOPEN:
	case PLUGINGUI_USER_CUSTOMCLOSE:
	case PLUGINGUI_EXTERNAL_SET_NORMVALUE:
	case PLUGINGUI_EXTERNAL_SET_ACTUALVALUE:
	{

		return false;
	}

	default:
		break;
	}

	return false; /// not handled
}


/**
\brief process a MIDI event

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param event a structure containing the MIDI event data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMIDIEvent(midiEvent& event)
{
	// --- IF PROCESSING AUDIO BLOCKS: push into vector for block processing
	if (!pluginDescriptor.processFrames)
	{
		processBlockInfo.pushMidiEvent(event);
		return true;
	}

	// --- IF PROCESSING AUDIO FRAMES: decode AND service this MIDI event here
	//     for sample accurate MIDI
	// myMIDIMessageHandler(event); // <-- you write this

	return true;
}

/**
\brief (for future use)

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param vectorJoysickData a structure containing joystick data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData)
{
	return true;
}

/**
\brief create all of your plugin parameters here

\return true if parameters were created, false if they already existed
*/
bool PluginCore::initPluginParameters()
{
	if (pluginParameterMap.size() > 0)
		return false;

	// --- Add your plugin parameter instantiation code bewtween these hex codes
	// **--0xDEA7--**


	// --- Declaration of Plugin Parameter Objects 
	PluginParameter* piParam = nullptr;

	// --- discrete control: On
	piParam = new PluginParameter(controlID::fx_On, "On", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&fx_On, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Cutoff
	piParam = new PluginParameter(controlID::filterFc_Hz, "Cutoff", "Hz", controlVariableType::kDouble, 20.000000, 10000.000000, 1000.000000, taper::kVoltOctaveTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&filterFc_Hz, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Q
	piParam = new PluginParameter(controlID::filterQ, "Q", "", controlVariableType::kDouble, 0.707000, 20.000000, 0.707000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&filterQ, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Attack
	piParam = new PluginParameter(controlID::attackTime_mSec, "Attack", "mSec", controlVariableType::kDouble, 1.000000, 250.000000, 20.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&attackTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Release
	piParam = new PluginParameter(controlID::releaseTime_mSec, "Release", "mSec", controlVariableType::kDouble, 1.000000, 2000.000000, 500.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&releaseTime_mSec, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Threshold
	piParam = new PluginParameter(controlID::threshold_dB, "Threshold", "dB", controlVariableType::kDouble, -20.000000, 0.000000, -6.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&threshold_dB, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Sensitivity
	piParam = new PluginParameter(controlID::sensitivity, "Sensitivity", "", controlVariableType::kDouble, 0.250000, 5.000000, 1.000000, taper::kAntiLogTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&sensitivity, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Gain
	piParam = new PluginParameter(controlID::filterOutputGain_dB, "Gain", "dB", controlVariableType::kDouble, -20.000000, 12.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&filterOutputGain_dB, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Analog Nyquist
	piParam = new PluginParameter(controlID::matchAnalogNyquistLPF, "Analog Nyquist", "SWITCH OFF,SWITCH ON", "SWITCH ON");
	piParam->setBoundVariable(&matchAnalogNyquistLPF, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Self Oscillate
	piParam = new PluginParameter(controlID::selfOscillate, "Self Oscillate", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&selfOscillate, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Enable NLP
	piParam = new PluginParameter(controlID::enableNLP, "Enable NLP", "SWITCH OFF,SWITCH ON", "SWITCH ON");
	piParam->setBoundVariable(&enableNLP, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Filter Type
	piParam = new PluginParameter(controlID::filterType, "Filter Type", "kSVF LP,kSVF HP,kSVF BP", "kSVF LP");
	piParam->setBoundVariable(&filterType, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- meter control: Meter
	piParam = new PluginParameter(controlID::vuMeter, "Meter", 10.00, 10.00, ENVELOPE_DETECT_MODE_RMS, meterCal::kLinearMeter);
	piParam->setBoundVariable(&vuMeter, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- discrete control: Boost Q
	piParam = new PluginParameter(controlID::enableGainComp, "Boost Q", "BOOST Q OFF,BOOST Q ON", "BOOST Q OFF");
	piParam->setBoundVariable(&enableGainComp, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: On/Off
	piParam = new PluginParameter(controlID::fx_OnOff_Toggle, "On/Off", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&fx_OnOff_Toggle, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- Aux Attributes
	AuxParameterAttribute auxAttribute;

	// --- RAFX GUI attributes
	// --- controlID::fx_On
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::fx_On, auxAttribute);

	// --- controlID::filterFc_Hz
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::filterFc_Hz, auxAttribute);

	// --- controlID::filterQ
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::filterQ, auxAttribute);

	// --- controlID::attackTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::attackTime_mSec, auxAttribute);

	// --- controlID::releaseTime_mSec
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::releaseTime_mSec, auxAttribute);

	// --- controlID::threshold_dB
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::threshold_dB, auxAttribute);

	// --- controlID::sensitivity
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::sensitivity, auxAttribute);

	// --- controlID::filterOutputGain_dB
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::filterOutputGain_dB, auxAttribute);

	// --- controlID::matchAnalogNyquistLPF
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::matchAnalogNyquistLPF, auxAttribute);

	// --- controlID::selfOscillate
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::selfOscillate, auxAttribute);

	// --- controlID::enableNLP
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741824);
	setParamAuxAttribute(controlID::enableNLP, auxAttribute);

	// --- controlID::filterType
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::filterType, auxAttribute);

	// --- controlID::vuMeter
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(134217728);
	setParamAuxAttribute(controlID::vuMeter, auxAttribute);

	// --- controlID::enableGainComp
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::enableGainComp, auxAttribute);

	// --- controlID::fx_OnOff_Toggle
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1610612736);
	setParamAuxAttribute(controlID::fx_OnOff_Toggle, auxAttribute);


	// **--0xEDA5--**

	// --- BONUS Parameter
	// --- SCALE_GUI_SIZE
	PluginParameter* piParamBonus = new PluginParameter(SCALE_GUI_SIZE, "Scale GUI", "tiny,small,medium,normal,large,giant", "normal");
	addPluginParameter(piParamBonus);

	// --- create the super fast access array
	initPluginParameterArray();

	return true;
}

/**
\brief use this method to add new presets to the list

NOTES:
- see the SDK for examples of use
- for non RackAFX users that have large paramter counts, there is a secret GUI control you
  can enable to write C++ code into text files, one per preset. See the SDK or http://www.willpirkle.com for details

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginPresets()
{
	// **--0xFF7A--**

	// --- Plugin Presets 
	int index = 0;
	PresetInfo* preset = nullptr;

	// --- Preset: Factory Preset
	preset = new PresetInfo(index++, "Factory Preset");
	initPresetParameters(preset->presetParameters);
	setPresetParameter(preset->presetParameters, controlID::fx_On, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::filterFc_Hz, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::filterQ, 0.707000);
	setPresetParameter(preset->presetParameters, controlID::attackTime_mSec, 20.000000);
	setPresetParameter(preset->presetParameters, controlID::releaseTime_mSec, 500.000000);
	setPresetParameter(preset->presetParameters, controlID::threshold_dB, -6.000000);
	setPresetParameter(preset->presetParameters, controlID::sensitivity, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filterOutputGain_dB, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::matchAnalogNyquistLPF, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::selfOscillate, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableNLP, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filterType, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableGainComp, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::fx_OnOff_Toggle, 0.000000);
	addPreset(preset);

	// --- Preset: Auto-Wah
	preset = new PresetInfo(index++, "Auto-Wah");
	initPresetParameters(preset->presetParameters);
	setPresetParameter(preset->presetParameters, controlID::fx_On, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filterFc_Hz, 554.112549);
	setPresetParameter(preset->presetParameters, controlID::filterQ, 11.800473);
	setPresetParameter(preset->presetParameters, controlID::attackTime_mSec, 29.959999);
	setPresetParameter(preset->presetParameters, controlID::releaseTime_mSec, 310.095032);
	setPresetParameter(preset->presetParameters, controlID::threshold_dB, -7.000000);
	setPresetParameter(preset->presetParameters, controlID::sensitivity, 0.849574);
	setPresetParameter(preset->presetParameters, controlID::filterOutputGain_dB, 9.280001);
	setPresetParameter(preset->presetParameters, controlID::matchAnalogNyquistLPF, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::selfOscillate, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableNLP, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filterType, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableGainComp, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::fx_OnOff_Toggle, 0.000000);
	addPreset(preset);

	// --- Preset: Auto-Wah II
	preset = new PresetInfo(index++, "Auto-Wah II");
	initPresetParameters(preset->presetParameters);
	setPresetParameter(preset->presetParameters, controlID::fx_On, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filterFc_Hz, 1168.085205);
	setPresetParameter(preset->presetParameters, controlID::filterQ, 10.546430);
	setPresetParameter(preset->presetParameters, controlID::attackTime_mSec, 29.959999);
	setPresetParameter(preset->presetParameters, controlID::releaseTime_mSec, 310.095032);
	setPresetParameter(preset->presetParameters, controlID::threshold_dB, -9.299999);
	setPresetParameter(preset->presetParameters, controlID::sensitivity, 0.473478);
	setPresetParameter(preset->presetParameters, controlID::filterOutputGain_dB, 7.040001);
	setPresetParameter(preset->presetParameters, controlID::matchAnalogNyquistLPF, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::selfOscillate, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableNLP, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filterType, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableGainComp, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::fx_OnOff_Toggle, 0.000000);
	addPreset(preset);

	// --- Preset: Funky Punky
	preset = new PresetInfo(index++, "Funky Punky");
	initPresetParameters(preset->presetParameters);
	setPresetParameter(preset->presetParameters, controlID::fx_On, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filterFc_Hz, 1000.000000);
	setPresetParameter(preset->presetParameters, controlID::filterQ, 13.343911);
	setPresetParameter(preset->presetParameters, controlID::attackTime_mSec, 24.979998);
	setPresetParameter(preset->presetParameters, controlID::releaseTime_mSec, 140.180054);
	setPresetParameter(preset->presetParameters, controlID::threshold_dB, -9.699999);
	setPresetParameter(preset->presetParameters, controlID::sensitivity, 0.624772);
	setPresetParameter(preset->presetParameters, controlID::filterOutputGain_dB, 3.040001);
	setPresetParameter(preset->presetParameters, controlID::matchAnalogNyquistLPF, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::selfOscillate, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableNLP, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::filterType, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::enableGainComp, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::fx_OnOff_Toggle, 0.000000);
	addPreset(preset);


	// **--0xA7FF--**

    return true;
}

/**
\brief setup the plugin description strings, flags and codes; this is ordinarily done through the ASPiKreator or CMake

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginDescriptors()
{
	// --- setup audio procssing style
	//
	// --- kProcessFrames and kBlockSize are set in plugindescription.h
	//
	// --- true:  process audio frames --- less efficient, but easier to understand when starting out
	//     false: process audio blocks --- most efficient, but somewhat more complex code
	pluginDescriptor.processFrames = kProcessFrames;

	// --- for block processing (if pluginDescriptor.processFrame == false),
	//     this is the block size
	processBlockInfo.blockSize = kBlockSize;

    pluginDescriptor.pluginName = PluginCore::getPluginName();
    pluginDescriptor.shortPluginName = PluginCore::getShortPluginName();
    pluginDescriptor.vendorName = PluginCore::getVendorName();
    pluginDescriptor.pluginTypeCode = PluginCore::getPluginType();

	// --- describe the plugin attributes; set according to your needs
	pluginDescriptor.hasSidechain = kWantSidechain;
	pluginDescriptor.latencyInSamples = kLatencyInSamples;
	pluginDescriptor.tailTimeInMSec = kTailTimeMsec;
	pluginDescriptor.infiniteTailVST3 = kVSTInfiniteTail;

    // --- AAX
    apiSpecificInfo.aaxManufacturerID = kManufacturerID;
    apiSpecificInfo.aaxProductID = kAAXProductID;
    apiSpecificInfo.aaxBundleID = kAAXBundleID;  /* MacOS only: this MUST match the bundle identifier in your info.plist file */
    apiSpecificInfo.aaxEffectID = "aaxDeveloper.";
    apiSpecificInfo.aaxEffectID.append(PluginCore::getPluginName());
    apiSpecificInfo.aaxPluginCategoryCode = kAAXCategory;

    // --- AU
    apiSpecificInfo.auBundleID = kAUBundleID;   /* MacOS only: this MUST match the bundle identifier in your info.plist file */
    apiSpecificInfo.auBundleName = kAUBundleName;

    // --- VST3
    apiSpecificInfo.vst3FUID = PluginCore::getVSTFUID(); // OLE string format
    apiSpecificInfo.vst3BundleID = kVST3BundleID;/* MacOS only: this MUST match the bundle identifier in your info.plist file */
	apiSpecificInfo.enableVST3SampleAccurateAutomation = kVSTSAA;
	apiSpecificInfo.vst3SampleAccurateGranularity = kVST3SAAGranularity;

    // --- AU and AAX
    apiSpecificInfo.fourCharCode = PluginCore::getFourCharCode();

    return true;
}

// --- static functions required for VST3/AU only --------------------------------------------- //
const char* PluginCore::getPluginBundleName() { return getPluginDescBundleName(); }
const char* PluginCore::getPluginName(){ return kPluginName; }
const char* PluginCore::getShortPluginName(){ return kShortPluginName; }
const char* PluginCore::getVendorName(){ return kVendorName; }
const char* PluginCore::getVendorURL(){ return kVendorURL; }
const char* PluginCore::getVendorEmail(){ return kVendorEmail; }
const char* PluginCore::getAUCocoaViewFactoryName(){ return AU_COCOA_VIEWFACTORY_STRING; }
pluginType PluginCore::getPluginType(){ return kPluginType; }
const char* PluginCore::getVSTFUID(){ return kVSTFUID; }
int32_t PluginCore::getFourCharCode(){ return kFourCharCode; }
