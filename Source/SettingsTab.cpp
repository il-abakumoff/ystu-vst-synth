#include "SettingsTab.h"
#include "PluginProcessor.h"
#include "GlobalSettings.h"

SettingsTab::SettingsTab(NewProjectAudioProcessor& p)
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

    updatePathDisplay();
}

SettingsTab::~SettingsTab()
{
    fileChooser.reset(); // Важно для корректного удаления
}

void SettingsTab::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    auto labelArea = bounds.removeFromTop(25);
    pathLabel.setBounds(labelArea.removeFromLeft(100));
    browseButton.setBounds(labelArea.removeFromRight(80));

    bounds.removeFromTop(5);
    pathDisplay.setBounds(bounds.removeFromTop(25));
}

void SettingsTab::updatePathDisplay()
{
    pathDisplay.setText(GlobalSettings::getInstance().getPresetPathForCurrentOS(),
        juce::dontSendNotification);
}

void SettingsTab::browseForPresetPath()
{
    // 1. Сохраняем unique_ptr как член класса
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select Presets Folder",
        juce::File(GlobalSettings::getInstance().getPresetPathForCurrentOS()),
        "*"
    );

    // 2. Захватываем raw pointer для колбэка
    auto* chooserPtr = fileChooser.get();

    // 3. Запускаем диалог
    chooserPtr->launchAsync(juce::FileBrowserComponent::openMode |
        juce::FileBrowserComponent::canSelectDirectories,
        [this, chooserPtr](const juce::FileChooser& chooser)
        {
            // 4. Обработка результата
            auto result = chooser.getResult();
            if (result.exists())
            {
                GlobalSettings::getInstance().setPresetPathForCurrentOS(result.getFullPathName());
                updatePathDisplay();
            }

            // 5. Явно освобождаем ресурсы
            fileChooser.reset();
        });
}