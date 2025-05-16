/*
  ==============================================================================

    FormatSection.cpp
    Created: 15 May 2025 12:20:30pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "FormatSection.h"

//==============================================================================
FormatSection::FormatSection(StateManager& s)
: inputSelector(s, UI::FormatType::input),
outputSelector(s, UI::FormatType::output),
sectionBackground(juce::ImageCache::getFromMemory(BinaryData::format_section3x_png, BinaryData::format_section3x_pngSize))
{
    addAndMakeVisible(inputSelector);
    addAndMakeVisible(outputSelector);
}

FormatSection::~FormatSection()
{
}

void FormatSection::paint (juce::Graphics& g)
{
    g.drawImage(sectionBackground, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);

    // Define arrow size
    
    const auto& refStart    = inputSelector;
    const auto& refEnd      = outputSelector;
    const float tipWidth    = (outputSelector.getX() - inputSelector.getRight()) * 0.4f;
    const float arrowHeight = 5.f;
    const float offset      = 2.f;
    const float yOffset     = -20.f;
    
    // Line
    juce::Point<float> p1(refStart.getBounds().getRight() + offset,
                          refStart.getBounds().getCentreY() - arrowHeight * 0.5f - yOffset);
    
    juce::Point<float> p2(refStart.getBounds().getRight() + offset,
                          refStart.getBounds().getCentreY() + arrowHeight * 0.5f - yOffset);
    
    juce::Point<float> p3(refEnd.getBounds().getX() - offset - tipWidth,
                          refEnd.getBounds().getCentreY() + arrowHeight * 0.5f - yOffset);
    
    juce::Point<float> p4(refEnd.getBounds().getX() - offset - tipWidth,
                          refEnd.getBounds().getCentreY() - arrowHeight * 0.5f - yOffset);
    
    // Tip
    juce::Point<float> p5(refEnd.getBounds().getX() - offset - tipWidth,
                          refEnd.getBounds().getCentreY() + arrowHeight * 1.5f - yOffset);
    
    juce::Point<float> p6(refEnd.getBounds().getX() - offset - tipWidth,
                          refEnd.getBounds().getCentreY() - arrowHeight * 1.5f - yOffset);
    
    juce::Point<float> p7(refEnd.getBounds().getX() - offset,
                          refEnd.getBounds().getCentreY() - yOffset);

    // Create arrow path
    juce::Path line, tip;
    line.startNewSubPath(p1);
    line.lineTo(p2);
    line.lineTo(p3);
    line.lineTo(p4);
    line.closeSubPath();

    tip.startNewSubPath(p5);
    tip.lineTo(p6);
    tip.lineTo(p7);
    tip.closeSubPath();
    
    g.setColour(UI::ColourDefinitions::outlineColour);
    g.fillPath(line);
    g.fillPath(tip);
}

void FormatSection::resized()
{
    const auto bounds = getLocalBounds();
    
    const float
    panelWidth  = bounds.getWidth(),
    panelHeight = bounds.getHeight(),
    padding     = 15,
    windowX     = bounds.getX(),
    windowY     = bounds.getY();
    
    inputSelector.setBounds(windowX,
                            windowY,
                            panelWidth * UI::FormatSectionFactors::selectorWidthFactor - padding,
                            panelHeight
                            );
    
    const float
    outputX = inputSelector.getRight() + 2 * padding,
    outputY = windowY;
    
    outputSelector.setBounds(outputX,
                             outputY,
                             panelWidth * UI::FormatSectionFactors::selectorWidthFactor - padding,
                             panelHeight
                             );
}
