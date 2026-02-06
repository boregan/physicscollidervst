#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "AnimatedSlider.h"

class ParticleVisualizer;

class ParameterSection : public juce::Component
{
public:
    ParameterSection(const juce::String& title, int paramCount);
    ~ParameterSection() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    std::vector<std::unique_ptr<AnimatedSlider>>& getSliders() { return sliders; }
    const juce::String& getTitle() const { return sectionTitle; }

private:
    juce::String sectionTitle;
    std::vector<std::unique_ptr<AnimatedSlider>> sliders;
    bool isHovered = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterSection)
};

class TheColliderAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        public juce::Slider::Listener,
                                        public juce::Timer
{
public:
    TheColliderAudioProcessorEditor(TheColliderAudioProcessor&);
    ~TheColliderAudioProcessorEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

    void sliderValueChanged(juce::Slider* slider) override;
    void mouseMove(const juce::MouseEvent& event) override;

private:
    TheColliderAudioProcessor& processorRef;

    // Visualizer (takes full background)
    std::unique_ptr<ParticleVisualizer> visualizer;

    // Preset selector
    juce::ComboBox presetCombo;

    // Control sections
    std::unique_ptr<ParameterSection> physicsSection;
    std::unique_ptr<ParameterSection> resonatorSection;
    std::unique_ptr<ParameterSection> fmSection;
    std::unique_ptr<ParameterSection> spectralSection;
    std::unique_ptr<ParameterSection> outputSection;

    // Parameter attachments
    std::vector<std::unique_ptr<juce::SliderParameterAttachment>> attachments;

    // Animation state
    float hoverAmountGlobal = 0.0f;
    juce::Point<int> lastMousePos;

    void setupAnimatedSlider(AnimatedSlider& slider, const juce::String& paramID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheColliderAudioProcessorEditor)
};
