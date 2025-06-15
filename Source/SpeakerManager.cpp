/*
  ==============================================================================

    SpeakerManager.cpp
    Created: 1 Dec 2024 7:53:13pm
    Author:  James Bedson

  ==============================================================================
*/

#include "SpeakerManager.h"
// ================================================================================================================
Speaker::Speaker(const float& azimuth,
                 const float& elevation,
                 const float& distance,
                 const bool& isLFE)
{
    attributes[Attributes::Azimuth]     = azimuth;
    attributes[Attributes::Elevation]   = elevation;
    attributes[Attributes::Distance]    = distance;
    attributes[Attributes::LFE]         = float(isLFE);
}

Speaker::~Speaker()
{

}

void Speaker::changeSpeakerAttribute(const Attributes &attribute, const float &value)
{
    if (isValidAttribute(attribute, value))
        attributes[attribute] = value;
    else
        jassertfalse;
}


bool Speaker::isValidAttribute(const Attributes &attribute, const float &value)
{
    
    switch (attribute) {
        case Azimuth:
            return value >= -360 && value <= 360;
            
        case Elevation:
            return value >= 0 && value <= 90;
            
        case Distance:
            return true;
            
        case LFE:
            if (value == 0.f or value == 1.f)
                return true;
            else return false;
    }
}

const float Speaker::getAttribute(const Attributes& attribute) const
{
    return attributes[attribute];
}

const bool Speaker::getBoolAttribute() const
{
    return attributes[Attributes::LFE];
}

// ==================================================================================================================

SpeakerManager::SpeakerManager(const juce::String treeType)
: speakerTree(treeType)
{
}

SpeakerManager::~SpeakerManager()
{
    
}

void SpeakerManager::addSpeaker(std::unique_ptr<Speaker> newSpeaker, int speakerID)
{
    
    // Internal Map
    if (speakerMap.find(speakerID) != speakerMap.end())
        jassertfalse;
    
    speakerMap[speakerID] = std::move(newSpeaker);
    
    // Speaker Tree
    juce::ValueTree speakerInfo {"Speaker_" + juce::String(speakerID)};
    
    speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::ID,
                            speakerID,
                            nullptr);
    
    speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::azimuth,
                            speakerMap[speakerID]->getAttribute(Speaker::Attributes::Azimuth),
                            nullptr);

    speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::elevation,
                            speakerMap[speakerID]->getAttribute(Speaker::Attributes::Elevation),
                            nullptr);

    speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::distance,
                            speakerMap[speakerID]->getAttribute(Speaker::Attributes::Distance),
                            nullptr);

    speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::isLFE,
                            speakerMap[speakerID]->getAttribute(Speaker::Attributes::LFE),
                            nullptr);

    speakerTree.appendChild(speakerInfo, nullptr);
}

void SpeakerManager::replaceSpeaker(std::unique_ptr<Speaker> newSpeaker, int speakerID)
{
    auto it = speakerMap.find(speakerID);
    if (it == speakerMap.end())
        jassertfalse;

    it->second = std::move(newSpeaker);
}

void SpeakerManager::removeSpeaker(int speakerID)
{
    auto it = speakerMap.find(speakerID);
    if (it == speakerMap.end()) {
        jassertfalse;
        return;
    }

    speakerMap.erase(it);
    std::map<int, std::unique_ptr<Speaker>> updatedSpeakerMap;
    
    int newID = 1;
    for (auto& pair : speakerMap) {
        updatedSpeakerMap[newID] = std::move(pair.second);
        ++newID;
    }
    
    speakerMap = std::move(updatedSpeakerMap);

    for (int i = speakerTree.getNumChildren() - 1; i >= 0; --i)
    {
        auto child = speakerTree.getChild(i);
        int childID = child.getProperty(ProcessingConstants::SpeakerProperties::ID);

        if (childID == speakerID)
            speakerTree.removeChild(i,
                                    nullptr);
        
        else if (childID > speakerID)
            child.setProperty(ProcessingConstants::SpeakerProperties::ID,
                              childID - 1,
                              nullptr);
    }
}

void SpeakerManager::saveCurrentLayoutToXML(const juce::File &xmlFile)
{
    if (auto xml = speakerTree.createXml()) {
        if (!xmlFile.existsAsFile()) {
            auto fileRes = xmlFile.create();
            
            if (fileRes.failed()) {
                DBG("Failed to create XML file for speaker layout: " + fileRes.getErrorMessage());
                jassertfalse;
                return;
            }
        }
        
        if (!xml->writeTo(xmlFile)) {
            DBG("Failed to write to XML file");
            jassertfalse;
        }
        
        else {
            DBG("Successfully saved speaker layout.");
        }
    }
    
    else {
        DBG("Failed to convert value tree to XML");
        jassertfalse;
    }
}

void SpeakerManager::loadValueTreeFromXML(const juce::File& xmlFile)
{
    auto xmlString      = xmlFile.loadFileAsString();
    auto newSpeakerTree = juce::ValueTree::fromXml(xmlString);

    if (!newSpeakerTree.isValid()) {
        jassertfalse;
        return; // TODO: Handle erroneous XML file
    }

    recoverStateFromValueTree(newSpeakerTree);
    //printSpeakerMapProperties();
    //printSpeakerTreeProperties();
}

void SpeakerManager::recoverStateFromValueTree(const juce::ValueTree& newValueTree) 
{
    DBG("Recovering State from speaker value tree");
    speakerTree         = newValueTree;
    if (!speakerTree.isValid())
        DBG("Invalid when recovering state");
    auto numSpeakers    = speakerTree.getNumChildren();
    speakerMap.clear();

    for (int i = 0; i < numSpeakers; i++) {
        auto speakerInfo = speakerTree.getChild(i);
        jassert(speakerInfo.isValid());

        auto id         = static_cast<int>(speakerInfo.getProperty(ProcessingConstants::SpeakerProperties::ID));
        auto azimuth    = static_cast<float>(speakerInfo.getProperty(ProcessingConstants::SpeakerProperties::azimuth));
        auto elevation  = static_cast<float>(speakerInfo.getProperty(ProcessingConstants::SpeakerProperties::elevation));
        auto distance   = static_cast<float>(speakerInfo.getProperty(ProcessingConstants::SpeakerProperties::distance));
        auto lfe        = static_cast<float>(speakerInfo.getProperty(ProcessingConstants::SpeakerProperties::isLFE));
        
        std::unique_ptr<Speaker> newSpeaker = std::make_unique<Speaker>(azimuth, elevation, distance, lfe);
        speakerMap[id] = std::move(newSpeaker);
    }
    generateSpeakerTree();
}


const std::vector<int> SpeakerManager::getVectorCurrentIDs() const
{
    std::vector<int> speakerIDs;
    speakerIDs.reserve(speakerMap.size());

    for (const auto& pair : speakerMap) {
        speakerIDs.push_back(pair.first);
    }

    return speakerIDs;
}

const Speaker* SpeakerManager::getSpeaker(int speakerID) const
{
    auto it = speakerMap.find(speakerID);
    
    if (it != speakerMap.end()) {
        return it->second.get();
    }
    
    else {
        jassertfalse;
        return nullptr;
    }
}

void SpeakerManager::generateSpeakerTree()
{
    speakerTree.removeAllChildren(nullptr);
    auto speakerIDs = getVectorCurrentIDs();
    
    for (auto id : speakerIDs) {
        
        juce::ValueTree speakerInfo("Speaker_" + juce::String(id));
        speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::ID, id, nullptr);
        
        auto currentSpeaker = getSpeaker(id);
        
        speakerInfo.setProperty(
                                ProcessingConstants::SpeakerProperties::azimuth,
                                currentSpeaker->getAttribute(Speaker::Attributes::Azimuth), nullptr
                                );
        
        speakerInfo.setProperty(
                                ProcessingConstants::SpeakerProperties::elevation,
                                currentSpeaker->getAttribute(Speaker::Attributes::Elevation), nullptr
                                );
        
        speakerInfo.setProperty(
                                ProcessingConstants::SpeakerProperties::distance,
                                currentSpeaker->getAttribute(Speaker::Attributes::Distance), nullptr
                                );
        
        speakerInfo.setProperty(
                                ProcessingConstants::SpeakerProperties::isLFE,
                                currentSpeaker->getAttribute(Speaker::Attributes::LFE), nullptr
                                );
        
        speakerTree.appendChild(speakerInfo, nullptr);
    }
}

void SpeakerManager::modifySpeakerProperty(int speakerID,
                                           Speaker::Attributes attribute,
                                           const float value)
{
    jassert(Speaker::isValidAttribute(attribute, value));
    
    // Modify Speaker in Map
    auto it             = speakerMap.find(speakerID);
    Speaker* speaker    = it->second.get();
    jassert(speaker != nullptr);
    speaker->changeSpeakerAttribute(attribute, value);
    
    // Modify Speaker in Speaker Tree
    juce::Identifier treeID {"Speaker_" + juce::String(speakerID)};
    auto speakerInfo        = speakerTree.getChildWithName(treeID);
    jassert(speakerInfo.isValid());
    
    switch (attribute) {
        case Speaker::Attributes::Azimuth:
            speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::azimuth,
                                    value,
                                    nullptr);
            break;
            
        case Speaker::Attributes::Elevation:
            speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::elevation,
                                    value,
                                    nullptr);
            break;
            
        case Speaker::Attributes::Distance:
            speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::distance,
                                    value,
                                    nullptr);
            break;
        
        case Speaker::Attributes::LFE:
            speakerInfo.setProperty(ProcessingConstants::SpeakerProperties::isLFE,
                                    value,
                                    nullptr);
    }
}

const juce::ValueTree& SpeakerManager::getSpeakerTree() const{
    return speakerTree;
}

unsigned const int SpeakerManager::getSpeakerCount() const
{
    return static_cast<unsigned int>(speakerMap.size());
}

void SpeakerManager::initEmptySpeakerTree() {
    speakerTree = juce::ValueTree {ProcessingConstants::TreeTags::inputSpeakerLayoutID};
}
