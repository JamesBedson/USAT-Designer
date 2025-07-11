/*
  ==============================================================================

    Header.h
    Created: 11 Jan 2025 1:21:19pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
#include "UIConstants.h"
#include "PluginProcessor.h"
//==============================================================================
/*
*/
class Header  : public juce::Component
{
public:
    Header(StateManager&, USATAudioProcessor& p);
    ~Header() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    StateManager& stateManager;
    
    juce::Label pluginName, about;
    juce::Label numInputs, numOutputs;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Header)
};
