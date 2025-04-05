/*
  ==============================================================================

    ParameterHandler.cpp
    Created: 26 Dec 2024 5:34:53pm
    Author:  James Bedson

  ==============================================================================
*/

#include "PluginParameterHandler.h"

PluginParameterHandler::PluginParameterHandler(APVTS& apvts)
: userParameters(apvts), coefficientTree(ProcessingConstants::TreeTags::coefficientsTreeType)
{
    initialiseCoefficients();
}

void PluginParameterHandler::initialiseCoefficients()
{
    coefficientTree.removeAllChildren(nullptr);
    
    for (juce::String coefficient : ProcessingConstants::Coeffs::coefficientTypes)
    {
        auto it = ProcessingConstants::Coeffs::defaultValues.find(coefficient);
        auto defaultValue = it->second;
        coefficientTree.setProperty(coefficient, defaultValue, nullptr);
    }
}

void PluginParameterHandler::modifyCoefficient(const juce::String &coefficientName,
                                               const float &coefficientValue)
{
    coefficientTree.setProperty(coefficientName,
                                coefficientValue,
                                nullptr);
}


