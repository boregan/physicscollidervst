#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <array>

class ResonatorMode
{
public:
    ResonatorMode() = default;

    void prepare(double sampleRate);
    float process(float input);

    void setFrequency(float freq);
    void setQ(float q);
    void setGain(float gain);

private:
    float ic1eq = 0.0f;
    float ic2eq = 0.0f;
    float frequency = 440.0f;
    float q = 100.0f;
    float gain = 1.0f;
    double sampleRate = 44100.0;
};

class ResonatorBank
{
public:
    static constexpr int MODE_COUNT = 32;

    ResonatorBank();
    ~ResonatorBank();

    void prepare(double sampleRate, int samplesPerBlock);
    float process(float input);

    void excite(float impactForce, float material, float position, float damping, int modeCount, float fundamentalFreq = 440.0f);
    void setModeCount(int count);
    void setMaterial(float material);
    void setPosition(float position);
    void setDamping(float damping);
    void setFundamental(float freq);

private:
    std::array<ResonatorMode, MODE_COUNT> modes;
    double sampleRate = 44100.0;
    int modeCount = 16;
    float material = 0.0f;
    float position = 0.5f;
    float damping = 0.3f;
    float fundamentalFrequency = 440.0f;

    void updateModeFrequencies();
    void updateModeGains();
    float getModeFrequency(int modeIndex);
    float getModeGain(int modeIndex);
};
