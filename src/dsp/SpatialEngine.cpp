#include "SpatialEngine.h"
#include <cmath>

constexpr float PI = 3.141592653589793f;

SpatialEngine::SpatialEngine()
{
}

SpatialEngine::~SpatialEngine()
{
}

void SpatialEngine::prepare(double sr)
{
    sampleRate = sr;
}

float SpatialEngine::panGainLeft(float pan)
{
    // pan: 0.0 = center, -1.0 = full left, 1.0 = full right
    // Equal-power panning
    return std::cos((pan + 1.0f) * PI / 4.0f);
}

float SpatialEngine::panGainRight(float pan)
{
    return std::sin((pan + 1.0f) * PI / 4.0f);
}

StereoSample SpatialEngine::process(float input, float particleX, float particleY, float stereoWidth)
{
    // particleX ranges from -1.0 to 1.0
    // -1.0 = full left, 1.0 = full right

    float pan = particleX * stereoWidth;
    pan = juce::jlimit(-1.0f, 1.0f, pan);

    float gainLeft = panGainLeft(pan);
    float gainRight = panGainRight(pan);

    // Optional: subtle brightness change based on Y position
    // particleY > 0 = slightly brighter (towards center)
    float brightnessMod = 1.0f + particleY * 0.05f;
    brightnessMod = juce::jlimit(0.8f, 1.2f, brightnessMod);

    StereoSample output;
    output.left = input * gainLeft * brightnessMod;
    output.right = input * gainRight * brightnessMod;

    return output;
}
