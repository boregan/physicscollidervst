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
    auto bounds = getLocalBounds().toFloat();

    // Animated gradient background
    float timeModulation = std::sin(juce::Time::getMillisecondCounterHiRes() * 0.0005f) * 0.5f + 0.5f;

    juce::ColourGradient bgGrad1(juce::Colour(10, 15, 25), 0, 0,
                                  juce::Colour(20, 30, 45), bounds.getWidth(), bounds.getHeight(), false);
    g.setGradientFill(bgGrad1);
    g.fillAll();

    // Animated overlay glow
    float glowIntensity = 0.05f + 0.03f * timeModulation;
    juce::ColourGradient glowGrad(juce::Colour(60, 120, 200).withAlpha(glowIntensity),
                                   bounds.getCentreX(), bounds.getCentreY(),
                                   juce::Colour(60, 120, 200).withAlpha(0.0f),
                                   bounds.getWidth() * 0.7f, bounds.getHeight() * 0.7f, true);
    g.setGradientFill(glowGrad);
    g.fillAll();

    // Animated grid with pulsing opacity
    float gridAlpha = 0.06f + 0.04f * timeModulation;
    g.setColour(juce::Colour::fromRGB(60, 80, 120).withAlpha(gridAlpha));
    for (int i = 0; i <= 8; ++i) {
        float y = bounds.getHeight() * (i / 8.0f);
        float x = bounds.getWidth() * (i / 8.0f);
        g.drawLine(bounds.getX(), y, bounds.getRight(), y, 0.5f);
        g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 0.5f);
    }

    // Draw boundaries
    drawBoundaries(g);

    // Draw particle trails
    drawParticleTrail(g);

    // Draw info with styling
    g.setColour(juce::Colour(100, 200, 255).withAlpha(0.9f));
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText(juce::String(activeVoices) + " VOICES",
               bounds.withHeight(25).reduced(5), juce::Justification::topLeft);
}

void ParticleVisualizer::drawBoundaries(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto innerBounds = bounds.reduced(20);

    float timeModulation = std::sin(juce::Time::getMillisecondCounterHiRes() * 0.002f) * 0.5f + 0.5f;

    // Animated outer glow
    g.setColour(juce::Colour(80, 150, 220).withAlpha(0.15f + 0.1f * timeModulation));
    g.drawRect(innerBounds.expanded(4), 4.0f);

    // Main boundary box with gradient effect
    juce::Path boundaryPath;
    boundaryPath.addRectangle(innerBounds);
    g.setColour(juce::Colour(100, 180, 255).withAlpha(0.7f));
    g.strokePath(boundaryPath, juce::PathStrokeType(2.0f));

    // Corner accents
    float cornerSize = 12.0f;
    g.setColour(juce::Colour(150, 200, 255).withAlpha(0.8f));

    // Top-left
    g.drawLine(innerBounds.getX(), innerBounds.getY(), innerBounds.getX() + cornerSize, innerBounds.getY(), 2.0f);
    g.drawLine(innerBounds.getX(), innerBounds.getY(), innerBounds.getX(), innerBounds.getY() + cornerSize, 2.0f);

    // Top-right
    g.drawLine(innerBounds.getRight(), innerBounds.getY(), innerBounds.getRight() - cornerSize, innerBounds.getY(), 2.0f);
    g.drawLine(innerBounds.getRight(), innerBounds.getY(), innerBounds.getRight(), innerBounds.getY() + cornerSize, 2.0f);

    // Bottom-left
    g.drawLine(innerBounds.getX(), innerBounds.getBottom(), innerBounds.getX() + cornerSize, innerBounds.getBottom(), 2.0f);
    g.drawLine(innerBounds.getX(), innerBounds.getBottom(), innerBounds.getX(), innerBounds.getBottom() - cornerSize, 2.0f);

    // Bottom-right
    g.drawLine(innerBounds.getRight(), innerBounds.getBottom(), innerBounds.getRight() - cornerSize, innerBounds.getBottom(), 2.0f);
    g.drawLine(innerBounds.getRight(), innerBounds.getBottom(), innerBounds.getRight(), innerBounds.getBottom() - cornerSize, 2.0f);

    // Draw boundary labels with glow
    g.setFont(juce::Font(10.0f, juce::Font::bold));
    g.setColour(juce::Colour(150, 200, 255).withAlpha(0.6f));
    g.drawText("◀", juce::Rectangle<float>(innerBounds.getX() - 15, innerBounds.getCentreY() - 8, 15, 16), juce::Justification::centred);
    g.drawText("▶", juce::Rectangle<float>(innerBounds.getRight(), innerBounds.getCentreY() - 8, 15, 16), juce::Justification::centred);
    g.drawText("▲", juce::Rectangle<float>(innerBounds.getCentreX() - 7, innerBounds.getY() - 15, 14, 15), juce::Justification::centred);
    g.drawText("▼", juce::Rectangle<float>(innerBounds.getCentreX() - 7, innerBounds.getBottom(), 14, 15), juce::Justification::centred);
}

void ParticleVisualizer::drawParticleTrail(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    float innerX = bounds.getX() + 20;
    float innerY = bounds.getY() + 20;
    float innerWidth = bounds.getWidth() - 40;
    float innerHeight = bounds.getHeight() - 40;

    // Draw thick glowing trails first (background layer)
    for (size_t i = 0; i < trails.size(); ++i) {
        const auto& trail = trails[i];

        float screenX = innerX + (trail.x + 1.0f) * 0.5f * innerWidth;
        float screenY = innerY + (1.0f - trail.y) * 0.5f * innerHeight;

        float alpha = trail.age;

        // Connect trail points with thick glowing lines
        if (i > 0) {
            const auto& prevTrail = trails[i - 1];
            float prevScreenX = innerX + (prevTrail.x + 1.0f) * 0.5f * innerWidth;
            float prevScreenY = innerY + (1.0f - prevTrail.y) * 0.5f * innerHeight;

            // Color interpolation based on age
            float hue = 0.6f + trail.age * 0.15f;  // Cyan to magenta
            juce::Colour trailCol = juce::Colour::fromHSV(hue, 0.7f, 0.8f, alpha * 0.6f);

            // Glow layer (thick transparent line)
            juce::ColourGradient glowGrad(trailCol.withAlpha(alpha * 0.3f), prevScreenX, prevScreenY,
                                          trailCol.withAlpha(0.0f), prevScreenX + 6, prevScreenY, true);
            g.setGradientFill(glowGrad);
            g.drawLine(prevScreenX, prevScreenY, screenX, screenY, 4.0f);

            // Main line
            g.setColour(trailCol);
            g.drawLine(prevScreenX, prevScreenY, screenX, screenY, 1.5f);
        }
    }

    // Draw trail points with glow effect
    for (size_t i = 0; i < trails.size(); ++i) {
        const auto& trail = trails[i];

        float screenX = innerX + (trail.x + 1.0f) * 0.5f * innerWidth;
        float screenY = innerY + (1.0f - trail.y) * 0.5f * innerHeight;

        float alpha = trail.age;
        float hue = 0.6f + trail.age * 0.15f;
        juce::Colour pointCol = juce::Colour::fromHSV(hue, 0.8f, 0.9f, alpha);

        // Glow halo
        juce::ColourGradient glowHalo(pointCol.withAlpha(alpha * 0.2f), screenX, screenY,
                                      pointCol.withAlpha(0.0f), screenX + 8, screenY, true);
        g.setGradientFill(glowHalo);
        g.fillEllipse(screenX - 8, screenY - 8, 16, 16);

        // Point itself
        float size = 1.5f + trail.age * 2.0f;
        g.setColour(pointCol);
        g.fillEllipse(screenX - size / 2, screenY - size / 2, size, size);

        // Bright center
        g.setColour(juce::Colours::white.withAlpha(alpha * 0.4f));
        g.fillEllipse(screenX - size / 4, screenY - size / 4, size / 2, size / 2);
    }

    // Draw latest particle position as mega bright focus point
    if (!trails.empty()) {
        const auto& latest = trails.back();
        float screenX = innerX + (latest.x + 1.0f) * 0.5f * innerWidth;
        float screenY = innerY + (1.0f - latest.y) * 0.5f * innerHeight;

        float pulseAmount = std::sin(juce::Time::getMillisecondCounterHiRes() * 0.01f) * 0.3f + 0.7f;

        // Large animated glow corona
        juce::ColourGradient coronaGlow(juce::Colour(255, 150, 100).withAlpha(0.15f * pulseAmount),
                                        screenX, screenY,
                                        juce::Colour(255, 150, 100).withAlpha(0.0f),
                                        screenX + 30 * pulseAmount, screenY, true);
        g.setGradientFill(coronaGlow);
        g.fillEllipse(screenX - 30 * pulseAmount, screenY - 30 * pulseAmount,
                      60 * pulseAmount, 60 * pulseAmount);

        // Main bright point
        g.setColour(juce::Colour(255, 200, 100).withAlpha(0.9f));
        g.fillEllipse(screenX - 5, screenY - 5, 10, 10);

        // Center highlight
        g.setColour(juce::Colours::white);
        g.fillEllipse(screenX - 2, screenY - 2, 4, 4);
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
