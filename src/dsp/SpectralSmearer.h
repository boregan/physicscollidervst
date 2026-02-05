#pragma once

#include <juce_core/juce_core.h>
#include <array>

class SpectralSmearer
{
public:
    static constexpr int FDN_LINES = 4;

    SpectralSmearer();
    ~SpectralSmearer();

    void prepare(double sampleRate, int samplesPerBlock);
    float process(float input);

    void setFDNMix(float mix);
    void setAtmosphere(float atmosphere);
    void setEntropy(float entropy);
    void setChaosType(int type);

private:
    double sampleRate = 44100.0;

    // FDN delay lines
    std::array<std::array<float, 16384>, FDN_LINES> delayBuffers;
    std::array<int, FDN_LINES> writeIndices;
    std::array<float, FDN_LINES> lastLPFilterOutputs;

    float fdnMix = 0.3f;
    float atmosphere = 0.5f;
    float entropy = 0.2f;
    int chaosType = 0;

    // Lorenz attractor for delay modulation
    float lorenzX = 0.1f, lorenzY = 0.0f, lorenzZ = 0.0f;
};
