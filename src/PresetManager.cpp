#include "PresetManager.h"

namespace Param {
    inline constexpr auto LaunchMode = "launchMode";
    inline constexpr auto BurstCount = "burstCount";
    inline constexpr auto StreamRate = "streamRate";
    inline constexpr auto Mass = "mass";
    inline constexpr auto Elasticity = "elasticity";
    inline constexpr auto Lifetime = "lifetime";
    inline constexpr auto Charge = "charge";
    inline constexpr auto GravityX = "gravityX";
    inline constexpr auto GravityY = "gravityY";
    inline constexpr auto FieldMode = "fieldMode";
    inline constexpr auto FieldStrength = "fieldStrength";
    inline constexpr auto WallTop = "wallTop";
    inline constexpr auto WallBottom = "wallBottom";
    inline constexpr auto WallLeft = "wallLeft";
    inline constexpr auto WallRight = "wallRight";
    inline constexpr auto ModeCount = "modeCount";
    inline constexpr auto Material = "material";
    inline constexpr auto Position = "position";
    inline constexpr auto Damping = "damping";
    inline constexpr auto Brightness = "brightness";
    inline constexpr auto FMDepth = "fmDepth";
    inline constexpr auto FMRatio = "fmRatio";
    inline constexpr auto ZapSpeed = "zapSpeed";
    inline constexpr auto ZapDrop = "zapDrop";
    inline constexpr auto FDNMix = "fdnMix";
    inline constexpr auto Atmosphere = "atmosphere";
    inline constexpr auto Entropy = "entropy";
    inline constexpr auto ChaosType = "chaosType";
    inline constexpr auto MasterVolume = "masterVolume";
    inline constexpr auto ResFmMix = "resFmMix";
    inline constexpr auto StereoWidth = "stereoWidth";
    inline constexpr auto Space = "space";
}

juce::ValueTree PresetManager::createPreset(const juce::String& name,
                                           int launchMode, int burstCount, float streamRate,
                                           float mass, float elasticity, float lifetime,
                                           float gravityX, float gravityY,
                                           float material, float position, float damping, float brightness,
                                           float fmDepth, float fmRatio, float zapSpeed, float zapDrop,
                                           float fdnMix, float atmosphere, float entropy, int chaosType)
{
    juce::ValueTree preset("Preset");
    preset.setProperty("name", name, nullptr);

    preset.setProperty(Param::LaunchMode, launchMode, nullptr);
    preset.setProperty(Param::BurstCount, burstCount, nullptr);
    preset.setProperty(Param::StreamRate, streamRate, nullptr);
    preset.setProperty(Param::Mass, mass, nullptr);
    preset.setProperty(Param::Elasticity, elasticity, nullptr);
    preset.setProperty(Param::Lifetime, lifetime, nullptr);
    preset.setProperty(Param::GravityX, gravityX, nullptr);
    preset.setProperty(Param::GravityY, gravityY, nullptr);
    preset.setProperty(Param::Material, material, nullptr);
    preset.setProperty(Param::Position, position, nullptr);
    preset.setProperty(Param::Damping, damping, nullptr);
    preset.setProperty(Param::Brightness, brightness, nullptr);
    preset.setProperty(Param::FMDepth, fmDepth, nullptr);
    preset.setProperty(Param::FMRatio, fmRatio, nullptr);
    preset.setProperty(Param::ZapSpeed, zapSpeed, nullptr);
    preset.setProperty(Param::ZapDrop, zapDrop, nullptr);
    preset.setProperty(Param::FDNMix, fdnMix, nullptr);
    preset.setProperty(Param::Atmosphere, atmosphere, nullptr);
    preset.setProperty(Param::Entropy, entropy, nullptr);
    preset.setProperty(Param::ChaosType, chaosType, nullptr);

    return preset;
}

std::vector<PresetManager::Preset> PresetManager::getFactoryPresets()
{
    std::vector<Preset> presets;

    // 1. Laser Blip - Classic sci-fi UI selection sound
    presets.push_back({
        "Laser Blip",
        createPreset("Laser Blip",
                    0, 4, 50.0f,              // LaunchMode=Single, BurstCount, StreamRate
                    1.0f, 0.8f, 5.0f,         // Mass, Elasticity, Lifetime
                    0.0f, 0.3f,               // GravityX, GravityY
                    0.0f, 0.5f, 0.3f, 0.7f,   // Material, Position, Damping, Brightness
                    0.8f, 1.0f, 50.0f, 4.0f,  // FMDepth, FMRatio, ZapSpeed, ZapDrop
                    0.0f, 0.5f, 0.0f, 0)      // FDNMix, Atmosphere, Entropy, ChaosType
    });

    // 2. Bouncing Ball - Decaying bounce rhythm
    presets.push_back({
        "Bouncing Ball",
        createPreset("Bouncing Ball",
                    0, 4, 50.0f,
                    2.0f, 0.95f, 3.0f,
                    0.0f, 1.0f,
                    0.1f, 0.5f, 0.8f, 0.5f,
                    0.2f, 2.0f, 100.0f, 1.0f,
                    0.2f, 0.7f, 0.1f, 0)
    });

    // 3. Iron Rain - Dense metallic impacts
    presets.push_back({
        "Iron Rain",
        createPreset("Iron Rain",
                    2, 8, 200.0f,             // StreamMode
                    1.5f, 0.7f, 1.0f,
                    0.0f, 0.2f,
                    0.5f, 0.5f, 0.5f, 0.6f,
                    0.4f, 1.5f, 80.0f, 2.0f,
                    0.5f, 0.6f, 0.3f, 0)
    });

    // 4. Gravity Well - Spiraling tension builder
    presets.push_back({
        "Gravity Well",
        createPreset("Gravity Well",
                    0, 4, 50.0f,
                    0.5f, 1.2f, 8.0f,
                    0.2f, 0.8f,               // Strong gravity
                    0.3f, 0.5f, 0.2f, 0.8f,
                    0.6f, 1.0f, 60.0f, 3.0f,
                    0.6f, 0.4f, 0.7f, 0)     // High entropy
    });

    // 5. Droid Malfunction - Glitchy, stuttering robot sounds
    presets.push_back({
        "Droid Malfunction",
        createPreset("Droid Malfunction",
                    1, 6, 100.0f,             // BurstMode
                    0.8f, 0.5f, 2.0f,
                    -0.1f, 0.5f,
                    0.8f, 0.3f, 0.7f, 0.9f,
                    0.9f, 3.0f, 30.0f, 5.0f,
                    0.8f, 0.3f, 1.0f, 1)     // Rössler chaos, max entropy
    });

    // 6. Crystal Cave - Shimmering ambient texture
    presets.push_back({
        "Crystal Cave",
        createPreset("Crystal Cave",
                    2, 4, 20.0f,              // Slow stream
                    0.3f, 0.9f, 10.0f,        // Long decay, light mass
                    0.0f, 0.0f,               // Zero gravity
                    0.1f, 0.5f, 0.0f, 0.9f,   // Low damping, bright
                    0.3f, 0.5f, 200.0f, 8.0f, // Slow zap
                    0.7f, 0.8f, 0.2f, 0)
    });

    // 7. Hull Breach - Dramatic impact → metallic decay
    presets.push_back({
        "Hull Breach",
        createPreset("Hull Breach",
                    0, 4, 50.0f,
                    3.0f, 1.1f, 6.0f,         // Heavy mass, super-elastic
                    0.0f, 0.5f,
                    0.2f, 0.2f, 0.4f, 0.95f,  // Bright, low damping
                    0.7f, 2.0f, 40.0f, 6.0f,
                    0.6f, 0.5f, 0.4f, 0)
    });

    // 8. Geiger Counter - Rapid stochastic clicks
    presets.push_back({
        "Geiger Counter",
        createPreset("Geiger Counter",
                    2, 4, 100.0f,             // Fast stream
                    0.2f, 0.3f, 0.5f,         // Very short lived
                    0.0f, 0.8f,
                    0.4f, 0.5f, 1.0f, 0.4f,   // High damping
                    0.2f, 0.5f, 20.0f, 1.0f,
                    0.3f, 0.9f, 0.5f, 2)      // DoublePendulum
    });

    // 9. Subspace Ping - Deep sonar pulse
    presets.push_back({
        "Subspace Ping",
        createPreset("Subspace Ping",
                    0, 4, 50.0f,
                    2.0f, 0.7f, 3.0f,
                    0.0f, -0.3f,              // Negative Y gravity
                    0.5f, 0.5f, 0.6f, 0.5f,
                    0.3f, 0.5f, 150.0f, 3.0f,// Deep pitch, slow sweep
                    0.4f, 0.6f, 0.0f, 0)
    });

    // 10. Quantum Flux - Evolving generative texture
    presets.push_back({
        "Quantum Flux",
        createPreset("Quantum Flux",
                    2, 6, 50.0f,
                    0.6f, 1.0f, 5.0f,
                    0.3f, 0.3f,
                    0.7f, 0.5f, 0.3f, 0.8f,
                    0.5f, 1.5f, 80.0f, 4.0f,
                    0.8f, 0.7f, 0.9f, 0)      // Max entropy Lorenz
    });

    return presets;
}
