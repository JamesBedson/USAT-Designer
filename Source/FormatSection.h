/*
  ==============================================================================

    FormatSection.h
    Created: 15 May 2025 12:20:30pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FormatSelectorPanel.h"

//==============================================================================
/*
*/
class FormatSection  : public juce::Component
{
public:
    FormatSection(StateManager&);
    ~FormatSection() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    FormatSelectorPanel
    inputSelector,
    outputSelector;
    
    juce::Image sectionBackground;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FormatSection)
};
