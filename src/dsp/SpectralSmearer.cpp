#include "SpectralSmearer.h"
#include <cmath>

constexpr float PI = 3.141592653589793f;

SpectralSmearer::SpectralSmearer()
{
    // Initialize delay buffers
    for (auto& buffer : delayBuffers)
        buffer.fill(0.0f);

    writeIndices.fill(0);
    lastLPFilterOutputs.fill(0.0f);
    lpCoeffs.fill(0.5f);

    // Mutually prime base delay lengths (Section 7.3)
    // At 44.1kHz:
    baseDelayLengths[0] = 140;   // ~3.17ms
    baseDelayLengths[1] = 200;   // ~4.53ms
    baseDelayLengths[2] = 296;   // ~6.71ms
    baseDelayLengths[3] = 413;   // ~9.37ms

    delayLengths = baseDelayLengths;
}

SpectralSmearer::~SpectralSmearer()
{
}

void SpectralSmearer::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;

    // Recalculate base delays for actual sample rate
    float srRatio = (float)sr / 44100.0f;
    baseDelayLengths[0] = (int)(140 * srRatio);
    baseDelayLengths[1] = (int)(200 * srRatio);
    baseDelayLengths[2] = (int)(296 * srRatio);
    baseDelayLengths[3] = (int)(413 * srRatio);

    delayLengths = baseDelayLengths;

    // Initialize LP filter coefficients for atmosphere damping
    for (int i = 0; i < FDN_LINES; ++i) {
        lpCoeffs[i] = 2.0f / (500.0f + 1.0f);  // ~100Hz LP, will be modulated by atmosphere
    }

    for (auto& buffer : delayBuffers)
        buffer.fill(0.0f);

    writeIndices.fill(0);
    lastLPFilterOutputs.fill(0.0f);
}

void SpectralSmearer::hadamardFeedback(std::array<float, FDN_LINES>& signals)
{
    // 4x4 Hadamard matrix (normalized by 1/2)
    // Every delay line feeds into every other with +/- pattern
    std::array<float, FDN_LINES> output;
    output[0] = (signals[0] + signals[1] + signals[2] + signals[3]) * 0.5f;
    output[1] = (signals[0] - signals[1] + signals[2] - signals[3]) * 0.5f;
    output[2] = (signals[0] + signals[1] - signals[2] - signals[3]) * 0.5f;
    output[3] = (signals[0] - signals[1] - signals[2] + signals[3]) * 0.5f;
    signals = output;
}

void SpectralSmearer::updateChaosSystem()
{
    // All chaos systems run at audio rate, but slowly drift the delays
    float dt = 0.0001f;  // Integration step

    if (chaosType == ChaosType::Lorenz) {
        // Lorenz attractor (Section 7.6)
        float sigma = 10.0f, rho = 28.0f, beta = 8.0f / 3.0f;
        float dx = sigma * (lorenzY - lorenzX);
        float dy = lorenzX * (rho - lorenzZ) - lorenzY;
        float dz = lorenzX * lorenzY - beta * lorenzZ;
        lorenzX += dx * dt;
        lorenzY += dy * dt;
        lorenzZ += dz * dt;
    } else if (chaosType == ChaosType::Rossler) {
        // Rössler attractor
        float a = 0.2f, b = 0.2f, c = 5.7f;
        float dx = -rosslerY - rosslerZ;
        float dy = rosslerX + a * rosslerY;
        float dz = b + rosslerZ * (rosslerX - c);
        rosslerX += dx * dt;
        rosslerY += dy * dt;
        rosslerZ += dz * dt;
    } else if (chaosType == ChaosType::DoublePendulum) {
        // Simplified double pendulum energy exchange
        float m1 = 1.0f, m2 = 1.0f, l = 1.0f, g = 9.81f;
        pendulumOmega -= (g / l) * std::sin(pendulumTheta) * dt;
        pendulumTheta += pendulumOmega * dt;
    } else if (chaosType == ChaosType::LFO) {
        // Standard LFO fallback
        lfoPhase += dt * 5.0f;
        if (lfoPhase > 1.0f)
            lfoPhase -= 1.0f;
    }
}

void SpectralSmearer::updateDelayModulation()
{
    // Map chaos outputs to delay modulation
    float modAmount = entropy * 0.5f;  // Max ±50% modulation

    for (int i = 0; i < FDN_LINES; ++i) {
        float modSignal = 0.0f;

        if (chaosType == ChaosType::Lorenz) {
            if (i == 0) modSignal = lorenzX / 25.0f;
            else if (i == 1) modSignal = lorenzY / 30.0f;
            else if (i == 2) modSignal = (lorenzZ - 25.0f) / 20.0f;
            else modSignal = (lorenzX / 25.0f) * (lorenzY / 30.0f);
        } else if (chaosType == ChaosType::Rossler) {
            modSignal = (i % 3 == 0) ? rosslerX / 10.0f : (i % 3 == 1) ? rosslerY / 10.0f : rosslerZ / 10.0f;
        } else if (chaosType == ChaosType::LFO) {
            modSignal = std::sin(2.0f * PI * (lfoPhase + i * 0.25f));
        }

        modSignal = juce::jlimit(-1.0f, 1.0f, modSignal);
        int modulatedDelay = baseDelayLengths[i] * (1.0f + modSignal * modAmount);
        delayLengths[i] = juce::jlimit(10, MAX_DELAY - 1, modulatedDelay);
    }
}

float SpectralSmearer::processFDN(float input)
{
    // Read from all delay lines
    std::array<float, FDN_LINES> delayedSignals;
    for (int i = 0; i < FDN_LINES; ++i) {
        int readIdx = (writeIndices[i] - delayLengths[i] + MAX_DELAY) % MAX_DELAY;
        delayedSignals[i] = delayBuffers[i][readIdx];
    }

    // Apply LP filter to each delay line (atmosphere control)
    // Higher atmosphere = brighter (higher LP cutoff)
    float atmosphericDamping = juce::jmap(atmosphere, 0.0f, 1.0f, 0.02f, 0.5f);
    for (int i = 0; i < FDN_LINES; ++i) {
        lastLPFilterOutputs[i] += atmosphericDamping * (delayedSignals[i] - lastLPFilterOutputs[i]);
        delayedSignals[i] = lastLPFilterOutputs[i];
    }

    // Hadamard feedback distribution
    hadamardFeedback(delayedSignals);

    // Mix input with feedback
    for (int i = 0; i < FDN_LINES; ++i)
        delayedSignals[i] = input * 0.5f + delayedSignals[i] * 0.5f;

    // Write to delay lines
    for (int i = 0; i < FDN_LINES; ++i) {
        delayBuffers[i][writeIndices[i]] = delayedSignals[i];
        writeIndices[i] = (writeIndices[i] + 1) % MAX_DELAY;
    }

    // Sum outputs
    float output = 0.0f;
    for (int i = 0; i < FDN_LINES; ++i)
        output += delayedSignals[i];

    return output / (float)FDN_LINES;
}

float SpectralSmearer::process(float input)
{
    // Update chaos system (slowly modulates delays)
    static int chaosCounter = 0;
    if (++chaosCounter >= 10) {
        updateChaosSystem();
        updateDelayModulation();
        chaosCounter = 0;
    }

    // Process FDN
    float wetSignal = processFDN(input);

    // Mix dry/wet
    float drySignal = input;
    float output = juce::jmap(fdnMix, 0.0f, 1.0f, drySignal, wetSignal);

    return output;
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
    chaosType = (ChaosType)juce::jlimit(0, 3, type);
}
