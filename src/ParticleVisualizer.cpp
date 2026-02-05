#include "ParticleVisualizer.h"

ParticleVisualizer::ParticleVisualizer()
{
    startTimer(33);  // ~30fps updates
}

ParticleVisualizer::~ParticleVisualizer()
{
    stopTimer();
}

void ParticleVisualizer::paint(juce::Graphics& g)
{
    // Dark background with subtle gradient
    g.fillAll(juce::Colour::fromRGB(15, 15, 20));

    auto bounds = getLocalBounds().toFloat();

    // Draw grid for visualization reference
    g.setColour(juce::Colour::fromRGB(40, 40, 50));
    for (int i = 0; i <= 4; ++i) {
        float y = bounds.getHeight() * (i / 4.0f);
        float x = bounds.getWidth() * (i / 4.0f);
        g.drawLine(bounds.getX(), y, bounds.getRight(), y, 0.5f);
        g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 0.5f);
    }

    // Draw boundaries
    drawBoundaries(g);

    // Draw particle trails
    drawParticleTrail(g);

    // Draw info
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText(juce::String(activeVoices) + " voice(s)",
               bounds.withHeight(20).reduced(5), juce::Justification::topLeft);
}

void ParticleVisualizer::drawBoundaries(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Draw boundary box
    g.setColour(juce::Colour::fromRGB(100, 150, 200));
    g.drawRect(bounds.reduced(20), 2.0f);

    // Draw boundary labels
    g.setFont(10.0f);
    g.drawText("L", bounds.withWidth(20), juce::Justification::centred);
    g.drawText("R", bounds.withX(bounds.getRight() - 20).withWidth(20), juce::Justification::centred);
    g.drawText("T", bounds.withHeight(20), juce::Justification::centred);
    g.drawText("B", bounds.withY(bounds.getBottom() - 20).withHeight(20), juce::Justification::centred);
}

void ParticleVisualizer::drawParticleTrail(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float innerX = bounds.getX() + 20;
    float innerY = bounds.getY() + 20;
    float innerWidth = bounds.getWidth() - 40;
    float innerHeight = bounds.getHeight() - 40;

    for (size_t i = 0; i < trails.size(); ++i) {
        const auto& trail = trails[i];

        // Map particle position (-1..1) to screen coordinates
        float screenX = innerX + (trail.x + 1.0f) * 0.5f * innerWidth;
        float screenY = innerY + (1.0f - trail.y) * 0.5f * innerHeight;

        // Fade older trails
        float alpha = trail.age;
        int brightness = (int)(50 + trail.age * 150);

        g.setColour(juce::Colour::fromRGB(brightness, brightness + 50, brightness + 100).withAlpha(alpha));

        // Draw trail points
        float size = 2.0f + trail.age * 3.0f;
        g.fillEllipse(screenX - size / 2, screenY - size / 2, size, size);

        // Connect trail points with lines
        if (i > 0) {
            const auto& prevTrail = trails[i - 1];
            float prevScreenX = innerX + (prevTrail.x + 1.0f) * 0.5f * innerWidth;
            float prevScreenY = innerY + (1.0f - prevTrail.y) * 0.5f * innerHeight;

            g.setColour(juce::Colour::fromRGB(100, 150, 200).withAlpha(alpha * 0.5f));
            g.drawLine(prevScreenX, prevScreenY, screenX, screenY, 1.0f);
        }
    }

    // Draw latest particle position as bright glow
    if (!trails.empty()) {
        const auto& latest = trails.back();
        float screenX = innerX + (latest.x + 1.0f) * 0.5f * innerWidth;
        float screenY = innerY + (1.0f - latest.y) * 0.5f * innerHeight;

        g.setColour(juce::Colours::yellow.withAlpha(0.8f));
        g.fillEllipse(screenX - 4, screenY - 4, 8, 8);

        g.setColour(juce::Colours::yellow.withAlpha(0.3f));
        g.drawEllipse(screenX - 8, screenY - 8, 16, 16, 1.0f);
    }
}

void ParticleVisualizer::addParticlePosition(float x, float y)
{
    ParticleTrail trail{x, y, 1.0f};
    trails.push_back(trail);

    if (trails.size() > MAX_TRAIL_LENGTH)
        trails.pop_front();

    // Age existing trails
    for (auto& t : trails) {
        t.age *= 0.95f;  // Exponential fade
    }

    repaint();
}

void ParticleVisualizer::timerCallback()
{
    repaint();
}
