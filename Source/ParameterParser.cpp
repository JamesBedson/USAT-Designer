/*
  ==============================================================================

    ParameterParser.cpp
    Created: 1 Dec 2024 6:09:44pm
    Author:  James Bedson

  ==============================================================================
*/

#include "ParameterParser.h"

const std::string ParameterParser::extractEncodingOptions(const juce::ValueTree& configTree,
                                                          const juce::ValueTree& inputSpeakerTree,
                                                          const juce::ValueTree& outputSpeakerTree)
{
    std::ostringstream configData;
    
    // Input
    auto inputOption    = configTree.getProperty(ProcessingConstants::EncodingOptions::inputType);
    formatUserParameterOption(configData, ProcessingConstants::EncodingOptions::inputType, inputOption);
    if (inputOption == ProcessingConstants::EncodingOptions::speakerLayout) {
        parseSpeakerLayout(configData, inputSpeakerTree);
    }
    
    // Output
    auto outputOption   = configTree.getProperty(ProcessingConstants::EncodingOptions::outputType);
    formatUserParameterOption(configData, ProcessingConstants::EncodingOptions::outputType, outputOption);
    if (outputOption == ProcessingConstants::EncodingOptions::speakerLayout) {
        parseSpeakerLayout(configData, outputSpeakerTree);
    }
    
    return configData.str();
}


const std::string ParameterParser::extractCoefficients(const juce::ValueTree& coefficientTree)
{
    std::ostringstream coefficientData;
    for (int i = 0; i < coefficientTree.getNumProperties(); i++)
    {
        juce::String coefficientName    = coefficientTree.getPropertyName(i).toString();
        float coefficientValue          = coefficientTree.getProperty(coefficientName);
        
        formatUserParameterOption(coefficientData,
                                  coefficientName,
                                  coefficientValue);
    }
    
    return coefficientData.str();
}


template <typename T>
void ParameterParser::formatUserParameterOption(std::ostringstream& parameterData,
                                     const juce::String& parameterName,
                                     const T& value)
{
    parameterData << parameterName.toStdString() << "=";

    if constexpr (std::is_arithmetic_v<T>) {
        // For numeric types, use std::to_string
        parameterData << std::to_string(value);
    } else if constexpr (std::is_same_v<T, std::string>) {
        // For std::string, directly append
        parameterData << value;
    } else if constexpr (std::is_same_v<T, juce::String>) {
        // For juce::String, convert to std::string
        parameterData << value.toStdString();
    } else {
        jassertfalse;
    }

    parameterData << ";";
}

void ParameterParser::parseSpeakerLayout(std::ostringstream& configData,
                                         const juce::ValueTree& speakerTree)
{
    for (int i = 0; i < speakerTree.getNumChildren(); i++) {
        
        auto currentSpeaker = speakerTree.getChild(i);
        auto speakerID      = currentSpeaker.getProperty(ProcessingConstants::SpeakerProperties::ID);
        auto azimuth        = currentSpeaker.getProperty(ProcessingConstants::SpeakerProperties::azimuth);
        auto elevation      = currentSpeaker.getProperty(ProcessingConstants::SpeakerProperties::elevation);
        auto distance       = currentSpeaker.getProperty(ProcessingConstants::SpeakerProperties::distance);
        
        formatSpeakerLayoutOption(configData, speakerID, azimuth, elevation, distance);
    }
}

void ParameterParser::formatSpeakerLayoutOption(std::ostringstream& speakerData,
                                                const int& id,
                                                const float& azimuth,
                                                const float& elevation,
                                                const float& distance)
{
    speakerData << id << "="
                << azimuth << ","
                << elevation << ","
                << distance
                << ";";
}
