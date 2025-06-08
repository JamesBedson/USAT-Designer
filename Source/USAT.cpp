/*
  ==============================================================================

    USAT.cpp
    Created: 8 Nov 2024 9:32:14pm
    Author:  James Bedson

  ==============================================================================
*/

#include "USAT.h"

USAT::USAT(juce::Value& progress,
           juce::Value& status,
           juce::Value& processCompleted,
           StateManager& s)
: progressValue(progress),
statusValue(status),
processCompleted(processCompleted),
stateManager(s)
{
    stateManager.signalNewGainMatrix.addListener(this);
    //pyThread = std::make_unique<PythonThread>(interpreter, gainsMatrix, base64PlotsStr);
}

USAT::~USAT()
{
    
}

// GAINS ========================================================================

void USAT::computeMatrix(const std::string& valueTreeXML,
                         std::function<void()> onComplete)
{
    pyThread->setNewValueTree(valueTreeXML);
    
    pyThread->setOnDoneCallback([this, onComplete]()
    {
        if (onComplete) {
            juce::MessageManager::callAsync(onComplete);
            processCompleted = true;
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

void USAT::produceSilence(juce::AudioBuffer<float> &buffer) {
    DBG("Producing Silence");
    for (int ch = 0; ch < buffer.getNumChannels(); ch++) {
        buffer.clear(ch, 0, buffer.getNumSamples());
    }
}

bool USAT::hostAndUSATInputDimensionsMatch(const int numInputChannelsHost)
{
    const int gainMatrixNumInputs   = gainsMatrix.getNumInputChannels();
    
    // No LFE
    if (LFEIndexIn == -1)
        return gainMatrixNumInputs == numInputChannelsHost;
    
    // containsLFE
    else
        return gainMatrixNumInputs == numInputChannelsHost - 1;
}

bool USAT::hostAndUSATOutputDimensionsMatch(const int numOutputChannelsHost) {
    const int gainMatrixNumOutputs = gainsMatrix.getNumOutputChannels();
    
    // No LFE
    if (LFEIndexOut == -1)
        return gainMatrixNumOutputs == numOutputChannelsHost;
    
    // contains LFE
    else
        return gainMatrixNumOutputs == numOutputChannelsHost - 1;
    
}
void USAT::prepare(double sampleRate,
                   int samplesPerBlock,
                   int numInputChannelsInHost,
                   int numOutputChannelsInHost,
                   int LFEChannelIndexInput,
                   int LFEChannelIndexOutput)
{
    LFEIndexIn  = LFEChannelIndexInput;
    LFEIndexOut = LFEChannelIndexOutput;
    
    auto inputChannelsMult  = gainsMatrix.getNumInputChannels();
    multiplicationInputBuffer.setSize(inputChannelsMult, samplesPerBlock);
    multiplicationInputBuffer.clear();
    
    auto outputChannelsTemp = gainsMatrix.getNumOutputChannels();
    tempOutputBuffer.setSize(outputChannelsTemp, samplesPerBlock);
    tempOutputBuffer.clear();
}


void USAT::process(juce::AudioBuffer<float> &buffer,
                   int numInputChannelsFromHost,
                   int numOutputChannelsFromHost)
{
    bool dimensionsOkay = hostAndUSATInputDimensionsMatch(numInputChannelsFromHost) && hostAndUSATOutputDimensionsMatch(numOutputChannelsFromHost);
    
    if (!dimensionsOkay) {
        produceSilence(buffer);
    } // Channels don't match --> silence
    
    else {
        juce::AudioBuffer<float>* inputBuffer = nullptr;
        
        if (LFEIndexIn == -1) {
            inputBuffer = &buffer; // can use the same buffer for input
        }
        
        else { // contains LFE channel that needs to be handled
            
            int copyCh = 0;
            for (int ch = 0; ch < numInputChannelsFromHost; ++ch)
            {
                if (ch == LFEIndexIn)
                    continue;           // skip LFE channel

                multiplicationInputBuffer.copyFrom(copyCh, 0, buffer, ch, 0, buffer.getNumSamples());
                ++copyCh; // only increment if copied
            }
            inputBuffer = &multiplicationInputBuffer;
        }
        
        auto numOutputChannelsMult  = gainsMatrix.getNumOutputChannels();
        auto numInputChannelsMult   = gainsMatrix.getNumInputChannels();
        auto numSamples             = buffer.getNumSamples();
        
        // Apply gain matrix to input buffer
        for (int chOut = 0; chOut < numOutputChannelsMult; chOut++) {
            
            float* dest = tempOutputBuffer.getWritePointer(chOut);
            
            for (int chIn = 0; chIn < numInputChannelsMult; chIn++) {
                
                const float* src = inputBuffer->getReadPointer(chIn);
                float gain       = gainsMatrix.get(chIn, chOut);

                if (chIn == 0)
                    juce::FloatVectorOperations::multiply(dest, src, gain, numSamples);
                else
                    juce::FloatVectorOperations::addWithMultiply(dest, src, gain, numSamples);
            }
        }
        
        // No LFE in output
        if (LFEIndexOut == -1) {
            // Copy all channels directly
            for (int ch = 0; ch < numOutputChannelsMult; ++ch) {
                buffer.copyFrom(ch, 0, tempOutputBuffer, ch, 0, numSamples);
            }
        }
        
        // LFE is present in output
        else {
            
            // Copying all channels before the LFE channel
            for (int ch = 0; ch < LFEIndexOut; ch++) {
                buffer.copyFrom(ch, 0, tempOutputBuffer, ch, 0, numSamples);
            }
            
            // Muting LFE channel
            buffer.clear(LFEIndexOut, 0, numSamples);
            
            // Copying all channels after the LFE channel
            for (int ch = LFEIndexOut; ch < numOutputChannelsFromHost - 1; ch++) {
                buffer.copyFrom(ch + 1, 0, tempOutputBuffer, ch, 0, numSamples);
            }
        }
    }
}

void USAT::fillMatrixFromValueTree(const juce::ValueTree& matrixTree) {
    
    auto channelCountTree = matrixTree.getChildWithName(ProcessingConstants::TreeTags::channelCountsID);
    
    const int inputChannels  = channelCountTree.getProperty(ProcessingConstants::GainMatrixTree::ChannelCount::inputChannelCount);
    const int outputChannels = channelCountTree.getProperty(ProcessingConstants::GainMatrixTree::ChannelCount::outputChannelCount);

    gainsMatrix.setNumChannels(inputChannels, outputChannels);
    
    auto matrixCoefficientsTree = matrixTree.getChildWithName(ProcessingConstants::TreeTags::matrixCoefficientsID);
    
    for (int chIn = 0; chIn < inputChannels; chIn++) {
        for (int chOut = 0; chOut < outputChannels; chOut++) {
            
            const auto coefficientID = ProcessingConstants::GainMatrixTree::MatrixCoefficient::getCoefficientID(chIn, chOut);
            const double value       = matrixCoefficientsTree.getProperty(coefficientID);
    
            gainsMatrix.assign(chIn, chOut, value);
        }
    }
    gainsMatrix.debugMatrix();
}

const GainMatrix& USAT::getGainMatrixInstance() const {
    return gainsMatrix;
}

const std::array<std::string, 6> USAT::getBase64Plots() const {
    return base64PlotsStr;
}

void USAT::valueChanged(juce::Value &value) {
    if (value.refersToSameSourceAs(stateManager.signalNewGainMatrix)) {
        if (static_cast<bool>(value.getValue()) == true) {
            auto gainMatrixTree = stateManager.gainMatrixTree.createCopy();
            fillMatrixFromValueTree(gainMatrixTree);
            value.setValue(false);
        }
    }
}
