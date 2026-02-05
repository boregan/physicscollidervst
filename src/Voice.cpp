#include "Voice.h"
#include <cmath>
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());

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

void Voice::noteOn(int note, float velocity, int launchMode, int burst, float rate,
                   float mass, float elasticity, float lifetime, float charge)
{
    midiNote = note;
    noteVelocity = velocity;
    currentLaunchMode = (LaunchMode)launchMode;
    burstCount = burst;
    streamRate = rate;
    streamPhase = 0.0f;
    accumulatedTime = 0.0f;
    burstParticles.clear();

    // Common particle parameters
    particle.mass = mass;
    particle.elasticity = elasticity;
    particle.lifetime = lifetime;
    particle.charge = charge;
    particle.age = 0.0f;
    particle.collisionCount = 0;
    particle.timeSinceLastCollision = 0.0f;

    if (currentLaunchMode == LaunchMode::SingleShot) {
        particle.x = std::sin((float)note * 0.1f) * 0.3f;
        particle.y = std::cos((float)note * 0.07f) * 0.3f;
        particle.launchForce = velocity;
        particle.launchAngle = 0.0f;
        particle.vx = velocity * 2.0f;
        particle.vy = 0.1f;
        particle.alive = true;
    } else if (currentLaunchMode == LaunchMode::Burst) {
        // Launch N particles in a spread pattern
        for (int i = 0; i < burstCount; ++i) {
            Particle p;
            p.mass = mass;
            p.elasticity = elasticity;
            p.lifetime = lifetime;
            p.charge = charge;
            p.age = 0.0f;
            p.collisionCount = 0;
            p.timeSinceLastCollision = 0.0f;
            p.alive = true;

            float angle = (2.0f * 3.141592653589793f * i) / (float)burstCount;
            p.x = std::sin(angle) * 0.2f;
            p.y = std::cos(angle) * 0.2f;
            p.launchForce = velocity;
            p.launchAngle = angle;
            p.vx = std::cos(angle) * velocity * 2.0f;
            p.vy = std::sin(angle) * velocity * 2.0f;

            burstParticles.push_back(p);
        }
        particle.alive = false;
    } else if (currentLaunchMode == LaunchMode::Stream) {
        particle.alive = false;
    } else if (currentLaunchMode == LaunchMode::Scatter) {
        particle.x = (std::sin((float)note * 0.1f + streamPhase) * 0.3f);
        particle.y = (std::cos((float)note * 0.07f + streamPhase) * 0.3f);
        particle.launchForce = velocity;
        particle.launchAngle = streamPhase;
        particle.vx = std::cos(streamPhase) * velocity * 2.0f;
        particle.vy = std::sin(streamPhase) * velocity * 2.0f;
        particle.alive = true;
    }
}

void Voice::noteOff()
{
    particle.lifetime = 0.1f;
    for (auto& p : burstParticles)
        p.lifetime = 0.1f;
}

float Voice::processSingleParticle(Particle& p, float gravityX, float gravityY, float fieldMode, float fieldStrength,
                                   float material, float position, float damping, float brightness,
                                   float fmDepth, float fmRatio, float zapSpeed, float zapDrop)
{
    if (!p.alive)
        return 0.0f;

    float accTime = 0.0f;
    float output = 0.0f;

    accTime += 1.0f / (float)sampleRate;
    static constexpr float PHYSICS_TICK = 0.001f;

    while (accTime >= PHYSICS_TICK) {
        p.vx += gravityX * PHYSICS_TICK;
        p.vy += gravityY * PHYSICS_TICK;

        if (fieldMode > 0) {
            float dx = 0.0f - p.x;
            float dy = 0.0f - p.y;
            float dist = std::sqrt(dx * dx + dy * dy);
            if (dist > 0.01f) {
                float fieldAccel = fieldStrength * 2.0f * PHYSICS_TICK;
                if (fieldMode == 1) {
                    p.vx += (dx / dist) * fieldAccel;
                    p.vy += (dy / dist) * fieldAccel;
                } else if (fieldMode == 2) {
                    p.vx -= (dx / dist) * fieldAccel;
                    p.vy -= (dy / dist) * fieldAccel;
                }
            }
        }

        p.x += p.vx * PHYSICS_TICK;
        p.y += p.vy * PHYSICS_TICK;

        bool collision = false;
        if (p.x > 1.0f) {
            p.x = 1.0f;
            p.vx *= -p.elasticity;
            collision = true;
        } else if (p.x < -1.0f) {
            p.x = -1.0f;
            p.vx *= -p.elasticity;
            collision = true;
        }

        if (p.y > 1.0f) {
            p.y = 1.0f;
            p.vy *= -p.elasticity;
            collision = true;
        } else if (p.y < -1.0f) {
            p.y = -1.0f;
            p.vy *= -p.elasticity;
            collision = true;
        }

        if (collision) {
            p.collisionCount++;
            p.lastImpactForce = std::sqrt(p.vx * p.vx + p.vy * p.vy) * p.mass;
            p.timeSinceLastCollision = 0.0f;

            int modeCount = juce::jlimit(4, 32, (int)(4 + p.lastImpactForce * 20.0f));
            float fundamentalFreq = juce::MidiMessage::getMidiNoteInHertz(midiNote);
            resonatorBank->excite(p.lastImpactForce, material, position, damping, modeCount, fundamentalFreq);
            fmGenerator->trigger(noteVelocity, zapDrop, zapSpeed, fmDepth, fmRatio);
        }

        p.age += PHYSICS_TICK;
        if (p.age >= p.lifetime)
            p.alive = false;

        p.timeSinceLastCollision += PHYSICS_TICK;
        accTime -= PHYSICS_TICK;
    }

    float noiseExciter = 0.0f;
    if (p.timeSinceLastCollision < 0.02f) {
        static float noisePhase = 0.0f;
        noisePhase += 123.456f / (float)sampleRate;
        noiseExciter = std::sin(noisePhase * 2.0f * 3.141592653589793f) * p.lastImpactForce * 0.5f;
        noiseExciter += std::sin(noisePhase * 312.456f / (float)sampleRate * 2.0f * 3.141592653589793f) * p.lastImpactForce * 0.3f;
    }

    float resonatorOut = resonatorBank->process(noiseExciter) * brightness;
    float fmOut = fmGenerator->process();

    return resonatorOut + fmOut * 0.5f;
}

float Voice::process(float gravityX, float gravityY, float fieldMode, float fieldStrength,
                     float material, float position, float damping, float brightness,
                     float fmDepth, float fmRatio, float zapSpeed, float zapDrop,
                     const int wallMaterials[4])
{
    float output = 0.0f;

    // Process primary particle
    output += processSingleParticle(particle, gravityX, gravityY, fieldMode, fieldStrength,
                                    material, position, damping, brightness,
                                    fmDepth, fmRatio, zapSpeed, zapDrop);

    // Process burst particles
    accumulatedTime += 1.0f / (float)sampleRate;
    if (currentLaunchMode == LaunchMode::Stream && accumulatedTime >= (1.0f / streamRate)) {
        Particle newParticle;
        newParticle.mass = particle.mass;
        newParticle.elasticity = particle.elasticity;
        newParticle.lifetime = 1.0f;
        newParticle.charge = particle.charge;
        newParticle.age = 0.0f;
        newParticle.collisionCount = 0;
        newParticle.timeSinceLastCollision = 0.0f;
        newParticle.alive = true;

        std::uniform_real_distribution<> dis(-0.3f, 0.3f);
        newParticle.x = dis(gen);
        newParticle.y = dis(gen);
        newParticle.launchForce = noteVelocity;
        newParticle.vx = (dis(gen) * 2.0f) * noteVelocity;
        newParticle.vy = (dis(gen) * 2.0f) * noteVelocity;

        burstParticles.push_back(newParticle);
        accumulatedTime -= (1.0f / streamRate);
    }

    // Process and remove dead burst particles
    for (int i = (int)burstParticles.size() - 1; i >= 0; --i) {
        output += processSingleParticle(burstParticles[i], gravityX, gravityY, fieldMode, fieldStrength,
                                        material, position, damping, brightness,
                                        fmDepth, fmRatio, zapSpeed, zapDrop);
        if (!burstParticles[i].alive)
            burstParticles.erase(burstParticles.begin() + i);
    }

    return output;
}

void Voice::launchParticle(float baseFrequency)
{
    // The resonator fundamental is set from MIDI note
}
