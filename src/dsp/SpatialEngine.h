#pragma once

#include <juce_core/juce_core.h>

struct StereoSample {
    float left, right;
};

class SpatialEngine
{
public:
    SpatialEngine();
    ~SpatialEngine();

    void prepare(double sampleRate);

    StereoSample process(float input, float particleX, float particleY, float stereoWidth);

private:
    double sampleRate = 44100.0;

    // Equal-power panning
    float panGainLeft(float pan);
    float panGainRight(float pan);
};
