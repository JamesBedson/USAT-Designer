/*
  ==============================================================================

    SpeakerLayoutPanel.h
    Created: 12 Jan 2025 2:21:51pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StateManager.h"
#include "UIConstants.h"
#include "CustomLNF.h"
//==============================================================================
/*
*/
class SpeakerLayoutPanel  : public juce::Component,
public juce::TableListBoxModel, public juce::Button::Listener
{
public:
    
    SpeakerLayoutPanel(StateManager&, const UI::FormatType);
    ~SpeakerLayoutPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    int getNumRows() override;
    
    static const Speaker::Attributes getSpeakerAttributeFromColumn(int columnID);
    void onLFEToggled(int row, bool isOn);
    
    void paintRowBackground (juce::Graphics&,
                             int rowNumber,
                             int width,
                             int height,
                             bool rowIsSelected) override;
    
    void paintCell (juce::Graphics&,
                    int rowNumber,
                    int columnId,
                    int width,
                    int height,
                    bool rowIsSelected) override;
    
    void buttonClicked (juce::Button*) override;
    
    void updateSpeakerState(int row,
                            int columnID,
                            float value);
    
    void cellClicked (int rowNumber,
                      int columnId,
                      const juce::MouseEvent&) override;
    
    void backgroundClicked (const juce::MouseEvent&) override;
    juce::TableListBox table;
    
    juce::Component* refreshComponentForCell (int rowNumber,
                                              int columnId,
                                              bool isRowSelected,
                                              Component* existingComponentToUpdate) override;
    
private:
    
    void initTable();
    const juce::String getText(const int columnNumber,
                               const int rowNumber,
                               const Speaker::Attributes);
    
    const UI::FormatType formatType;
    
    int speakerIDSelected {0};
    int activeLFERow;

    void initialiseLFEState();
    
    juce::TextButton
    addSpeaker,
    removeSpeaker;
    
    juce::Image background;
    
    CustomLNF lookAndFeel;
    StateManager& stateManager;
    SpeakerManager* speakerManager;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpeakerLayoutPanel)
};

class EditableTextComponent : public juce::Label

{
public:
    
    EditableTextComponent(SpeakerLayoutPanel& layoutPanel)
    : owner(layoutPanel) {
        setEditable(true);
        setJustificationType(juce::Justification::centred);
        
    }
    
    void mouseDown (const juce::MouseEvent& event) override {
        owner.table.selectRowsBasedOnModifierKeys(row, event.mods, false);
    }
    
    void textWasEdited() override {
        owner.updateSpeakerState(row, columnID, getText().getFloatValue());
    }
    
    void setRowAndColumn(const int newRow, const int newColumnID)
    {
        row = newRow;
        columnID = newColumnID;
        // set text ?
    }
    
    void paint(juce::Graphics& g) override {
        
        if (!isBeingEdited()) {
            g.setColour(juce::Colours::white);
            g.setFont(UI::Fonts::getMainFontWithSize(12.f));
            g.drawFittedText(getText(),
                             getLocalBounds(),
                             juce::Justification::left,
                             1);
        }
    }
    
private:
    SpeakerLayoutPanel& owner;
    int row, columnID;
    juce::Colour textColour;
};

class EditableToggleComponent : public juce::ToggleButton
{
public:
    EditableToggleComponent(SpeakerLayoutPanel& layoutPanel)
        : owner(layoutPanel)
    {
        setClickingTogglesState(true);
        onClick = [this]() {
            owner.onLFEToggled(row, getToggleState());
            // owner.updateSpeakerState(row, 5, float(getToggleState()));
        };
    }

    void setRow(int r) { row = r; }


private:
    SpeakerLayoutPanel& owner;
    int row;
};
