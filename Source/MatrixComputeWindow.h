/*
  ==============================================================================

    MatrixComputeWindow.h
    Created: 14 Apr 2025 8:19:25pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include "ProcessingConstants.h"

class BlockingPopup : public juce::Component, public juce::Value::Listener {
  
public:
    BlockingPopup(USATAudioProcessor* p)
    : progressBar(progressValue),
    audioProcessor(p)
    {
        audioProcessor->progressValue.addListener(this);
        audioProcessor->statusValue.addListener(this);
        
        auto encodingSettings     = audioProcessor->stateManager.createEncodingSettingsTree();
        juce::String inputFormat("Input Format: ");
        juce::String outputFormat("Output Format: ");
        
        inputFormat << encodingSettings.getProperty(ProcessingConstants::EncodingOptions::inputType).toString();
        outputFormat << encodingSettings.getProperty(ProcessingConstants::EncodingOptions::outputType).toString();
        
        addAndMakeVisible(inputFormatSelected);
        inputFormatSelected.setText(inputFormat, juce::dontSendNotification);
        
        addAndMakeVisible(outputFormatSelected);
        outputFormatSelected.setText(outputFormat, juce::dontSendNotification);
        
        addAndMakeVisible(messageLabel);
        messageLabel.setText("Processing, please wait...", juce::dontSendNotification);
        messageLabel.setJustificationType(juce::Justification::centred);
        
        addAndMakeVisible(progressBar);
        progressBar.setStyle(juce::ProgressBar::Style::linear);
        
        addAndMakeVisible(cancel);
        cancel.setButtonText("Cancel");
        cancel.onClick = [this]() {
            this->audioProcessor->cancelDecoding();
        };
    }
    
    ~BlockingPopup() {
        audioProcessor->progressValue.removeListener(this);
        audioProcessor->statusValue.removeListener(this);
    }
    
    void resized() override
    {
        const float
        windowWidth         = getWidth(),
        windowHeight        = getHeight(),
        widthPadding        = windowWidth * 0.05f,
        totalBarButtonWidth = windowWidth - widthPadding,
        barButtonPadding    = totalBarButtonWidth * 0.05f,
        topLabelPadding     = windowHeight * 0.05f;
        
        inputFormatSelected.setBounds(widthPadding,
                                      topLabelPadding,
                                      windowWidth * 0.3f,
                                      windowHeight * 0.1f);
        
        outputFormatSelected.setBounds(inputFormatSelected.getBounds().getTopRight().getX() + barButtonPadding,
                                       topLabelPadding,
                                       windowWidth * 0.3f,
                                       windowHeight * 0.1f);
        
        const float
        progressBarWidth    = totalBarButtonWidth - totalBarButtonWidth * 0.3f,
        progressBarHeight   = windowHeight * 0.2f,
        progressBarX        = widthPadding,
        progressBarY        = windowHeight * 0.7f;
        
        progressBar.setBounds(progressBarX,
                              progressBarY,
                              progressBarWidth,
                              progressBarHeight);
        
        const float
        buttonWidth         = totalBarButtonWidth - totalBarButtonWidth * 0.8f,
        buttonHeight        = progressBarHeight,
        buttonX             = progressBar.getRight() + barButtonPadding,
        buttonY             = progressBarY;
        
        cancel.setBounds(buttonX,
                         buttonY,
                         buttonWidth,
                         buttonHeight);
        
        const float
        messageLabelWidth   = windowWidth * 0.8f,
        messageLabelHeight  = windowHeight * 0.2f,
        messageOffset       = windowHeight * 0.3f,
        messageLabelCentreX = getBounds().getCentreX(),
        messageLabelCentreY = getBounds().getCentreY() - messageOffset;
        
        messageLabel.setSize(messageLabelWidth, messageLabelHeight);
        messageLabel.setCentrePosition(messageLabelCentreX, messageLabelCentreY);
        
    }
    
    void valueChanged(juce::Value& value) override {
        
        if (value.refersToSameSourceAs(audioProcessor->progressValue)) {
            progressValue = value.getValue();
        }
        
        else if (value.refersToSameSourceAs(audioProcessor->statusValue)) {
            messageLabel.setText(value.getValue(), juce::dontSendNotification);
        }
    }
    
private:
    double              progressValue {0.f};
    std::string         statusValue;
    juce::Label         inputFormatSelected;
    juce::Label         outputFormatSelected;
    juce::Label         messageLabel;
    juce::ProgressBar   progressBar;
    USATAudioProcessor* audioProcessor;
    juce::TextButton    cancel;
    
};

class MatrixComputeWindow : public juce::DialogWindow {

public:
public:
    MatrixComputeWindow(USATAudioProcessor* p) : DialogWindow("Please Wait", juce::Colours::lightgrey, true)
    {
        setContentOwned(new BlockingPopup(p), true);
        setResizable(false, false);
    }
    
    void closeButtonPressed() override
    {

    }
    
private:
    
};
