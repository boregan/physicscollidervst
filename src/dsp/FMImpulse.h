#pragma once

#include <juce_core/juce_core.h>
#include <array>

class FMImpulseGenerator
{
public:
    FMImpulseGenerator();
    ~FMImpulseGenerator();

    void prepare(double sampleRate, int samplesPerBlock);
    float process();

    void trigger(float velocity, float zapDrop, float zapSpeed, float fmDepth, float fmRatio);
    bool isActive() const { return active; }

private:
    // Sampling
    double sampleRate = 44100.0;
    int oversampleRatio = 4;

    // Phase accumulators (double precision for phase coherence)
    double carrierPhase = 0.0;
    double modulatorPhase = 0.0;

    // Frequency sweeps (exponential decay)
    float carrierFreqStart = 0.0f;
    float carrierFreqEnd = 0.0f;
    float modulatorFreqStart = 0.0f;
    float modulatorFreqEnd = 0.0f;

    float fmDepth = 0.0f;
    float fmRatio = 1.0f;

    // Envelope
    float envelopeGain = 0.0f;
    float envelopeDecayRate = 0.99f;

    // Duration
    int samplesRemaining = 0;
    int totalDuration = 0;
    bool active = false;

    // Halfband filter state for downsampling (simple single-pole for now)
    float lpState = 0.0f;
    float lpCoeff = 0.5f;
};

