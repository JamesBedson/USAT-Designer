/*
  ==============================================================================

    CustomLNF.h
    Created: 15 May 2025 5:49:15pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include "UIConstants.h"
class CustomLNF : public juce::LookAndFeel_V4 {

public:
    
    void drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                       int buttonX, int buttonY, int buttonW, int buttonH,
                       juce::ComboBox& box) override
    {
        
        auto cornerSize = height * UI::Geometry::cornerRoundingFactor;
        juce::Rectangle<int> boxBounds (0, 0, width, height);

        g.setColour (UI::ColourDefinitions::outlineColour.darker());
        g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);
        
        g.setColour(UI::ColourDefinitions::backgroundColour);
        g.fillRoundedRectangle(boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize);

        juce::Rectangle<int> arrowZone (width - 30, 0, 15, height);
        juce::Path path;
        
        path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 3.0f);
        path.lineTo           ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 3.0f);
        path.lineTo           ((float) arrowZone.getCentreX(),     (float) arrowZone.getCentreY() + 3.0f);
        path.closeSubPath();

        g.setColour (box.findColour (juce::ComboBox::arrowColourId)
                         .withAlpha (box.isEnabled() ? 0.9f : 0.2f));

        g.fillPath (path);
    }
    
    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds (8, 1,
                         box.getWidth() - 30,
                         box.getHeight() - 2);

        label.setFont (getComboBoxFont (box));
    }
    
    juce::Font getComboBoxFont (juce::ComboBox& box) override
    {
        return juce::Font(juce::FontOptions {"Futura", 15.f, juce::Font::plain});
    }
    
    void drawPopupMenuBackground(juce::Graphics& g, int width, int height) override
    {
        g.fillAll(UI::ColourDefinitions::backgroundColour);
    }

    void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted,
                           bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override
    {
        if (isHighlighted)
            g.fillAll(UI::ColourDefinitions::darkhighlightColour.withLightness(0.15f));

        juce::Font font("Futura", 15.0f, juce::Font::plain);
        g.setFont(font);

        g.setColour(UI::ColourDefinitions::outlineColour);
        g.drawText(text, area.reduced(4), juce::Justification::centredLeft, true);
    }

    // Optional: change menu font globally
    juce::Font getPopupMenuFont() override
    {
        return juce::Font("Futura", 15.0f, juce::Font::plain);
    }
    
    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool shouldDrawButtonAsHighlighted,
                               bool shouldDrawButtonAsDown) override
    {
        auto cornerSize = button.getHeight() * UI::Geometry::cornerRoundingFactor;
        auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

        auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                          .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

        if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
            baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

        g.setColour (baseColour);

        auto flatOnLeft   = button.isConnectedOnLeft();
        auto flatOnRight  = button.isConnectedOnRight();
        auto flatOnTop    = button.isConnectedOnTop();
        auto flatOnBottom = button.isConnectedOnBottom();

        if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
        {
            juce::Path path;
            path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                      bounds.getWidth(), bounds.getHeight(),
                                      cornerSize, cornerSize,
                                      ! (flatOnLeft  || flatOnTop),
                                      ! (flatOnRight || flatOnTop),
                                      ! (flatOnLeft  || flatOnBottom),
                                      ! (flatOnRight || flatOnBottom));

            g.fillPath (path);

            g.setColour (button.findColour (juce::ComboBox::outlineColourId));
            g.strokePath (path, juce::PathStrokeType (1.0f));
        }
        else
        {

            g.setColour (UI::ColourDefinitions::outlineColour);
            g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
        }
    }
    
    void drawRotarySlider (juce::Graphics& g,
                           int x,
                           int y,
                           int width,
                           int height,
                           float sliderPos,
                           const float rotaryStartAngle,
                           const float rotaryEndAngle,
                           juce::Slider& slider) override
    {
        const float
        radius  = juce::jmin(width, height) / 2.f,
        centerX = (float) x + (float) width * 0.5f,
        centerY = (float) y + (float) height * 0.5f,
        rx      = centerX - radius,
        ry      = centerY - radius,
        rw      = radius * 2.0f;
        
        const float
        sliderRange = rotaryEndAngle - rotaryStartAngle,
        angle       = rotaryStartAngle + (sliderPos * sliderRange),
        outerRadius = radius * 0.83f;

        auto bounds     = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);
        auto toAngle    = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW      = juce::jmin (8.0f, radius * 0.5f);
        auto arcRadius  = radius - lineW * 0.5f;
        
        const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

        if (radius > 12.0f)
        {
            if (slider.isEnabled())
                g.setColour(juce::Colours::white.darker().darker().darker());
            
            else
                g.setColour (juce::Colours::red);

            const float thickness = 0.9f;

            juce::Path outlineArc;
            outlineArc.addPieSegment(rx + (radius - outerRadius),
                                     ry + (radius - outerRadius),
                                     rw - (2.0f * (radius - outerRadius)),
                                     rw - (2.0f * (radius - outerRadius)),
                                     rotaryStartAngle,
                                     rotaryEndAngle,
                                     thickness + 0.07f);
            
            outlineArc.closeSubPath();
            //g.strokePath(outlineArc, juce::PathStrokeType(slider.isEnabled() ? (isMouseOver ? 2.0f : 1.2f) : 0.3f));

            //Outer part
            if (slider.isEnabled())
                g.setColour(UI::ColourDefinitions::accentColour);
            else
                g.setColour(juce::Colours::black);

            //Inner part
            {
                juce::Path filledArc;
                filledArc.addPieSegment(rx + (radius - outerRadius),
                                        ry + (radius - outerRadius),
                                        rw - (2.0f * (radius - outerRadius)),
                                        rw - (2.0f * (radius - outerRadius)),
                                        rotaryStartAngle,
                                        angle,
                                        thickness);
                
                g.fillPath (filledArc);
            }

            {
                const float innerRadius = radius * 0.2f;

                juce::Path ellipsePath;
                auto ellipseHeight  = innerRadius * 7.0f;
                auto ellipseWidth   = innerRadius * 7.0f;
                
                ellipsePath.addEllipse(centerX - (ellipseHeight * 0.5f),
                                       centerY - (ellipseWidth * 0.5f),
                                       ellipseHeight,
                                       ellipseWidth);
                
                g.setColour(UI::ColourDefinitions::outlineColour.withLightness(0.95f));
                g.fillPath(ellipsePath);

                auto thumbWidth = 3.5f;
                juce::Point<float> thumbPoint (
                                               bounds.getCentreX() + arcRadius * 0.65f * std::cos (toAngle - juce::MathConstants<float>::halfPi),
                                               bounds.getCentreY() + arcRadius * 0.65f * std::sin (toAngle - juce::MathConstants<float>::halfPi));

                g.setColour(juce::Colours::black);
                g.fillEllipse (juce::Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
            }
        }
    }
    
private:
    
};
