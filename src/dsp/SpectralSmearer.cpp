#include "SpectralSmearer.h"

SpectralSmearer::SpectralSmearer()
{
    for (auto& buffer : delayBuffers)
        buffer.fill(0.0f);
    writeIndices.fill(0);
    lastLPFilterOutputs.fill(0.0f);
}

SpectralSmearer::~SpectralSmearer()
{
}

void SpectralSmearer::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    for (auto& buffer : delayBuffers)
        buffer.fill(0.0f);
    writeIndices.fill(0);
    lastLPFilterOutputs.fill(0.0f);
}

float SpectralSmearer::process(float input)
{
    // Phase 1 stub - just return dry signal
    return input;
}

void SpectralSmearer::setFDNMix(float mix)
{
    fdnMix = juce::jlimit(0.0f, 1.0f, mix);
}

void SpectralSmearer::setAtmosphere(float atm)
{
    atmosphere = juce::jlimit(0.0f, 1.0f, atm);
}

void SpectralSmearer::setEntropy(float ent)
{
    entropy = juce::jlimit(0.0f, 1.0f, ent);
}

void SpectralSmearer::setChaosType(int type)
{
    chaosType = juce::jlimit(0, 3, type);
}
