/*
  ==============================================================================

    FormatSelectorPanel.h
    Created: 11 Jan 2025 2:13:58pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
#include "UIConstants.h"
#include "LayoutSelectorPanel.h"
#include "AmbisonicsSelectorPanel.h"
#include "CustomLNF.h"
//==============================================================================
/*
*/
class FormatSelectorPanel  : public juce::Component,
public juce::ComboBox::Listener
{
public:
    
    FormatSelectorPanel(StateManager&, const UI::FormatType);
    ~FormatSelectorPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;

private:
    StateManager& stateManager;
    
    juce::ComboBox comboBox;
    std::unique_ptr<APVTS::ComboBoxAttachment> comboBoxAttachment;
    LayoutSelectorPanel     layoutSelectorPanel;
    AmbisonicsSelectorPanel ambisonicsSelectorPanel;
    bool paintLayoutSelector = false;
    
    CustomLNF lookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FormatSelectorPanel)
};
