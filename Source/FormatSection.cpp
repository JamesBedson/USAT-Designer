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
