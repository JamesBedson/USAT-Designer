/*
  ==============================================================================

    Header.cpp
    Created: 11 Jan 2025 1:21:19pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Header.h"

//==============================================================================
Header::Header(StateManager& s)
: stateManager(s)
{
    addAndMakeVisible(pluginName);
    pluginName.setText("usat designer.", juce::dontSendNotification);
    pluginName.setFont(juce::Font(juce::FontOptions {"Futura", 40.f, juce::Font::plain}));
}

Header::~Header()
{
}

void Header::paint (juce::Graphics& g)
{
    
}

void Header::resized()
{
    auto boundsReduced = getLocalBounds().reduced(5.f);
    
    const float
    width = boundsReduced.getWidth() * 0.5f,
    height = boundsReduced.getHeight();
    pluginName.setBounds(boundsReduced.getX(), boundsReduced.getY(), width, height);
    
}
