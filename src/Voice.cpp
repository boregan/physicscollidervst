#include "Voice.h"
#include <cmath>

Voice::Voice()
    : resonatorBank(std::make_unique<ResonatorBank>()),
      fmGenerator(std::make_unique<FMImpulseGenerator>())
{
}

Voice::~Voice()
{
}

void Voice::prepare(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    resonatorBank->prepare(sr, samplesPerBlock);
    fmGenerator->prepare(sr, samplesPerBlock);
}

void Voice::noteOn(int note, float velocity, float mass, float elasticity, float lifetime, float charge, float launchAngle)
{
    midiNote = note;
    noteVelocity = velocity;

    particle.mass = mass;
    particle.elasticity = elasticity;
    particle.lifetime = lifetime;
    particle.charge = charge;
    particle.age = 0.0f;
    particle.collisionCount = 0;
    particle.timeSinceLastCollision = 0.0f;

    // Random launch position (center-ish area)
    particle.x = (std::sin((float)note * 0.1f) * 0.3f);
    particle.y = (std::cos((float)note * 0.07f) * 0.3f);

    // Launch velocity based on angle and velocity
    particle.launchForce = velocity;
    particle.launchAngle = launchAngle;
    particle.vx = std::cos(launchAngle) * velocity * 2.0f;
    particle.vy = std::sin(launchAngle) * velocity * 2.0f;

    particle.alive = true;

    // Launch resonator
    launchParticle(juce::MidiMessage::getMidiNoteInHertz(midiNote));

    accumulatedTime = 0.0f;
}

void Voice::noteOff()
{
    particle.lifetime = 0.1f;
}

float Voice::process(float gravityX, float gravityY, float fieldMode, float fieldStrength,
                     float material, float position, float damping, float brightness,
                     float fmDepth, float fmRatio, float zapSpeed, float zapDrop,
                     const int wallMaterials[4])
{
    if (!particle.alive)
        return 0.0f;

    float output = 0.0f;

    // Accumulate time for physics simulation
    accumulatedTime += 1.0f / (float)sampleRate;

    // Run physics tick (subdivided from audio rate)
    static constexpr float PHYSICS_TICK = 0.001f;
    while (accumulatedTime >= PHYSICS_TICK) {
        // Update particle physics
        particle.vx += gravityX * PHYSICS_TICK;
        particle.vy += gravityY * PHYSICS_TICK;

        // Apply field forces
        if (fieldMode > 0) {
            float dx = 0.0f - particle.x;
            float dy = 0.0f - particle.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 0.01f) {
                float fieldAccel = fieldStrength * 2.0f * PHYSICS_TICK;
                if (fieldMode == 1) {
                    // Attract
                    particle.vx += (dx / dist) * fieldAccel;
                    particle.vy += (dy / dist) * fieldAccel;
                } else if (fieldMode == 2) {
                    // Repel
                    particle.vx -= (dx / dist) * fieldAccel;
                    particle.vy -= (dy / dist) * fieldAccel;
                }
            }
        }

        // Update position
        particle.x += particle.vx * PHYSICS_TICK;
        particle.y += particle.vy * PHYSICS_TICK;

        // Check collisions
        bool collision = false;
        if (particle.x > 1.0f) {
            particle.x = 1.0f;
            particle.vx *= -particle.elasticity;
            collision = true;
        } else if (particle.x < -1.0f) {
            particle.x = -1.0f;
            particle.vx *= -particle.elasticity;
            collision = true;
        }

        if (particle.y > 1.0f) {
            particle.y = 1.0f;
            particle.vy *= -particle.elasticity;
            collision = true;
        } else if (particle.y < -1.0f) {
            particle.y = -1.0f;
            particle.vy *= -particle.elasticity;
            collision = true;
        }

        if (collision) {
            particle.collisionCount++;
            particle.lastImpactForce = std::sqrt(particle.vx * particle.vx + particle.vy * particle.vy) * particle.mass;
            particle.timeSinceLastCollision = 0.0f;

            // Trigger resonator excitation
            int modeCount = juce::jlimit(4, 32, (int)(4 + particle.lastImpactForce * 20.0f));
            float fundamentalFreq = juce::MidiMessage::getMidiNoteInHertz(midiNote);
            resonatorBank->excite(particle.lastImpactForce, material, position, damping, modeCount, fundamentalFreq);

            // Trigger FM impulse
            fmGenerator->trigger(noteVelocity, zapDrop, zapSpeed, fmDepth, fmRatio);
        }

        particle.age += PHYSICS_TICK;
        if (particle.age >= particle.lifetime)
            particle.alive = false;

        particle.timeSinceLastCollision += PHYSICS_TICK;
        accumulatedTime -= PHYSICS_TICK;
    }

    // Generate resonator output
    float noiseExciter = 0.0f;
    if (particle.timeSinceLastCollision < 0.02f) {
        // Generate noise burst for 20ms after collision
        static float noisePhase = 0.0f;
        noisePhase += 123.456f / (float)sampleRate;
        noiseExciter = std::sin(noisePhase * 2.0f * 3.141592653589793f) * particle.lastImpactForce * 0.5f;
        noiseExciter += std::sin(noisePhase * 312.456f / (float)sampleRate * 2.0f * 3.141592653589793f) * particle.lastImpactForce * 0.3f;
    }

    float resonatorOut = resonatorBank->process(noiseExciter) * brightness;
    float fmOut = fmGenerator->process();

    output = resonatorOut + fmOut * 0.5f;

    return output;
}

void Voice::launchParticle(float baseFrequency)
{
    // The resonator fundamental is set from MIDI note
    // This will be used in the resonator's mode frequency calculation
}
