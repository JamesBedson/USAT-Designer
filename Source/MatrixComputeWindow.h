/*
  ==============================================================================

    MatrixComputeWindow.h
    Created: 14 Apr 2025 8:19:25pm
    Author:  James Bedson

  ==============================================================================
*/

#pragma once

class BlockingPopup : public juce::Component, public juce::Value::Listener {
  
public:
    BlockingPopup(USATAudioProcessor* p)
    : progressBar(progressValue),
    audioProcessor(p)
    {
        audioProcessor->progressValue.addListener(this);
        addAndMakeVisible(progressBar);
        progressBar.setStyle(juce::ProgressBar::Style::linear);
            
        addAndMakeVisible(messageLabel);
        messageLabel.setText("Processing, please wait...", juce::dontSendNotification);
        messageLabel.setJustificationType(juce::Justification::centred);
        
        addAndMakeVisible(cancel);
        cancel.setButtonText("Cancel");
        cancel.onClick = [this]() {
            this->audioProcessor->cancelDecoding();
        };
    }
    
    ~BlockingPopup() {
        audioProcessor->progressValue.removeListener(this);
    }
    
    void resized() override
    {
        const float
        windowWidth         = getWidth(),
        windowHeight        = getHeight(),
        widthPadding        = windowWidth * 0.05f,
        totalBarButtonWidth = windowWidth - widthPadding,
        barButtonPadding    = totalBarButtonWidth * 0.05f;
        
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
        progressValue = value.getValue();
    }
    
private:
    double              progressValue {0.f};
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
