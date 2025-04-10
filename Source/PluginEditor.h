/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StateManager.h"
#include "UIConstants.h"
#include "Header.h"
#include "ControlSection.h"
#include "VisualisationSection.h"
#include "DecoderSettingsPanel.h"

//==============================================================================

class RasterComponent  : public juce::Component
{
public:
    RasterComponent(USATAudioProcessor& p);
    ~RasterComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    USATAudioProcessor&     audioProcessor;
    Header                  header;
    ControlSection          controlSection;
    VisualisationSection    visualisationSection;
    DecoderSettingsPanel    decoderPanel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RasterComponent)
};

//==============================================================================
class WrappedAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    WrappedAudioProcessorEditor(USATAudioProcessor& p);
    ~WrappedAudioProcessorEditor() override;
    
    void resized() override;
    
private:
    
    RasterComponent     rasterComponent;
    USATAudioProcessor& audioProcessor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WrappedAudioProcessorEditor)
};
