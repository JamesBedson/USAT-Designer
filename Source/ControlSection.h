/*
  ==============================================================================

    ControlSection.h
    Created: 11 Jan 2025 1:22:02pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
#include "ParameterPanel.h"
#include "UIConstants.h"
#include "CustomLNF.h"

//==============================================================================
/*
*/
class ControlSection  : public juce::Component
{
public:
    ControlSection(StateManager&);
    ~ControlSection() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    StateManager& stateManager;
    
    juce::ToggleButton parameterTypeSwitch;
    juce::TextButton firstPageButton, secondPageButton;
    juce::ImageButton parameterStyleSwitch;
    
    juce::Label simple, advanced;
    ParameterPanel
    parameterPanel;
    
    juce::Image sectionBackground;
    CustomLNF lookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlSection)
};
