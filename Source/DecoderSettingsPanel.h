/*
  ==============================================================================

    DecoderSettingsPanel.h
    Created: 11 Jan 2025 1:51:44pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MatrixComputeWindow.h"
//==============================================================================
/*
*/
class DecoderSettingsPanel
: public juce::Component,
public juce::Button::Listener

{
public:
    DecoderSettingsPanel(USATAudioProcessor&);
    ~DecoderSettingsPanel() override;
    
    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    
private:
    USATAudioProcessor& audioProcessor;
    juce::TextButton
        decode,
        save,
        load;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DecoderSettingsPanel)
};
