#include "SettingsTab.h"
#include "PluginProcessor.h"
#include "GlobalSettings.h"

SettingsTab::SettingsTab(PluginProcessor& p)
    : processor(p)
{
    pathLabel.setText("Preset path:", juce::dontSendNotification);
    addAndMakeVisible(pathLabel);

    pathDisplay.setReadOnly(true);
    addAndMakeVisible(pathDisplay);

    browseButton.setButtonText("Browse...");
    browseButton.onClick = [this] { browseForPresetPath(); };
    addAndMakeVisible(browseButton);
    browseButton.setWantsKeyboardFocus(true);

    themeLabel.setText("Color Scheme:", juce::dontSendNotification);
    addAndMakeVisible(themeLabel);

    themeSelector.addItem("Midnight", 1);
    themeSelector.addItem("Light", 2);
    themeSelector.onChange = [this] {
        if (onThemeChanged)
            onThemeChanged(themeSelector.getSelectedId());
        };
    themeSelector.setSelectedId(1); // По умолчанию Midnight
    addAndMakeVisible(themeSelector);


    updatePathDisplay();
}

SettingsTab::~SettingsTab()
{
    fileChooser.reset();
}

void SettingsTab::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    auto labelArea = bounds.removeFromTop(25);
    pathLabel.setBounds(labelArea.removeFromLeft(100));
    browseButton.setBounds(labelArea.removeFromRight(80));

    bounds.removeFromTop(5);
    pathDisplay.setBounds(bounds.removeFromTop(25));

    bounds.removeFromTop(20);
    themeLabel.setBounds(bounds.removeFromTop(25).removeFromLeft(150));
    themeSelector.setSelectedId(GlobalSettings::getInstance().getThemeId());
    themeSelector.setBounds(bounds.removeFromTop(25).removeFromLeft(150));
    themeSelector.onChange = [this] {
        int selectedId = themeSelector.getSelectedId();
        GlobalSettings::getInstance().setThemeId(selectedId);
        if (onThemeChanged)
            onThemeChanged(selectedId);
        };

}

void SettingsTab::updatePathDisplay()
{
    pathDisplay.setText(GlobalSettings::getInstance().getPresetPathForCurrentOS(),
        juce::dontSendNotification);
}

void SettingsTab::browseForPresetPath()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select Presets Folder",
        juce::File(GlobalSettings::getInstance().getPresetPathForCurrentOS()),
        "*"
    );

    auto* chooserPtr = fileChooser.get();

    chooserPtr->launchAsync(juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectDirectories,
        [this, chooserPtr](const juce::FileChooser& chooser)
        {
            auto result = chooser.getResult();
            if (result.exists())
            {
                GlobalSettings::getInstance().setPresetPathForCurrentOS(result.getFullPathName());
                updatePathDisplay();
            }
            fileChooser.reset();
        });
}