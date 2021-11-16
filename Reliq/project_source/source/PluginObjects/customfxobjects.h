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
\ingroup FX-Objects
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
\ingroup FX-Objects
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
\ingroup FX-Objects
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
\struct DigitalDelayParameters
\ingroup FX-Objects
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

        return *this;
    }

    // Suppress generation of move constructor and move assignment operator
    DigitalDelayParameters(const DigitalDelayParameters&& params) noexcept
        : algorithm{params.algorithm},
          mix{params.mix},
          Level_dB{params.Level_dB},
          feedback_Pct{params.feedback_Pct},
          updateType{params.updateType},
          leftDelay_mSec{params.leftDelay_mSec},
          rightDelay_mSec{params.rightDelay_mSec},
          delayRatio_Pct{params.delayRatio_Pct},
          emulateAnalog{params.emulateAnalog}
    {
    }


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
};

/**
\class DigitalDelay
\ingroup FX-Objects
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
    double getOutput(double xn, double yn) const;
    double filter(double yn);
    double getDn(double xn, double yn);
    void updateParameters(DigitalDelayParameters _parameters);
    void resetLpf(double _sampleRate);

    DigitalDelayParameters parameters; ///< object parameters
    ZVAFilter zvaFilter;

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
