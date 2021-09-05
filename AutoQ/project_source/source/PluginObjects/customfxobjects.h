#pragma once
#include "fxobjects.h"

/**
\struct AutoQEnvelopeFollowerParameters
\ingroup FX-Objects
\brief
Custom parameter structure for the AutoQEnvelopeFollower object.

\author Steve Dwyer - Adapted from Will Pirkle http://www.willpirkle.com
\remark This object is based on the EnvelopeFollowerParameters class included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
struct AutoQEnvelopeFollowerParameters
{
	AutoQEnvelopeFollowerParameters()
	= default;

	/** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
	AutoQEnvelopeFollowerParameters& operator=(const AutoQEnvelopeFollowerParameters& params)
	// need this override for collections to work
	{
		if (this == &params)
			return *this;

		// Filter parameters
		filterAlgorithm = params.filterAlgorithm;
		fc = params.fc;
		Q = params.Q;
		filterOutputGain_dB = params.filterOutputGain_dB;
		enableGainComp = params.enableGainComp;
		matchAnalogNyquistLPF = params.matchAnalogNyquistLPF;
		selfOscillate = params.selfOscillate;
		enableNLP = params.enableNLP;

		// Detector parameters
		attackTime_mSec = params.attackTime_mSec;
		releaseTime_mSec = params.releaseTime_mSec;
		threshold_dB = params.threshold_dB;
		sensitivity = params.sensitivity;

		return *this;
	}

	// Filter parameters
	vaFilterAlgorithm filterAlgorithm = vaFilterAlgorithm::kSVF_LP; ///< va filter algorithm
	double fc = 1000.0; ///< va filter fc
	double Q = 0.707; ///< va filter Q
	double filterOutputGain_dB = 0.0; ///< va filter gain (normally unused)
	bool enableGainComp = false; ///< enable gain compensation (see book)
	bool matchAnalogNyquistLPF = true; ///< match analog gain at Nyquist
	bool selfOscillate = false; ///< enable selfOscillation
	bool enableNLP = true; ///< enable non linear processing (use oversampling for best results)

	// Detector parameters
	double attackTime_mSec = 20.0; ///< detector attack time
	double releaseTime_mSec = 500.0; ///< detector release time
	double threshold_dB = -6.0; ///< detector threshold in dB
	double sensitivity = 1.0; ///< detector sensitivity
};

/**
\class EnvelopeFollower
\ingroup FX-Objects
\brief
The EnvelopeFollower object implements a traditional envelope follower effect modulating a LPR fc value
using the strength of the detected input.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use EnvelopeFollowerParameters structure to get/set object params.

\author Will Pirkle http://www.willpirkle.com
\remark This object is included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2018 / 09 / 7
*/
class AutoQEnvelopeFollower : public IAudioSignalProcessor
{
public:
	AutoQEnvelopeFollower();

	virtual ~AutoQEnvelopeFollower()
	= default; /* D-TOR */

	/** reset members to initialized state */
	bool reset(double _sampleRate) override;

	/** get parameters: note use of custom structure for passing param data */
	/**
	\return EnvelopeFollowerParameters custom data structure
	*/
	virtual AutoQEnvelopeFollowerParameters getParameters();

	/** set parameters: note use of custom structure for passing param data */
	/**
	\param params custom data structure
	*/
	virtual void setParameters(const AutoQEnvelopeFollowerParameters& params);

	/** return false: this object only processes samples */
	bool canProcessAudioFrame() override;

	/** process input x(n) through the envelope follower to produce return value y(n) */
	/**
	\param xn input
	\return the processed sample
	*/
	double processAudioSample(double xn) override;

protected:
	AutoQEnvelopeFollowerParameters parameters; ///< object parameters

	// --- 1 filter and 1 detector
	ZVAFilter filter; ///< filter to modulate
	AudioDetector detector; ///< detector to track input signal

private:
	virtual bool filterParametersUpdated(ZVAFilterParameters filterParams, const AutoQEnvelopeFollowerParameters& params);

	virtual void updateFilterParameters(const AutoQEnvelopeFollowerParameters& params);

	virtual bool detectorParametersUpdated(AudioDetectorParameters adParams, const AutoQEnvelopeFollowerParameters& params);

	virtual void updateDetectorParameters(const AutoQEnvelopeFollowerParameters& params);
};
