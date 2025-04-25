#pragma once
#include <JuceHeader.h>

class GlobalSettings
{
public:
    static GlobalSettings& getInstance();

    void loadSettings();
    void saveSettings();

    // Методы для работы с путями
    juce::String getPresetPathForCurrentOS() const;
    void setPresetPathForCurrentOS(const juce::String& path);

private:
    GlobalSettings();
    ~GlobalSettings() = default;

    juce::File getSettingsFile() const;
    juce::File getSettingsDirectory() const;
    bool ensureSettingsDirectoryExists() const;

    juce::CriticalSection lock;

    // Храним пути для всех ОС
    juce::String winPresetPath;
    juce::String macPresetPath;
    juce::String linuxPresetPath;
};