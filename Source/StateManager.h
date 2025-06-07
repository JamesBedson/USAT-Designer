/*
  ==============================================================================

    StateManager.h
    Created: 1 Dec 2024 4:38:44pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ProcessingConstants.h"
#include "SpeakerManager.h"
#include "GainMatrix.h"

using APVTS = juce::AudioProcessorValueTreeState;

class StateManager {

public:
    StateManager(APVTS& apvts);
    ~StateManager();
    
    static const juce::File presetsDirectory;
    static const juce::File resourceDirectory;
    static const juce::File pythonScriptsDirectory;
    static const juce::File speakerLayoutDirectory;
    
    void ensureDirectoryExists(const juce::File& directory);
    void initCoefficientsTree();
    
    // Value Trees
    const juce::ValueTree createGlobalValueTree() const;
    const juce::ValueTree createEncodingSettingsTree() const;
    const juce::ValueTree createInputAmbisonicsTree() const;
    const juce::ValueTree createOutputAmbisonicsTree() const;
    const juce::ValueTree createGainMatrixTree(const GainMatrix& matrix) const;
    const juce::ValueTree createPlotTree(const std::array<std::string, 5> base64PlotStrings) const;
    
    void debugValueTree(const juce::ValueTree& tree) const;
    
    APVTS&          apvts;
    SpeakerManager  inputSpeakerManager;
    SpeakerManager  outputSpeakerManager;
    juce::ValueTree coefficientsTree;
    
private:
    
};
