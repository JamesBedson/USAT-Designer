/*
  ==============================================================================

    MatrixComputeWindow.h
    Created: 14 Apr 2025 8:19:25pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once
#include "ProcessingConstants.h"
#include "CustomLNF.h"

class BlockingPopup : public juce::Component, public juce::Value::Listener {
  
public:
    BlockingPopup(USATAudioProcessor* p)
    : progressBar(progressValue),
    audioProcessor(p),
    background(juce::ImageCache::getFromMemory(BinaryData::background3x_png, BinaryData::background3x_pngSize))
    {
        audioProcessor->progressValue.addListener(this);
        audioProcessor->statusValue.addListener(this);
    
        addAndMakeVisible(messageLabel);
        messageLabel.setLookAndFeel(&lookAndFeel);
        messageLabel.setJustificationType(juce::Justification::centred);
    
        addAndMakeVisible(progressBar);
        progressBar.setStyle(juce::ProgressBar::Style::linear);
    }
    
    ~BlockingPopup() {
        audioProcessor->progressValue.removeListener(this);
        audioProcessor->statusValue.removeListener(this);
        messageLabel.setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override {
        g.drawImage(background, getLocalBounds().toFloat());
    }
    
    void resized() override
    {
        const float
        windowWidth         = getWidth(),
        windowHeight        = getHeight(),
        widthPadding        = windowWidth * 0.05f;
        
        const float
        progressBarWidth    = windowWidth * 0.8f,
        progressBarHeight   = windowHeight * 0.2f,
        progressBarCentreX  = getLocalBounds().getCentreX(),
        progressBarCentreY  = windowHeight * 0.75f;
        
        progressBar.setSize(progressBarWidth, progressBarHeight);
        progressBar.setCentrePosition(progressBarCentreX, progressBarCentreY);
        
        const float
        messageLabelWidth   = windowWidth * 0.8f,
        messageLabelHeight  = windowHeight * 0.2f,
        messageOffset       = windowHeight * 0.2f,
        messageLabelCentreX = getLocalBounds().getCentreX(),
        messageLabelCentreY = getLocalBounds().getCentreY() - messageOffset;
        
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
    juce::Label         messageLabel;
    juce::ProgressBar   progressBar;
    USATAudioProcessor* audioProcessor;
    juce::Image         background;
    CustomLNF           lookAndFeel;
};

class MatrixComputeWindow : public juce::DialogWindow,
juce::Value::Listener
{

public:
    MatrixComputeWindow(USATAudioProcessor& p) :
    DialogWindow("Please Wait", juce::Colours::lightgrey, true),
    processor(p)
    {
        setContentOwned(new BlockingPopup(&p), true);
        setResizable(false, false);
        processor.processCompleted.addListener(this);
    }
    
    void valueChanged(juce::Value& value) override {
        
        if (value.refersToSameSourceAs(processor.processCompleted)) {
            if (value == true) exitModalState();
        }
    }
    
private:
    
    USATAudioProcessor& processor;
    
};
