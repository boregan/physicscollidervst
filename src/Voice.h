#pragma once

#include "dsp/CollisionDetector.h"
#include "dsp/ResonatorBank.h"
#include "dsp/FMImpulse.h"
#include <juce_core/juce_core.h>

class Voice
{
public:
    enum class LaunchMode {
        SingleShot = 0,
        Burst = 1,
        Stream = 2,
        Scatter = 3
    };

    Voice();
    ~Voice();

    void prepare(double sampleRate, int samplesPerBlock);

    void noteOn(int midiNote, float velocity, int launchMode, int burstCount, float streamRate,
                float mass, float elasticity, float lifetime, float charge);
    void noteOff();

    float process(float gravityX, float gravityY, float fieldMode, float fieldStrength,
                  float material, float position, float damping, float brightness,
                  float fmDepth, float fmRatio, float zapSpeed, float zapDrop,
                  const int wallMaterials[4]);

    bool isActive() const { return particle.alive || (int)burstParticles.size() > 0; }

    const Particle& getParticle() const { return particle; }

private:
    Particle particle;
    std::vector<Particle> burstParticles;
    std::unique_ptr<ResonatorBank> resonatorBank;
    std::unique_ptr<FMImpulseGenerator> fmGenerator;

    double sampleRate = 44100.0;
    int midiNote = 60;
    float noteVelocity = 0.0f;
    LaunchMode currentLaunchMode = LaunchMode::SingleShot;
    int burstCount = 4;
    float streamRate = 50.0f;
    float streamPhase = 0.0f;

    // Physics simulation
    static constexpr float PHYSICS_DT = 0.001f;
    float accumulatedTime = 0.0f;

    void launchParticle(float baseFrequency);
    float processSingleParticle(Particle& p, float gravityX, float gravityY, float fieldMode, float fieldStrength,
                                float material, float position, float damping, float brightness,
                                float fmDepth, float fmRatio, float zapSpeed, float zapDrop);
};
