#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SettingsTab : public juce::Component
{
public:
    SettingsTab(NewProjectAudioProcessor& p);
    ~SettingsTab() override;
    void resized() override;

private:
    NewProjectAudioProcessor& processor;
    juce::Label pathLabel;
    juce::TextEditor pathDisplay;
    juce::TextButton browseButton;    
    juce::TextButton testButton;

    void updatePathDisplay();
    void browseForPresetPath();

    std::unique_ptr<juce::FileChooser> fileChooser;
};