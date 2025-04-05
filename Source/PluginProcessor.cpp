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
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#endif
userParameters(*this, nullptr, juce::Identifier("USAT Designer"),
#include "ParameterDefinitions.h"
           ),
stateManager(userParameters)
{
    decoder.setChannelCounts(getTotalNumInputChannels(), getTotalNumOutputChannels());
    decode();
}

USATAudioProcessor::~USATAudioProcessor()
{
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
    std::string globalValueTree    = stateManager.createGlobalValueTree().toXmlString().toStdString();
    std::string scriptPath          = stateManager.getPythonScript().getFullPathName().toStdString();
    decoder.computeMatrix(scriptPath, globalValueTree);
}


void USATAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // check if gains are loaded
    // decoder.decoderReady();
    
    // check if channel and matrix dimensions match (how to handle case when they don't?)
    jassert(decoder.channelAndMatrixDimensionsMatch());
}

void USATAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool USATAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void USATAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    decoder.process(buffer);
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
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
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
