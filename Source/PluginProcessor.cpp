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
                       .withInput ("Input",  juce::AudioChannelSet::ambisonic(5), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::ambisonic(5), true)
                     #endif
                       ),
#endif
stateManager(userParameters),
userParameters(*this, nullptr, juce::Identifier("USAT Designer"),
#include "ParameterDefinitions.h"
                        )
{
    juce::MessageManager::callAsync([this](){
        decoder = std::make_unique<USAT>(progressValue,
                                         statusValue,
                                         stateManager,
                                         gainMatrix);
        decoder->currentSamplesPerBlock = currentNumSamples;
    });
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


//==============================================================================
void USATAudioProcessor::decode()
{
    juce::MessageManager::callAsync([this](){
        stateManager.signalPlots.setValue(false);
    });
    
    decoder->matrixReadyAtomic.store(false);
    std::string usatParameters = stateManager.createParameterValueTree().toXmlString().toStdString();
    if (decoder) {
        decoder->computeMatrix(usatParameters, [this]() {
            
            const auto matrix           = decoder->getGainMatrixInstance();
            const auto base64Plots      = decoder->getBase64Plots();
            
            DBG("Updating gain matrix");
            stateManager.gainMatrixTree = stateManager.createGainMatrixTree(matrix).createCopy();
            stateManager.updateGainMatrixCoefficients(stateManager.gainMatrixTree);
            
            auto plotsTree              = stateManager.createPlotTree(base64Plots);
            stateManager.plotsTree      = plotsTree.createCopy();
            stateManager.updatePlots(stateManager.plotsTree);
            
        });
    }
}

void USATAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentNumSamples = samplesPerBlock;
    
    if (decoder) {
        
        auto gainMatrixTree     = stateManager.createGainMatrixTree(decoder->getGainMatrixInstance());
        auto LFEChannelIndices  = gainMatrixTree.getChildWithName(ProcessingConstants::TreeTags::LFEChannelIndices);
        int LFEInputChannelIdx  = LFEChannelIndices.getProperty(ProcessingConstants::GainMatrixTree::LFEIndices::inputLFEChannelIndex);
        int LFEOutputChannelIdx = LFEChannelIndices.getProperty(ProcessingConstants::GainMatrixTree::LFEIndices::outputLFEChannelIndex);
        //DBG(getTotalNumInputChannels());
        //DBG(getTotalNumOutputChannels());
        decoder->prepare(sampleRate,
                        samplesPerBlock,
                        LFEInputChannelIdx,
                        LFEOutputChannelIdx);
        
    }
}

void USATAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool USATAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{        
    #if ! JucePlugin_IsSynth
    if (layouts.getMainInputChannelSet().isDisabled())
        return false;
    #endif

    return true;
}
#endif




void USATAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    juce::ScopedNoDenormals noDenormals;
    if (decoder) {
        decoder->process(buffer,
                         buffer.getNumChannels(),       // Input channels in buffer
                         getTotalNumOutputChannels()    // Total Number of output channels;
                         );
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
    const juce::ValueTree usatParams = stateManager.createParameterValueTree();
    mainState.addChild(usatParams, -1, nullptr);
    
    if (decoder) {
        const juce::ValueTree gainMatrixTree = stateManager.createGainMatrixTree(gainMatrix);
        if (gainMatrixTree.getNumChildren() > 0) {
            mainState.addChild(gainMatrixTree, -1, nullptr);
        }
    }
    
    const juce::ValueTree plotTree = stateManager.plotsTree.createCopy();
    mainState.addChild(plotTree, -1, nullptr);
    
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
            
            // Speaker In
            auto speakersInput  = params.getChildWithName(ProcessingConstants::TreeTags::inputSpeakerLayoutID);
            if (speakersInput.isValid())
                stateManager.inputSpeakerManager.recoverStateFromValueTree(speakersInput);
            else
                stateManager.inputSpeakerManager.initEmptySpeakerTree();
            
            // Speaker Out
            auto speakersOutput = params.getChildWithName(ProcessingConstants::TreeTags::outputSpeakerLayoutID);
            if (speakersOutput.isValid())
                stateManager.outputSpeakerManager.recoverStateFromValueTree(speakersOutput);
            else
                stateManager.outputSpeakerManager.initEmptySpeakerTree();
            
            // Coefficients
            auto coefficients   = params.getChildWithName(ProcessingConstants::TreeTags::coefficientsID);
            if (coefficients.isValid())
                stateManager.updateCoefficients(coefficients);
            else
                stateManager.initCoefficientsTree();
            
            // Plots
            auto plots = mainState.getChildWithName(ProcessingConstants::TreeTags::allPlotsID);
            if (plots.isValid())
                stateManager.updatePlots(plots);
            else
                stateManager.initPlotsTree();
            
            // Gains
            auto gainMatrix = mainState.getChildWithName(ProcessingConstants::TreeTags::gainMatrixID);
            if (gainMatrix.isValid() && gainMatrix.getNumChildren() > 0) {
                if (decoder)
                    decoder->matrixReadyAtomic.store(false);
                stateManager.updateGainMatrixCoefficients(gainMatrix);
            }
            else
                stateManager.initGainMatrixTree();
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new USATAudioProcessor();
}
