#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>

class Particle;
class ResonatorBank;
class FMImpulseGenerator;
class CollisionDetector;
class SpectralSmearer;

class TheColliderAudioProcessor : public juce::AudioProcessor,
                                   public juce::AudioProcessorValueTreeState::Listener
{
public:
    TheColliderAudioProcessor();
    ~TheColliderAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "The Collider"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 5.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // AudioProcessorValueTreeState access
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    // Parameter callback
    void parameterChanged(const juce::String& parameterID, float newValue) override;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    static constexpr int VOICE_COUNT = 8;

    // Voice management
    std::array<std::unique_ptr<Particle>, VOICE_COUNT> voices;
    std::array<std::unique_ptr<ResonatorBank>, VOICE_COUNT> resonatorBanks;
    std::array<std::unique_ptr<FMImpulseGenerator>, VOICE_COUNT> fmGenerators;
    std::unique_ptr<CollisionDetector> collisionDetector;
    std::unique_ptr<SpectralSmearer> spectralSmearer;

    double sampleRate = 44100.0;
    int nextVoiceIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TheColliderAudioProcessor)
};
