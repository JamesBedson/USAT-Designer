/*
  ==============================================================================

    VisualisationSection.h
    Created: 11 Jan 2025 1:22:20pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
#include "VisualisationPanel.h"
#include "UIConstants.h"
//==============================================================================
/*
*/
class VisualisationSection  : public juce::Component
{
public:
    VisualisationSection(StateManager&);
    ~VisualisationSection() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    StateManager& stateManager;
    
    VisualisationPanel
    visPanelTop,
    visPanelBottom;
    
    juce::Image sectionBackground;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualisationSection)
};
