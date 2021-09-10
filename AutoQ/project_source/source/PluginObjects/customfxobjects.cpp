#include "customfxobjects.h"

#include "utilities.h"

AutoQEnvelopeFollower::AutoQEnvelopeFollower()
{
	// --- setup the filter
	ZVAFilterParameters filterParams;
	filterParams.filterAlgorithm = vaFilterAlgorithm::kSVF_LP;
	filterParams.fc = 1000.0;
	filterParams.enableGainComp = false;
	filterParams.matchAnalogNyquistLPF = true;
	filterParams.selfOscillate = false;
	filterParams.enableNLP = true;
	filter.setParameters(filterParams);

	// --- setup the detector
	AudioDetectorParameters adParams;
	adParams.attackTime_mSec = 20.0;
	adParams.releaseTime_mSec = 500.0;
	adParams.detectMode = TLD_AUDIO_DETECT_MODE_RMS;
	adParams.detect_dB = true;
	adParams.clampToUnityMax = false;
	detector.setParameters(adParams);
} /* C-TOR */

bool AutoQEnvelopeFollower::reset(double _sampleRate)
{
	filter.reset(_sampleRate);
	detector.reset(_sampleRate);
	return true;
}

AutoQEnvelopeFollowerParameters AutoQEnvelopeFollower::getParameters() { return parameters; }

void AutoQEnvelopeFollower::setParameters(const AutoQEnvelopeFollowerParameters& params)
{
	updateFilterParameters(params);
	updateDetectorParameters(params);

	// --- save
	parameters = params;
}

bool AutoQEnvelopeFollower::canProcessAudioFrame() { return false; }

double AutoQEnvelopeFollower::processAudioSample(double xn)
{
	// --- calc threshold
	const double threshValue = pow(10.0, parameters.threshold_dB / 20.0);

	// --- detect the signal
	const double detect_dB = detector.processAudioSample(xn);
	const double detectValue = pow(10.0, detect_dB / 20.0);
	const double deltaValue = detectValue - threshValue;

	ZVAFilterParameters filterParams = filter.getParameters();
	filterParams.fc = parameters.fc;
	filterParams.Q = parameters.Q;

	// --- if above the threshold, modulate the filter fc
	if (deltaValue > 0.0) // || delta_dB > 0.0)
	{
		// --- fc Computer
		// --- best results are with linear values when detector is in dB mode
		const double modulatorValue = (deltaValue * parameters.sensitivity);

		// --- calculate modulated frequency
		filterParams.fc = doUnipolarModulationFromMin(modulatorValue, parameters.fc, kMaxFilterFrequency);
	}

	// --- update with new modulated frequency
	filter.setParameters(filterParams);

	// --- perform the filtering operation
	return filter.processAudioSample(xn);
}

bool AutoQEnvelopeFollower::filterParametersUpdated(const ZVAFilterParameters filterParams, const AutoQEnvelopeFollowerParameters& params)
{
	return filterParams.filterAlgorithm != params.filterAlgorithm ||
		!isFloatEqual(filterParams.fc, params.fc) ||
		!isFloatEqual(filterParams.Q, params.Q) ||
		!isFloatEqual(filterParams.enableGainComp, params.enableGainComp) ||
		!isFloatEqual(filterParams.filterOutputGain_dB, params.filterOutputGain_dB) ||
		filterParams.matchAnalogNyquistLPF != params.matchAnalogNyquistLPF ||
		filterParams.enableNLP != params.enableNLP ||
		filterParams.selfOscillate != params.selfOscillate;
}


void AutoQEnvelopeFollower::updateFilterParameters(const AutoQEnvelopeFollowerParameters& params)
{
	ZVAFilterParameters filterParams = filter.getParameters();
	if (filterParametersUpdated(filterParams, params))
	{
		filterParams.filterAlgorithm = params.filterAlgorithm;
		filterParams.fc = params.fc;
		filterParams.Q = params.Q;
		filterParams.enableGainComp = params.enableGainComp;
		filterParams.filterOutputGain_dB = params.filterOutputGain_dB;
		filterParams.matchAnalogNyquistLPF = params.matchAnalogNyquistLPF;
		filterParams.enableNLP = params.enableNLP;
		filterParams.selfOscillate = params.selfOscillate;
		filter.setParameters(filterParams);
	}
}

bool AutoQEnvelopeFollower::detectorParametersUpdated(const AudioDetectorParameters adParams, const AutoQEnvelopeFollowerParameters& params)
{
	return !isFloatEqual(adParams.attackTime_mSec, params.attackTime_mSec) ||
		!isFloatEqual(adParams.releaseTime_mSec, params.releaseTime_mSec);
}

void AutoQEnvelopeFollower::updateDetectorParameters(const AutoQEnvelopeFollowerParameters& params)
{
	AudioDetectorParameters adParams = detector.getParameters();

	if (detectorParametersUpdated(adParams, params))
	{
		adParams.attackTime_mSec = params.attackTime_mSec;
		adParams.releaseTime_mSec = params.releaseTime_mSec;
		detector.setParameters(adParams);
	}
}
