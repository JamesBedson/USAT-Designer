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

class USAT {
    
public:
    
    USAT(juce::Value& progress, juce::Value& status);
    ~USAT();
    
    enum MatrixDim{
        Row,
        Column
    };
    
    // Matrix Processing
    void computeMatrix(const std::string& valueTreeXML,
                       std::function<void()> onComplete);
    
    void signalStopPythread();
    const bool decodingMatrixReady();
    
    void prepare(double sampleRate,
                 int samplesPerBlock,
                 int numInputChannelsInHost,
                 int numOutputChannelsInHost);
    
    void process(juce::AudioBuffer<float>& buffer,
                 int numInputChannelsFromHost,
                 int numOutputChannelsFromHost);
    
    const GainMatrix& getGainMatrixInstance() const;
    const std::array<std::string, 5> getBase64Plots() const;
    
    void fillMatrixFromValueTree(const juce::ValueTree&);
    
private:    
    const int getMatrixChannelCountIn();
    const int getMatrixChannelCountOut();
    
    juce::AudioBuffer<float> tempOutputBuffer;
    
    int currentChannelCountIn;
    int currentChannelCountOut;
    
    ProcessingConstants::PythonParameterNameMap pythonParameterMap;
    
    bool matrixReady;
    PythonInterpreter interpreter;
    std::unique_ptr<PythonThread> pyThread;
    
    GainMatrix gainsMatrix;
    std::array<std::string, 5> base64PlotsStr;
    juce::Value& progressValue;
    juce::Value& statusValue;
};
