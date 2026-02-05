#include "ResonatorBank.h"
#include <cmath>

// ResonatorMode - TPT SVF Implementation
void ResonatorMode::prepare(double sr)
{
    sampleRate = sr;
    ic1eq = 0.0f;
    ic2eq = 0.0f;
}

float ResonatorMode::process(float input)
{
    if (sampleRate == 0.0)
        return 0.0f;

    float g = std::tan(3.141592653589793f * frequency / (float)sampleRate);
    float k = 1.0f / q;
    float a1 = 1.0f / (1.0f + g * (g + k));
    float a2 = g * a1;
    float a3 = g * a2;

    float v3 = input - ic2eq;
    float v1 = a1 * ic1eq + a2 * v3;
    float v2 = ic2eq + a2 * ic1eq + a3 * v3;

    ic1eq = 2.0f * v1 - ic1eq;
    ic2eq = 2.0f * v2 - ic2eq;

    float bandpass = v1;
    return bandpass * gain;
}

void ResonatorMode::setFrequency(float freq)
{
    frequency = juce::jlimit(20.0f, 20000.0f, freq);
}

void ResonatorMode::setQ(float q_val)
{
    q = juce::jlimit(1.0f, 10000.0f, q_val);
}

void ResonatorMode::setGain(float g)
{
    gain = g;
}

// ResonatorBank
ResonatorBank::ResonatorBank()
{
    for (auto& mode : modes)
        mode.prepare(44100.0);
}

ResonatorBank::~ResonatorBank()
{
}

void ResonatorBank::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    for (auto& mode : modes)
        mode.prepare(sr);
    updateModeFrequencies();
}

float ResonatorBank::process(float input)
{
    float output = 0.0f;
    for (int i = 0; i < modeCount; ++i)
        output += modes[i].process(input);
    return output / (float)modeCount;
}

void ResonatorBank::excite(float impactForce, float mat, float pos, float damp, int count)
{
    material = mat;
    position = pos;
    damping = damp;
    setModeCount(count);
    updateModeFrequencies();
    updateModeGains();

    for (int i = 0; i < modeCount; ++i) {
        float modeDecay = 1.0f / (1.0f + i * 0.1f);
        float impactFactor = 1.0f + impactForce * 0.05f;
        float baseQ = juce::jmap(damping, 0.0f, 1.0f, 100.0f, 5000.0f);
        float q = baseQ * modeDecay / impactFactor;
        modes[i].setQ(q);
    }
}

void ResonatorBank::setModeCount(int count)
{
    modeCount = juce::jlimit(4, MODE_COUNT, count);
}

void ResonatorBank::setMaterial(float mat)
{
    material = juce::jlimit(0.0f, 1.0f, mat);
    updateModeFrequencies();
}

void ResonatorBank::setPosition(float pos)
{
    position = juce::jlimit(0.0f, 1.0f, pos);
    updateModeGains();
}

void ResonatorBank::setDamping(float damp)
{
    damping = juce::jlimit(0.0f, 1.0f, damp);
}

void ResonatorBank::updateModeFrequencies()
{
    for (int i = 0; i < modeCount; ++i)
        modes[i].setFrequency(getModeFrequency(i));
}

void ResonatorBank::updateModeGains()
{
    for (int i = 0; i < modeCount; ++i)
        modes[i].setGain(getModeGain(i));
}

float ResonatorBank::getModeFrequency(int modeIndex)
{
    // Fundamental frequency (would be set from MIDI note - hardcoded to A4 for now)
    float fundamental = 440.0f;

    // Interpolate between harmonic and inharmonic based on material parameter
    // Material 0.0 = harmonic series (1:2:3:4...)
    // Material 1.0 = randomized/inharmonic

    float ratio = 1.0f + modeIndex;

    if (material < 0.25f) {
        // Harmonic series with subtle detuning
        float t = material / 0.25f;
        float detuning = 1.0f + t * 0.03f * modeIndex;
        ratio = (1.0f + modeIndex) * detuning;
    } else if (material < 0.5f) {
        // Bell-like inharmonic
        ratio = (1.0f + modeIndex) * (1.0f + material * 0.3f);
    } else if (material < 0.75f) {
        // More inharmonic
        ratio = (1.0f + modeIndex * 0.7f) * (1.0f + material * 0.5f);
    } else {
        // Highly inharmonic
        ratio = (1.0f + modeIndex * 0.5f) * (1.0f + material * 0.7f);
    }

    return juce::jlimit(20.0f, 20000.0f, fundamental * ratio);
}

float ResonatorBank::getModeGain(int modeIndex)
{
    // Position parameter controls where the object is struck
    // 0.5 = center (emphasizes odd harmonics)
    // 0.0 or 1.0 = edge (flat distribution)

    float k = modeIndex + 1.0f;
    float gainValue = std::sin(3.141592653589793f * k * position);
    return juce::jlimit(0.0f, 1.0f, std::abs(gainValue));
}
