/*
  ==============================================================================

    ParameterParser.h
    Created: 1 Dec 2024 6:09:44pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ProcessingConstants.h"
#include "StateManager.h"


class ParameterParser {
    
public:
    ParameterParser();
    
    static const std::string extractEncodingOptions(const juce::ValueTree& configTree,
                                                    const juce::ValueTree& inputSpeakerTree,
                                                    const juce::ValueTree& outputSpeakerTree);
    
    static const std::string extractCoefficients(const juce::ValueTree& coefficientTree);
    
private:
    
    static void parseEncodingOptions(std::ostringstream& parameterData,
                              const juce::String& encodingOption);
    
    static void parseSpeakerLayout(std::ostringstream&,
                            const juce::ValueTree& speakerTree);
    
    template <typename T>
    static void formatUserParameterOption(std::ostringstream& parameterData,
                                   const juce::String& parameterName,
                                   const T& value);
    
    static void formatSpeakerLayoutOption(std::ostringstream& parameterData,
                                   const int& id,
                                   const float& azimuth,
                                   const float& elevation,
                                   const float& distance);
    
    const ProcessingConstants::PythonParameterNameMap pythonParameterMap;
};
