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

AutoQEnvelopeFollower::~AutoQEnvelopeFollower()
= default; /* D-TOR */

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
const PhaserAPFParameters* Phaser::getPhaserApfParameters()
{
	return nsPhaserParams;
}

PhaserMixCoeffs Phaser::getPhaserMixCoeffs()
{
	return idealPhaserMixCoeffs;
}

filterAlgorithm Phaser::getFilterAlgorithm()
{
	return filterAlgorithm::kAPF1; //kAPF1;  // can also use 2nd order
}

double Phaser::getFilterQ()
{
	return 0.707; //0.001;  // use low Q if using 2nd order APFs
}

Phaser::Phaser()
{
	OscillatorParameters lfoparams = lfo.getParameters();
	lfoparams.waveform = generatorWaveform::kTriangle; // kTriangle LFO for phaser
	lfo.setParameters(lfoparams);

	AudioFilterParameters params = apfs[0].getParameters();
	params.algorithm = getFilterAlgorithm();
	params.Q = getFilterQ();

	for (auto& apf : apfs)
	{
		apf.setParameters(params);
	}
}

Phaser::~Phaser()
= default;

bool Phaser::reset(double _sampleRate)
{
	// --- reset LFO
	lfo.reset(_sampleRate);

	// --- reset APFs
	for (auto& apf : apfs)
	{
		apf.reset(_sampleRate);
	}

	return true;
}

double Phaser::processAudioSample(double xn)
{
	const SignalGenData lfoData = lfo.renderAudioOutput();

	// --- create the bipolar modulator value
	double lfoValue = lfoData.normalOutput;
	if (parameters.quadPhaseLFO)
		lfoValue = lfoData.quadPhaseOutput_pos;

	const double depth = parameters.lfoDepth_Pct / 100.0;
	const double modulatorValue = lfoValue * depth;

	const PhaserAPFParameters* apfParams = getPhaserApfParameters();
	double gammas[PHASER_STAGES];
	double gamma = 1;
	for (uint32_t i = 0; i < PHASER_STAGES; i++)
	{
		// --- calculate modulated values for each APF; note they have different ranges
		AudioFilterParameters params = apfs[i].getParameters();
		params.fc = doBipolarModulation(modulatorValue, apfParams[i].minF, apfParams[i].maxF);
		apfs[i].setParameters(params);

		// --- calculate gamma values
		gamma = apfs[PHASER_STAGES - (i + 1)].getG_value() * gamma;
		gammas[i] = gamma;
	}

	// --- create combined feedback
	double Sn = 0;
	for (uint32_t i = 0; i < PHASER_STAGES; i++)
	{
		Sn += i < PHASER_STAGES - 1 ? gammas[PHASER_STAGES - (i+2)] * apfs[i].getS_value() : apfs[i].getS_value();
	}
	
	// --- set the alpha0 value
	const double K = parameters.intensity_Pct / 100.0;
	const double alpha0 = 1.0 / (1.0 + K * gamma);

	// --- form input to first APF
	double apfsOutput = alpha0 * (xn + K * Sn);

	// --- cascade of APFs
	for (auto& apf : apfs)
	{
		apfsOutput = apf.processAudioSample(apfsOutput);
	}

	// Mix dry & wet signal, based on chosen coefficients
	const PhaserMixCoeffs mixCoeefs = getPhaserMixCoeffs();
	const double output = mixCoeefs.dry * xn + mixCoeefs.wet * apfsOutput;

	return output;
}

bool Phaser::canProcessAudioFrame() { return false; }

PhaserParameters Phaser::getParameters() { return parameters; }


bool Phaser::parametersUpdated(const OscillatorParameters lfoparams, const PhaserParameters& params)
{
	return !isFloatEqual(lfoparams.frequency_Hz, params.lfoRate_Hz) ||
		lfoparams.waveform != params.lfoWaveform;
}

void Phaser::updateParameters(const PhaserParameters& params)
{
	OscillatorParameters lfoparams = lfo.getParameters();

	if (parametersUpdated(lfoparams, params))
	{
		lfoparams.waveform = params.lfoWaveform;
		lfoparams.frequency_Hz = params.lfoRate_Hz;
		lfo.setParameters(lfoparams);
	}
}

void Phaser::setParameters(const PhaserParameters& params)
{
	// --- update LFO waveform & rate
	updateParameters(params);
	// --- save
	parameters = params;
}
