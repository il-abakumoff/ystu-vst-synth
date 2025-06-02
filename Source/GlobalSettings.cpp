#include "GlobalSettings.h"

GlobalSettings& GlobalSettings::getInstance()
{
    static GlobalSettings instance;
    return instance;
}

GlobalSettings::GlobalSettings()
{
    loadSettings();
}

juce::File GlobalSettings::getSettingsDirectory() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("YSTU-Synth");
}

juce::File GlobalSettings::getSettingsFile() const
{
    return getSettingsDirectory().getChildFile("settings.xml");
}

bool GlobalSettings::ensureSettingsDirectoryExists() const
{
    auto dir = getSettingsDirectory();
    if (!dir.exists())
    {
        return dir.createDirectory();
    }
    return true;
}

void GlobalSettings::loadSettings()
{
    const juce::ScopedLock sl(lock);

    if (!ensureSettingsDirectoryExists())
    {
        DBG("Failed to create settings directory!");
        return;
    }

    auto file = getSettingsFile();
    if (!file.existsAsFile()) return;

    std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument(file).getDocumentElement());
    if (xml == nullptr || !xml->hasTagName("SETTINGS")) return;

    // Загрузка путей для всех ОС
    if (auto* winPath = xml->getChildByName("winPresetPath"))
        winPresetPath = winPath->getStringAttribute("path");

    if (auto* macPath = xml->getChildByName("MacOsPresetPath"))
        macPresetPath = macPath->getStringAttribute("path");

    if (auto* linuxPath = xml->getChildByName("LinuxPresetPath"))
        linuxPresetPath = linuxPath->getStringAttribute("path");

    if (auto* theme = xml->getChildByName("Theme"))
        themeId = theme->getIntAttribute("id", 1); // 1 — Midnight по умолчанию
}

void GlobalSettings::saveSettings()
{
    const juce::ScopedLock sl(lock);

    if (!ensureSettingsDirectoryExists()) return;

    auto file = getSettingsFile();
    auto tempFile = file.getSiblingFile("settings.tmp");

    std::unique_ptr<juce::XmlElement> xml(new juce::XmlElement("SETTINGS"));

    // Сохраняем пути для всех ОС
    auto* winElement = xml->createNewChildElement("winPresetPath");
    winElement->setAttribute("path", winPresetPath);

    auto* macElement = xml->createNewChildElement("MacOsPresetPath");
    macElement->setAttribute("path", macPresetPath);

    auto* linuxElement = xml->createNewChildElement("LinuxPresetPath");
    linuxElement->setAttribute("path", linuxPresetPath);

    auto* themeElement = xml->createNewChildElement("Theme");
    themeElement->setAttribute("id", themeId);

    if (!xml->writeTo(tempFile)) return;
    tempFile.moveFileTo(file);
}

juce::String GlobalSettings::getPresetPathForCurrentOS() const
{
    const juce::ScopedLock sl(lock);

#if JUCE_WINDOWS
    return winPresetPath;
#elif JUCE_MAC
    return macPresetPath;
#else
    return linuxPresetPath;
#endif
}

void GlobalSettings::setPresetPathForCurrentOS(const juce::String& path)
{
    const juce::ScopedLock sl(lock);

#if JUCE_WINDOWS
    winPresetPath = path;
#elif JUCE_MAC
    macPresetPath = path;
#else
    linuxPresetPath = path;
#endif

    saveSettings();
}

void GlobalSettings::setThemeId(int id)
{
    const juce::ScopedLock sl(lock);
    themeId = id;
    saveSettings();
}

int GlobalSettings::getThemeId() const
{
    const juce::ScopedLock sl(lock);
    return themeId;
}

juce::File GlobalSettings::getWavetableDirectory() const
{
    auto waveFolder = getSettingsDirectory().getChildFile("WaveTables");
    waveFolder.createDirectory(); // создает, если не существует
    return waveFolder;
}