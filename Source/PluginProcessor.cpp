/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
USATAudioProcessor::USATAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::ambisonic(1), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::ambisonic(1), true)
                     #endif
                       ),
#endif
userParameters(*this, nullptr, juce::Identifier("USAT Designer"),
#include "ParameterDefinitions.h"
           ),
stateManager(userParameters),
decoder(progressValue, statusValue)
{
    //decode();
}

USATAudioProcessor::~USATAudioProcessor()
{
    cancelDecoding();
}

//==============================================================================
const juce::String USATAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool USATAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool USATAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool USATAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double USATAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int USATAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int USATAudioProcessor::getCurrentProgram()
{
    return 0;
}

void USATAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String USATAudioProcessor::getProgramName (int index)
{
    return {};
}

void USATAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void USATAudioProcessor::decode()
{
    // TODO: Do prior check as to whether the channel dimensions will match and ask user whether they want to continue if they don't
    std::string globalValueTree = stateManager.createGlobalValueTree().toXmlString().toStdString();
    decoder.computeMatrix(globalValueTree);
    
    const auto matrix           = decoder.getGainMatrixInstance();
    auto decodingMatrixTree     = stateManager.createGainMatrixTree(matrix);
    
    stateManager.debugValueTree(decodingMatrixTree);
}

void USATAudioProcessor::cancelDecoding() {
    DBG("Stopping Thread");
    decoder.signalStopPythread();
}

void USATAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    decoder.prepare(sampleRate,
                    samplesPerBlock,
                    getTotalNumInputChannels(),
                    getTotalNumOutputChannels());
}

void USATAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool USATAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Allow any number of channels on input and output
    // Optionally, enforce at least some channels to avoid edge cases
    if (layouts.getMainOutputChannelSet().isDisabled())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainInputChannelSet().isDisabled())
        return false;
   #endif

    return true; // Support any input and output channel configuration determined by the host.
}
#endif

void USATAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    if (decoder.decodingMatrixReady()) {
        decoder.process(buffer, getTotalNumInputChannels(), getTotalNumOutputChannels());
    }
}

//==============================================================================
bool USATAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* USATAudioProcessor::createEditor()
{
    return new WrappedAudioProcessorEditor (*this);
}

//==============================================================================
void USATAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    
}

void USATAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new USATAudioProcessor();
}
