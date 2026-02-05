#pragma once

#include <juce_core/juce_core.h>
#include <array>

class SpectralSmearer
{
public:
    static constexpr int FDN_LINES = 4;
    static constexpr int MAX_DELAY = 16384;  // ~370ms at 44.1kHz

    enum ChaosType {
        Lorenz = 0,
        Rossler = 1,
        DoublePendulum = 2,
        LFO = 3
    };

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
    std::array<std::array<float, MAX_DELAY>, FDN_LINES> delayBuffers;
    std::array<int, FDN_LINES> writeIndices;
    std::array<int, FDN_LINES> delayLengths;
    std::array<float, FDN_LINES> lastLPFilterOutputs;
    std::array<float, FDN_LINES> lpCoeffs;

    float fdnMix = 0.3f;
    float atmosphere = 0.5f;
    float entropy = 0.2f;
    ChaosType chaosType = ChaosType::Lorenz;

    // Chaotic modulation sources
    float lorenzX = 0.1f, lorenzY = 0.0f, lorenzZ = 0.0f;
    float rosslerX = 0.1f, rosslerY = 0.0f, rosslerZ = 0.0f;
    float pendulumTheta = 0.1f, pendulumOmega = 0.0f;
    float lfoPhase = 0.0f;

    // Base delay times (mutually prime)
    std::array<int, FDN_LINES> baseDelayLengths;

    void updateChaosSystem();
    void updateDelayModulation();
    float processFDN(float input);

    // Hadamard matrix feedback
    void hadamardFeedback(std::array<float, FDN_LINES>& signals);
};
