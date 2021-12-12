#pragma once
#include "fxobjects.h"
#include "utilities.h"

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
\struct DelayGainCalculatorParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for DelayGainCalculator object.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 11
*/
struct DelayGainCalculatorParameters
{
    DelayGainCalculatorParameters() = default;
    ~DelayGainCalculatorParameters() = default;

    // Explicitly use default Copy constructor & Copy assignment operator
    DelayGainCalculatorParameters(const DelayGainCalculatorParameters&) = default;
    DelayGainCalculatorParameters& operator=(const DelayGainCalculatorParameters& params)
    {
        if (this == &params) return *this;

	    threshold_dB = params.threshold_dB;
		sensitivity = params.sensitivity;
	    wetGainMin_dB = params.wetGainMin_dB;
	    wetGainMax_dB = params.wetGainMax_dB;
        return *this;
    }

    // Explicitly use default  Move constructor & Move assignment operator
    DelayGainCalculatorParameters(DelayGainCalculatorParameters&& params) = default;
    DelayGainCalculatorParameters& operator=(DelayGainCalculatorParameters&& params) noexcept
    {
        if (this == &params) return *this;

	    threshold_dB = params.threshold_dB;
		sensitivity = params.sensitivity;
	    wetGainMin_dB = params.wetGainMin_dB;
	    wetGainMax_dB = params.wetGainMax_dB;
        return *this;
    }

	double threshold_dB = 0.0;
	double sensitivity = 0.0;
	double wetGainMin_dB = 0.0;
	double wetGainMax_dB = 0.0;
};

/**
\class DelayGainCalculator
\ingroup Custom-FX-Objects
\brief
The DelayGainCalculator calculates the gain to wet signal based on detected envelope.

Audio I/O:
- None.

Control I/F:
- Use DelayGainCalculatorParameters structure to get/set object params.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 11
*/

class DelayGainCalculator : public IAudioSignalProcessor
{
public:
    DelayGainCalculator(); /* C-TOR */
    virtual ~DelayGainCalculator(); /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    DelayGainCalculator(const DelayGainCalculator&) = delete;
    DelayGainCalculator& operator=(const DelayGainCalculator&) = delete;

    // Suppress generation of move constructor and move assignment operator
    DelayGainCalculator(const DelayGainCalculator&&) = delete;
    DelayGainCalculator& operator=(const DelayGainCalculator&&) = delete;

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
    \return DelayGainCalculatorParameters custom data structure
    */
    DelayGainCalculatorParameters getParameters() const;

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    void setParameters(const DelayGainCalculatorParameters& _parameters);

private:
    DelayGainCalculatorParameters parameters;
};

/**
\struct SideChainSignalProcessorParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for SideChainSignalProcessor object.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 09
*/
struct SideChainSignalProcessorParameters
{
    SideChainSignalProcessorParameters() = default;
    virtual ~SideChainSignalProcessorParameters() = default;

    // Explicitly use default Copy constructor & Copy assignment operator
    SideChainSignalProcessorParameters(const SideChainSignalProcessorParameters&) = default;
    SideChainSignalProcessorParameters& operator=(const SideChainSignalProcessorParameters&) = default;

    // Explicitly use default  Move constructor & Move assignment operator
    SideChainSignalProcessorParameters(SideChainSignalProcessorParameters&& params) = default;
    SideChainSignalProcessorParameters& operator=(SideChainSignalProcessorParameters&&) = default;
};

/**
\struct DefaultSideChainSignalProcessorParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for SideChainSignalProcessor object.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 09
*/
struct DefaultSideChainSignalProcessorParameters : SideChainSignalProcessorParameters
{
    DefaultSideChainSignalProcessorParameters() = default;
    ~DefaultSideChainSignalProcessorParameters() override = default;

    // Explicitly use default Copy constructor & Copy assignment operator
    DefaultSideChainSignalProcessorParameters(const DefaultSideChainSignalProcessorParameters&) = default;
    DefaultSideChainSignalProcessorParameters& operator=(const DefaultSideChainSignalProcessorParameters&) = default;

    // Explicitly use default Move constructor & Move assignment operator
    DefaultSideChainSignalProcessorParameters(DefaultSideChainSignalProcessorParameters&& params) = default;
    DefaultSideChainSignalProcessorParameters& operator=(DefaultSideChainSignalProcessorParameters&&) = default;
};

/**
\struct EnvelopeDetectorSideChainSignalProcessorParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for EnvelopeDetectorSideChainSignalProcessor object.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 11
*/
struct EnvelopeDetectorSideChainSignalProcessorParameters : SideChainSignalProcessorParameters
{
    EnvelopeDetectorSideChainSignalProcessorParameters() = default;
    ~EnvelopeDetectorSideChainSignalProcessorParameters() override = default;

    // Explicitly use default Copy constructor & Copy assignment operator
    EnvelopeDetectorSideChainSignalProcessorParameters(const EnvelopeDetectorSideChainSignalProcessorParameters&) = default;
    EnvelopeDetectorSideChainSignalProcessorParameters& operator=(const EnvelopeDetectorSideChainSignalProcessorParameters& params)
    {
        if (this == &params) return *this;

	    sideChainGain_dB = params.sideChainGain_dB;
	    attackTime_mSec = params.attackTime_mSec;
	    releaseTime_mSec = params.releaseTime_mSec;
	    threshold_dB = params.threshold_dB;
		sensitivity = params.sensitivity;
	    wetGainMin_dB = params.wetGainMin_dB;
	    wetGainMax_dB = params.wetGainMax_dB;
        return *this;
    }

    // Explicitly use default  Move constructor & Move assignment operator
    EnvelopeDetectorSideChainSignalProcessorParameters(EnvelopeDetectorSideChainSignalProcessorParameters&& params) = default;
    EnvelopeDetectorSideChainSignalProcessorParameters& operator=(EnvelopeDetectorSideChainSignalProcessorParameters&& params) noexcept
    {
        if (this == &params) return *this;

	    sideChainGain_dB = params.sideChainGain_dB;
	    attackTime_mSec = params.attackTime_mSec;
	    releaseTime_mSec = params.releaseTime_mSec;
	    threshold_dB = params.threshold_dB;
		sensitivity = params.sensitivity;
	    wetGainMin_dB = params.wetGainMin_dB;
	    wetGainMax_dB = params.wetGainMax_dB;
        return *this;
    }

	double sideChainGain_dB = 0.0;
	double attackTime_mSec = 0.0;
	double releaseTime_mSec = 0.0;
	double threshold_dB = 0.0;
	double sensitivity = 0.0;
	double wetGainMin_dB = 0.0;
	double wetGainMax_dB = 0.0;
};

/**
\class SideChainSignalProcessor
\ingroup Custom-FX-Objects
\brief
The SideChainSignalProcessor interface for all Side Chain Signal Processors.

Audio I/O:
- None.

Control I/F:
- Use SideChainSignalProcessorParameters structure to get/set object params.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 10
*/
template <class SideChainProcessorParams>
class SideChainSignalProcessor : public IAudioSignalProcessor
{
public:
    /**
     * Virtual D-TOR for pure Pure-Virtual Interface Class
     */
    SideChainSignalProcessor<SideChainProcessorParams>() = default;

    /**
     * Virtual D-TOR for pure Pure-Virtual Interface Class
     */
    virtual ~SideChainSignalProcessor<SideChainProcessorParams>() = default;

    // Suppress generation of copy constructor and copy assignment operator
    SideChainSignalProcessor<SideChainProcessorParams>(const SideChainSignalProcessor<SideChainProcessorParams>&) = default;
    virtual SideChainSignalProcessor<SideChainProcessorParams>& operator=(const SideChainSignalProcessor<SideChainProcessorParams>&) = delete;

    // Suppress generation of move constructor and move assignment operator
    SideChainSignalProcessor<SideChainProcessorParams>(const SideChainSignalProcessor<SideChainProcessorParams>&&) = delete;
    virtual SideChainSignalProcessor<SideChainProcessorParams>& operator=(const SideChainSignalProcessor<SideChainProcessorParams>&&) = delete;

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return DigitalDelayParameters custom data structure
    */
    virtual SideChainProcessorParams getParameters() const = 0;

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    virtual void setParameters(const SideChainProcessorParams& _parameters) = 0;
};

/**
\class DefaultSideChainSignalProcessor
\ingroup Custom-FX-Objects
\brief
The DefaultSideChainSignalProcessor simply always returns the value 1 from its processAudioSampleMethod.

Audio I/O:
- None.

Control I/F:
- Use SideChainSignalProcessorParameters structure to get/set object params.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 09
*/
template <class SideChainProcessorParams>
class DefaultSideChainSignalProcessor : public SideChainSignalProcessor<SideChainProcessorParams>
{
public:
    DefaultSideChainSignalProcessor<SideChainProcessorParams>() = default; /* C-TOR */
    ~DefaultSideChainSignalProcessor<SideChainProcessorParams>() override = default; /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    DefaultSideChainSignalProcessor<SideChainProcessorParams>(const DefaultSideChainSignalProcessor<SideChainProcessorParams>&) = default;
    DefaultSideChainSignalProcessor<SideChainProcessorParams>& operator=(const DefaultSideChainSignalProcessor<SideChainProcessorParams>&) = delete;

    // Suppress generation of move constructor and move assignment operator
    DefaultSideChainSignalProcessor<SideChainProcessorParams>(const DefaultSideChainSignalProcessor<SideChainProcessorParams>&&) = delete;
    DefaultSideChainSignalProcessor<SideChainProcessorParams>& operator=(const DefaultSideChainSignalProcessor<SideChainProcessorParams>&&) = delete;

    /** reset members to initialized state */
    bool reset(double _sampleRate) override
    {
        return true;
    }

    /** process audio through Clipping Stage */
    /**
    \param xn input
    \return the processed sample
    */
    double processAudioSample(double xn) override
    {
        return 1.0;
    }

    /** return true: this object can also process frames */
    bool canProcessAudioFrame() override
    {
        return false;
    }

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return DigitalDelayParameters custom data structure
    */
    SideChainProcessorParams getParameters() const override
    {
        return parameters;
    }

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    void setParameters(const SideChainProcessorParams& _parameters) override
    {
        parameters = _parameters;
    }

private:
    DefaultSideChainSignalProcessorParameters parameters;
};

/**
\class EnvelopeDetectorSideChainSignalProcessor
\ingroup Custom-FX-Objects
\brief
The EnvelopeDetectorSideChainSignalProcessor uses an Envelope Detector to attenuate a wet signal during high amplitude periods.

Audio I/O:
- None.

Control I/F:
- Use EnvelopeDetectorSideChainSignalProcessorParameters structure to get/set object params.

\author Steve Dwyer
\version Revision : 1.0
\date Date : 2021 / 12 / 11
*/
template <class SideChainProcessorParams>
class EnvelopeDetectorSideChainSignalProcessor : public SideChainSignalProcessor<SideChainProcessorParams>
{
public:
    EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>()
    {
        // --- setup the detector
        AudioDetectorParameters adParams;
        adParams.attackTime_mSec = 20.0;
        adParams.releaseTime_mSec = 500.0;
        adParams.detectMode = TLD_AUDIO_DETECT_MODE_RMS;
        adParams.detect_dB = true;
        adParams.clampToUnityMax = false;
        envDetector.setParameters(adParams);

        // --- setup the DelayGainCalculator
        DelayGainCalculatorParameters delayGainCalculatorParams;
        delayGainCalculatorParams.sensitivity = 1.0;
        delayGainCalculatorParams.threshold_dB = -6.0;
        delayGainCalculatorParams.wetGainMin_dB = 0.0;
        delayGainCalculatorParams.wetGainMax_dB = 0.0;
        delayGainCalculator.setParameters(delayGainCalculatorParams);
    }/* C-TOR */

    ~EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>() override = default; /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>(const EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>&) = delete;
    EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>& operator=(const EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>&) = delete;

    // Suppress generation of move constructor and move assignment operator
    EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>(const EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>&&) = delete;
    EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>& operator=(const EnvelopeDetectorSideChainSignalProcessor<SideChainProcessorParams>&&) = delete;

    /** reset members to initialized state */
    bool reset(double _sampleRate) override
    {
	    envDetector.reset(_sampleRate);
	    delayGainCalculator.reset(_sampleRate);
	    return true;
    }

    /** process audio through Clipping Stage */
    /**
    \param xn input
    \return the processed sample
    */
    double processAudioSample(double xn) override
    {
        // Amplify signal by sideChainGain
        const double sideChainGain = pow(10.0, parameters.sideChainGain_dB / 20.0);
        xn *= sideChainGain;

        // --- detect the signal
	    const double detect_dB = envDetector.processAudioSample(xn);
	    const double detectValue = pow(10.0, detect_dB / 20.0);

        // Pass it through Delay Gain Calculator
        const double yn = delayGainCalculator.processAudioSample(detectValue);

        return 1 - yn;
    }

    /** return true: this object can also process frames */
    bool canProcessAudioFrame() override
    {
        return false;
    }

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return SideChainSignalProcessorParameters custom data structure
    */
    SideChainProcessorParams getParameters() const override
    {
	    // --- Upcast & return - Is there a better way to achieve polymorphism?
        return parameters;
    }

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    void setParameters(const SideChainProcessorParams& _parameters) override
    {
	    // --- Downcast & save - Is there a better way to achieve polymorphism?
	    parameters = _parameters;

        updateDetectorParameters(parameters);
    }

private:
    EnvelopeDetectorSideChainSignalProcessorParameters parameters;

	AudioDetector envDetector; ///< detector to track input signal
	DelayGainCalculator delayGainCalculator; // Calculate gain to wet signal based on detected envelope

    static bool detectorParametersUpdated(AudioDetectorParameters adParams, const SideChainProcessorParams& params)
    {
	    return !isFloatEqual(adParams.attackTime_mSec, params.attackTime_mSec) ||
		    !isFloatEqual(adParams.releaseTime_mSec, params.releaseTime_mSec);
    }

    void updateDetectorParameters(const SideChainProcessorParams& params)
    {
	    AudioDetectorParameters adParams = envDetector.getParameters();

	    if (detectorParametersUpdated(adParams, params))
	    {
		    adParams.attackTime_mSec = params.attackTime_mSec;
		    adParams.releaseTime_mSec = params.releaseTime_mSec;
		    envDetector.setParameters(adParams);
	    }
    }
};

/**
\class DigitalDelayParameters
\ingroup Custom-FX-Objects
\brief
Custom parameter structure for the DigitalDelay object.

\author Steve Dwyer - Adapted from Will Pirkle http://www.willpirkle.com
\remark This object is based on the AudioDelayParameters class included in Designing Audio Effects Plugins in C++ 2nd Ed. by Will Pirkle
\version Revision : 1.0
\date Date : 2021 / 09 / 06
*/
template <class SideChainProcessorParams>
class DigitalDelayParameters
{
public:
    DigitalDelayParameters<SideChainProcessorParams>() = default;
    ~DigitalDelayParameters<SideChainProcessorParams>() = default;

    // Explicitly use default copy constructor
    DigitalDelayParameters<SideChainProcessorParams>(const DigitalDelayParameters<SideChainProcessorParams>&) = default;

    // Copy assignment operator
    /** all FXObjects parameter objects require overloaded= operator so remember to add new entries if you add new variables. */
    DigitalDelayParameters<SideChainProcessorParams>& operator=(const DigitalDelayParameters<SideChainProcessorParams>& params)
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

        sideChainSignalProcessorParameters = params.sideChainSignalProcessorParameters;

        return *this;
    }

    // Move constructor
    DigitalDelayParameters<SideChainProcessorParams>(DigitalDelayParameters<SideChainProcessorParams>&& params) noexcept
        : algorithm{params.algorithm},
          mix{params.mix},
          Level_dB{params.Level_dB},
          feedback_Pct{params.feedback_Pct},
          updateType{params.updateType},
          leftDelay_mSec{params.leftDelay_mSec},
          rightDelay_mSec{params.rightDelay_mSec},
          delayRatio_Pct{params.delayRatio_Pct},
          emulateAnalog{params.emulateAnalog},
          sideChainSignalProcessorParameters{params.sideChainSignalProcessorParameters}
    {
    }

    // Suppress generation of move assignment operator
    DigitalDelayParameters<SideChainProcessorParams>& operator=(DigitalDelayParameters<SideChainProcessorParams>&&) = default;

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
    SideChainProcessorParams sideChainSignalProcessorParameters;
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
template <class SideChainProcessor, class SideChainProcessorParams>
class DigitalDelay : public IAudioSignalProcessor
{
public:
    DigitalDelay<SideChainProcessor, SideChainProcessorParams>(SideChainProcessor& _sideChainSignalProcessor) :  sideChainSignalProcessor{_sideChainSignalProcessor}
    {
    } /* C-TOR */
    virtual ~DigitalDelay<SideChainProcessor, SideChainProcessorParams>() = default; /* D-TOR */

    // Suppress generation of copy constructor and copy assignment operator
    DigitalDelay<SideChainProcessor, SideChainProcessorParams>(const DigitalDelay<SideChainProcessor, SideChainProcessorParams>&) = delete;
    DigitalDelay<SideChainProcessor, SideChainProcessorParams>& operator=(const DigitalDelay<SideChainProcessor, SideChainProcessorParams>&) = delete;

    // Suppress generation of move constructor and move assignment operator
    DigitalDelay<SideChainProcessor, SideChainProcessorParams>(const DigitalDelay<SideChainProcessor, SideChainProcessorParams>&&) = delete;
    DigitalDelay<SideChainProcessor, SideChainProcessorParams>& operator=(const DigitalDelay<SideChainProcessor, SideChainProcessorParams>&&) = delete;

    /** reset members to initialized state */
    bool reset(double _sampleRate) override
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

    /** process audio through phaser */
    /**
    \param xn input
    \return the processed sample
    */
    double processAudioSample(double xn) override
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

    /** return true: this object can also process frames */
    bool canProcessAudioFrame() override
    {
        return true;
    }

    /** process STEREO audio delay in frames */
    bool processAudioFrame(const float* inputFrame,
                           /* ptr to one frame of data: pInputFrame[0] = left, pInputFrame[1] = right, etc...*/
                           float* outputFrame,
                           uint32_t inputChannels,
                           uint32_t outputChannels) override
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

    /** get parameters: note use of custom structure for passing param data */
    /**
    \return DigitalDelayParameters custom data structure
    */
    DigitalDelayParameters<SideChainProcessorParams> getParameters() const
    {
        return parameters;
    }

    /** set parameters: note use of custom structure for passing param data */
    /**
    \param _parameters custom data structure
    */
    void setParameters(const DigitalDelayParameters<SideChainProcessorParams>& _parameters)
    {
        updateParameters(_parameters);

        // --- save; rest of updates are cheap on CPU
        parameters = _parameters;
    }

    /** creation function */
    void createDelayBuffers(double _sampleRate, double _bufferLength_mSec)
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

private:
    double getOutputMix(double xn, double yn) const
    {
        const double dryMix = 1.0 - mix;
        const double wetMix = sideChainSignalProcessor.processAudioSample(xn) * mix;

        return (dryMix * xn + wetMix * yn) * level_dB;
    }

    double filter(double yn)
    {
        if (parameters.emulateAnalog)
        {
            yn = zvaFilter.processAudioSample(yn);
        }
        return yn;
    }

    double getDn(double xn, double yn)
    {
        return xn + filter((parameters.feedback_Pct * 0.01) * yn);
    }

    void updateParameters(const DigitalDelayParameters<SideChainProcessorParams>& _parameters)
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

        sideChainSignalProcessor.setParameters(_parameters.sideChainSignalProcessorParameters);
    }

    void resetLpf(double _sampleRate)
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

    DigitalDelayParameters<SideChainProcessorParams> parameters; ///< object parameters
    ZVAFilter zvaFilter;
    SideChainProcessor& sideChainSignalProcessor;

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
Tone/Volume stage - https://www.electrosmash.com/tube-screamer-analysis.

Audio I/O:
- Processes mono input to mono output OR stereo  input to stereo output.

Control I/F:
- Use AnalogToneParameters structure to get/set object params.

\author Steve Dwyer - Based on Eric Tarr's Analog Modeling Workshop - https://www.hackaudio.com/
\remark This class is based on the TSTone class designed by Eric Tarr
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
\remark This class is based on the TSClipper class designed by Eric Tarr
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
