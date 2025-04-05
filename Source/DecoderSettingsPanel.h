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
//==============================================================================
/*
*/
class DecoderSettingsPanel  : public juce::Component
{
public:
    DecoderSettingsPanel(USATAudioProcessor&);
    ~DecoderSettingsPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    USATAudioProcessor& audioProcessor;
    
    juce::TextButton
    decode;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DecoderSettingsPanel)
};
