#pragma once

#include "dsp/CollisionDetector.h"
#include "dsp/ResonatorBank.h"
#include "dsp/FMImpulse.h"
#include <juce_core/juce_core.h>

class Voice
{
public:
    Voice();
    ~Voice();

    void prepare(double sampleRate, int samplesPerBlock);

    void noteOn(int midiNote, float velocity, float mass, float elasticity, float lifetime, float charge, float launchAngle);
    void noteOff();

    float process(float gravityX, float gravityY, float fieldMode, float fieldStrength,
                  float material, float position, float damping, float brightness,
                  float fmDepth, float fmRatio, float zapSpeed, float zapDrop,
                  const int wallMaterials[4]);

    bool isActive() const { return particle.alive; }

    const Particle& getParticle() const { return particle; }

private:
    Particle particle;
    std::unique_ptr<ResonatorBank> resonatorBank;
    std::unique_ptr<FMImpulseGenerator> fmGenerator;

    double sampleRate = 44100.0;
    int midiNote = 60;
    float noteVelocity = 0.0f;

    // Physics simulation
    static constexpr float PHYSICS_DT = 0.001f;
    float accumulatedTime = 0.0f;

    void launchParticle(float baseFrequency);
};
