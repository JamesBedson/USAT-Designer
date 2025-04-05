/*
  ==============================================================================

    TranscodingHandler.h
    Created: 26 Dec 2024 5:36:18pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ProcessingConstants.h"
#include "SpeakerManager.h"

using APVTS = juce::AudioProcessorValueTreeState;

class TranscodingConfigHandler {

public:
    
    TranscodingConfigHandler();

    SpeakerManager  speakerManagerInput,
                    speakerManagerOutput;
    
    void initialiseConfigTree();
    void modifyConfigTree(juce::String encodingOption,
                          juce::String formatOption);
    
    juce::ValueTree& getConfigTree();
    
private:
    juce::ValueTree configTree;
    
};
