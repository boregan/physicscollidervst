#include "FMImpulse.h"
#include <cmath>

constexpr float PI = 3.141592653589793f;

FMImpulseGenerator::FMImpulseGenerator()
{
}

FMImpulseGenerator::~FMImpulseGenerator()
{
}

void FMImpulseGenerator::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    lpCoeff = 2.0f / (float)(sr / (float)oversampleRatio + 1.0f);
}

float FMImpulseGenerator::process()
{
    if (!active || samplesRemaining <= 0)
        return 0.0f;

    float accumulator = 0.0f;

    // Process at 4x oversampling rate
    for (int os = 0; os < oversampleRatio; ++os) {
        // Interpolate frequencies (exponential sweep)
        float progress = 1.0f - (samplesRemaining / (float)totalDuration);
        float expDecay = std::exp(-progress * 3.0f);

        float carrierFreq = carrierFreqStart * expDecay + carrierFreqEnd * (1.0f - expDecay);
        float modulatorFreq = modulatorFreqStart * expDecay + modulatorFreqEnd * (1.0f - expDecay);

        // Update phase accumulators
        carrierPhase += carrierFreq / (sampleRate * oversampleRatio);
        modulatorPhase += modulatorFreq / (sampleRate * oversampleRatio);

        // Wrap phases to prevent numerical drift (keep in 0..1 range)
        if (carrierPhase > 1.0)
            carrierPhase -= 1.0;
        if (modulatorPhase > 1.0)
            modulatorPhase -= 1.0;

        // Phase Modulation (through-zero stable)
        // modSignal modulates the carrier phase directly
        float modSignal = std::sin(2.0f * PI * modulatorPhase) * fmDepth;
        float output = std::sin(2.0f * PI * (carrierPhase + modSignal));

        // Exponential decay envelope (independent of sweep)
        output *= envelopeGain;

        accumulator += output;
    }

    // Average oversampled output
    accumulator /= (float)oversampleRatio;

    // Simple one-pole LP filter for downsampling
    lpState += lpCoeff * (accumulator - lpState);
    float filtered = lpState;

    // Update envelope
    envelopeGain *= envelopeDecayRate;

    samplesRemaining--;
    if (samplesRemaining <= 0)
        active = false;

    return filtered;
}

void FMImpulseGenerator::trigger(float velocity, float zapDrop, float zapSpeed, float fmDepth_val, float fmRatio_val)
{
    // Map velocity to starting frequency
    // Velocity 1.0 (max) → 8000 Hz, Velocity 0.0 (min) → 200 Hz
    float carrierStartFreq = juce::jmap(velocity, 0.0f, 1.0f, 200.0f, 8000.0f);

    // Zap drop: pitch falls by N octaves
    // Each octave = factor of 2
    float octaveDrop = zapDrop;
    float carrierEndFreq = carrierStartFreq / std::pow(2.0f, octaveDrop);

    carrierFreqStart = carrierStartFreq;
    carrierFreqEnd = carrierEndFreq;

    // Modulator frequencies (same sweep pattern)
    fmRatio = juce::jlimit(0.5f, 8.0f, fmRatio_val);
    modulatorFreqStart = carrierStartFreq / fmRatio;
    modulatorFreqEnd = carrierEndFreq / fmRatio;

    // FM depth (scaled by velocity for dynamic expressiveness)
    fmDepth = fmDepth_val * velocity * 5.0f;
    fmDepth = juce::jlimit(0.0f, 50.0f, fmDepth);

    // Duration in samples
    totalDuration = (int)((zapSpeed / 1000.0) * sampleRate);
    samplesRemaining = totalDuration;

    // Initial envelope (exponential decay)
    envelopeGain = 1.0f;
    // Compute decay rate: reach ~0 at end of sweep
    envelopeDecayRate = std::pow(0.001f, 1.0f / (float)totalDuration);

    // Reset filter state
    lpState = 0.0f;
    carrierPhase = 0.0;
    modulatorPhase = 0.0;

    active = true;
}
