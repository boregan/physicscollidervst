#pragma once

#include <juce_core/juce_core.h>

class FMImpulseGenerator
{
public:
    FMImpulseGenerator();
    ~FMImpulseGenerator();

    void prepare(double sampleRate, int samplesPerBlock);
    float process();

    void trigger(float velocity, float zapDrop, float zapSpeed, float fmDepth, float fmRatio);
    bool isActive() const;

private:
    double sampleRate = 44100.0;
    double carrierPhase = 0.0;
    double modulatorPhase = 0.0;

    float carrierFreq = 0.0f;
    float modulatorFreq = 0.0f;
    float fmDepth = 0.0f;
    float envelopeGain = 0.0f;

    int samplesRemaining = 0;
    bool active = false;
};
