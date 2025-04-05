/*
  ==============================================================================

    TranscodingHandler.cpp
    Created: 26 Dec 2024 5:36:18pm
    Author:  James Bedson

  ==============================================================================
*/

#include "TranscodingConfigHandler.h"

TranscodingConfigHandler::TranscodingConfigHandler()
    : speakerManagerInput(ProcessingConstants::SpeakerProperties::inputTreeType),
    speakerManagerOutput(ProcessingConstants::SpeakerProperties::outputTreeType)
{
    speakerManagerInput.generateSpeakerTree();
    speakerManagerOutput.generateSpeakerTree();
}

void TranscodingConfigHandler::initialiseConfigTree()
{
    configTree.removeAllChildren(nullptr);
    
    // Input
    configTree.setProperty(ProcessingConstants::EncodingOptions::inputType,
                           ProcessingConstants::EncodingOptions::ambisonics,
                           nullptr);
    // Output
    configTree.setProperty(ProcessingConstants::EncodingOptions::outputType,
                           ProcessingConstants::EncodingOptions::speakerLayout,
                           nullptr);
}

void TranscodingConfigHandler::modifyConfigTree(juce::String encodingOption,
                                                juce::String formatOption)
{
    configTree.setProperty(encodingOption, formatOption, nullptr);
}

juce::ValueTree& TranscodingConfigHandler::getConfigTree()
{
    return configTree;
}
