#include "PluginEditor.h"
#include "ParticleVisualizer.h"
#include "PresetManager.h"

TheColliderAudioProcessorEditor::TheColliderAudioProcessorEditor(TheColliderAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1200, 700);
    setLookAndFeel(&getLookAndFeel());

    // Create visualizer
    visualizer = std::make_unique<ParticleVisualizer>();
    addAndMakeVisible(*visualizer);

    // Preset selector
    presetCombo.addItemList(juce::StringArray("Default", "Laser Blip", "Bouncing Ball", "Iron Rain",
                                             "Gravity Well", "Droid Malfunction", "Crystal Cave",
                                             "Hull Breach", "Geiger Counter", "Subspace Ping", "Quantum Flux"), 1);
    presetCombo.setSelectedItemIndex(0);
    addAndMakeVisible(presetCombo);

    // Setup sliders - organized by section
    auto setupSlider = [this, &p](juce::Slider& slider, const juce::String& paramID) {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 18);
        addAndMakeVisible(slider);
        auto* param = p.getAPVTS().getParameter(paramID);
        if (param != nullptr) {
            attachments.push_back(std::make_unique<juce::SliderParameterAttachment>(*param, slider));
        }
    };

    setupSlider(gravityXSlider, "gravityX");
    setupSlider(gravityYSlider, "gravityY");
    setupSlider(massSlider, "mass");
    setupSlider(elasticitySlider, "elasticity");
    setupSlider(materialSlider, "material");
    setupSlider(dampingSlider, "damping");
    setupSlider(brightnessSlider, "brightness");
    setupSlider(fmDepthSlider, "fmDepth");
    setupSlider(fmRatioSlider, "fmRatio");
    setupSlider(entropySlider, "entropy");
    setupSlider(atmosphereSlider, "atmosphere");
    setupSlider(masterVolumeSlider, "masterVolume");
    setupSlider(stereoWidthSlider, "stereoWidth");
    setupSlider(fdnMixSlider, "fdnMix");
}

TheColliderAudioProcessorEditor::~TheColliderAudioProcessorEditor()
{
}

void TheColliderAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(20, 20, 25));

    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawText("THE COLLIDER - Physics Particle Synthesizer", 10, 10, 400, 25, juce::Justification::left);

    // Section headers
    g.setFont(11.0f);
    g.setColour(juce::Colour::fromRGB(150, 150, 180));

    int yOffset = 280;
    g.drawText("PHYSICS", 15, yOffset, 80, 18, juce::Justification::left);
    g.drawText("RESONATOR", 130, yOffset, 80, 18, juce::Justification::left);
    g.drawText("FM IMPULSE", 250, yOffset, 100, 18, juce::Justification::left);
    g.drawText("SPECTRAL", 380, yOffset, 80, 18, juce::Justification::left);
    g.drawText("OUTPUT", 510, yOffset, 80, 18, juce::Justification::left);
}

void TheColliderAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // Visualizer takes top 270 pixels
    visualizer->setBounds(10, 45, bounds.getWidth() - 20, 225);

    // Preset selector
    presetCombo.setBounds(10, 280, 180, 25);

    // Sliders organized in 5 columns x 2-3 rows
    int col0 = 15, col1 = 130, col2 = 250, col3 = 380, col4 = 510;
    int sliderWidth = 50, sliderHeight = 60;
    int rowHeight = sliderHeight + 15;

    // Row 1 (y = 315)
    gravityXSlider.setBounds(col0, 315, sliderWidth, sliderHeight);
    materialSlider.setBounds(col1, 315, sliderWidth, sliderHeight);
    fmDepthSlider.setBounds(col2, 315, sliderWidth, sliderHeight);
    atmosphereSlider.setBounds(col3, 315, sliderWidth, sliderHeight);
    masterVolumeSlider.setBounds(col4, 315, sliderWidth, sliderHeight);

    // Row 2 (y = 315 + rowHeight)
    gravityYSlider.setBounds(col0, 315 + rowHeight, sliderWidth, sliderHeight);
    dampingSlider.setBounds(col1, 315 + rowHeight, sliderWidth, sliderHeight);
    fmRatioSlider.setBounds(col2, 315 + rowHeight, sliderWidth, sliderHeight);
    entropySlider.setBounds(col3, 315 + rowHeight, sliderWidth, sliderHeight);
    stereoWidthSlider.setBounds(col4, 315 + rowHeight, sliderWidth, sliderHeight);

    // Row 3 (y = 315 + 2*rowHeight)
    massSlider.setBounds(col0, 315 + 2 * rowHeight, sliderWidth, sliderHeight);
    brightnessSlider.setBounds(col1, 315 + 2 * rowHeight, sliderWidth, sliderHeight);
    elasticitySlider.setBounds(col2, 315 + 2 * rowHeight, sliderWidth, sliderHeight);
    fdnMixSlider.setBounds(col3, 315 + 2 * rowHeight, sliderWidth, sliderHeight);
}

void TheColliderAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    // Slider value changes are handled by SliderParameterAttachment
}
