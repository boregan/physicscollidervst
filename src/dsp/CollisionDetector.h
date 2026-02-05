#pragma once

#include <juce_core/juce_core.h>

struct Particle
{
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
    float mass = 1.0f;
    float charge = 0.0f;
    float elasticity = 0.8f;
    float lifetime = 5.0f;
    float age = 0.0f;
    float launchAngle = 0.0f;
    float launchForce = 1.0f;
    int collisionCount = 0;
    float lastImpactForce = 0.0f;
    float timeSinceLastCollision = 0.0f;
    bool alive = false;
};

class CollisionDetector
{
public:
    CollisionDetector();
    ~CollisionDetector();

    void prepare(double sampleRate);
    void update(Particle& particle, float gravityX, float gravityY, float fieldMode, float fieldStrength, float dt);

    bool checkCollision(Particle& particle, int wallMaterial[4]);
    float computeImpactForce(float velocity, float mass);

private:
    double sampleRate = 44100.0;
};
