/*
  ==============================================================================

    StateManager.cpp
    Created: 1 Dec 2024 4:38:44pm
    Author:  James Bedson

  ==============================================================================
*/

#include "StateManager.h"

const juce::File StateManager::presetsDirectory
{
        juce::File::getSpecialLocation(juce::File::SpecialLocationType::userDocumentsDirectory).getChildFile(ProjectInfo::companyName)
            .getChildFile(ProjectInfo::projectName)
};

const juce::File StateManager::resourceDirectory
{
#if JUCE_MAC
    juce::File::getSpecialLocation(juce::File::currentApplicationFile)
        .getParentDirectory()
        .getChildFile(ProcessingConstants::Paths::resourceDirectory)
#elif JUCE_WINDOWS
    juce::File::getSpecialLocation(juce::File::currentApplicationFile)
        .getParentDirectory()
        .getChildFile(ProcessingConstants::Paths::resourceDirectory)
#else
    juce::File::getCurrentWorkingDirectory().getChildFile(ProcessingConstants::Paths::resourceDirectory)
#endif
};

const juce::File StateManager::pythonScriptsDirectory
{
    resourceDirectory.getChildFile(ProcessingConstants::Paths::scriptsDirectory)
};

const juce::File StateManager::speakerLayoutDirectory
{
    presetsDirectory.getChildFile(ProcessingConstants::Paths::speakerLayoutDir)
};


StateManager::StateManager(APVTS& apvts)
: apvts(apvts),
inputSpeakerManager(ProcessingConstants::TreeTags::inputSpeakerLayoutID),
outputSpeakerManager(ProcessingConstants::TreeTags::outputSpeakerLayoutID),
coefficientsTree(ProcessingConstants::TreeTags::coefficientsID)
{
    ensureDirectoryExists(presetsDirectory);
    ensureDirectoryExists(speakerLayoutDirectory);
    ensureDirectoryExists(resourceDirectory);
    ensureDirectoryExists(pythonScriptsDirectory);
    
    initCoefficientsTree();
}

StateManager::~StateManager()
{}

void StateManager::ensureDirectoryExists(const juce::File &directory)
{
    if (!directory.exists()) {
        auto result = directory.createDirectory();
        if (result.failed()) {
            DBG("Could not create directory: " + result.getErrorMessage());
            jassertfalse;
        }
    }
}

void StateManager::initCoefficientsTree()
{
        
    for (juce::String coefficient : ProcessingConstants::Coeffs::coefficientTypes)
    {
        auto it = ProcessingConstants::Coeffs::defaultValues.find(coefficient);
        auto defaultValue = it->second;
        coefficientsTree.setProperty(coefficient, defaultValue, nullptr);
    }

}

const juce::ValueTree StateManager::createInputAmbisonicsTree() const
{
    
    juce::ValueTree ambisonicsTree {ProcessingConstants::TreeTags::inputAmbisonicsID};
    
    int ambisonicsOrderIn = apvts.getParameterAsValue(ProcessingConstants::EncodingOptions::Ambisonics::orderIn).getValue();
    
    ambisonicsTree.setProperty(ProcessingConstants::EncodingOptions::Ambisonics::orderIn, ambisonicsOrderIn + 1, nullptr);
    
    return ambisonicsTree;
}

const juce::ValueTree StateManager::createOutputAmbisonicsTree() const
{
    
    juce::ValueTree ambisonicsTree {ProcessingConstants::TreeTags::outputAmbisonicsID};
    
    int ambisonicsOrderOut = apvts.getParameterAsValue(ProcessingConstants::EncodingOptions::Ambisonics::orderOut).getValue();
    
    ambisonicsTree.setProperty(ProcessingConstants::EncodingOptions::Ambisonics::orderOut, ambisonicsOrderOut + 1, nullptr);
    
    return ambisonicsTree;
}

const juce::ValueTree StateManager::createEncodingSettingsTree() const
{
    juce::ValueTree encodingTree {ProcessingConstants::TreeTags::encodingSettingsID};
    
    int inputType          = apvts.getParameterAsValue(ProcessingConstants::EncodingOptions::inputType).getValue();
    int outputType         = apvts.getParameterAsValue(ProcessingConstants::EncodingOptions::outputType).getValue();

    // INPUT ================================================================================================
    if (inputType == 0) { // SpeakerLayout
        encodingTree.setProperty(ProcessingConstants::EncodingOptions::inputType,
                                   ProcessingConstants::EncodingOptions::speakerLayout, nullptr);
    }
    
    else if (inputType == 1) { // Ambisonics
        encodingTree.setProperty(ProcessingConstants::EncodingOptions::inputType,
                                 ProcessingConstants::EncodingOptions::ambisonics, nullptr);
    }
    
    else
        jassertfalse;
    
    // OUTPUT ===============================================================================================
    if (outputType == 0) {
        encodingTree.setProperty(ProcessingConstants::EncodingOptions::outputType,
                                 ProcessingConstants::EncodingOptions::speakerLayout, nullptr);
    }
    
    else if (outputType == 1) {
        encodingTree.setProperty(ProcessingConstants::EncodingOptions::outputType,
                                 ProcessingConstants::EncodingOptions::ambisonics, nullptr);
    }
    
    else
        jassertfalse;
    
    return encodingTree;
}

const juce::ValueTree StateManager::createGlobalValueTree() const
{
    // Create Separate ValueTree for APVTS parameters
    auto encodingSettingsTree   = createEncodingSettingsTree();
    auto ambisonicsInTree       = createInputAmbisonicsTree();
    auto ambisonicsOutTree      = createOutputAmbisonicsTree();
    auto speakerLayoutInTree    = inputSpeakerManager.getSpeakerTree().createCopy();
    auto speakerLayoutOutTree   = outputSpeakerManager.getSpeakerTree().createCopy();
    auto coeffTree              = coefficientsTree.createCopy();
    
    juce::ValueTree globalTree {ProcessingConstants::TreeTags::stateParametersID};
    
    globalTree.addChild(encodingSettingsTree, -1, nullptr);
    globalTree.addChild(ambisonicsInTree, -1, nullptr);
    globalTree.addChild(ambisonicsOutTree, -1, nullptr);
    globalTree.addChild(speakerLayoutInTree, -1, nullptr);
    globalTree.addChild(speakerLayoutOutTree, -1, nullptr);
    globalTree.addChild(coeffTree, -1, nullptr);
    
    return globalTree;
}

const juce::ValueTree StateManager::createGainMatrixTree(const GainMatrix& matrix) const
{
    
    juce::ValueTree
        globalGainMatrixTree    {ProcessingConstants::TreeTags::gainMatrixID},
        channelCountTree        {ProcessingConstants::TreeTags::channelCountsID},
        matrixTree              {ProcessingConstants::TreeTags::matrixCoefficientsID};
    
    auto inputChannels  = matrix.getNumInputChannels();
    auto outputChannels = matrix.getNumOutputChannels();
    jassert(inputChannels > 0 && outputChannels > 0);
    
    channelCountTree.setProperty(ProcessingConstants::GainMatrixTree::ChannelCount::inputChannelCount, inputChannels, nullptr);
    channelCountTree.setProperty(ProcessingConstants::GainMatrixTree::ChannelCount::outputChannelCount, outputChannels, nullptr);
    globalGainMatrixTree.addChild(channelCountTree, -1, nullptr);
    
    for (int chIn = 0; chIn < inputChannels; chIn++) {
        for (int chOut = 0; chOut < outputChannels; chOut++) {
            
            auto matrixValue            = matrix.get(chIn, chOut);
            juce::String coefficientID  = ProcessingConstants::GainMatrixTree::MatrixCoefficient::baseCoefficientID;
            
            coefficientID << juce::String(chIn);
            coefficientID << juce::String(chOut);
            
            matrixTree.setProperty(coefficientID, matrixValue, nullptr);
        }
    }
    
    globalGainMatrixTree.addChild(matrixTree, -1, nullptr);
    return globalGainMatrixTree;
}

const juce::ValueTree StateManager::createPlotTree(const std::array<std::string, 5> base64PlotStrings) const {
    
    juce::ValueTree energyPlotTree              {ProcessingConstants::TreeTags::energyPlotID};
    energyPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[0]), nullptr);
    
    juce::ValueTree radialIntensityPlotTree     {ProcessingConstants::TreeTags::radialIntensityPlotID};
    radialIntensityPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[1]), nullptr);
    
    juce::ValueTree transverseIntensityPlotTree {ProcessingConstants::TreeTags::transverseIntensityPlotID};
    transverseIntensityPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[2]), nullptr);
    
    juce::ValueTree angularErrorPlotTree        {ProcessingConstants::TreeTags::angularErrorPlotID};
    angularErrorPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[3]), nullptr);
    
    juce::ValueTree sourceWidthPlotTree         {ProcessingConstants::TreeTags::sourceWidthPlotID};
    sourceWidthPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[4]), nullptr);
    
    juce::ValueTree allPlotsTree {ProcessingConstants::TreeTags::allPlotsID};
    allPlotsTree.addChild(energyPlotTree, -1, nullptr);
    allPlotsTree.addChild(radialIntensityPlotTree, -1, nullptr);
    allPlotsTree.addChild(transverseIntensityPlotTree, -1, nullptr);
    allPlotsTree.addChild(angularErrorPlotTree, -1, nullptr);
    allPlotsTree.addChild(sourceWidthPlotTree, -1, nullptr);
    //debugValueTree(energyPlotTree);
    return allPlotsTree;
}

void StateManager::debugValueTree(const juce::ValueTree& tree) const {
    DBG(tree.toXmlString());
}
