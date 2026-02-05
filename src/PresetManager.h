#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

class PresetManager
{
public:
    struct Preset {
        juce::String name;
        juce::ValueTree tree;
    };

    static std::vector<Preset> getFactoryPresets();
    static juce::ValueTree createPreset(const juce::String& name,
                                       int launchMode, int burstCount, float streamRate,
                                       float mass, float elasticity, float lifetime,
                                       float gravityX, float gravityY,
                                       float material, float position, float damping, float brightness,
                                       float fmDepth, float fmRatio, float zapSpeed, float zapDrop,
                                       float fdnMix, float atmosphere, float entropy, int chaosType);
};
