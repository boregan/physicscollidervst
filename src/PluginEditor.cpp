#include "PluginEditor.h"

TheColliderAudioProcessorEditor::TheColliderAudioProcessorEditor(TheColliderAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(800, 600);
    setLookAndFeel(&getLookAndFeel());

    // Setup sliders
    massSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    massSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(massSlider);
    massAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *p.getAPVTS().getParameter("mass"), massSlider);

    elasticitySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    elasticitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(elasticitySlider);
    elasticityAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *p.getAPVTS().getParameter("elasticity"), elasticitySlider);

    gravityYSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    gravityYSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(gravityYSlider);
    gravityYAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *p.getAPVTS().getParameter("gravityY"), gravityYSlider);
}

TheColliderAudioProcessorEditor::~TheColliderAudioProcessorEditor()
{
}

void TheColliderAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("The Collider", getLocalBounds(), juce::Justification::centredTop, true);

    g.setFont(14.0f);
    g.drawText("Phase 1: Skeleton", getLocalBounds().withTrimmedTop(50), juce::Justification::centredTop, true);
}

void TheColliderAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    bounds = bounds.withTrimmedTop(120);

    auto sliderWidth = bounds.getWidth() / 3;
    massSlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(10));
    elasticitySlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(10));
    gravityYSlider.setBounds(bounds.removeFromLeft(sliderWidth).reduced(10));
}

void TheColliderAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // Slider value changes are handled by SliderParameterAttachment
}
