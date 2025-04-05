/*
  ==============================================================================

    SpeakerManager.h
    Created: 1 Dec 2024 7:53:13pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ProcessingConstants.h"

class Speaker {

public:
    
    enum SphericalCoordinates{
        Azimuth     = 0,
        Elevation   = 1,
        Distance    = 2
    };
    
    Speaker(const float& azimuth,
            const float& elevation,
            const float& distance = 1.f);
    
    ~Speaker();
    
    void changeSpeakerCoordinates(const SphericalCoordinates& coordinate, const float& value);
    const float getCoordinate(const SphericalCoordinates& coordinate) const;
    static bool isValidCoordinate(const SphericalCoordinates& coordinate, const float& value);
    
    void printCoordinates() {
        for (int i = 0; i < 3; i++) {
            DBG("Coordinate " << i + 1 << ": " << coordinates[i]);
        }
    }
    
private:
    
    std::array<float, 3>
        coordinates;
};

class SpeakerManager {
    
public:
    SpeakerManager(const juce::String treeType);
    ~SpeakerManager();
    
    void generateSpeakerTree();
    const juce::ValueTree& getSpeakerTree() const;
    
    const std::vector<int> getVectorCurrentIDs() const;
    const Speaker* getSpeaker(int speakerID) const;
    
    void addSpeaker(std::unique_ptr<Speaker> newSpeaker, int speakerID);
    void replaceSpeaker(std::unique_ptr<Speaker> newSpeaker, int speakerID);
    void removeSpeaker(int speakerID);
    void modifySpeakerProperty(int speakerID,
                               Speaker::SphericalCoordinates,
                               const float value);

    unsigned const int getSpeakerCount() const;
    
    void saveCurrentLayoutToXML(const juce::File& xmlFile);
    void loadValueTreeFromXML(const juce::File& xmlFile);
    void recoverStateFromValueTree(const juce::ValueTree& newValueTree);

    void printSpeakerMapProperties() {
        DBG("Printing all speakers in Map:");
        DBG("===============================");
        
        for (auto& pair : speakerMap) {
            DBG("Speaker ID: " << pair.first);
            DBG("====================");
            pair.second->printCoordinates();
            DBG("");
        }
    }
    
    void printSpeakerTreeProperties() {
        DBG("Printing all speakers in Tree:");
        DBG("===============================");
        
        for (int i = 0; i < speakerTree.getNumChildren(); i++) {
            
            auto child      = speakerTree.getChild(i);
            auto ident      = child.getProperty(ProcessingConstants::SpeakerProperties::ID);
            auto azimuth    = child.getProperty(ProcessingConstants::SpeakerProperties::azimuth);
            auto elevation  = child.getProperty(ProcessingConstants::SpeakerProperties::elevation);
            auto distance   = child.getProperty(ProcessingConstants::SpeakerProperties::distance);
            
            DBG("ID: " << ident.toString());
            DBG("Azimuth: " << azimuth.toString());
            DBG("Elevation: " << elevation.toString());
            DBG("Distance: " << distance.toString());
            DBG("=======================================");
        }
    }
    
private:
    
    std::map<int, std::unique_ptr<Speaker>> speakerMap;
    juce::ValueTree speakerTree;
    
};
