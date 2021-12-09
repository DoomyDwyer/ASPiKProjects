#pragma once
#include "fxobjects.h"

/**
\struct AutoQEnvelopeFollowerParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for the AutoQEnvelopeFollower object.

\author Steve Dwyer - Adapted from Will Pirkle http://www.willpirkle.com
\remark This object is based on the EnvelopeFollowerParameters class included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2021/ 09 / 06
*/
struct AutoQEnvelopeFollowerParameters
{
    AutoQEnvelopeFollowerParameters() = default;
    ~AutoQEnvelopeFollowerParameters() = default;

    // Explicitly use default copy constructor
    AutoQEnvelopeFollowerParameters(const AutoQEnvelopeFollowerParameters&) = default;

    // Copy assignment operator
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

    // Suppress generation of move constructor and move assignment operator
    AutoQEnvelopeFollowerParameters(const AutoQEnvelopeFollowerParameters&&) = delete;
    AutoQEnvelopeFollowerParameters& operator=(const AutoQEnvelopeFollowerParameters&&) = delete;

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
\class AutoQEnvelopeFollower
\ingroup Custom-FX-Objects
\brief
The AutoQEnvelopeFollower object implements a traditional envelope follower effect modulating a LPR fc value
using the strength of the detected input.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use EnvelopeFollowerParameters structure to get/set object params.

\author Steve Dwyer - Adapted from Will Pirkle http://www.willpirkle.com
\remark This object is based on the EnvelopeFollower class included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2021/ 09 / 06
*/
class AutoQEnvelopeFollower : public IAudioSignalProcessor
{
public:
    AutoQEnvelopeFollower(); /* C-TOR */
    virtual ~AutoQEnvelopeFollower(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    AutoQEnvelopeFollower(const AutoQEnvelopeFollower&) = delete;
    AutoQEnvelopeFollower& operator=(const AutoQEnvelopeFollower&) = delete;

    // Suppress generation of move constructor and move assignment operator
    AutoQEnvelopeFollower(const AutoQEnvelopeFollower&&) = delete;
    AutoQEnvelopeFollower& operator=(const AutoQEnvelopeFollower&&) = delete;

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
    virtual bool filterParametersUpdated(ZVAFilterParameters filterParams,
                                         const AutoQEnvelopeFollowerParameters& params);

    virtual void updateFilterParameters(const AutoQEnvelopeFollowerParameters& params);

    virtual bool detectorParametersUpdated(AudioDetectorParameters adParams,
                                           const AutoQEnvelopeFollowerParameters& params);

    virtual void updateDetectorParameters(const AutoQEnvelopeFollowerParameters& params);
};

/**
\struct PhaserParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for the PhaserParameters object.

\author Steve Dwyer - Adapted from Will Pirkle http://www.willpirkle.com
\remark This object is based on the PhaseShifterParameters class included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2021 / 09 / 06
*/
struct PhaserParameters
{
    PhaserParameters() = default;
    ~PhaserParameters() = default;

    // Explicitly use default copy constructor
    PhaserParameters(const PhaserParameters&) = default;

    // Copy assignment operator
    /** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
    PhaserParameters& operator=(const PhaserParameters& params)
    {
        if (this == &params)
            return *this;

        lfoWaveform = params.lfoWaveform;
        lfoRate_Hz = params.lfoRate_Hz;
        lfoDepth_Pct = params.lfoDepth_Pct;
        intensity_Pct = params.intensity_Pct;
        quadPhaseLFO = params.quadPhaseLFO;
        return *this;
    }

    PhaserParameters(const PhaserParameters&& params) noexcept
        : lfoWaveform{params.lfoWaveform},
          lfoRate_Hz{params.lfoRate_Hz},
          lfoDepth_Pct{params.lfoDepth_Pct},
          intensity_Pct{params.intensity_Pct},
          quadPhaseLFO{params.quadPhaseLFO}
    {
    }

    // Suppress generation of move assignment operator
    PhaserParameters& operator=(const PhaserParameters&&) = delete;

    // --- individual parameters
    generatorWaveform lfoWaveform = generatorWaveform::kTriangle;
    double lfoRate_Hz = 0.0; ///< phaser LFO rate in Hz
    double lfoDepth_Pct = 0.0; ///< phaser LFO depth in %
    double intensity_Pct = 0.0; ///< phaser feedback in %
    bool quadPhaseLFO = false; ///< quad phase LFO flag
};

struct PhaserApfParameters
{
    const double minF;
    const double maxF;
};

struct PhaserMixCoeffs
{
    const double dry;
    const double wet;
};

/**
\class Phaser
\ingroup Custom-FX-Objects
\brief
The Phaser object implements a six-stage phaser.

Audio I/O:
- Processes mono input to mono output.

Control I/F:
- Use BiquadParameters structure to get/set object params.
Custom parameter structure for the PhaserParameters object.

\author Steve Dwyer - Adapted from Will Pirkle http://www.willpirkle.com
\remark This object is based on the PhaseShifter class included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2021 / 09 / 06
*/
class Phaser : public IAudioSignalProcessor
{
public:
    Phaser(); /* C-TOR */
    virtual ~Phaser(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    Phaser(const Phaser&) = delete;
    Phaser& operator=(const Phaser&) = delete;

    // Suppress generation of move constructor and move assignment operator
    Phaser(const Phaser&&) = delete;
    Phaser& operator=(const Phaser&&) = delete;

    /** reset members to initialized state */
    bool reset(double _sampleRate) override;

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return PhaserParameters custom data structure
    */
    virtual PhaserParameters getParameters();

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param params custom data structure
    */
    virtual void setParameters(const PhaserParameters& params);

    /** return false: this object only processes samples */
    bool canProcessAudioFrame() override;

    /** process audio through phaser */
    /**
    \param xn input
    \return the processed sample
    */
    double processAudioSample(double xn) override;

protected:
    PhaserParameters parameters; ///< the object parameters
    AudioFilter apfs[PHASER_STAGES]; ///< six APF objects
    LFO lfo; ///< the one and only LFO

private:
    // --- these are the ideal band definitions
    const PhaserApfParameters idealPhaserParams[PHASER_STAGES] = {
        {16.0, 1600.0}, {33.0, 3300.0}, {48.0, 4800.0}, {98.0, 9800.0}, {160.0, 16000.0}, {260.0, 20480.0}
    };

    // --- these are the exact values from the National Semiconductor Phaser design
    const PhaserApfParameters nsPhaserParams[PHASER_STAGES] = {
        {32.0, 1500.0}, {68.0, 3400.0}, {96.0, 4800.0}, {212.0, 10000.0}, {320.0, 16000.0}, {636.0, 20480.0}
    };

    // -3dB coefficients
    const PhaserMixCoeffs min3dBPhaserMixCoeffs = {0.707, 0.707};
    // National Semiconductor design ratio
    const PhaserMixCoeffs nsPhaserMixCoeffs = {0.5, 5.0};
    // Other dry/wet mix coefficients
    const PhaserMixCoeffs idealPhaserMixCoeffs = {0.125, 1.25};
    const PhaserMixCoeffs otherPhaserMixCoeffs = {0.25, 2.5};

    static filterAlgorithm getFilterAlgorithm();

    static double getFilterQ();

    virtual bool parametersUpdated(OscillatorParameters lfoparams, const PhaserParameters& params);

    virtual void updateParameters(const PhaserParameters& params);

    virtual const PhaserApfParameters* getPhaserApfParameters();

    virtual PhaserMixCoeffs getPhaserMixCoeffs();
};

/**
\struct SideChainProcessorParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for SideChainProcessor object.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 09 / 09
*/
struct SideChainProcessorParameters
{
    SideChainProcessorParameters() = default;
    ~SideChainProcessorParameters() = default;

    // Explicitly use default Copy constructor & Copy assignment operator
    SideChainProcessorParameters(const SideChainProcessorParameters&) = default;
    SideChainProcessorParameters& operator=(const SideChainProcessorParameters& params) = default;

    // Explicitly use default  Move constructor & Move assignment operator
    SideChainProcessorParameters(SideChainProcessorParameters&& params) = default;
    SideChainProcessorParameters& operator=(SideChainProcessorParameters&&) = default;
};

/**
\class DefaultSideChainProcessor
\ingroup Custom-FX-Objects
\brief
The DefaultSideChainProcessor simply always returns the value 1 from its processAudioSampleMethod.

Audio I/O:
- None.

Control I/F:
- None.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 09
*/
class DefaultSideChainProcessor : public IAudioSignalProcessor
{
public:
    DefaultSideChainProcessor(); /* C-TOR */
    virtual ~DefaultSideChainProcessor(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    DefaultSideChainProcessor(const DefaultSideChainProcessor&) = delete;
    DefaultSideChainProcessor& operator=(const DefaultSideChainProcessor&) = delete;

    // Suppress generation of move constructor and move assignment operator
    DefaultSideChainProcessor(const DefaultSideChainProcessor&&) = delete;
    DefaultSideChainProcessor& operator=(const DefaultSideChainProcessor&&) = delete;

    /** reset members to initialized state */
    bool reset(double _sampleRate) override;

    /** process audio through Clipping Stage */
    /**
    \param xn input
    \return the processed sample
    */
    double processAudioSample(double xn) override;

    /** return true: this object can also process frames */
    bool canProcessAudioFrame() override;

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return DigitalDelayParameters custom data structure
    */
    SideChainProcessorParameters getParameters() const;

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    void setParameters(SideChainProcessorParameters _parameters);

private:
    SideChainProcessorParameters parameters;
};

/**
\struct DigitalDelayParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for the DigitalDelay object.

\author Steve Dwyer - Adapted from Will Pirkle http://www.willpirkle.com
\remark This object is based on the AudioDelayParameters class included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2021 / 09 / 06
*/
struct DigitalDelayParameters
{
    DigitalDelayParameters() = default;
    ~DigitalDelayParameters() = default;

    // Explicitly use default copy constructor
    DigitalDelayParameters(const DigitalDelayParameters&) = default;

    // Copy assignment operator
    /** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
    DigitalDelayParameters& operator=(const DigitalDelayParameters& params)
    // need this override for collections to work
    {
        if (this == &params)
            return *this;

        algorithm = params.algorithm;
        mix = params.mix;
        Level_dB = params.Level_dB;
        feedback_Pct = params.feedback_Pct;

        updateType = params.updateType;
        leftDelay_mSec = params.leftDelay_mSec;
        rightDelay_mSec = params.rightDelay_mSec;
        delayRatio_Pct = params.delayRatio_Pct;
        emulateAnalog = params.emulateAnalog;

        sideChainProcessorParameters = params.sideChainProcessorParameters;

        return *this;
    }

    // Move constructor
    DigitalDelayParameters(const DigitalDelayParameters&& params) noexcept
        : algorithm{params.algorithm},
          mix{params.mix},
          Level_dB{params.Level_dB},
          feedback_Pct{params.feedback_Pct},
          updateType{params.updateType},
          leftDelay_mSec{params.leftDelay_mSec},
          rightDelay_mSec{params.rightDelay_mSec},
          delayRatio_Pct{params.delayRatio_Pct},
          emulateAnalog{params.emulateAnalog},
          sideChainProcessorParameters{params.sideChainProcessorParameters}
    {
    }

    // Suppress generation of move assignment operator
    DigitalDelayParameters& operator=(const DigitalDelayParameters&&) = delete;

    // --- individual parameters
    delayAlgorithm algorithm = delayAlgorithm::kNormal; ///< delay algorithm
    double mix = 0.5; ///< wet / dry mix
    double Level_dB = -3.0; ///< output level in dB
    double feedback_Pct = 0.0; ///< feedback as a % value

    delayUpdateType updateType = delayUpdateType::kLeftAndRight; ///< update algorithm
    double leftDelay_mSec = 0.0; ///< left delay time
    double rightDelay_mSec = 0.0; ///< right delay time
    double delayRatio_Pct = 0.0; ///< delay ratio: right length = (delayRatio)*(left length)
    bool emulateAnalog = false;

    // Any parameters required for the Side Chain Processor
    SideChainProcessorParameters sideChainProcessorParameters;
};

/**
\class DigitalDelay
\ingroup Custom-FX-Objects
\brief
The DigitalDelay object implements a stereo audio delay with multiple delay algorithms.

Audio I/O:
- Processes mono input to mono output OR stereo output.

Control I/F:
- Use DigitalDelayParameters structure to get/set object params.

\author Steve Dwyer - Adapted from Will Pirkle http://www.willpirkle.com
\remark This object is based on the AudioDelay class included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2021 / 09 / 06
*/
class DigitalDelay : public IAudioSignalProcessor
{
public:
    DigitalDelay(); /* C-TOR */
    virtual ~DigitalDelay(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    DigitalDelay(const DigitalDelay&) = delete;
    DigitalDelay& operator=(const DigitalDelay&) = delete;

    // Suppress generation of move constructor and move assignment operator
    DigitalDelay(const DigitalDelay&&) = delete;
    DigitalDelay& operator=(const DigitalDelay&&) = delete;

    /** reset members to initialized state */
    bool reset(double _sampleRate) override;

    /** process audio through phaser */
    /**
    \param xn input
    \return the processed sample
    */
    double processAudioSample(double xn) override;

    /** return true: this object can also process frames */
    bool canProcessAudioFrame() override;

    /** process STEREO audio delay in frames */
    bool processAudioFrame(const float* inputFrame,
                           /* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
                           float* outputFrame,
                           uint32_t inputChannels,
                           uint32_t outputChannels) override;

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return DigitalDelayParameters custom data structure
    */
    DigitalDelayParameters getParameters() const;

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    void setParameters(DigitalDelayParameters _parameters);

    /** creation function */
    void createDelayBuffers(double _sampleRate, double _bufferLength_mSec);

private:
    double getOutputMix(double xn, double yn);
    double filter(double yn);
    double getDn(double xn, double yn);
    void updateParameters(DigitalDelayParameters _parameters);
    void resetLpf(double _sampleRate);

    DigitalDelayParameters parameters; ///< object parameters
    ZVAFilter zvaFilter;
    DefaultSideChainProcessor sideChainSignalProcessor;

    double sampleRate = 0.0; ///< current sample rate
    double samplesPerMSec = 0.0; ///< samples per millisecond, for easy access calculation
    double delayInSamples_L = 0.0; ///< double includes fractional part
    double delayInSamples_R = 0.0; ///< double includes fractional part
    double bufferLength_mSec = 0.0; ///< buffer length in mSec
    unsigned int bufferLength = 0; ///< buffer length in samples
    double mix = 0.5; ///< wet / dry mix
    double level_dB = 0.707; ///< output level default = -3dB

    // --- delay buffer of doubles
    CircularBuffer<double> delayBuffer_L; ///< LEFT delay buffer of doubles
    CircularBuffer<double> delayBuffer_R; ///< RIGHT delay buffer of doubles
};

/**
\struct AnalogToneParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for the AnalogTone object.

\author Based on Eric Tarr's Analog Modeling Workshop - https://www.hackaudio.com/
\version Revision : 1.0
\date Date : 2021 / 12 / 06
*/
struct AnalogToneParameters
{
    AnalogToneParameters() = default;
    ~AnalogToneParameters() = default;

    // Explicitly use default copy constructor
    AnalogToneParameters(const AnalogToneParameters&) = default;

    // Copy assignment operator
    /** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
    AnalogToneParameters& operator=(const AnalogToneParameters& params)
    // need this override for collections to work
    {
        if (this == &params)
            return *this;

        tone = params.tone;
        level = params.level;

        return *this;
    }

    // Move constructor
    AnalogToneParameters(const AnalogToneParameters&& params) noexcept
        : tone{params.tone},
          level{params.level}
    {
    }

    // Suppress generation of move assignment operator
    AnalogToneParameters& operator=(const AnalogToneParameters&&) = delete;

    // --- individual parameters
    double tone = 0.5; ///< wet / dry mix
    double level = 0.5; ///< output level (0..1)
};

/**
\class AnalogTone
\ingroup Custom-FX-Objects
\brief
The AnalogTone object implements a stereo tone control, based on Analog circuit modeling of the Ibanez Tube Screamer's
Tone/Volume stage - https://www.electrosmash.com/tube-screamer-analysis..

Audio I/O:
- Processes mono input to mono output OR stereo  input to stereo output.

Control I/F:
- Use AnalogToneParameters structure to get/set object params.

\author Steve Dwyer - Based on Eric Tarr's Analog Modeling Workshop - https://www.hackaudio.com/
\remark This object is based on the TSTone class designed by Eric Tarr
\version Revision : 1.0
\date Date : 2021 / 12 / 05
*/
class AnalogTone : public IAudioSignalProcessor
{
public:
    AnalogTone(); /* C-TOR */
    virtual ~AnalogTone(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    AnalogTone(const AnalogTone&) = delete;
    AnalogTone& operator=(const AnalogTone&) = delete;

    // Suppress generation of move constructor and move assignment operator
    AnalogTone(const AnalogTone&&) = delete;
    AnalogTone& operator=(const AnalogTone&&) = delete;

    /** reset members to initialized state */
    bool reset(double _sampleRate) override;

    /** process audio through Tone Control */
    /**
    \param xn input
    \return the processed sample
    */
    double processAudioSample(double xn) override;

    /** return true: this object can also process frames */
    bool canProcessAudioFrame() override;

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return AnalogToneParameters custom data structure
    */
    AnalogToneParameters getParameters() const;

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    void setParameters(AnalogToneParameters _parameters);

private:
    bool parametersUpdated(AnalogToneParameters _parameters) const;
    void updateParameters(AnalogToneParameters _parameters);
    void updateCoefficients();

    AnalogToneParameters parameters; ///< object parameters
    double tone = 0.5; // Cannot be exactly 0 or 1
    double level = 1.0; // [0.000001 - 1] (-120 dB to 0 dB)

    double Fs = 48000.0;
    double Ts = 1.0 / Fs;

    const double C1 = .22e-6;
    double R1 = Ts / (2.0 * C1);

    const double C2 = .22e-6;
    double R2 = Ts / (2.0 * C2);

    const double C3 = .22e-6;
    double R3 = Ts / (2.0 * C3);

    const double C4 = 1e-6;
    double R4 = Ts / (2.0 * C4);

    const double R5 = 220.0;
    const double R6 = 1000.0;
    const double R7 = 1000.0;
    const double R8 = 220.0;

    const double R9 = 1000.0;
    double R10 = 100e3 * (1.0 - level);
    double R11 = 100e3 * level;

    double P1 = 20000.0 * tone;
    double P2 = 20000.0 * (1.0 - tone);

    // Grouped Resistances
    double G2 = 1.0 + R2 / P1 + R5 / P1;
    double G3 = 1.0 + R3 / P2 + R8 / P2;
    double Gx = 1.0 + R7 / (G3 * P2);
    double Gz = (1.0 / R1 + 1.0 / R6 + 1.0 / (G2 * P1));
    double Go = (1.0 + R10 / R11 + R9 / R11 + R4 / R11);
    double Gr = 1.0 + P1 / R2 + R5 / R2;
    double Gs = 1.0 + P2 / R3 + R8 / R3;

    // States
    double x1 = 0.0;
    double x2 = 0.0;
    double x3 = 0.0;
    double x4 = 0.0;

    // Filter coefficients
    double b0 = Gx / (Go * R6 * Gz);
    double b1 = Gx / (Go * Gz);
    double b2 = Gx * R2 / (G2 * Gz * Go * P1);
    double b3 = -R3 * R7 / (Go * G3 * P2);
    double b4 = -R4 / Go;
};

/**
\struct AnalogClipperParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for the AnalogClipper object.

\author Based on Eric Tarr's Analog Modeling Workshop - https://www.hackaudio.com/
\version Revision : 1.0
\date Date : 2021 / 12 / 05
*/
struct AnalogClipperParameters
{
    AnalogClipperParameters() = default;
    ~AnalogClipperParameters() = default;

    // Explicitly use default copy constructor
    AnalogClipperParameters(const AnalogClipperParameters&) = default;

    // Copy assignment operator
    /** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
    AnalogClipperParameters& operator=(const AnalogClipperParameters& params)
    // need this override for collections to work
    {
        if (this == &params)
            return *this;

        drive = params.drive;

        return *this;
    }

    // Suppress generation of move constructor and move assignment operator
    AnalogClipperParameters(const AnalogClipperParameters&& params) noexcept
        : drive{params.drive}
    {
    }

    // Suppress generation of move constructor and move assignment operator
    AnalogClipperParameters& operator=(const AnalogClipperParameters&&) = delete;

    // --- individual parameters
    double drive = 0.5; ///< overdrive factor (0..1)
};

/**
\class AnalogClipper
\ingroup Custom-FX-Objects
\brief
The AnalogClipper object implements a stereo clipper control, based on Analog circuit modeling of the Ibanez Tube Screamer's
Clipping Amp circuit - https://www.electrosmash.com/tube-screamer-analysis.

Audio I/O:
- Processes mono input to mono output OR stereo  input to stereo output.

Control I/F:
- Use AnalogToneParameters structure to get/set object params.

\author Steve Dwyer - Based on Eric Tarr's Analog Modeling Workshop - https://www.hackaudio.com/
\remark This object is based on the TSClipper class designed by Eric Tarr
\version Revision : 1.0
\date Date : 2021 / 12 / 05
*/
class AnalogClipper : public IAudioSignalProcessor
{
public:
    AnalogClipper(); /* C-TOR */
    virtual ~AnalogClipper(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    AnalogClipper(const AnalogClipper&) = delete;
    AnalogClipper& operator=(const AnalogClipper&) = delete;

    // Suppress generation of move constructor and move assignment operator
    AnalogClipper(const AnalogClipper&&) = delete;
    AnalogClipper& operator=(const AnalogClipper&&) = delete;

    /** reset members to initialized state */
    bool reset(double _sampleRate) override;

    /** process audio through Clipping Stage */
    /**
    \param xn input
    \return the processed sample
    */
    double processAudioSample(double xn) override;

    /** return true: this object can also process frames */
    bool canProcessAudioFrame() override;

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return AnalogClipperParameters custom data structure
    */
    AnalogClipperParameters getParameters() const;

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    void setParameters(AnalogClipperParameters _parameters);

private:
    bool parametersUpdated(AnalogClipperParameters _parameters) const;
    void updateParameters(AnalogClipperParameters _parameters);
    void updateCoefficients();

    AnalogClipperParameters parameters; ///< object parameters
    double drive = 0.5; ///< overdrive factor (0..1)
       
    const double eta = 1.0; // Change for non-ideal diode
    const double Is = 1e-15;
    const double Vt = 26e-3;

    double Fs = 48000.0;
    double Ts = 1.0 / Fs;

    const double C1 = 47e-9;
    const double C2 = 51e-12;

    double R1 = Ts / (2.0 * C1);
    double R2 = Ts / (2. * C2);
    double P1 = drive * 500e3;
    double R3 = 51000.0 + P1;
    double R4 = 4700.0;

    // Combined Resistances
    double G1 = (1.0 + R4 / R1);
    double G4 = (1.0 + R1 / R4);

    // States
    double x1 = 0.0;
    double x2 = 0.0;
    double Vd = 0.0;

    const double thr = 0.00000000001;
};
