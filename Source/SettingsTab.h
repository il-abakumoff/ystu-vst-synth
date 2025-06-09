#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SettingsTab : public juce::Component
{
public:
    SettingsTab(PluginProcessor& p);
    ~SettingsTab() override;
    void resized() override;

    std::function<void(int)> onThemeChanged;

private:
    PluginProcessor& processor;
    juce::Label pathLabel;
    juce::TextEditor pathDisplay;
    juce::TextButton browseButton;    
    juce::TextButton testButton;

    juce::Label themeLabel;
    juce::ComboBox themeSelector;

    void updatePathDisplay();
    void browseForPresetPath();

    std::unique_ptr<juce::FileChooser> fileChooser;
};