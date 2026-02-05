#include "CollisionDetector.h"
#include <cmath>

CollisionDetector::CollisionDetector()
{
}

CollisionDetector::~CollisionDetector()
{
}

void CollisionDetector::prepare(double sr)
{
    sampleRate = sr;
}

void CollisionDetector::update(Particle& particle, float gravityX, float gravityY, float fieldMode, float fieldStrength, float dt)
{
    // Apply gravity
    particle.vx += gravityX * dt;
    particle.vy += gravityY * dt;

    // Apply field forces if enabled
    if (fieldMode != 0) {
        float dx = 0.0f - particle.x;
        float dy = 0.0f - particle.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist > 0.01f) {
            if (fieldMode == 1) {
                // Attract
                particle.vx += (dx / dist) * fieldStrength * dt;
                particle.vy += (dy / dist) * fieldStrength * dt;
            } else if (fieldMode == 2) {
                // Repel
                particle.vx -= (dx / dist) * fieldStrength * dt;
                particle.vy -= (dy / dist) * fieldStrength * dt;
            }
        }
    }

    // Update position
    particle.x += particle.vx * dt;
    particle.y += particle.vy * dt;

    // Age particle
    particle.age += dt;
    if (particle.age >= particle.lifetime)
        particle.alive = false;

    // Update time since last collision
    particle.timeSinceLastCollision += dt;
}

bool CollisionDetector::checkCollision(Particle& particle, int wallMaterial[4])
{
    bool collided = false;

    // Check boundaries
    if (particle.x > 1.0f) {
        particle.x = 1.0f;
        particle.vx *= -particle.elasticity;
        collided = true;
    } else if (particle.x < -1.0f) {
        particle.x = -1.0f;
        particle.vx *= -particle.elasticity;
        collided = true;
    }

    if (particle.y > 1.0f) {
        particle.y = 1.0f;
        particle.vy *= -particle.elasticity;
        collided = true;
    } else if (particle.y < -1.0f) {
        particle.y = -1.0f;
        particle.vy *= -particle.elasticity;
        collided = true;
    }

    if (collided) {
        particle.collisionCount++;
        particle.lastImpactForce = computeImpactForce(
            std::sqrt(particle.vx * particle.vx + particle.vy * particle.vy), particle.mass);
        particle.timeSinceLastCollision = 0.0f;
    }

    return collided;
}

float CollisionDetector::computeImpactForce(float velocity, float mass)
{
    return std::abs(velocity) * mass;
}
