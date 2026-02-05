#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

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

    // Test sliders for Phase 1
    juce::Slider massSlider;
    juce::Slider elasticitySlider;
    juce::Slider gravityYSlider;

    std::unique_ptr<juce::SliderParameterAttachment> massAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> elasticityAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> gravityYAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheColliderAudioProcessorEditor)
};
