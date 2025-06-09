#pragma once
#include <JuceHeader.h>

class GlobalSettings
{
public:
    static GlobalSettings& getInstance();

    void loadSettings();
    void saveSettings();

    juce::String getPresetPathForCurrentOS() const;
    void setPresetPathForCurrentOS(const juce::String& path);

    void setThemeId(int id);
    int getThemeId() const;

    juce::File getWavetableDirectory() const;

private:
    GlobalSettings();
    ~GlobalSettings() = default;

    juce::File getSettingsFile() const;
    juce::File getSettingsDirectory() const;
    bool ensureSettingsDirectoryExists() const;

    juce::CriticalSection lock;
    juce::String winPresetPath;
    juce::String macPresetPath;
    juce::String linuxPresetPath;

    int themeId = 1;
};