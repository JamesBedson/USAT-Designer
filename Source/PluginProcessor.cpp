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
decoder(progressValue, statusValue, processCompleted)
{
    stateManager.signalNewGainMatrix.addListener(this);
}

USATAudioProcessor::~USATAudioProcessor()
{
    //cancelDecoding();
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

void USATAudioProcessor::valueChanged(juce::Value& value)
{
    if (value.refersToSameSourceAs(stateManager.signalNewGainMatrix))
    {
        if (static_cast<bool>(stateManager.signalCoefficients.getValue()) ==  true)
        {
            decoder.fillMatrixFromValueTree(stateManager.gainMatrixTree);
            stateManager.signalNewGainMatrix = false;
        }
    }
}

//==============================================================================
void USATAudioProcessor::decode()
{
    // TODO: Do prior check as to whether the channel dimensions will match and ask user whether they want to continue if they don't
    std::string globalValueTree = stateManager.createParameterValueTree().toXmlString().toStdString();
    decoder.computeMatrix(globalValueTree, [this]() {
        
        const auto matrix           = decoder.getGainMatrixInstance();
        const auto base64Plots      = decoder.getBase64Plots();
        
        stateManager.gainMatrixTree = stateManager.createGainMatrixTree(matrix);
        stateManager.plotsTree      = stateManager.createPlotTree(base64Plots);
        stateManager.updatePlots(stateManager.plotsTree);
    });
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
    
    /*
    juce::AudioPlayHead* playHead = getPlayHead();
    if (playHead != nullptr)
    {
        playheadCurrentPosition = playHead->getPosition()->getTimeInSamples();
        if (isPlaying && playheadCurrentPosition == playheadPreviousPosition) {
            stoppedPlaying  = true;
            isPlaying       = false;
            playheadCurrentPosition     = 0;
            playheadPreviousPosition    = 0;
            
            // UI Callback
        }
        
        else {
            playheadPreviousPosition = playheadCurrentPosition;
        }
    }*/
    
    
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
    juce::ValueTree mainState {ProcessingConstants::TreeTags::mainStateID};
    const juce::ValueTree globalTree = stateManager.createParameterValueTree();
    mainState.addChild(globalTree, -1, nullptr);
    
    if (decoder.decodingMatrixReady()) {
        const juce::ValueTree gainMatrixTree = stateManager.createGainMatrixTree(decoder.getGainMatrixInstance());
        mainState.addChild(gainMatrixTree, -1, nullptr);
    }
    
    std::unique_ptr<juce::XmlElement> xml (mainState.createXml());
    if (xml)
        copyXmlToBinary(*xml, destData);
}

void USATAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState  = getXmlFromBinary(data, sizeInBytes);
    
    if (xmlState != nullptr) {
        juce::ValueTree mainState = juce::ValueTree::fromXml(*xmlState);
        
        if (mainState.isValid())
        {
            // USAT_State_Parameters
            auto params = mainState.getChildWithName(ProcessingConstants::TreeTags::stateParametersID);
    
            auto settings           = params.getChildWithName(ProcessingConstants::TreeTags::settingsID);
            auto ambisonicsInput    = params.getChildWithName(ProcessingConstants::TreeTags::inputAmbisonicsID);
            auto ambisonicsOutput   = params.getChildWithName(ProcessingConstants::TreeTags::outputAmbisonicsID);
            stateManager.updateAPVTSParameters(settings, ambisonicsInput, ambisonicsOutput);
            
            auto speakersInput  = params.getChildWithName(ProcessingConstants::TreeTags::inputSpeakerLayoutID);
            stateManager.inputSpeakerManager.recoverStateFromValueTree(speakersInput);
            
            auto speakersOutput = params.getChildWithName(ProcessingConstants::TreeTags::outputSpeakerLayoutID);
            stateManager.outputSpeakerManager.recoverStateFromValueTree(speakersOutput);
            
            auto coefficients   = params.getChildWithName(ProcessingConstants::TreeTags::coefficientsID);
            stateManager.updateCoefficients(coefficients);
            
            auto plots = mainState.getChildWithName(ProcessingConstants::TreeTags::allPlotsID);
            stateManager.updatePlots(plots);
            
            auto gainMatrix = mainState.getChildWithName(ProcessingConstants::TreeTags::gainMatrixID);
            stateManager.updateGainMatrixCoefficients(gainMatrix);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new USATAudioProcessor();
}
