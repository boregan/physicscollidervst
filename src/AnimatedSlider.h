#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <cmath>

class AnimatedSlider : public juce::Slider
{
public:
    AnimatedSlider() : juce::Slider()
    {
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(100, 200, 255));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(50, 100, 150));
        setColour(juce::Slider::thumbColourId, juce::Colour(150, 220, 255));
        setColour(juce::TextEditor::backgroundColourId, juce::Colour(20, 25, 35));
        setColour(juce::TextEditor::textColourId, juce::Colour(150, 220, 255));
        setColour(juce::TextEditor::outlineColourId, juce::Colour(80, 150, 200));
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        float centerX = bounds.getCentreX();
        float centerY = bounds.getCentreY();
        float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.35f;

        // Animated background glow
        float glowAlpha = 0.15f + 0.1f * std::sin(juce::Time::getMillisecondCounterHiRes() * 0.003f);
        juce::ColourGradient glowGrad(juce::Colour(100, 200, 255).withAlpha(glowAlpha),
                                      centerX, centerY,
                                      juce::Colour(100, 200, 255).withAlpha(0.0f),
                                      centerX + radius * 1.2f, centerY, true);
        g.setGradientFill(glowGrad);
        g.fillEllipse(centerX - radius * 1.3f, centerY - radius * 1.3f, radius * 2.6f, radius * 2.6f);

        // Dark background circle
        g.setColour(juce::Colour(15, 20, 30).withAlpha(0.8f));
        g.fillEllipse(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f);

        // Outer ring
        g.setColour(juce::Colour(80, 150, 200).withAlpha(0.4f));
        g.drawEllipse(centerX - radius, centerY - radius, radius * 2.0f, radius * 2.0f, 1.5f);

        // Inner ring
        g.setColour(juce::Colour(120, 200, 255).withAlpha(0.3f));
        g.drawEllipse(centerX - radius * 0.7f, centerY - radius * 0.7f, radius * 1.4f, radius * 1.4f, 1.0f);

        // Slider fill
        float normalizedValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
        float rotationAngle = -juce::MathConstants<float>::pi * 0.75f + normalizedValue * juce::MathConstants<float>::pi * 1.5f;

        // Animated color based on value
        float hue = normalizedValue * 0.4f;  // HSV hue variation
        juce::Colour fillCol = juce::Colour::fromHSV(hue, 0.8f, 0.9f, 0.9f);

        // Draw filled arc
        juce::Path arcPath;
        arcPath.addCentredArc(centerX, centerY, radius * 0.85f, radius * 0.85f,
                              0.0f, -juce::MathConstants<float>::pi * 0.75f, rotationAngle, true);
        g.setColour(fillCol);
        g.strokePath(arcPath, juce::PathStrokeType(3.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Animated knob thumb
        float thumbX = centerX + std::cos(rotationAngle) * radius * 0.85f;
        float thumbY = centerY + std::sin(rotationAngle) * radius * 0.85f;

        // Thumb glow
        juce::ColourGradient thumbGlow(fillCol.withAlpha(0.4f), thumbX, thumbY,
                                       fillCol.withAlpha(0.0f), thumbX + 8.0f, thumbY, true);
        g.setGradientFill(thumbGlow);
        g.fillEllipse(thumbX - 8.0f, thumbY - 8.0f, 16.0f, 16.0f);

        // Thumb itself
        g.setColour(fillCol);
        g.fillEllipse(thumbX - 5.0f, thumbY - 5.0f, 10.0f, 10.0f);

        // Bright center highlight
        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillEllipse(thumbX - 2.0f, thumbY - 2.0f, 4.0f, 4.0f);
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AnimatedSlider)
};
