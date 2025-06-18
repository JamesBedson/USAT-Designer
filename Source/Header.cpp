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
Header::Header(StateManager& s, USATAudioProcessor& p)
: stateManager(s)
{
    addAndMakeVisible(pluginName);
    pluginName.setText("usat designer.", juce::dontSendNotification);
    pluginName.setFont(UI::Fonts::getMainFontWithSize(40.f));
    /*
    int numInputsHost   = p.getBusesLayout().getMainInputChannels();
    int numOutputsHost  = p.getBusesLayout().getMainOutputChannels();

    addAndMakeVisible(numInputs);
    addAndMakeVisible(numOutputs);
    numInputs.setText("Input: " + juce::String(numInputsHost), juce::dontSendNotification);
    numOutputs.setText("Output: " + juce::String(numOutputsHost), juce::dontSendNotification);
     */
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
    width   = boundsReduced.getWidth() * 0.5f,
    height  = boundsReduced.getHeight();
    pluginName.setBounds(boundsReduced.getX(), boundsReduced.getY(), width, height);
    
    numInputs.setBounds(pluginName.getRight(), pluginName.getY(), width * 0.2f, height);
    numOutputs.setBounds(numInputs.getRight(), numInputs.getY(), width * 0.2f, height);
    
}
