#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "dsp/ResonatorBank.h"
#include "dsp/FMImpulse.h"
#include "dsp/CollisionDetector.h"
#include "dsp/SpectralSmearer.h"

namespace Param {
    // Particle Parameters
    inline constexpr auto LaunchMode = "launchMode";
    inline constexpr auto BurstCount = "burstCount";
    inline constexpr auto StreamRate = "streamRate";
    inline constexpr auto Mass = "mass";
    inline constexpr auto Elasticity = "elasticity";
    inline constexpr auto Lifetime = "lifetime";
    inline constexpr auto Charge = "charge";

    // Simulation Parameters
    inline constexpr auto GravityX = "gravityX";
    inline constexpr auto GravityY = "gravityY";
    inline constexpr auto FieldMode = "fieldMode";
    inline constexpr auto FieldStrength = "fieldStrength";
    inline constexpr auto WallTop = "wallTop";
    inline constexpr auto WallBottom = "wallBottom";
    inline constexpr auto WallLeft = "wallLeft";
    inline constexpr auto WallRight = "wallRight";

    // Resonator Parameters
    inline constexpr auto ModeCount = "modeCount";
    inline constexpr auto Material = "material";
    inline constexpr auto Position = "position";
    inline constexpr auto Damping = "damping";
    inline constexpr auto Brightness = "brightness";

    // FM Impulse Parameters
    inline constexpr auto FMDepth = "fmDepth";
    inline constexpr auto FMRatio = "fmRatio";
    inline constexpr auto ZapSpeed = "zapSpeed";
    inline constexpr auto ZapDrop = "zapDrop";

    // Spectral Smear Parameters
    inline constexpr auto FDNMix = "fdnMix";
    inline constexpr auto Atmosphere = "atmosphere";
    inline constexpr auto Entropy = "entropy";
    inline constexpr auto ChaosType = "chaosType";

    // Master Parameters
    inline constexpr auto MasterVolume = "masterVolume";
    inline constexpr auto ResFmMix = "resFmMix";
    inline constexpr auto StereoWidth = "stereoWidth";
    inline constexpr auto Space = "space";
}

TheColliderAudioProcessor::TheColliderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
        ),
#endif
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    for (int i = 0; i < VOICE_COUNT; ++i) {
        apvts.addParameterListener(Param::Mass, this);
        apvts.addParameterListener(Param::Elasticity, this);
        apvts.addParameterListener(Param::Damping, this);
        apvts.addParameterListener(Param::Material, this);
    }
}

TheColliderAudioProcessor::~TheColliderAudioProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout
TheColliderAudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    // Particle Parameters
    params.push_back(std::make_unique<AudioParameterChoice>(
        Param::LaunchMode, "Launch Mode",
        StringArray("Single Shot", "Burst", "Stream", "Scatter"), 0));

    params.push_back(std::make_unique<AudioParameterInt>(
        Param::BurstCount, "Burst Count", 2, 16, 4));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::StreamRate, "Stream Rate", NormalisableRange<float>(10.0f, 500.0f), 50.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Mass, "Mass",
        NormalisableRange<float>(0.1f, 10.0f, 0.0f, 0.5f), 1.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Elasticity, "Elasticity", NormalisableRange<float>(0.0f, 1.5f), 0.8f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Lifetime, "Lifetime",
        NormalisableRange<float>(0.1f, 30.0f, 0.0f, 0.3f), 5.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Charge, "Charge", NormalisableRange<float>(-1.0f, 1.0f), 0.0f));

    // Simulation Parameters
    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::GravityX, "Gravity X", NormalisableRange<float>(-1.0f, 1.0f), 0.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::GravityY, "Gravity Y", NormalisableRange<float>(-1.0f, 1.0f), 0.3f));

    params.push_back(std::make_unique<AudioParameterChoice>(
        Param::FieldMode, "Field Mode",
        StringArray("None", "Attract", "Repel", "Orbit"), 0));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::FieldStrength, "Field Strength", NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    // Wall Materials
    StringArray materials("Metal", "Glass", "Wood", "Membrane", "Crystal", "Void", "Sticky");
    params.push_back(std::make_unique<AudioParameterChoice>(
        Param::WallTop, "Wall Top Material", materials, 0));
    params.push_back(std::make_unique<AudioParameterChoice>(
        Param::WallBottom, "Wall Bottom Material", materials, 0));
    params.push_back(std::make_unique<AudioParameterChoice>(
        Param::WallLeft, "Wall Left Material", materials, 0));
    params.push_back(std::make_unique<AudioParameterChoice>(
        Param::WallRight, "Wall Right Material", materials, 0));

    // Resonator Parameters
    params.push_back(std::make_unique<AudioParameterInt>(
        Param::ModeCount, "Mode Count", 4, 32, 16));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Material, "Material", NormalisableRange<float>(0.0f, 1.0f), 0.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Position, "Strike Position", NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Damping, "Damping", NormalisableRange<float>(0.0f, 1.0f), 0.3f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Brightness, "Brightness", NormalisableRange<float>(0.0f, 1.0f), 0.7f));

    // FM Impulse Parameters
    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::FMDepth, "FM Depth", NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::FMRatio, "FM Ratio", NormalisableRange<float>(0.5f, 8.0f), 1.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::ZapSpeed, "Zap Speed",
        NormalisableRange<float>(5.0f, 500.0f, 0.0f, 0.4f), 50.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::ZapDrop, "Zap Drop", NormalisableRange<float>(1.0f, 8.0f), 4.0f));

    // Spectral Smear Parameters
    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::FDNMix, "FDN Mix", NormalisableRange<float>(0.0f, 1.0f), 0.3f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Atmosphere, "Atmosphere", NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Entropy, "Entropy", NormalisableRange<float>(0.0f, 1.0f), 0.2f));

    params.push_back(std::make_unique<AudioParameterChoice>(
        Param::ChaosType, "Chaos Type",
        StringArray("Lorenz", "Rössler", "DoublePendulum", "LFO"), 0));

    // Master Parameters
    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::MasterVolume, "Volume",
        NormalisableRange<float>(-60.0f, 6.0f), -6.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::ResFmMix, "Res/FM Mix", NormalisableRange<float>(0.0f, 1.0f), 0.7f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::StereoWidth, "Stereo Width", NormalisableRange<float>(0.0f, 2.0f), 1.0f));

    params.push_back(std::make_unique<AudioParameterFloat>(
        Param::Space, "Space", NormalisableRange<float>(0.0f, 1.0f), 0.3f));

    return { params.begin(), params.end() };
}

void TheColliderAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    // Parameter change callbacks will be implemented as needed
}

void TheColliderAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;

    // Initialize voices
    for (int i = 0; i < VOICE_COUNT; ++i) {
        resonatorBanks[i] = std::make_unique<ResonatorBank>();
        resonatorBanks[i]->prepare(sampleRate, samplesPerBlock);

        fmGenerators[i] = std::make_unique<FMImpulseGenerator>();
        fmGenerators[i]->prepare(sampleRate, samplesPerBlock);
    }

    // Initialize shared systems
    collisionDetector = std::make_unique<CollisionDetector>();
    spectralSmearer = std::make_unique<SpectralSmearer>();
    spectralSmearer->prepare(sampleRate, samplesPerBlock);
}

void TheColliderAudioProcessor::releaseResources()
{
    // Cleanup
}

bool TheColliderAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
#endif
}

void TheColliderAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Process MIDI input (will be implemented in Phase 2)
    for (auto metadata : midiMessages) {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn()) {
            // Launch particle - implementation in next phase
        } else if (msg.isNoteOff()) {
            // Stop particle - implementation in next phase
        }
    }

    // For now, output silence
    buffer.clear();
}

juce::AudioProcessorEditor* TheColliderAudioProcessor::createEditor()
{
    return new TheColliderAudioProcessorEditor(*this);
}

void TheColliderAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyValueTreeToXml(true);
    copyXmlToBinary(*state, destData);
}

void TheColliderAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState != nullptr)
        apvts.replaceValueTreeFromXml(*xmlState);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TheColliderAudioProcessor();
}
