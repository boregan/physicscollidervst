#include "FMImpulse.h"
#include <cmath>

FMImpulseGenerator::FMImpulseGenerator()
{
}

FMImpulseGenerator::~FMImpulseGenerator()
{
}

void FMImpulseGenerator::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;
}

float FMImpulseGenerator::process()
{
    if (!active || samplesRemaining <= 0)
        return 0.0f;

    carrierPhase += carrierFreq / sampleRate;
    modulatorPhase += modulatorFreq / sampleRate;

    // Wrap phases
    if (carrierPhase > 1.0)
        carrierPhase -= 1.0;
    if (modulatorPhase > 1.0)
        modulatorPhase -= 1.0;

    float modSignal = std::sin(2.0f * 3.141592653589793f * modulatorPhase) * fmDepth;
    float output = std::sin(2.0f * 3.141592653589793f * (carrierPhase + modSignal));

    // Exponential decay envelope
    output *= envelopeGain;
    envelopeGain *= 0.99f;

    samplesRemaining--;
    if (samplesRemaining <= 0)
        active = false;

    return output;
}

void FMImpulseGenerator::trigger(float velocity, float zapDrop, float zapSpeed, float fmDepth_val, float fmRatio)
{
    // Map velocity to starting frequency (127 → 8kHz, 1 → 200Hz)
    carrierFreq = juce::jmap(velocity, 0.0f, 1.0f, 200.0f, 8000.0f);

    // Modulator frequency
    modulatorFreq = carrierFreq / fmRatio;

    // FM depth
    fmDepth = fmDepth_val;

    // Duration in samples
    samplesRemaining = (int)((zapSpeed / 1000.0) * sampleRate);

    // Initial envelope
    envelopeGain = 1.0f;

    active = true;
}

bool FMImpulseGenerator::isActive() const
{
    return active;
}
