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
#include "FormatSelectorPanel.h"
#include "ParameterPanel.h"
#include "UIConstants.h"

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
    
    FormatSelectorPanel
    inputSelector,
    outputSelector;
    
    ParameterPanel
    parameterPanel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlSection)
};
