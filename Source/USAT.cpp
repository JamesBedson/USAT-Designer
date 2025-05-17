/*
  ==============================================================================

    USAT.cpp
    Created: 8 Nov 2024 9:32:14pm
    Author:  James Bedson

  ==============================================================================
*/

#include "USAT.h"

USAT::USAT(juce::Value& progress, juce::Value& status)
: progressValue(progress),
statusValue(status)
{
    matrixReady = false;
    currentChannelCountIn   = 0;
    currentChannelCountOut  = 0;
    //pyThread = std::make_unique<PythonThread>(interpreter, gainsMatrix);
}

USAT::~USAT()
{
    
}

// CONFIGURATION ================================================================

const bool USAT::decodingMatrixReady()
{
    return matrixReady;
}

// GAINS ========================================================================

void USAT::computeMatrix(const std::string& valueTreeXML,
                         std::function<void()> onComplete)
{
    matrixReady = false;
    pyThread->setNewValueTree(valueTreeXML);
    
    pyThread->setOnDoneCallback([this, onComplete]()
    {
        matrixReady = true;
        DBG("Matrix is done!");
        
        if (onComplete) {
            juce::MessageManager::callAsync(onComplete);
        }
    });
    
    pyThread->setOnProgressCallback([this](float progress) {
        
        juce::MessageManager::callAsync([this, progress]() {
            progressValue.setValue(progress);
        });
    });
    
    pyThread->setOnStatusCallback([this](std::string status) {
        
        juce::MessageManager::callAsync([this, status]() {
            statusValue.setValue(juce::String(status));
        });
    });
    
    pyThread->startThread();
}

void USAT::signalStopPythread() {
    pyThread->signalThreadShouldExit();
    DBG("Signaled Exit.");
}

// ==============================================================
void USAT::prepare(double sampleRate,
                   int samplesPerBlock,
                   int numInputChannelsInHost,
                   int numOutputChannelsInHost)
{
    currentChannelCountIn   = numInputChannelsInHost;
    currentChannelCountOut  = numOutputChannelsInHost;
    
    if (currentChannelCountOut > 0) {
        tempOutputBuffer.setSize(currentChannelCountOut, samplesPerBlock);
        tempOutputBuffer.clear();

        if (!gainsMatrix.verifyMatrixDimensions(numInputChannelsInHost, numOutputChannelsInHost)) {
            // TODO: Handle Input and Output Dimension Mismatch. Dialog Box or something
        };
    }
}


void USAT::process(juce::AudioBuffer<float> &buffer,
                   int numInputChannelsFromHost,
                   int numOutputChannelsFromHost)
{
    const int numSamples        = buffer.getNumSamples();
    const int numInputChannels  = juce::jmin(numInputChannelsFromHost, gainsMatrix.getNumInputChannels());
    const int numOutputChannels = gainsMatrix.getNumOutputChannels();
    
    if (gainsMatrix.verifyMatrixDimensions(numInputChannelsFromHost, numOutputChannelsFromHost)) {
 
        for (int chOut = 0; chOut < numOutputChannels; chOut++) {
            float* dest = tempOutputBuffer.getWritePointer(chOut);
            
            for (int chIn = 0; chIn < numInputChannels; chIn++) {
                const float* src    = buffer.getReadPointer(chIn);
                float gain          = gainsMatrix.get(chIn, chOut);
                
                if (chIn == 0)
                    juce::FloatVectorOperations::multiply(dest, src, gain, numSamples);
                
                else
                    juce::FloatVectorOperations::addWithMultiply(dest, src, gain, numSamples);
            } // Input Channels
        } // Output Channels
        
        for (int ch = 0; ch < numOutputChannels; ++ch)
            buffer.copyFrom(ch, 0, tempOutputBuffer, ch, 0, numSamples);
    }
}

void USAT::fillMatrixFromValueTree(const juce::ValueTree& matrixTree) {
    
    const int inputChannels  = matrixTree.getProperty(ProcessingConstants::GainMatrixTree::ChannelCount::inputChannelCount);
    const int outputChannels = matrixTree.getProperty(ProcessingConstants::GainMatrixTree::ChannelCount::outputChannelCount);
    
    //jassert(inputChannels > 0 && outputChannels > 0);
    
    gainsMatrix.setNumChannels(inputChannels, outputChannels);
    
    for (int chIn = 0; chIn < inputChannels; chIn++) {
        for (int chOut = 0; chOut < outputChannels; chOut++) {
            
            const auto coefficientID = ProcessingConstants::GainMatrixTree::MatrixCoefficient::getCoefficientID(chIn, chOut);
            const double value       = matrixTree.getProperty(coefficientID);
    
            gainsMatrix.assign(chIn, chOut, value);
        }
    }
}

const GainMatrix& USAT::getGainMatrixInstance() const {
    return gainsMatrix;
}
