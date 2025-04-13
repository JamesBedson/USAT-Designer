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

const juce::File StateManager::getPythonScript()
{
    return pythonScriptsDirectory.getChildFile(ProcessingConstants::Paths::scriptName);
}

StateManager::StateManager(APVTS& apvts)
: apvts(apvts),
pluginParameterHandler(apvts)
{
    ensureDirectoryExists(presetsDirectory);
    ensureDirectoryExists(speakerLayoutDirectory);
    ensureDirectoryExists(resourceDirectory);
    ensureDirectoryExists(pythonScriptsDirectory);
}

StateManager::~StateManager()
{
    
}

const juce::ValueTree StateManager::createInputAmbisonicsTree() const {
    
    juce::ValueTree ambisonicsTree {ProcessingConstants::TreeTags::inputAmbisonicsTreeType};
    
    int ambisonicsOrderIn = apvts.getParameterAsValue(ProcessingConstants::EncodingOptions::Ambisonics::orderIn).getValue();
    
    ambisonicsTree.setProperty(ProcessingConstants::EncodingOptions::Ambisonics::orderIn, ambisonicsOrderIn + 1, nullptr);
    
    return ambisonicsTree;
}

const juce::ValueTree StateManager::createOutputAmbisonicsTree() const {
    
    juce::ValueTree ambisonicsTree {ProcessingConstants::TreeTags::outputAmbisonicsTreeType};
    
    int ambisonicsOrderOut = apvts.getParameterAsValue(ProcessingConstants::EncodingOptions::Ambisonics::orderOut).getValue();
    
    ambisonicsTree.setProperty(ProcessingConstants::EncodingOptions::Ambisonics::orderOut, ambisonicsOrderOut + 1, nullptr);
    
    return ambisonicsTree;
}
const juce::ValueTree StateManager::createEncodingSettingsTree() const
{
    juce::ValueTree encodingTree {ProcessingConstants::TreeTags::encodingTreeType};
    
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
    auto speakerLayoutInTree    = transcodingConfigHandler.speakerManagerInput.getSpeakerTree().createCopy();
    auto speakerLayoutOutTree   = transcodingConfigHandler.speakerManagerOutput.getSpeakerTree().createCopy();
    auto coefficientsTree       = pluginParameterHandler.getCoefficientTree().createCopy();
    
    juce::ValueTree globalTree {ProcessingConstants::TreeTags::globalTreeType};
    
    globalTree.addChild(encodingSettingsTree, -1, nullptr);
    globalTree.addChild(ambisonicsInTree, -1, nullptr);
    globalTree.addChild(ambisonicsOutTree, -1, nullptr);
    globalTree.addChild(speakerLayoutInTree, -1, nullptr);
    globalTree.addChild(speakerLayoutOutTree, -1, nullptr);
    globalTree.addChild(coefficientsTree, -1, nullptr);
    
    return globalTree;
}

void StateManager::debugValueTree(const juce::ValueTree& tree) const {
    DBG(tree.toXmlString());
}
