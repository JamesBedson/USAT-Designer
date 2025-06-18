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

const juce::File StateManager::transcodingsDirectory
{
    presetsDirectory.getChildFile(ProcessingConstants::Paths::transcodingsDir)
};


StateManager::StateManager(APVTS& apvts)
: formatSettings(apvts),
inputSpeakerManager(ProcessingConstants::TreeTags::inputSpeakerLayoutID),
outputSpeakerManager(ProcessingConstants::TreeTags::outputSpeakerLayoutID),
coefficientsTree(ProcessingConstants::TreeTags::coefficientsID)
{
    ensureDirectoryExists(presetsDirectory);
    ensureDirectoryExists(speakerLayoutDirectory);
    ensureDirectoryExists(resourceDirectory);
    ensureDirectoryExists(pythonScriptsDirectory);
    
    initCoefficientsTree();
    initGainMatrixTree();
    initPlotsTree();
    signalNewGainMatrix.setValue(false);
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

void StateManager::initPlotsTree()
{
    plotsTree = juce::ValueTree(ProcessingConstants::TreeTags::allPlotsID);
}

void StateManager::initGainMatrixTree()
{
    gainMatrixTree = juce::ValueTree(ProcessingConstants::TreeTags::gainMatrixID);
}

const juce::ValueTree StateManager::createInputAmbisonicsTree() const
{
    
    juce::ValueTree ambisonicsTree {ProcessingConstants::TreeTags::inputAmbisonicsID};
    
    int ambisonicsOrderIn = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::Ambisonics::orderIn).getValue();
    
    ambisonicsTree.setProperty(ProcessingConstants::EncodingOptions::Ambisonics::orderIn, ambisonicsOrderIn + 1, nullptr);
    
    return ambisonicsTree;
}

const juce::ValueTree StateManager::createOutputAmbisonicsTree() const
{
    
    juce::ValueTree ambisonicsTree {ProcessingConstants::TreeTags::outputAmbisonicsID};
    
    int ambisonicsOrderOut = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::Ambisonics::orderOut).getValue();
    
    ambisonicsTree.setProperty(ProcessingConstants::EncodingOptions::Ambisonics::orderOut, ambisonicsOrderOut + 1, nullptr);
    
    return ambisonicsTree;
}

const juce::ValueTree StateManager::createSettingsTree() const
{
    juce::ValueTree settingsTree {ProcessingConstants::TreeTags::settingsID};
    
    int inputType          = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::inputType).getValue();
    int outputType         = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::outputType).getValue();

    // INPUT ================================================================================================
    if (inputType == 0) { // SpeakerLayout
        settingsTree.setProperty(ProcessingConstants::EncodingOptions::inputType,
                                   ProcessingConstants::EncodingOptions::speakerLayout, nullptr);
    }
    
    else if (inputType == 1) { // Ambisonics
        settingsTree.setProperty(ProcessingConstants::EncodingOptions::inputType,
                                 ProcessingConstants::EncodingOptions::ambisonics, nullptr);
    }
    
    else
        jassertfalse;
    
    // OUTPUT ===============================================================================================
    if (outputType == 0) {
        settingsTree.setProperty(ProcessingConstants::EncodingOptions::outputType,
                                 ProcessingConstants::EncodingOptions::speakerLayout, nullptr);
    }
    
    else if (outputType == 1) {
        settingsTree.setProperty(ProcessingConstants::EncodingOptions::outputType,
                                 ProcessingConstants::EncodingOptions::ambisonics, nullptr);
    }
    
    else
        jassertfalse;
    
    return settingsTree;
}

const juce::ValueTree StateManager::createParameterValueTree() const
{
    // Create Separate ValueTree for APVTS parameters
    auto settingsTree           = createSettingsTree().createCopy();
    auto ambisonicsInTree       = createInputAmbisonicsTree().createCopy();
    auto ambisonicsOutTree      = createOutputAmbisonicsTree().createCopy();
    auto speakerLayoutInTree    = inputSpeakerManager.getSpeakerTree().createCopy();
    if (!speakerLayoutInTree.isValid())
        DBG("Speaker layout is invalid");
    auto speakerLayoutOutTree   = outputSpeakerManager.getSpeakerTree().createCopy();
    if (speakerLayoutOutTree.isValid())
        DBG("Output layout is valid in state manager");
    auto coeffTree              = coefficientsTree.createCopy();
    
    juce::ValueTree parameters {ProcessingConstants::TreeTags::stateParametersID};
    
    parameters.addChild(settingsTree, -1, nullptr);
    parameters.addChild(ambisonicsInTree, -1, nullptr);
    parameters.addChild(ambisonicsOutTree, -1, nullptr);
    parameters.addChild(speakerLayoutInTree, -1, nullptr);
    parameters.addChild(speakerLayoutOutTree, -1, nullptr);
    parameters.addChild(coeffTree, -1, nullptr);
    
    //debugValueTree(parameters);
    return parameters;
}

const juce::ValueTree StateManager::createGainMatrixTree(const GainMatrix& matrix) const
{
    
    juce::ValueTree
        globalGainMatrixTree    {ProcessingConstants::TreeTags::gainMatrixID},
        channelCountTree        {ProcessingConstants::TreeTags::channelCountsID},
        LFEIndexTree            {ProcessingConstants::TreeTags::LFEChannelIndices},
        matrixTree              {ProcessingConstants::TreeTags::matrixCoefficientsID};
    
    auto inputChannels  = matrix.getNumInputChannels();
    auto outputChannels = matrix.getNumOutputChannels();
    //jassert(inputChannels > 0 && outputChannels > 0);
    
    // Channel Counts
    channelCountTree.setProperty(ProcessingConstants::GainMatrixTree::ChannelCount::inputChannelCount, inputChannels, nullptr);
    channelCountTree.setProperty(ProcessingConstants::GainMatrixTree::ChannelCount::outputChannelCount, outputChannels, nullptr);
    
    // LFE Indices
    int indexInput  = getLFEChannelIndexInput();
    int indexOutput = getLFEChannelIndexOutput();
    
    LFEIndexTree.setProperty(ProcessingConstants::GainMatrixTree::LFEIndices::inputLFEChannelIndex, indexInput, nullptr);
    LFEIndexTree.setProperty(ProcessingConstants::GainMatrixTree::LFEIndices::outputLFEChannelIndex, indexOutput, nullptr);
    
    globalGainMatrixTree.addChild(channelCountTree, -1, nullptr);
    globalGainMatrixTree.addChild(LFEIndexTree, -1, nullptr);
    
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

const juce::ValueTree StateManager::createPlotTree(const std::array<std::string, 6> base64PlotStrings) const {
    
    juce::ValueTree energyPlotTree              {ProcessingConstants::TreeTags::energyPlotID};
    energyPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[0]), nullptr);
    
    juce::ValueTree pressurePlotTree            {ProcessingConstants::TreeTags::pressurePlotID};
    pressurePlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[1]), nullptr);
    
    juce::ValueTree radialIntensityPlotTree     {ProcessingConstants::TreeTags::radialIntensityPlotID};
    radialIntensityPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[2]), nullptr);
    
    juce::ValueTree transverseIntensityPlotTree {ProcessingConstants::TreeTags::transverseIntensityPlotID};
    transverseIntensityPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[3]), nullptr);
    
    juce::ValueTree angularErrorPlotTree        {ProcessingConstants::TreeTags::angularErrorPlotID};
    angularErrorPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[4]), nullptr);
    
    juce::ValueTree sourceWidthPlotTree         {ProcessingConstants::TreeTags::sourceWidthPlotID};
    sourceWidthPlotTree.setProperty(ProcessingConstants::TreeTags::plotData, juce::String(base64PlotStrings[5]), nullptr);
    
    juce::ValueTree allPlotsTree {ProcessingConstants::TreeTags::allPlotsID};
    allPlotsTree.addChild(energyPlotTree, -1, nullptr);
    allPlotsTree.addChild(pressurePlotTree, -1, nullptr);
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

const juce::ValueTree StateManager::createGlobalStateTree() const
{
    juce::ValueTree mainState {ProcessingConstants::TreeTags::mainStateID};
    auto parameterValueTree = createParameterValueTree();
    
    mainState.addChild(parameterValueTree, -1, nullptr);
    
    if (gainMatrixTree.isValid()) {
        mainState.addChild(gainMatrixTree.createCopy(), -1, nullptr);
    }
    
    if (plotsTree.isValid()) {
        mainState.addChild(plotsTree.createCopy(), -1, nullptr);
    }
    
    return mainState;
}

void StateManager::saveStateParametersToXML(const juce::File &xmlFile)
{
    auto globalState = createGlobalStateTree();
    debugValueTree(globalState);
    if (auto xml = globalState.createXml()) {
        if (!xmlFile.existsAsFile()) {
            auto fileRes = xmlFile.create();
            
            if (fileRes.failed()) {
                DBG("Failed to create XML file for USAT transcoding: " + fileRes.getErrorMessage());
                jassertfalse;
                return;
            }
        }
        
        if (!xml->writeTo(xmlFile)) {
            DBG("Failed to write to XML file!");
            jassertfalse;
        }
        
        else {
            DBG("Successfully saved USAT transcoding file!");
        }
    }
    
    else {
        DBG("Failed to convert value tree to XML");
        jassertfalse;
    }
}

void StateManager::updateAPVTSParameters(const juce::ValueTree &settings,
                                         const juce::ValueTree &ambisonicsInput,
                                         const juce::ValueTree &ambisonicsOutput)
{
    // INPUT ================================================================================================
    auto inputFormat    = settings.getProperty(ProcessingConstants::EncodingOptions::inputType);
    auto outputFormat   = settings.getProperty(ProcessingConstants::EncodingOptions::outputType);
    
    if (inputFormat == ProcessingConstants::EncodingOptions::speakerLayout) {
        auto apvtsInput = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::inputType);
        apvtsInput.setValue(0); // Speaker layout
    }
    
    else if (inputFormat == ProcessingConstants::EncodingOptions::ambisonics) {
        auto apvtsInput = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::inputType);
        apvtsInput.setValue(1); // Ambisonics
    }
    
    else {
        jassertfalse;
    }
    
    // OUTPUT ===============================================================================================
    if (outputFormat == ProcessingConstants::EncodingOptions::speakerLayout) {
        auto apvtsOutput = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::outputType);
        apvtsOutput.setValue(0); // Speaker layout
    }
    
    else if (outputFormat == ProcessingConstants::EncodingOptions::ambisonics) {
        auto apvtsOutput = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::outputType);
        apvtsOutput.setValue(1); // Ambisonics
    }
    
    else {
        jassertfalse;
    }
    
    // AMBISONICS ORDERS ====================================================================================
    auto ambisonicsInputOrder   = ambisonicsInput.getProperty(ProcessingConstants::EncodingOptions::Ambisonics::orderIn);
    auto inputOrderVal          = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::Ambisonics::orderIn);
    inputOrderVal.setValue(ambisonicsInputOrder);
    
    auto ambisonicsOutputOrder  = ambisonicsOutput.getProperty(ProcessingConstants::EncodingOptions::Ambisonics::orderOut);
    auto outputOrderVal         = formatSettings.getParameterAsValue(ProcessingConstants::EncodingOptions::Ambisonics::orderOut);
    outputOrderVal.setValue(outputOrderVal);
}

void StateManager::updateCoefficients(const juce::ValueTree &coefficients)
{
    if (coefficients.isValid()) {
        coefficientsTree = coefficients.createCopy();
        
        juce::MessageManager::callAsync([this]() {
            signalCoefficients.setValue(true);
        });
    }
}

void StateManager::updatePlots(const juce::ValueTree &plots)
{
    if (plots.isValid()) {
        plotsTree = plots.createCopy();
        if (plotsTree.getNumChildren() != 0) {
        
            juce::MessageManager::callAsync([this]() {
                signalPlots.setValue(true);
            });
        }
    }
    else {
        initPlotsTree();
        
        juce::MessageManager::callAsync([this]() {
            signalPlots.setValue(true);
        });
    }
}

const int StateManager::getLFEChannelIndexInput() const
{
    auto settingsTree = createSettingsTree();
    auto inputFormat = settingsTree.getProperty(ProcessingConstants::EncodingOptions::inputType);
    
    if (inputFormat == ProcessingConstants::EncodingOptions::ambisonics)
        return -1;
    
    else {
        auto inputSpeakerTree = inputSpeakerManager.getSpeakerTree();
        
        for (int i = 0; i < inputSpeakerTree.getNumChildren(); i++) {
            auto speaker    = inputSpeakerTree.getChild(i);
            float isLFE     = speaker.getProperty(ProcessingConstants::SpeakerProperties::isLFE);
            
            if (static_cast<bool>(isLFE))
                return i;
        }
        return -1;
    }
}

const int StateManager::getLFEChannelIndexOutput() const
{
    auto settingsTree = createSettingsTree();
    auto outputFormat = settingsTree.getProperty(ProcessingConstants::EncodingOptions::outputType);
    
    if (outputFormat == ProcessingConstants::EncodingOptions::ambisonics)
        return -1;
    
    else {
        auto outputSpeakerTree = outputSpeakerManager.getSpeakerTree();
        
        for (int i = 0; i < outputSpeakerTree.getNumChildren(); i++) {
            auto speaker    = outputSpeakerTree.getChild(i);
            float isLFE     = speaker.getProperty(ProcessingConstants::SpeakerProperties::isLFE);
            
            if (static_cast<bool>(isLFE))
                return i;
        }
        return -1;
    }
}
void StateManager::updateGainMatrixCoefficients(const juce::ValueTree& gainMatrix)
{
    if (gainMatrix.isValid()) {
        gainMatrixTree = gainMatrix.createCopy();
        if (gainMatrixTree.getNumChildren() != 0) {
            
            // Update the gain matrix
            juce::MessageManager::callAsync([this]() {
                signalNewGainMatrix.setValue(true);
            });
        }
    }
    
    else {
        initGainMatrixTree();
        
        juce::MessageManager::callAsync([this]() {
            signalNewGainMatrix.setValue(true);
        });
    }
}

void StateManager::loadStateParametersFromXML(const juce::File& xmlFile)
{
    auto xmlString      = xmlFile.loadFileAsString();
    auto globalState    = juce::ValueTree::fromXml(xmlString);
    debugValueTree(globalState);
    auto xml = juce::XmlDocument::parse(xmlString);
    
    if (!globalState.isValid()) {
        jassertfalse;
        return;
    }
    
    // USAT_State_Parameters
    auto params = globalState.getChildWithName(ProcessingConstants::TreeTags::stateParametersID);
    
    // Settings =================
    auto settings           = params.getChildWithName(ProcessingConstants::TreeTags::settingsID);
    auto ambisonicsInput    = params.getChildWithName(ProcessingConstants::TreeTags::inputAmbisonicsID);
    auto ambisonicsOutput   = params.getChildWithName(ProcessingConstants::TreeTags::outputAmbisonicsID);
    updateAPVTSParameters(settings, ambisonicsInput, ambisonicsOutput);
    
    // SPEAKERS =================
    auto speakersInput  = params.getChildWithName(ProcessingConstants::TreeTags::inputSpeakerLayoutID);
    inputSpeakerManager.recoverStateFromValueTree(speakersInput);
    
    auto speakersOutput = params.getChildWithName(ProcessingConstants::TreeTags::outputSpeakerLayoutID);
    outputSpeakerManager.recoverStateFromValueTree(speakersOutput);
    
    // COEFFICIENTS =============
    auto coefficients = params.getChildWithName(ProcessingConstants::TreeTags::coefficientsID);
    updateCoefficients(coefficients);
    
    // GAIN MATRIX ==============
    auto gainMatrix = globalState.getChildWithName(ProcessingConstants::TreeTags::gainMatrixID);
    updateGainMatrixCoefficients(gainMatrix);
    
    // PLOTS ====================
    auto plots = globalState.getChildWithName(ProcessingConstants::TreeTags::allPlotsID);
    updatePlots(plots);
}
