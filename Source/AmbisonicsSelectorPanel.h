/*
  ==============================================================================

    AmbisonicsSelectorPanel.h
    Created: 12 Jan 2025 11:57:08am
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "UIConstants.h"
#include "StateManager.h"
//==============================================================================
/*
*/

class AmbisonicsSelectorPanel  : public juce::Component
{
public:
    AmbisonicsSelectorPanel(StateManager&,
                            const UI::FormatType);
    ~AmbisonicsSelectorPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    const UI::FormatType formatType;
    std::unique_ptr<APVTS::ComboBoxAttachment> comboBoxAttachment;
    
    juce::ComboBox
    orders;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmbisonicsSelectorPanel)
};
