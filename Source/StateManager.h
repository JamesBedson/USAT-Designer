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
    static const juce::File transcodingsDirectory;
    
    void ensureDirectoryExists(const juce::File& directory);
    void initCoefficientsTree();
    void initGainMatrixTree();
    void initPlotsTree();
    
    // Value Trees
    const juce::ValueTree createParameterValueTree() const;
    const juce::ValueTree createSettingsTree() const;
    const juce::ValueTree createInputAmbisonicsTree() const;
    const juce::ValueTree createOutputAmbisonicsTree() const;
    const juce::ValueTree createGainMatrixTree(const GainMatrix& matrix) const;
    const juce::ValueTree createPlotTree(const std::array<std::string, 6> base64PlotStrings) const;
    const juce::ValueTree createGlobalStateTree() const;
    
    void updateAPVTSParameters(const juce::ValueTree& settings,
                               const juce::ValueTree& ambisonicsInput,
                               const juce::ValueTree& ambisonicsOutput);
    
    void updateCoefficients(const juce::ValueTree& coefficients);
    void updatePlots(const juce::ValueTree& plots);
    void updateGainMatrixCoefficients(const juce::ValueTree& gainMatrix);
    
    void saveStateParametersToXML(const juce::File& xmlFile);
    void loadStateParametersFromXML(const juce::File& xmlFile);
    void debugValueTree(const juce::ValueTree& tree) const;
    
    const int getLFEChannelIndexInput() const;
    const int getLFEChannelIndexOutput() const;
    
    APVTS&          formatSettings;
    SpeakerManager  inputSpeakerManager;
    SpeakerManager  outputSpeakerManager;
    juce::ValueTree coefficientsTree;
    juce::ValueTree gainMatrixTree;
    juce::ValueTree plotsTree;
    
    juce::Value signalNewGainMatrix;
    juce::Value signalPlots;
    juce::Value signalCoefficients;
    
    int LFEChannelIndexInput, LFEChanenlIndexOutput;
    
private:
    
};
