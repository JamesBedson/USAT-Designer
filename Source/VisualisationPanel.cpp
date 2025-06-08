/*
  ==============================================================================

    VisualisationPanel.cpp
    Created: 12 Jan 2025 11:13:02am
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VisualisationPanel.h"

//==============================================================================
VisualisationPanel::VisualisationPanel(StateManager& s)
: stateManager(s),
panelBackground(juce::ImageCache::getFromMemory(BinaryData::placeholder3x_png, BinaryData::placeholder3x_pngSize))
{
}

VisualisationPanel::~VisualisationPanel()
{
}

void VisualisationPanel::displayImage(const juce::Image &img)
{
    panelBackground = img;
}

void VisualisationPanel::paint (juce::Graphics& g)
{
    g.drawImage(panelBackground, getLocalBounds().toFloat());
}

void VisualisationPanel::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}


