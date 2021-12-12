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

AutoQEnvelopeFollower::~AutoQEnvelopeFollower() = default; /* D-TOR */

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

bool AutoQEnvelopeFollower::filterParametersUpdated(const ZVAFilterParameters filterParams,
                                                    const AutoQEnvelopeFollowerParameters& params)
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

bool AutoQEnvelopeFollower::detectorParametersUpdated(const AudioDetectorParameters adParams,
                                                      const AutoQEnvelopeFollowerParameters& params)
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

const PhaserApfParameters* Phaser::getPhaserApfParameters()
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

Phaser::~Phaser() = default;

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

    const double depth = parameters.lfoDepth_Pct * 0.01;
    const double modulatorValue = lfoValue * depth;

    const PhaserApfParameters* apfParams = getPhaserApfParameters();
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

    // --- set the alpha0 value
    const double K = parameters.intensity_Pct * 0.01;
    const double alpha0 = 1.0 / (1.0 + K * gamma);

    // --- create combined feedback
    double Sn = 0;
    for (uint32_t i = 0; i < PHASER_STAGES; i++)
    {
        Sn += i < PHASER_STAGES - 1 ? gammas[PHASER_STAGES - (i + 2)] * apfs[i].getS_value() : apfs[i].getS_value();
    }

    // --- form input to first APF
    double apfsOutput = alpha0 * (xn + K * Sn);

    // --- cascade of APFs
    for (auto& apf : apfs)
    {
        apfsOutput = apf.processAudioSample(apfsOutput);
    }

    // Mix dry & wet signal, based on chosen coefficients
    const PhaserMixCoeffs mixCoeffs = getPhaserMixCoeffs();
    const double output = mixCoeffs.dry * xn + mixCoeffs.wet * apfsOutput;

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

DelayGainCalculator::DelayGainCalculator() = default; /* C-TOR */
DelayGainCalculator::~DelayGainCalculator() = default; /* D-TOR */

bool DelayGainCalculator::reset(double _sampleRate)
{
	return true;
};

bool DelayGainCalculator::canProcessAudioFrame()
{
    return false;
}

double DelayGainCalculator::processAudioSample(double xn)
{
	// --- calc threshold
	const double threshValue = pow(10.0, parameters.threshold_dB / 20.0);
	const double deltaValue = xn - threshValue;

	const double wetGainMin = pow(10.0, parameters.wetGainMin_dB / 20.0);
	const double wetGainMax = pow(10.0, parameters.wetGainMax_dB / 20.0);

    double yn = 1.0;
    // --- if above the threshold, modulate the filter fc
	if (deltaValue > 0.0) // || delta_dB > 0.0)
	{
		// --- best results are with linear values when detector is in dB mode
		double modulatorValue = (deltaValue * parameters.sensitivity);
        boundValue(modulatorValue, wetGainMin, wetGainMax);
        yn = modulatorValue;
	}

    return yn;
}

DelayGainCalculatorParameters DelayGainCalculator::getParameters() const
{
    return parameters;
}

void DelayGainCalculator::setParameters(const DelayGainCalculatorParameters& _parameters)
{
        // --- save
        parameters = _parameters;
}

AnalogTone::AnalogTone() = default;

AnalogTone::~AnalogTone() = default;

bool AnalogTone::reset(double _sampleRate)
{
    if (!isFloatEqual(Fs, _sampleRate)) {
        Fs = _sampleRate;
        updateCoefficients();
    }

    return true;
}

double AnalogTone::processAudioSample(double xn)
{
    const double Vo = b0 * xn + b1 * x1 + b2 * x2 + b3 * x3 + b4 * x4;
    const double vx = xn / (R6 * Gz) + x1 / Gz + x2 * R2 / (G2 * Gz * P1);
    x1 = (2.0 / R1) * (vx) - x1;
    x2 = (2.0 / R2) * (vx / Gr + x2 * (P1 + R5) / Gr) - x2;
    x3 = (2.0 / R3) * (vx / Gs + x3 * (P2 + R8) / Gs) - x3;
    x4 = 2 * Vo / R11 + x4;
    return Vo;
}

bool AnalogTone::canProcessAudioFrame() { return false; }

AnalogToneParameters AnalogTone::getParameters() const { return parameters; }

bool AnalogTone::parametersUpdated(AnalogToneParameters _parameters) const
{
    return !isFloatEqual(parameters.tone, _parameters.tone) || !isFloatEqual(parameters.level, _parameters.level);
}

void AnalogTone::updateParameters(AnalogToneParameters _parameters)
{
    if (parametersUpdated(_parameters))
    {
        tone = 0.0000001 + 0.999998 * _parameters.tone;
        level = 0.000001 + 0.999999 * _parameters.level;
        updateCoefficients();
    }
}

void AnalogTone::setParameters(AnalogToneParameters _parameters)
{
    // --- update tone & level
    updateParameters(_parameters);
    // --- save
    parameters = _parameters;
}

void AnalogTone::updateCoefficients(){
    Ts = 1.0 / Fs;

    R1 = Ts / (2.0 * C1);
    R2 = Ts / (2.0 * C2);
    R3 = Ts / (2.0 * C3);
    R4 = Ts / (2.0 * C4);

    R10 = 100e3 * (1.0 - level);
    R11 = 100e3 * level;

    P1 = 20000.0 * tone;
    P2 = 20000.0 * (1.0 - tone);

    // Grouped resistances
    G2 = 1.0 + R2 / P1 + R5 / P1;
    G3 = 1.0 + R3 / P2 + R8 / P2;
    Gx = 1.0 + R7 / (G3 * P2);
    Gz = (1.0 / R1 + 1.0 / R6 + 1.0 / (G2 * P1));
    Go = (1.0 + R10 / R11 + R9 / R11 + R4 / R11);
    Gr = 1.0 + P1 / R2 + R5 / R2;
    Gs = 1.0 + P2 / R3 + R8 / R3;

    b0 = Gx / (Go * R6 * Gz);
    b1 = Gx / (Go * Gz);
    b2 = Gx * R2 / (G2 * Gz * Go * P1);
    b3 = -R3 * R7 / (Go * G3 * P2);
    b4 = -R4 / Go;
}

AnalogClipper::AnalogClipper() = default;

AnalogClipper::~AnalogClipper() = default;

bool AnalogClipper::reset(double _sampleRate)
{
    if (!isFloatEqual(Fs, _sampleRate)) {
        Fs = _sampleRate;
        updateCoefficients();
    }

    return true;
}

double AnalogClipper::processAudioSample(double xn)
{
    const double p = -xn / (G4 * R4) + R1 / (G4 * R4) * x1 - x2;

    int iter = 1;
    double b = 1.0;
    double fd = p + Vd / R2 + Vd / R3 + 2.0 * Is * sinh(Vd / (eta * Vt));
    while (iter < 50 && abs(fd) > thr)
    {
        const double den = 2.0 * Is / (eta * Vt) * cosh(Vd / (eta * Vt)) + 1.0 / R2 + 1.0 / R3;
        const double Vnew = Vd - b * fd / den;
        const double fn = p + Vnew / R2 + Vnew / R3 + 2.0 * Is * sinh(Vnew / (eta * Vt));
        if (abs(fn) < abs(fd))
        {
            Vd = Vnew;
            b = 1.0;
        }
        else
        {
            b *= 0.5;
        }
        fd = p + Vd / R2 + Vd / R3 + 2.0 * Is * sinh(Vd / (eta * Vt));
        iter++;
    }
    const double Vo = Vd + xn;
    x1 = (2.0 / R1) * (xn / G1 + x1 * R4 / G1) - x1;
    x2 = (2.0 / R2) * (Vd) - x2;

    return Vo;
}

bool AnalogClipper::canProcessAudioFrame() { return false; }

AnalogClipperParameters AnalogClipper::getParameters() const { return parameters; }

bool AnalogClipper::parametersUpdated(AnalogClipperParameters _parameters) const
{
    return !isFloatEqual(parameters.drive, _parameters.drive);
}

void AnalogClipper::updateParameters(AnalogClipperParameters _parameters)
{
    if (parametersUpdated(_parameters))
    {
        drive = _parameters.drive;
        updateCoefficients();
    }
}

void AnalogClipper::setParameters(AnalogClipperParameters _parameters)
{
    // --- update tone & level
    updateParameters(_parameters);
    // --- save
    parameters = _parameters;
}

void AnalogClipper::updateCoefficients(){
    Ts = 1.0 / Fs;
    R1 = Ts / (2.0 * C1);
    R2 = Ts / (2. * C2);
    P1 = drive * 500e3;
    R3 = 51000.0 + P1;
    R4 = 4700.0;

    // Combined Resistances
    G1 = (1.0 + R4 / R1);
    G4 = (1.0 + R1 / R4);
 }
