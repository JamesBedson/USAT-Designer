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
visPanelBottom(s),
sectionBackground(juce::ImageCache::getFromMemory(BinaryData::vis_section3x_png, BinaryData::vis_section3x_pngSize))
{
    addAndMakeVisible(visPanelTop);
    addAndMakeVisible(visPanelBottom);
}

VisualisationSection::~VisualisationSection()
{
}

void VisualisationSection::paint (juce::Graphics& g)
{
    g.drawImage(sectionBackground, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
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
