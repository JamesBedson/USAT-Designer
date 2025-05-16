/*
  ==============================================================================

    LayoutSelectorPanel.h
    Created: 12 Jan 2025 11:56:56am
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
#include "UIConstants.h"
#include "SpeakerLayoutWindow.h"
#include "CustomLNF.h"
//==============================================================================
/*
*/
class LayoutSelectorPanel  : public juce::Component,
public juce::Button::Listener
{
public:
    LayoutSelectorPanel(StateManager&,
                        const UI::FormatType);
    ~LayoutSelectorPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void buttonClicked (juce::Button*) override;
    
    void setChannelText() {
        
        int speakerCount = 0;
        juce::String labelText;
        
        if (formatType == UI::FormatType::input) {
            auto& speakerManager    = stateManager.inputSpeakerManager;
            speakerCount            = speakerManager.getSpeakerCount();
        }
        else {
            auto& speakerManager    = stateManager.outputSpeakerManager;
            speakerCount            = speakerManager.getSpeakerCount();
        }
        
        labelText << "configuration: ";
        if (speakerCount == 0)
            labelText << "none defined";
        
        else
            labelText << juce::String(speakerCount);
        
        channelsLabel.setText(labelText, juce::dontSendNotification);
    }
    
    void setFormatText() {
        if (formatType == UI::FormatType::input) {
            formatLabel.setText("input", juce::dontSendNotification);
        }
        else {
            formatLabel.setText("output", juce::dontSendNotification);
        }
    }

private:
   
    juce::TextButton
    editLayout,
    exportLayout,
    loadLayout;
    
    juce::Label
    channelsLabel,
    formatLabel;
    
    CustomLNF lookAndFeel;

    juce::Label
        layoutName;

    void showExportDialog();
    void showImportDialog();
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    const UI::FormatType formatType;
    juce::Component::SafePointer<SpeakerLayoutWindow> layoutWindow;
    StateManager& stateManager;
    
    juce::Image background;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutSelectorPanel)
};
