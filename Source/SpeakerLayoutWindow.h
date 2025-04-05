/*
  ==============================================================================

    SpeakerLayoutWindow.h
    Created: 12 Jan 2025 2:21:16pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "StateManager.h"
#include "SpeakerLayoutPanel.h"

class SpeakerLayoutWindow : public juce::DocumentWindow {
  
public:
    SpeakerLayoutWindow(const juce::String windowName,
                        StateManager& s,
                        const UI::FormatType formatType)
    
    : juce::DocumentWindow(windowName,
                           juce::Colours::transparentBlack,
                           juce::DocumentWindow::allButtons
                           )
    {
        setContentOwned(new SpeakerLayoutPanel(s, formatType), true);
        setResizable(false, false);
    }
    
    void closeButtonPressed() override {
        delete this;
    }
    
};
