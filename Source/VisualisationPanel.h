/*
  ==============================================================================

    VisualisationPanel.h
    Created: 12 Jan 2025 11:13:02am
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
//==============================================================================
/*
*/
class VisualisationPanel  : public juce::Component
{
public:
    VisualisationPanel(StateManager&);
    ~VisualisationPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void displayImage(const juce::Image& img);
    
private:
    StateManager& stateManager;
    juce::Image panelBackground;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VisualisationPanel)
};
