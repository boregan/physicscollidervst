cmake_minimum_required(VERSION 3.21)

project(TheCollider VERSION 0.0.1 LANGUAGES CXX)

# Add JUCE
add_subdirectory(JUCE)

# Create the plugin
juce_add_plugin(TheCollider
    PRODUCT_NAME "TheCollider"
    COMPANY_NAME "PhysicsSynthesizers"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Coll
    PLUGIN_CODE Coll
    FORMATS VST3 AU Standalone
    VST3_CATEGORIES "Synth"
    AU_MAIN_TYPE "kAudioUnitType_MusicDevice"
    BUNDLE_ID "com.PhysicsSynthesizers.TheCollider"
)

# Add source files
target_sources(TheCollider PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/Voice.cpp
    src/PresetManager.cpp
    src/ParticleVisualizer.cpp
    src/dsp/ResonatorBank.cpp
    src/dsp/FMImpulse.cpp
    src/dsp/CollisionDetector.cpp
    src/dsp/SpectralSmearer.cpp
    src/dsp/SpatialEngine.cpp
)

# Link JUCE
target_link_libraries(TheCollider PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

# Set C++ standard
target_compile_features(TheCollider PRIVATE cxx_std_17)
