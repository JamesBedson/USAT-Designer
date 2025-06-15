/*
  ==============================================================================

    USAT.h
    Created: 8 Nov 2024 9:32:14pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <Python.h>
#include <JuceHeader.h>
#include "ProcessingConstants.h"
#include "StateManager.h"
#include "PythonInterpreter.h"
#include "GainMatrix.h"

using APVTS     = juce::AudioProcessorValueTreeState;

class USAT : public juce::Value::Listener {
    
public:
    
    USAT(juce::Value& progress,
         juce::Value& status,
         juce::Value& processCompleted,
         StateManager& stateManager);
    ~USAT();
    
    enum MatrixDim{
        Row,
        Column
    };
    
    // Matrix Processing
    void computeMatrix(const std::string& valueTreeXML,
                       std::function<void()> onComplete);
    
    void signalStopPythread();
    
    void prepare(double sampleRate,
                 int samplesPerBlock,
                 int numInputChannelsInHost,
                 int numOutputChannelsInHost,
                 int LFEChannelIndexInput,
                 int LFEChannelIndexOutput);
    
    void process(juce::AudioBuffer<float>& buffer,
                 int numInputChannelsFromHost,
                 int numOutputChannelsFromHost);
    
    const GainMatrix& getGainMatrixInstance() const;
    const std::array<std::string, 6> getBase64Plots() const;
    
    void fillMatrixFromValueTree(const juce::ValueTree&);
    void valueChanged(juce::Value &value) override;
    void produceSilence(juce::AudioBuffer<float>& buffer);
    bool hostAndUSATInputDimensionsMatch(const int numInputChannelsHost);
    bool hostAndUSATOutputDimensionsMatch(const int numOutputChannelsHost);
    
private:    
    
    juce::AudioBuffer<float> tempInputBuffer;
    juce::AudioBuffer<float> tempOutputBuffer;
    juce::AudioBuffer<float> multiplicationInputBuffer;
    
    ProcessingConstants::PythonParameterNameMap pythonParameterMap;
    
    PythonInterpreter interpreter;
    std::unique_ptr<PythonThread> pyThread;
    
    GainMatrix gainsMatrix;
    std::array<std::string, 6> base64PlotsStr;
    juce::Value& progressValue;
    juce::Value& statusValue;
    juce::Value& processCompleted;
    juce::Value updateGainMatrixXML;
    StateManager& stateManager;
    
    int LFEIndexIn, LFEIndexOut;
};
