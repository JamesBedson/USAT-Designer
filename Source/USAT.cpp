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
           StateManager& s,
           GainMatrix& g)
: gainsMatrix(g),
progressValue(progress),
statusValue(status),
stateManager(s)
{
    DBG("Loaded Decoder");
    stateManager.signalNewGainMatrix.addListener(this);
    matrixReadyAtomic.store(false);
}

USAT::~USAT()
{
    if (pyThread && pyThread->isThreadRunning())
        {
            pyThread->signalThreadShouldExit();
            pyThread->waitForThreadToExit(1000);
        }
        pyThread = nullptr;
    updateGainMatrixXML.removeListener(this);
}

// GAINS ========================================================================

void USAT::computeMatrix(const std::string& valueTreeXML,
                         std::function<void()> onComplete)
{
    gainsMatrix.clear();
    pyThread = std::make_unique<PythonThread>(interpreter, gainsMatrix, base64PlotsStr);
    pyThread->setNewValueTree(valueTreeXML);
    
    pyThread->setOnDoneCallback([onComplete]() {
        if (onComplete)
            onComplete();
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
    //DBG("Producing Silence");
    for (int ch = 0; ch < buffer.getNumChannels(); ch++) {
        buffer.clear(ch, 0, buffer.getNumSamples());
    }
}

bool USAT::hostAndUSATInputDimensionsMatch(const int numInputChannelsHost)
{
    const int gainMatrixNumInputs   = gainsMatrix.getNumInputChannels();
    
    // No LFE
    if (LFEIndexIn == -1)
    {
        //DBG("No LFE");
        //DBG("Number of input channels: " << juce::String(numInputChannelsHost));
        return gainMatrixNumInputs == numInputChannelsHost;
    }
    
    // containsLFE
    else {
        //DBG("Input contains LFE");
        return gainMatrixNumInputs == numInputChannelsHost - 1;
    }
}

bool USAT::hostAndUSATOutputDimensionsMatch(const int numOutputChannelsHost) {
    int numChannelsThatShouldBePresent {0};
    const int gainMatrixNumOutputs = gainsMatrix.getNumOutputChannels();
    
    // No LFE
    if (LFEIndexOut == -1) {
        numChannelsThatShouldBePresent = gainMatrixNumOutputs;
    }
    
    // contains LFE
    else {
        numChannelsThatShouldBePresent = gainMatrixNumOutputs + 1;
    }
    //DBG("Number of output channels that should be present: " + juce::String(numChannelsThatShouldBePresent));
    //DBG("Number of output channels defined in host: " + juce::String(numOutputChannelsHost));
    
    return numChannelsThatShouldBePresent <= numOutputChannelsHost;
    
}
void USAT::prepare(double sampleRate,
                   int samplesPerBlock,
                   int LFEChannelIndexInput,
                   int LFEChannelIndexOutput)
{
    currentSamplesPerBlock  = samplesPerBlock;
    prepareTempBuffers();
    //matrixReadyAtomic.store(true);
}

void USAT::prepareTempBuffers() {
    auto inputChannelsMult  = gainsMatrix.getNumInputChannels();
    
    if (inputChannelsMult <= 0) {
        matrixReadyAtomic.store(false);
        return;
    }
    
    DBG("Setting multiplication buffer:");
    DBG("Input channels: " + juce::String(inputChannelsMult));
    DBG("Current samples per block: " + juce::String(currentSamplesPerBlock));
    multiplicationInputBuffer.setSize(inputChannelsMult, currentSamplesPerBlock);
    multiplicationInputBuffer.clear();
    DBG("After Setting Ch (matrix): " + juce::String(multiplicationInputBuffer.getNumChannels()));
    DBG("After Setting Smpl (matrix): " + juce::String(multiplicationInputBuffer.getNumChannels()));
    
    auto outputChannelsTemp = gainsMatrix.getNumOutputChannels();
    if (outputChannelsTemp <= 0) {
        matrixReadyAtomic.store(false);
        return;
    }
    
    DBG("Setting temp out buffer:");
    DBG("Input channels: " + juce::String(outputChannelsTemp));
    DBG("Current samples per block: " + juce::String(currentSamplesPerBlock));
    tempOutputBuffer.setSize(outputChannelsTemp, currentSamplesPerBlock);
    tempOutputBuffer.clear();
    
    DBG("After Setting Ch (temp): " + juce::String(tempOutputBuffer.getNumChannels()));
    DBG("After Setting Smpl (temp): " + juce::String(tempOutputBuffer.getNumChannels()));
    
    matrixReadyAtomic.store(true);
}
void USAT::process(juce::AudioBuffer<float> &buffer,
                   int numInputChannelsFromHost,
                   int totalNumOutputChannelsHost)
{
   
    bool inputChannelsMatch     = hostAndUSATInputDimensionsMatch(numInputChannelsFromHost);
    bool outputChannelsMatch    = hostAndUSATOutputDimensionsMatch(totalNumOutputChannelsHost);
    bool dimensionsOkay         = inputChannelsMatch && outputChannelsMatch;
    
    if (dimensionsOkay == true && matrixReadyAtomic.load() == true) {
        juce::AudioBuffer<float>* inputBuffer = nullptr;
        
        // LFE INPUT ====================================================================
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
        
        tempOutputBuffer.clear();
        // Matrix multiplication =======================================================
        auto numOutputChannelsMult  = gainsMatrix.getNumOutputChannels();
        auto numInputChannelsMult   = gainsMatrix.getNumInputChannels();
        auto numSamples             = buffer.getNumSamples();
        
        // Apply gain matrix to input buffer
        for (int chOut = 0; chOut < numOutputChannelsMult; chOut++) {
            float* dest = tempOutputBuffer.getWritePointer(chOut);
            for (int chIn = 0; chIn < numInputChannelsMult; chIn++) {
                
                const float* src = inputBuffer->getReadPointer(chIn);
                float gain       = gainsMatrix.get(chIn, chOut);

                if (chIn == 0) {
                    juce::FloatVectorOperations::multiply(dest, src, gain, numSamples);
                    //DBG("Destination channel 1: " + juce::String(dest[0]));
                }
                else {
                    juce::FloatVectorOperations::addWithMultiply(dest, src, gain, numSamples);
                    //DBG("Destination channel x: " + juce::String(dest[0]));
                }
                    
            }
        }
        int numOutputChannelsFromHost =  numOutputChannelsMult + 1;
        // LFE OUTPUT =================================================================
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
            for (int ch = LFEIndexOut + 1; ch < numOutputChannelsFromHost; ch++) {
                buffer.copyFrom(ch, 0, tempOutputBuffer, ch - 1, 0, numSamples);
            }
        }
        
        if (totalNumOutputChannelsHost > numOutputChannelsFromHost) {
            for (int ch = numOutputChannelsFromHost; ch < totalNumOutputChannelsHost; ++ch) {
                buffer.clear(ch, 0, buffer.getNumSamples());
            }
        }
    } // IF PROCESSING
    else {
        DBG("Producing Silence");
        produceSilence(buffer);
    }
}

void USAT::fillMatrixFromValueTree(const juce::ValueTree& matrixTree) {
    
    auto channelCountTree   = matrixTree.getChildWithName(ProcessingConstants::TreeTags::channelCountsID);
    auto LFEIndexTree       = matrixTree.getChildWithName(ProcessingConstants::TreeTags::LFEChannelIndices);
    auto coefficientsTree   = matrixTree.getChildWithName(ProcessingConstants::TreeTags::matrixCoefficientsID);
    
    if (!(channelCountTree.isValid()) && !(LFEIndexTree.isValid()) && !(coefficientsTree.isValid()) )
        {
            matrixReadyAtomic.store(false);
        }
    
    if (channelCountTree.getNumProperties() <= 0
        && LFEIndexTree.getNumProperties() <= 0
        && coefficientsTree.getNumProperties() <= 0) {
        matrixReadyAtomic.store(false);
    }
    
    // CHANNELS
    const int inputChannels  = channelCountTree.getProperty(ProcessingConstants::
                                                            GainMatrixTree::
                                                            ChannelCount::
                                                            inputChannelCount);
    const int outputChannels = channelCountTree.getProperty(ProcessingConstants::
                                                            GainMatrixTree::
                                                            ChannelCount::
                                                            outputChannelCount);
    DBG("Input Channels: " + juce::String(inputChannels));
    DBG("Output Channels: " + juce::String(outputChannels));
    
    gainsMatrix.setNumChannels(static_cast<int>(inputChannels),
                               static_cast<int>(outputChannels));
    
    // LFE
    const auto LFEIndexInProp   = LFEIndexTree.getProperty(ProcessingConstants::
                                                           GainMatrixTree::
                                                           LFEIndices::
                                                           inputLFEChannelIndex);
    
    const auto LFEIndexOutProp  = LFEIndexTree.getProperty(ProcessingConstants::
                                                           GainMatrixTree::
                                                           LFEIndices::
                                                           outputLFEChannelIndex);
    
    LFEIndexIn  = static_cast<int>(LFEIndexInProp);
    LFEIndexOut = static_cast<int>(LFEIndexOutProp);
    
    // COEFFICIENTS
    auto matrixCoefficientsTree = matrixTree.getChildWithName(ProcessingConstants::TreeTags::matrixCoefficientsID);
    for (int chIn = 0; chIn < inputChannels; chIn++) {
        for (int chOut = 0; chOut < outputChannels; chOut++) {
            
            const auto coefficientID = ProcessingConstants::GainMatrixTree::MatrixCoefficient::getCoefficientID(chIn, chOut);
            const double value       = matrixCoefficientsTree.getProperty(coefficientID);
    
            gainsMatrix.assign(chIn, chOut, value);
        }
    }
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
            DBG("Finished filling matrix.");
            prepareTempBuffers();
            DBG("Finished preparing Buffers.");
            value.setValue(false);
        }
    }
}
