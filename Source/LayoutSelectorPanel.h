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

private:
    juce::TextButton
    editLayout,
    exportLayout,
    loadLayout;

    juce::Label
        layoutName;

    void showExportDialog();
    void showImportDialog();
    std::unique_ptr<juce::FileChooser> fileChooser;
    
    const UI::FormatType formatType;
    juce::Component::SafePointer<SpeakerLayoutWindow> layoutWindow;
    StateManager& stateManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LayoutSelectorPanel)
};
