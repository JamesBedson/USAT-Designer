/*
  ==============================================================================

    StateManager.h
    Created: 1 Dec 2024 4:38:44pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ProcessingConstants.h"
#include "SpeakerManager.h"
#include "TranscodingConfigHandler.h"
#include "PluginParameterHandler.h"

using APVTS = juce::AudioProcessorValueTreeState;

class StateManager {

public:
    StateManager(APVTS& apvts);
    ~StateManager();
    static const juce::File presetsDirectory;
    static const juce::File resourceDirectory;
    static const juce::File pythonScriptsDirectory;
    static const juce::File speakerLayoutDirectory;
    static const juce::File getPythonScript();
    
    void ensureDirectoryExists(const juce::File& directory)
    {
        if (!directory.exists())
        {
            auto result = directory.createDirectory();
            if (result.failed())
            {
                DBG("Could not create directory: " + result.getErrorMessage());
                jassertfalse;
            }
        }
    }
    
    const juce::ValueTree createGlobalValueTree() const;
    const juce::ValueTree createEncodingSettingsTree() const;
    const juce::ValueTree createInputAmbisonicsTree() const;
    const juce::ValueTree createOutputAmbisonicsTree() const;
    void debugValueTree(const juce::ValueTree& tree) const;
    
    APVTS&                      apvts;
    PluginParameterHandler      pluginParameterHandler;
    TranscodingConfigHandler    transcodingConfigHandler;
    
private:
    
};
