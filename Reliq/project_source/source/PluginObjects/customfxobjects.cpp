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

DefaultSideChainProcessor::DefaultSideChainProcessor() = default; /* C-TOR */
DefaultSideChainProcessor::~DefaultSideChainProcessor() = default; /* D-TOR */

bool DefaultSideChainProcessor::reset(double _sampleRate)
{
    return true;
};

bool DefaultSideChainProcessor::canProcessAudioFrame()
{
    return false;
}

double DefaultSideChainProcessor::processAudioSample(double xn)
{
    return 1.0;
}

SideChainProcessorParameters DefaultSideChainProcessor::getParameters() const
{
    return parameters;
}

void DefaultSideChainProcessor::setParameters(const SideChainProcessorParameters _parameters)
{
    parameters = _parameters;
}

DigitalDelay::DigitalDelay() = default; /* C-TOR */
DigitalDelay::~DigitalDelay() = default; /* D-TOR */

void DigitalDelay::resetLpf(double _sampleRate)
{
    zvaFilter.reset(_sampleRate);

    // Set the LPF parameters
    // --- all objects share  same params, so get first
    ZVAFilterParameters params;

    // --- update with our GUI parameter variables
    params.fc = 1000;
    params.Q = 0;
    params.filterOutputGain_dB = 0.707;

    // --- Discrete Plugin Variables 
    params.filterAlgorithm = vaFilterAlgorithm::kLPF1;
    params.enableGainComp = false;
    params.matchAnalogNyquistLPF = false;
    params.selfOscillate = false;
    params.enableNLP = false;

    // --- apply to parameters to LPF
    zvaFilter.setParameters(params);
}

bool DigitalDelay::reset(double _sampleRate)
{
    // --- if sample rate did not change
    if (isFloatEqual(sampleRate, _sampleRate))
    {
        // --- just flush buffer and return
        delayBuffer_L.flushBuffer();
        delayBuffer_R.flushBuffer();
        return true;
    }

    // --- create new buffer, will store sample rate and length(mSec)
    createDelayBuffers(_sampleRate, bufferLength_mSec);

    // Reset the LPF each time and set params
    resetLpf(_sampleRate);

    // Reset the sideChainSignalProcessor
    sideChainSignalProcessor.reset(_sampleRate);

    return true;
}

double DigitalDelay::getOutputMix(double xn, double yn)
{
    const double dryMix = 1.0 - mix;
    const double wetMix = sideChainSignalProcessor.processAudioSample(xn) * mix;

    return (dryMix * xn + wetMix * yn) * level_dB;
}

double DigitalDelay::filter(double yn)
{
    if (parameters.emulateAnalog)
    {
        yn = zvaFilter.processAudioSample(yn);
    }
    return yn;
}

double DigitalDelay::getDn(double xn, const double yn)
{
    return xn + filter((parameters.feedback_Pct * 0.01) * yn);
}

double DigitalDelay::processAudioSample(double xn)
{
    // --- read delay
    const double yn = delayBuffer_L.readBuffer(delayInSamples_L);

    // --- create input for delay buffer
    const double dn = getDn(xn, yn);

    // --- write to delay buffer
    delayBuffer_L.writeBuffer(dn);

    // --- form mixture out
    const double output = getOutputMix(xn, yn);

    return output;
}

bool DigitalDelay::canProcessAudioFrame() { return true; }

bool DigitalDelay::processAudioFrame(const float* inputFrame,
                                     /* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
                                     float* outputFrame,
                                     uint32_t inputChannels,
                                     uint32_t outputChannels)
{
    // --- make sure we have input and outputs
    if (inputChannels == 0 || outputChannels == 0)
        return false;

    // --- make sure we support this delay algorithm
    if (parameters.algorithm != delayAlgorithm::kNormal &&
        parameters.algorithm != delayAlgorithm::kPingPong)
        return false;

    // --- if only one output channel, revert to mono operation
    if (outputChannels == 1)
    {
        // --- process left channel only
        outputFrame[0] = static_cast<float>(processAudioSample(inputFrame[0]));
        return true;
    }

    // --- if we get here we know we have 2 output channels
    //
    // --- pick up inputs
    //
    // --- LEFT channel
    const double xnL = inputFrame[0];

    // --- RIGHT channel (duplicate left input if mono-in)
    const double xnR = inputChannels > 1 ? inputFrame[1] : xnL;

    // --- read delay LEFT
    const double ynL = delayBuffer_L.readBuffer(delayInSamples_L);

    // --- read delay RIGHT
    const double ynR = delayBuffer_R.readBuffer(delayInSamples_R);

    // --- create input for delay buffer with LEFT channel info
    const double dnL = getDn(xnL, ynL);

    // --- create input for delay buffer with RIGHT channel info
    const double dnR = getDn(xnR, ynR);

    // --- decode
    if (parameters.algorithm == delayAlgorithm::kNormal)
    {
        // --- write to LEFT delay buffer with LEFT channel info
        delayBuffer_L.writeBuffer(dnL);

        // --- write to RIGHT delay buffer with RIGHT channel info
        delayBuffer_R.writeBuffer(dnR);
    }
    else if (parameters.algorithm == delayAlgorithm::kPingPong)
    {
        // --- write to LEFT delay buffer with RIGHT channel info
        delayBuffer_L.writeBuffer(dnR);

        // --- write to RIGHT delay buffer with LEFT channel info
        delayBuffer_R.writeBuffer(dnL);
    }

    // --- form Left mixture out
    const double outputL = getOutputMix(xnL, ynL);

    // --- form Right mixture out
    const double outputR = getOutputMix(xnR, ynR);

    // --- set left channel
    outputFrame[0] = static_cast<float>(outputL);

    // --- set right channel
    outputFrame[1] = static_cast<float>(outputR);

    return true;
}

DigitalDelayParameters DigitalDelay::getParameters() const { return parameters; }

void DigitalDelay::updateParameters(const DigitalDelayParameters _parameters)
{
    // --- check level in dB for calc
    if (!isFloatEqual(_parameters.Level_dB, parameters.Level_dB))
        level_dB = pow(10.0, _parameters.Level_dB / 20.0);

    mix = _parameters.mix;

    // --- check update type first:
    if (_parameters.updateType == delayUpdateType::kLeftAndRight)
    {
        // --- set left and right delay times
        // --- calculate total delay time in samples + fraction
        const double newDelayInSamples_L = _parameters.leftDelay_mSec * (samplesPerMSec);
        const double newDelayInSamples_R = _parameters.rightDelay_mSec * (samplesPerMSec);

        // --- new delay time with fraction
        delayInSamples_L = newDelayInSamples_L;
        delayInSamples_R = newDelayInSamples_R;
    }
    else if (_parameters.updateType == delayUpdateType::kLeftPlusRatio)
    {
        // --- get and validate ratio
        double delayRatio = _parameters.delayRatio_Pct * 0.01;
        boundValue(delayRatio, 0.0, 1.0);

        // --- calculate total delay time in samples + fraction
        const double newDelayInSamples = _parameters.leftDelay_mSec * (samplesPerMSec);

        // --- new delay time with fraction
        delayInSamples_L = newDelayInSamples;
        delayInSamples_R = delayInSamples_L * delayRatio;
    }

    sideChainSignalProcessor.setParameters(_parameters.sideChainProcessorParameters);
}

void DigitalDelay::setParameters(const DigitalDelayParameters _parameters)
{
    updateParameters(_parameters);

    // --- save; rest of updates are cheap on CPU
    parameters = _parameters;
}

void DigitalDelay::createDelayBuffers(double _sampleRate, double _bufferLength_mSec)
{
    // --- store for math
    bufferLength_mSec = _bufferLength_mSec;
    sampleRate = _sampleRate;
    samplesPerMSec = sampleRate / 1000.0;

    // --- total buffer length including fractional part
    bufferLength = static_cast<unsigned>(bufferLength_mSec * (samplesPerMSec)) + 1; // +1 for fractional part

    // --- create new buffer
    delayBuffer_L.createCircularBuffer(bufferLength);
    delayBuffer_R.createCircularBuffer(bufferLength);
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
