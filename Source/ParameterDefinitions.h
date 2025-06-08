/*
  ==============================================================================

    ParameterDefinitions.h
    Created: 20 Nov 2024 8:45:02pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

{
    // =========================================================================
    // Settings
    
    // input type
    std::make_unique<juce::AudioParameterChoice>
 (juce::ParameterID {ProcessingConstants::EncodingOptions::inputType, 1},
  ProcessingConstants::EncodingOptions::inputType,
  ProcessingConstants::EncodingOptions::encodingChoices,
  1),
    
    
    // output type
    std::make_unique<juce::AudioParameterChoice>
 (juce::ParameterID {ProcessingConstants::EncodingOptions::outputType, 1},
  ProcessingConstants::EncodingOptions::outputType,
  ProcessingConstants::EncodingOptions::encodingChoices,
  1),
    
    
    // ambisonics order input
    std::make_unique<juce::AudioParameterChoice>
 (juce::ParameterID {ProcessingConstants::EncodingOptions::Ambisonics::orderIn, 1},
  ProcessingConstants::EncodingOptions::Ambisonics::orderIn,
  ProcessingConstants::EncodingOptions::Ambisonics::orderChoices,
  0),
    
    // ambisonics order output
    std::make_unique<juce::AudioParameterChoice>
 (juce::ParameterID {ProcessingConstants::EncodingOptions::Ambisonics::orderOut, 1},
  ProcessingConstants::EncodingOptions::Ambisonics::orderOut,
  ProcessingConstants::EncodingOptions::Ambisonics::orderChoices,
  0)
    
}
