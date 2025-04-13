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
using Matrix    = std::vector<std::vector<float>>;

class USAT {
    
public:
    
    USAT();
    ~USAT();
    
    enum MatrixDim{
        Row,
        Column
    };
    
    // Matrix Processing
    void computeMatrix(const std::string& valueTreeXML);
    const bool decodingMatrixReady();
    
    void prepare(double sampleRate,
                 int samplesPerBlock,
                 int numInputChannelsInHost,
                 int numOutputChannelsInHost);
    
    void process(juce::AudioBuffer<float>& buffer,
                 int numInputChannelsFromHost,
                 int numOutputChannelsFromHost);
    
    std::unique_ptr<PythonThread> pyThread;
    GainMatrix gainsMatrix;
    
private:    
    const int getMatrixChannelCountIn();
    const int getMatrixChannelCountOut();
    
    juce::AudioBuffer<float> tempOutputBuffer;
    
    int currentChannelCountIn;
    int currentChannelCountOut;
    
    ProcessingConstants::PythonParameterNameMap pythonParameterMap;
    
    bool matrixReady;
    PythonInterpreter interpreter;
};
