#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <deque>

struct ParticleTrail {
    float x, y;
    float age;  // 0.0 to 1.0 (1.0 = newest, 0.0 = oldest)
};

class ParticleVisualizer : public juce::Component, public juce::Timer
{
public:
    ParticleVisualizer();
    ~ParticleVisualizer() override;

    void paint(juce::Graphics& g) override;
    void timerCallback() override;

    void addParticlePosition(float x, float y);
    void setActiveVoiceCount(int count) { activeVoices = count; }

private:
    std::deque<ParticleTrail> trails;
    static constexpr int MAX_TRAIL_LENGTH = 100;
    int activeVoices = 0;

    void drawParticleTrail(juce::Graphics& g);
    void drawBoundaries(juce::Graphics& g);
};
