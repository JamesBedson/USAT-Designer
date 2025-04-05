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

//==============================================================================
/*
*/
class Header  : public juce::Component
{
public:
    Header(StateManager&);
    ~Header() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    StateManager& stateManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Header)
};
