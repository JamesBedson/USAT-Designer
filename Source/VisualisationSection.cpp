/*
  ==============================================================================

    VisualisationSection.cpp
    Created: 11 Jan 2025 1:22:20pm
    Author:  James Bedson

  ==============================================================================
*/

#include <JuceHeader.h>
#include "VisualisationSection.h"

//==============================================================================
VisualisationSection::VisualisationSection(StateManager& s)
: stateManager(s),
visPanelTop(s),
visPanelBottom(s)
{
    addAndMakeVisible(visPanelTop);
    addAndMakeVisible(visPanelBottom);

}

VisualisationSection::~VisualisationSection()
{
}

void VisualisationSection::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("VisualisationSection", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
}

void VisualisationSection::resized()
{
    const float
    sectionWidth    = getWidth(),
    sectionHeight   = getHeight(),
    padding         = sectionHeight * UI::VisualisationSectionFactors::paddingFactor,
    visPanelWidth   = sectionWidth - 2.f * padding,
    visPanelHeight  = sectionHeight / 2.f - 1.5f * padding;
    
    const float
    xTopVisPanel    = padding,
    yTopVisPanel    = padding;
    
    visPanelTop.setBounds(xTopVisPanel,
                          yTopVisPanel,
                          visPanelWidth,
                          visPanelHeight
                          );
    
    const float
    xBottomVisPanel = padding,
    yBottomVisPanel = visPanelTop.getBottom() + padding;
    
    visPanelBottom.setBounds(xBottomVisPanel,
                             yBottomVisPanel,
                             visPanelWidth,
                             visPanelHeight
                             );
    
}
