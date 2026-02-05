#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class ParticleVisualizer;

class TheColliderAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        public juce::Slider::Listener
{
public:
    TheColliderAudioProcessorEditor(TheColliderAudioProcessor&);
    ~TheColliderAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

private:
    TheColliderAudioProcessor& processorRef;

    // Visualizer
    std::unique_ptr<ParticleVisualizer> visualizer;

    // Preset selector
    juce::ComboBox presetCombo;

    // Physics sliders
    juce::Slider gravityXSlider, gravityYSlider;
    juce::Slider massSlider, elasticitySlider;

    // Resonator sliders
    juce::Slider materialSlider, dampingSlider, brightnessSlider;

    // FM Impulse sliders
    juce::Slider fmDepthSlider, fmRatioSlider;

    // Spectral/FDN sliders
    juce::Slider atmosphereSlider, entropySlider, fdnMixSlider;

    // Output sliders
    juce::Slider masterVolumeSlider, stereoWidthSlider;

    // Parameter attachments
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> attachments;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheColliderAudioProcessorEditor)
};
