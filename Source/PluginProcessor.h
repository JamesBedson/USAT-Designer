/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProcessingConstants.h"
#include "USAT.h"
#include "StateManager.h"


//==============================================================================

class USATAudioProcessor  : public juce::AudioProcessor
{
public:
    using APVTS = juce::AudioProcessorValueTreeState;
    //==============================================================================
    USATAudioProcessor();
    ~USATAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    StateManager stateManager;
    std::unique_ptr<USAT> decoder;
    
    void decode();
    
    GainMatrix  gainMatrix;
    juce::Value progressValue;
    juce::Value statusValue;
    juce::Value processCompleted;
    juce::Value matrixIsReady;
    std::atomic<bool> matrixReadyAtomic;
    int currentNumSamples {0};
    
    bool isPlaying, stoppedPlaying;
    int64_t playheadCurrentPosition {0}, playheadPreviousPosition {0};
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (USATAudioProcessor)
    APVTS userParameters;
    
};
