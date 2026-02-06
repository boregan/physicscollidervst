#include "PluginEditor.h"
#include "ParticleVisualizer.h"
#include "PresetManager.h"

// ============================================================================
// ParameterSection Implementation
// ============================================================================

ParameterSection::ParameterSection(const juce::String& title, int paramCount)
    : sectionTitle(title)
{
    for (int i = 0; i < paramCount; ++i) {
        auto slider = std::make_unique<AnimatedSlider>();
        addAndMakeVisible(*slider);
        sliders.push_back(std::move(slider));
    }
}

void ParameterSection::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Animated semi-transparent background with border
    float timeVar = std::sin(juce::Time::getMillisecondCounterHiRes() * 0.001f) * 0.5f + 0.5f;
    float bgAlpha = 0.15f + 0.05f * timeVar;
    float borderAlpha = 0.4f + 0.2f * timeVar;

    // Background panel
    g.setColour(juce::Colour(40, 80, 120).withAlpha(bgAlpha));
    g.fillRoundedRectangle(bounds.reduced(2), 8.0f);

    // Border glow
    juce::ColourGradient borderGrad(juce::Colour(100, 180, 255).withAlpha(borderAlpha),
                                     bounds.getX(), bounds.getY(),
                                     juce::Colour(100, 180, 255).withAlpha(borderAlpha * 0.2f),
                                     bounds.getX(), bounds.getY() + 5, false);
    g.setGradientFill(borderGrad);
    g.drawRoundedRectangle(bounds.reduced(2), 8.0f, 1.5f);

    // Title
    g.setColour(juce::Colour(150, 220, 255).withAlpha(0.8f));
    g.setFont(juce::Font(12.0f, juce::Font::bold));
    g.drawText(sectionTitle, bounds.removeFromTop(20).reduced(5), juce::Justification::centred);
}

void ParameterSection::resized()
{
    auto bounds = getLocalBounds().reduced(5);
    bounds.removeFromTop(20);  // Leave space for title

    int sliderSize = 50;
    int spacing = 8;
    int perRow = 3;

    for (size_t i = 0; i < sliders.size(); ++i) {
        int row = (int)i / perRow;
        int col = (int)i % perRow;

        int x = col * (sliderSize + spacing);
        int y = row * (sliderSize + spacing) + 5;

        sliders[i]->setBounds(x, y, sliderSize, sliderSize);
    }
}

// ============================================================================
// TheColliderAudioProcessorEditor Implementation
// ============================================================================

TheColliderAudioProcessorEditor::TheColliderAudioProcessorEditor(TheColliderAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1600, 900);
    startTimer(16);  // ~60fps

    // Create visualizer (background)
    visualizer = std::make_unique<ParticleVisualizer>();
    addAndMakeVisible(*visualizer);

    // Create control sections
    physicsSection = std::make_unique<ParameterSection>("PHYSICS", 2);
    resonatorSection = std::make_unique<ParameterSection>("RESONATOR", 3);
    fmSection = std::make_unique<ParameterSection>("FM IMPULSE", 2);
    spectralSection = std::make_unique<ParameterSection>("SPECTRAL", 3);
    outputSection = std::make_unique<ParameterSection>("OUTPUT", 2);

    addAndMakeVisible(*physicsSection);
    addAndMakeVisible(*resonatorSection);
    addAndMakeVisible(*fmSection);
    addAndMakeVisible(*spectralSection);
    addAndMakeVisible(*outputSection);

    // Setup all sliders with parameter attachments
    auto& physics = physicsSection->getSliders();
    setupAnimatedSlider(*physics[0], "gravityX");
    setupAnimatedSlider(*physics[1], "gravityY");

    auto& resonator = resonatorSection->getSliders();
    setupAnimatedSlider(*resonator[0], "material");
    setupAnimatedSlider(*resonator[1], "damping");
    setupAnimatedSlider(*resonator[2], "brightness");

    auto& fm = fmSection->getSliders();
    setupAnimatedSlider(*fm[0], "fmDepth");
    setupAnimatedSlider(*fm[1], "fmRatio");

    auto& spectral = spectralSection->getSliders();
    setupAnimatedSlider(*spectral[0], "atmosphere");
    setupAnimatedSlider(*spectral[1], "entropy");
    setupAnimatedSlider(*spectral[2], "fdnMix");

    auto& output = outputSection->getSliders();
    setupAnimatedSlider(*output[0], "masterVolume");
    setupAnimatedSlider(*output[1], "stereoWidth");

    // Preset selector
    presetCombo.addItemList(juce::StringArray("Default", "Laser Blip", "Bouncing Ball", "Iron Rain",
                                             "Gravity Well", "Droid Malfunction", "Crystal Cave",
                                             "Hull Breach", "Geiger Counter", "Subspace Ping", "Quantum Flux"), 1);
    presetCombo.setSelectedItemIndex(0);
    presetCombo.setColour(juce::ComboBox::backgroundColourId, juce::Colour(30, 50, 80));
    presetCombo.setColour(juce::ComboBox::textColourId, juce::Colour(150, 220, 255));
    presetCombo.setColour(juce::ComboBox::outlineColourId, juce::Colour(100, 150, 200));
    addAndMakeVisible(presetCombo);

    presetCombo.onChange = [this]() {
        int presetIndex = presetCombo.getSelectedItemIndex();
        if (presetIndex > 0) {
            auto presets = PresetManager::getFactoryPresets();
            if (presetIndex - 1 < (int)presets.size()) {
                processorRef.getAPVTS().state = presets[presetIndex - 1].tree;
            }
        }
    };
}

TheColliderAudioProcessorEditor::~TheColliderAudioProcessorEditor()
{
    stopTimer();
}

void TheColliderAudioProcessorEditor::setupAnimatedSlider(AnimatedSlider& slider, const juce::String& paramID)
{
    slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);

    auto* param = processorRef.getAPVTS().getParameter(paramID);
    if (param != nullptr) {
        attachments.push_back(std::make_unique<juce::SliderParameterAttachment>(*param, slider));
    }

    slider.addListener(this);
}

void TheColliderAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background is drawn by visualizer
}

void TheColliderAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Visualizer fills entire background
    visualizer->setBounds(bounds);

    // Control sections overlay
    int sectionWidth = 150;
    int sectionHeight = 200;
    int padding = 15;
    int topMargin = 50;

    int x = padding;
    int y = topMargin;

    physicsSection->setBounds(x, y, sectionWidth, sectionHeight);
    x += sectionWidth + padding;

    resonatorSection->setBounds(x, y, sectionWidth, sectionHeight);
    x += sectionWidth + padding;

    fmSection->setBounds(x, y, sectionWidth, sectionHeight);
    x += sectionWidth + padding;

    spectralSection->setBounds(x, y, sectionWidth, sectionHeight);
    x += sectionWidth + padding;

    outputSection->setBounds(x, y, sectionWidth, sectionHeight);

    // Preset selector (top left corner)
    presetCombo.setBounds(padding, padding, 180, 28);
}

void TheColliderAudioProcessorEditor::timerCallback()
{
    // Smooth animation state updates
    hoverAmountGlobal *= 0.95f;
    visualizer->repaint();
}

void TheColliderAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // Trigger visual feedback
    hoverAmountGlobal = 1.0f;
}

void TheColliderAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
    lastMousePos = event.getPosition();
    hoverAmountGlobal = juce::jmin(1.0f, hoverAmountGlobal + 0.1f);
}
