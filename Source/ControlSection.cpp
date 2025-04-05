/*
  ==============================================================================

    ControlSection.cpp
    Created: 11 Jan 2025 1:22:02pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ControlSection.h"

//==============================================================================
ControlSection::ControlSection(StateManager& s)
: stateManager(s),
inputSelector(s, UI::FormatType::input),
outputSelector(s, UI::FormatType::output),
parameterPanel(s)
{
    addAndMakeVisible(inputSelector);
    addAndMakeVisible(outputSelector);
    addAndMakeVisible(parameterPanel);
}

ControlSection::~ControlSection()
{
}

void ControlSection::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("ControlSection", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
    
}

void ControlSection::resized()
{
    const float
    panelWidth  = getWidth(),
    panelHeight = getHeight();
    
    inputSelector.setBounds(0.f,
                            0.f,
                            panelWidth * UI::ControlSectionFactors::formatWidthFactor,
                            panelHeight * UI::ControlSectionFactors::formatHeightFactor
                            );
    
    const float
    outputX = inputSelector.getRight(),
    outputY = 0.f;
    
    outputSelector.setBounds(outputX,
                             outputY,
                             panelWidth * UI::ControlSectionFactors::formatWidthFactor,
                             panelHeight * UI::ControlSectionFactors::formatHeightFactor
                             );
    
    const float
    paramsX = getX(),
    paramsY = inputSelector.getBottom();
    
    parameterPanel.setBounds(paramsX,
                             paramsY,
                             panelWidth,
                             panelHeight * UI::ControlSectionFactors::paramsHeightFactor
                             );

}
