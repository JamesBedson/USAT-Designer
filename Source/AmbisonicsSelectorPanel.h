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
#include "CustomLNF.h"
//==============================================================================
/*
*/

class AmbisonicsSelectorPanel  :
public juce::Component,
public juce::ComboBox::Listener
{
public:
    AmbisonicsSelectorPanel(StateManager&,
                            const UI::FormatType);
    ~AmbisonicsSelectorPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged (juce::ComboBox* comboBoxThatHasChanged) override;

private:
    const UI::FormatType formatType;
    std::unique_ptr<APVTS::ComboBoxAttachment> comboBoxAttachment;
    
    void setChannelConfigText() {
        const int idx           = orders.getSelectedItemIndex();
        const int numChannels   = getAmbisonicsChannels(idx + 1);
        juce::String labelText;
        labelText << "configuration: ";
        labelText << juce::String(numChannels);
        labelText << " (ch)";
        channelsLabel.setText(labelText, juce::dontSendNotification);
    }
    
    void setFormatLabelText() {
        if (formatType == UI::FormatType::input) {
            formatLabel.setText("input", juce::dontSendNotification);
        }
        
        else {
            formatLabel.setText("output", juce::dontSendNotification);
        }
    }
    
    static const int getAmbisonicsChannels(const int order) {
        return std::pow((order + 1), 2);
    }
    
    juce::ComboBox
    orders;
    
    juce::Label
    orderLabel, channelsLabel, formatLabel;
    
    juce::Image background;
    
    CustomLNF lookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmbisonicsSelectorPanel)
};
