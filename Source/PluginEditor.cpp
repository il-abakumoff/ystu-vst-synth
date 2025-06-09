#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include "CustomLookAndFeel.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <JuceHeader.h>
#include "GlobalSettings.h"
#include "SettingsTab.h"
#include "EffectsTab.h"


PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    tabs(juce::TabbedButtonBar::TabsAtTop),
    customLookAndFeel(std::make_unique<CustomLookAndFeel>()),
    settingsTab(std::make_unique<SettingsTab>(p)),
    effectsTab(std::make_unique<EffectsTab>())
{
    auto& settings = GlobalSettings::getInstance();

    setLookAndFeel(customLookAndFeel.get());

    mainPanel = std::make_unique<MainPanel>();
    matrixPanel = std::make_unique<MatrixPanel>();
    effectsTab = std::make_unique<EffectsTab>();
    effectsTab->connectEffectCallbacks([this](int index, EffectType type) {
        if (index >= 0 && index < static_cast<int>(audioProcessor.effects.size()))
        {
            audioProcessor.effects[index].type = type;
        }
        });

    effectsTab->connectParameterCallbacks([this](int block, int param, float value) {
        if (block >= 0 && block < static_cast<int>(audioProcessor.effects.size()))
        {
            auto& fx = audioProcessor.effects[block];
            auto sampleRate = audioProcessor.getSampleRate();

            switch (fx.type)
            {
            case EffectType::Distortion:
            {
                if (param == 0) fx.distortionDrive = value;
                else if (param == 1)
                {
                    fx.distortionTone = value;
                    DBG("Tone filter cutoff updated: " << value);
                    auto sampleRate = audioProcessor.getSampleRate();
                    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, value);
                    *fx.toneFilterDry.state = *coeffs;
                    *fx.toneFilterWet.state = *coeffs;

                }
                else if (param == 2) fx.distortionMix = value;
                break;
            }

            case EffectType::Chorus:
            {
                if (param == 0) fx.chorus.setRate(value);   
                else if (param == 1) fx.chorus.setDepth(value);
                else if (param == 2) fx.chorusMix = value;  
                break;
            }

            case EffectType::Phaser:
            {
                if (param == 0) fx.phaser.setRate(value);    
                else if (param == 1) fx.phaser.setDepth(value);
                else if (param == 2) fx.phaserMix = value;  
                break;
            }

            case EffectType::Reverb:
            {
                if (param == 0)
                {
                    fx.reverbParams.roomSize = value;
                }
                else if (param == 1)
                {
                    fx.reverbParams.damping = value;
                }
                else if (param == 2)
                {
                    fx.reverbMix = value;
                }

                fx.reverbParams.wetLevel = 1.0f;
                fx.reverbParams.dryLevel = 0.0f;
                fx.reverb.setParameters(fx.reverbParams);
                break;
            }

            case EffectType::Delay:
            {
                if (param == 0)
                {
                    fx.delayTimeMs = value;
                    fx.delay.setDelay(value);
                }
                else if (param == 1)
                {
                    fx.delayFeedback = value;
                }
                else if (param == 2)
                {
                    fx.delayMix = value;
                }
                break;
            }
            
            default: break;
            }
        }
        });

    setupMainPanel();
    setupMatrixPanel();

    tabs.addTab("Main", juce::Colours::transparentBlack, mainPanel.get(), false);
    tabs.addTab("Matrix", juce::Colours::transparentBlack, matrixPanel.get(), false);
    tabs.addTab("Settings", juce::Colours::transparentBlack, settingsTab.get(), false);
    tabs.addTab("Effects", juce::Colours::transparentBlack, effectsTab.get(), false);

    addAndMakeVisible(tabs);

    filterDisplay = std::make_unique<FilterResponseDisplay>(audioProcessor);
    addAndMakeVisible(filterDisplay.get());

    setSize(800, 650);

    int savedTheme = GlobalSettings::getInstance().getThemeId();
    if (savedTheme == 1)
        customLookAndFeel->setColourScheme(juce::LookAndFeel_V4::getMidnightColourScheme());
    else
        customLookAndFeel->setColourScheme(juce::LookAndFeel_V4::getLightColourScheme());

    settingsTab->onThemeChanged = [this](int themeId) {
        customLookAndFeel->setTheme(themeId);
        lookAndFeelChanged();
        repaint();
        };
}

void PluginEditor::setupMainPanel()
{
    // PRESET FIELDS
    {
        mainPanel->addAndMakeVisible(mainPanel->presetLabel);
        mainPanel->presetLabel.setText("No preset loaded", juce::dontSendNotification);
        mainPanel->presetLabel.setJustificationType(juce::Justification::centredLeft);

        mainPanel->addAndMakeVisible(mainPanel->loadButton);
        mainPanel->loadButton.onClick = [this] {
            auto presetPath = GlobalSettings::getInstance().getPresetPathForCurrentOS();
            mainPanel->fileChooser = std::make_unique<juce::FileChooser>(
                "Load Preset", juce::File(presetPath), "*.ystusp");

            mainPanel->fileChooser->launchAsync(juce::FileBrowserComponent::openMode,
                [this](const juce::FileChooser& chooser) {
                    auto file = chooser.getResult();
                    if (file.existsAsFile()) {
                        audioProcessor.loadPresetFromFile(file);
                        mainPanel->presetLabel.setText(file.getFileName(), juce::dontSendNotification);
                    }
                    mainPanel->fileChooser.reset();
                });
            };

        mainPanel->addAndMakeVisible(mainPanel->saveButton);
        mainPanel->saveButton.onClick = [this] {
            auto presetPath = GlobalSettings::getInstance().getPresetPathForCurrentOS();
            mainPanel->fileChooser = std::make_unique<juce::FileChooser>(
                "Save Preset As", juce::File(presetPath), "*.ystusp");

            mainPanel->fileChooser->launchAsync(juce::FileBrowserComponent::saveMode,
                [this](const juce::FileChooser& chooser) {
                    auto file = chooser.getResult();
                    if (file != juce::File{}) {
                        file.withFileExtension(".ystusp");
                        audioProcessor.savePresetToFile(file);
                        mainPanel->presetLabel.setText(file.getFileName(), juce::dontSendNotification);
                    }
                    mainPanel->fileChooser.reset();
                });
            };
    }

    // OSC 1
    {
        mainPanel->osc1WaveSelector.clear();

        mainPanel->addAndMakeVisible(mainPanel->osc1WaveSelector);
        int id = 1;
        for (const auto& pair : audioProcessor.wavetableList)
            mainPanel->osc1WaveSelector.addItem(pair.first, id++);

        mainPanel->osc1WaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            audioProcessor.getAPVTS(), "osc1Wave", mainPanel->osc1WaveSelector);

        mainPanel->addAndMakeVisible(mainPanel->osc1VolumeSlider);
        mainPanel->osc1VolumeSlider.setSliderStyle(juce::Slider::Rotary);
        mainPanel->osc1VolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        mainPanel->osc1VolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "osc1Volume", mainPanel->osc1VolumeSlider);

        mainPanel->addAndMakeVisible(mainPanel->osc1PitchSlider);
        mainPanel->osc1PitchSlider.setSliderStyle(juce::Slider::Rotary);
        mainPanel->osc1PitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        mainPanel->osc1PitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "osc1Pitch", mainPanel->osc1PitchSlider);

        mainPanel->addAndMakeVisible(mainPanel->osc1FineSlider);
        mainPanel->osc1FineSlider.setSliderStyle(juce::Slider::Rotary);
        mainPanel->osc1FineSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        mainPanel->osc1FineAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "osc1Fine", mainPanel->osc1FineSlider);
    }

    // OSC 2
    {
        mainPanel->osc2WaveSelector.clear();

        mainPanel->addAndMakeVisible(mainPanel->osc2WaveSelector);
        int id = 1;
        for (const auto& pair : audioProcessor.wavetableList)
            mainPanel->osc2WaveSelector.addItem(pair.first, id++);

        mainPanel->osc2WaveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            audioProcessor.getAPVTS(), "osc2Wave", mainPanel->osc2WaveSelector);

        mainPanel->addAndMakeVisible(mainPanel->osc2VolumeSlider);
        mainPanel->osc2VolumeSlider.setSliderStyle(juce::Slider::Rotary);
        mainPanel->osc2VolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        mainPanel->osc2VolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "osc2Volume", mainPanel->osc2VolumeSlider);

        mainPanel->addAndMakeVisible(mainPanel->osc2PitchSlider);
        mainPanel->osc2PitchSlider.setSliderStyle(juce::Slider::Rotary);
        mainPanel->osc2PitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        mainPanel->osc2PitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "osc2Pitch", mainPanel->osc2PitchSlider);
        
        mainPanel->addAndMakeVisible(mainPanel->osc2FineSlider);
        mainPanel->osc2FineSlider.setSliderStyle(juce::Slider::Rotary);
        mainPanel->osc2FineSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        mainPanel->osc2FineAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "osc2Fine", mainPanel->osc2FineSlider);
    }

    // OSC LABELS
    {
        mainPanel->addAndMakeVisible(mainPanel->osc1VolumeLabel);
        mainPanel->osc1VolumeLabel.setText("Volume", juce::dontSendNotification);
        mainPanel->osc1VolumeLabel.attachToComponent(&mainPanel->osc1VolumeSlider, false);
        mainPanel->osc1VolumeLabel.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->osc1PitchLabel);
        mainPanel->osc1PitchLabel.setText("Pitch", juce::dontSendNotification);
        mainPanel->osc1PitchLabel.attachToComponent(&mainPanel->osc1PitchSlider, false);
        mainPanel->osc1PitchLabel.setJustificationType(juce::Justification::centred);        
        
        mainPanel->addAndMakeVisible(mainPanel->osc1FineLabel);
        mainPanel->osc1FineLabel.setText("Fine", juce::dontSendNotification);
        mainPanel->osc1FineLabel.attachToComponent(&mainPanel->osc1FineSlider, false);
        mainPanel->osc1FineLabel.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->osc2VolumeLabel);
        mainPanel->osc2VolumeLabel.setText("Volume", juce::dontSendNotification);
        mainPanel->osc2VolumeLabel.attachToComponent(&mainPanel->osc2VolumeSlider, false);
        mainPanel->osc2VolumeLabel.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->osc2PitchLabel);
        mainPanel->osc2PitchLabel.setText("Pitch", juce::dontSendNotification);
        mainPanel->osc2PitchLabel.attachToComponent(&mainPanel->osc2PitchSlider, false);
        mainPanel->osc2PitchLabel.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->osc2FineLabel);
        mainPanel->osc2FineLabel.setText("Fine", juce::dontSendNotification);
        mainPanel->osc2FineLabel.attachToComponent(&mainPanel->osc2FineSlider, false);
        mainPanel->osc2FineLabel.setJustificationType(juce::Justification::centred);
    }

    // AMP ADSR
    {
        mainPanel->addAndMakeVisible(mainPanel->attackSlider1);
        mainPanel->attackSlider1.setSliderStyle(juce::Slider::Rotary);
        mainPanel->attackSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        mainPanel->attackAttachment1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "attack", mainPanel->attackSlider1);

        mainPanel->addAndMakeVisible(mainPanel->decaySlider1);
        mainPanel->decaySlider1.setSliderStyle(juce::Slider::Rotary);
        mainPanel->decaySlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        mainPanel->decayAttachment1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "decay", mainPanel->decaySlider1);

        mainPanel->addAndMakeVisible(mainPanel->sustainSlider1);
        mainPanel->sustainSlider1.setSliderStyle(juce::Slider::Rotary);
        mainPanel->sustainSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        mainPanel->sustainAttachment1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "sustain", mainPanel->sustainSlider1);

        mainPanel->addAndMakeVisible(mainPanel->releaseSlider1);
        mainPanel->releaseSlider1.setSliderStyle(juce::Slider::Rotary);
        mainPanel->releaseSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        mainPanel->releaseAttachment1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "release", mainPanel->releaseSlider1);
    }

    // AMP ADSR LABELS
    {
        mainPanel->addAndMakeVisible(mainPanel->attackLabel1);
        mainPanel->attackLabel1.setText("Attack", juce::dontSendNotification);
        mainPanel->attackLabel1.attachToComponent(&mainPanel->attackSlider1, false);
        mainPanel->attackLabel1.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->decayLabel1);
        mainPanel->decayLabel1.setText("Decay", juce::dontSendNotification);
        mainPanel->decayLabel1.attachToComponent(&mainPanel->decaySlider1, false);
        mainPanel->decayLabel1.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->sustainLabel1);
        mainPanel->sustainLabel1.setText("Sustain", juce::dontSendNotification);
        mainPanel->sustainLabel1.attachToComponent(&mainPanel->sustainSlider1, false);
        mainPanel->sustainLabel1.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->releaseLabel1);
        mainPanel->releaseLabel1.setText("Release", juce::dontSendNotification);
        mainPanel->releaseLabel1.attachToComponent(&mainPanel->releaseSlider1, false);
        mainPanel->releaseLabel1.setJustificationType(juce::Justification::centred);
    }

    // MOD ADSR
    {
        mainPanel->addAndMakeVisible(mainPanel->attackSlider2);
        mainPanel->attackSlider2.setSliderStyle(juce::Slider::Rotary);
        mainPanel->attackSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        mainPanel->attackAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "modAttack", mainPanel->attackSlider2);

        mainPanel->addAndMakeVisible(mainPanel->decaySlider2);
        mainPanel->decaySlider2.setSliderStyle(juce::Slider::Rotary);
        mainPanel->decaySlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        mainPanel->decayAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "modDecay", mainPanel->decaySlider2);

        mainPanel->addAndMakeVisible(mainPanel->sustainSlider2);
        mainPanel->sustainSlider2.setSliderStyle(juce::Slider::Rotary);
        mainPanel->sustainSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        mainPanel->sustainAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "modSustain", mainPanel->sustainSlider2);

        mainPanel->addAndMakeVisible(mainPanel->releaseSlider2);
        mainPanel->releaseSlider2.setSliderStyle(juce::Slider::Rotary);
        mainPanel->releaseSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        mainPanel->releaseAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "modRelease", mainPanel->releaseSlider2);
    }

    // MOD ADSR LABELS
    {
        mainPanel->addAndMakeVisible(mainPanel->attackLabel2);
        mainPanel->attackLabel2.setText("Mod Attack", juce::dontSendNotification);
        mainPanel->attackLabel2.attachToComponent(&mainPanel->attackSlider2, false);
        mainPanel->attackLabel2.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->decayLabel2);
        mainPanel->decayLabel2.setText("Mod Decay", juce::dontSendNotification);
        mainPanel->decayLabel2.attachToComponent(&mainPanel->decaySlider2, false);
        mainPanel->decayLabel2.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->sustainLabel2);
        mainPanel->sustainLabel2.setText("Mod Sustain", juce::dontSendNotification);
        mainPanel->sustainLabel2.attachToComponent(&mainPanel->sustainSlider2, false);
        mainPanel->sustainLabel2.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->releaseLabel2);
        mainPanel->releaseLabel2.setText("Mod Release", juce::dontSendNotification);
        mainPanel->releaseLabel2.attachToComponent(&mainPanel->releaseSlider2, false);
        mainPanel->releaseLabel2.setJustificationType(juce::Justification::centred);
    }

    // FILTER
    {
        mainPanel->addAndMakeVisible(mainPanel->filterTypeSelector);
        mainPanel->filterTypeSelector.addItem("Low-pass", 1);
        mainPanel->filterTypeSelector.addItem("High-pass", 2);
        mainPanel->filterTypeSelector.addItem("Band-pass", 3);
        mainPanel->filterTypeSelector.addItem("Notch", 4);
        mainPanel->filterTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            audioProcessor.getAPVTS(), "filterType", mainPanel->filterTypeSelector);

        mainPanel->addAndMakeVisible(mainPanel->filterCutoffSlider);
        mainPanel->filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
        mainPanel->filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        mainPanel->filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "filterCutoff", mainPanel->filterCutoffSlider);

        mainPanel->addAndMakeVisible(mainPanel->filterResonanceSlider);
        mainPanel->filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
        mainPanel->filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        mainPanel->filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.getAPVTS(), "filterResonance", mainPanel->filterResonanceSlider);
    }

    // FILTER LABELS
    {
        mainPanel->addAndMakeVisible(mainPanel->filterTypeLabel);
        mainPanel->filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
        mainPanel->filterTypeLabel.attachToComponent(&mainPanel->filterTypeSelector, false);
        mainPanel->filterTypeLabel.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->filterCutoffLabel);
        mainPanel->filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
        mainPanel->filterCutoffLabel.attachToComponent(&mainPanel->filterCutoffSlider, false);
        mainPanel->filterCutoffLabel.setJustificationType(juce::Justification::centred);

        mainPanel->addAndMakeVisible(mainPanel->filterResonanceLabel);
        mainPanel->filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
        mainPanel->filterResonanceLabel.attachToComponent(&mainPanel->filterResonanceSlider, false);
        mainPanel->filterResonanceLabel.setJustificationType(juce::Justification::centred);
    }

    // FILTER VISUALISATION
    {
        auto callback = [this]() { filterDisplay->shouldUpdate = true; };
        mainPanel->filterCutoffSlider.onValueChange = callback;
        mainPanel->filterResonanceSlider.onValueChange = callback;
        mainPanel->filterTypeSelector.onChange = [this] { filterDisplay->shouldUpdate = true; };
    }
}

void PluginEditor::setupMatrixPanel()
{
    constexpr int numRows = 4;
    const int rowHeight = 30;
    const int comboBoxWidth = 150;
    const int sliderWidth = 200;

    for (int i = 0; i < numRows; ++i)
    {
        MatrixPanel::ModulationRow row;

        row.sourceCombo = std::make_unique<juce::ComboBox>();
        matrixPanel->addAndMakeVisible(row.sourceCombo.get());
        row.sourceCombo->addItem("None", 1);
        row.sourceCombo->addItem("Env 1", 2);
        row.sourceCombo->addItem("Env 2", 3);
        row.sourceCombo->setSelectedId(1);

        row.targetCombo = std::make_unique<juce::ComboBox>();
        matrixPanel->addAndMakeVisible(row.targetCombo.get());
        row.targetCombo->addItem("None", 1);
        row.targetCombo->addItem("Filter Cutoff", 2);
        row.targetCombo->addItem("Osc1 Pitch", 3);
        row.targetCombo->addItem("Osc2 Pitch", 4);
        row.targetCombo->setSelectedId(1);

        row.amountSlider = std::make_unique<juce::Slider>();
        matrixPanel->addAndMakeVisible(row.amountSlider.get());
        row.amountSlider->setSliderStyle(juce::Slider::LinearHorizontal);
        row.amountSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        row.amountSlider->setRange(-1.0, 1.0, 0.01);
        row.amountSlider->setValue(0.0);

        row.sourceLabel = std::make_unique<juce::Label>();
        matrixPanel->addAndMakeVisible(row.sourceLabel.get());
        row.sourceLabel->setText("Source:", juce::dontSendNotification);
        row.sourceLabel->attachToComponent(row.sourceCombo.get(), true);

        row.targetLabel = std::make_unique<juce::Label>();
        matrixPanel->addAndMakeVisible(row.targetLabel.get());
        row.targetLabel->setText("Target:", juce::dontSendNotification);
        row.targetLabel->attachToComponent(row.targetCombo.get(), true);

        row.amountLabel = std::make_unique<juce::Label>();
        matrixPanel->addAndMakeVisible(row.amountLabel.get());
        row.amountLabel->setText("Amount:", juce::dontSendNotification);
        row.amountLabel->attachToComponent(row.amountSlider.get(), true);

        matrixPanel->modulationRows.push_back(std::move(row));
    }
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    if (tabs.getCurrentTabIndex() == 0)
    {
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f));
        g.drawText("OSCILLATORS", mainPanel->getLocalBounds().removeFromTop(30),
            juce::Justification::centred, false);

        auto bottomArea = mainPanel->getLocalBounds().removeFromBottom(160);
    }
    else if (tabs.getCurrentTabIndex() == 1)
    {
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f));
        g.drawText("MODULATION MATRIX", matrixPanel->getLocalBounds().removeFromTop(30),
            juce::Justification::centred, false);
    }
}

void PluginEditor::resized()
{
    tabs.setBounds(getLocalBounds());

    auto area = getLocalBounds();
    filterDisplay->setBounds(area.removeFromBottom(150).reduced(10));
    tabs.setBounds(area);
}

void PluginEditor::MainPanel::resized()
{
    auto area = getLocalBounds();
    const int margin = 10; 
    const int sliderHeight = 80; 
    const int labelHeight = 20;

    auto presetArea = area.removeFromBottom(40);
    auto buttonWidth = 80;
    auto labelWidth = presetArea.getWidth() - 2 * buttonWidth - 30;

    presetLabel.setBounds(presetArea.removeFromLeft(labelWidth));
    loadButton.setBounds(presetArea.removeFromLeft(buttonWidth).reduced(5));
    saveButton.setBounds(presetArea.removeFromLeft(buttonWidth).reduced(5));

    auto oscArea = area.removeFromTop(200);
    {
        auto osc1Area = oscArea.removeFromLeft(getWidth() / 2).reduced(margin);
        osc1WaveSelector.setBounds(osc1Area.removeFromTop(30));
        osc1Area.removeFromTop(margin); // Добавляем отступ между выбором волны и слайдерами

        auto osc1SliderArea = osc1Area.removeFromTop(sliderHeight);
        osc1VolumeSlider.setBounds(osc1SliderArea.removeFromLeft(osc1SliderArea.getWidth() / 3));
        osc1PitchSlider.setBounds(osc1SliderArea.removeFromLeft(osc1SliderArea.getWidth() / 2));
        osc1FineSlider.setBounds(osc1SliderArea);

        osc1VolumeLabel.setBounds(osc1VolumeSlider.getX(), osc1VolumeSlider.getBottom() + 5,
            osc1VolumeSlider.getWidth(), labelHeight);
        osc1PitchLabel.setBounds(osc1PitchSlider.getX(), osc1PitchSlider.getBottom() + 5,
            osc1PitchSlider.getWidth(), labelHeight);
        osc1FineLabel.setBounds(osc1FineSlider.getX(), osc1FineSlider.getBottom() + 5,
            osc1FineSlider.getWidth(), labelHeight);

        auto osc2Area = oscArea.reduced(margin);
        osc2WaveSelector.setBounds(osc2Area.removeFromTop(30));
        osc2Area.removeFromTop(margin); // Добавляем отступ

        auto osc2SliderArea = osc2Area.removeFromTop(sliderHeight);
        osc2VolumeSlider.setBounds(osc2SliderArea.removeFromLeft(osc2SliderArea.getWidth() / 3));
        osc2PitchSlider.setBounds(osc2SliderArea.removeFromLeft(osc2SliderArea.getWidth() / 2));
        osc2FineSlider.setBounds(osc2SliderArea);

        osc2VolumeLabel.setBounds(osc2VolumeSlider.getX(), osc2VolumeSlider.getBottom() + 5,
            osc2VolumeSlider.getWidth(), labelHeight);
        osc2PitchLabel.setBounds(osc2PitchSlider.getX(), osc2PitchSlider.getBottom() + 5,
            osc2PitchSlider.getWidth(), labelHeight);
        osc2FineLabel.setBounds(osc2FineSlider.getX(), osc2FineSlider.getBottom() + 5,
            osc2FineSlider.getWidth(), labelHeight);
    }

    auto bottomArea = area.reduced(margin);
    {
        auto filterArea = bottomArea.removeFromLeft(bottomArea.getWidth() / 2);
        filterTypeSelector.setBounds(filterArea.removeFromTop(30));

        auto filterSliderArea = filterArea.removeFromTop(120);
        auto filterCutoffArea = filterSliderArea.removeFromLeft(filterSliderArea.getWidth() / 2);
        auto filterResonanceArea = filterSliderArea;

        filterCutoffSlider.setBounds(filterCutoffArea.removeFromTop(150));
        filterCutoffLabel.setBounds(filterCutoffSlider.getX(), filterCutoffSlider.getBottom() + 5,
            filterCutoffSlider.getWidth(), labelHeight);

        filterResonanceSlider.setBounds(filterResonanceArea.removeFromTop(150));
        filterResonanceLabel.setBounds(filterResonanceSlider.getX(), filterResonanceSlider.getBottom() + 5,
            filterResonanceSlider.getWidth(), labelHeight);

        auto adsrArea = bottomArea;
        auto adsrHeight = adsrArea.getHeight() / 2;

        auto mainAdsr = adsrArea.removeFromTop(adsrHeight);
        auto mainAdsrSliderHeight = mainAdsr.getHeight() - labelHeight - margin;

        attackSlider1.setBounds(mainAdsr.removeFromLeft(mainAdsr.getWidth() / 4).removeFromTop(mainAdsrSliderHeight));
        attackLabel1.setBounds(attackSlider1.getX(), attackSlider1.getBottom() + 5,
            attackSlider1.getWidth(), labelHeight);

        decaySlider1.setBounds(mainAdsr.removeFromLeft(mainAdsr.getWidth() / 3).removeFromTop(mainAdsrSliderHeight));
        decayLabel1.setBounds(decaySlider1.getX(), decaySlider1.getBottom() + 5,
            decaySlider1.getWidth(), labelHeight);

        sustainSlider1.setBounds(mainAdsr.removeFromLeft(mainAdsr.getWidth() / 2).removeFromTop(mainAdsrSliderHeight));
        sustainLabel1.setBounds(sustainSlider1.getX(), sustainSlider1.getBottom() + 5,
            sustainSlider1.getWidth(), labelHeight);

        releaseSlider1.setBounds(mainAdsr.removeFromTop(mainAdsrSliderHeight));
        releaseLabel1.setBounds(releaseSlider1.getX(), releaseSlider1.getBottom() + 5,
            releaseSlider1.getWidth(), labelHeight);

        auto modAdsr = adsrArea;
        auto modAdsrSliderHeight = modAdsr.getHeight() - labelHeight - margin;

        attackSlider2.setBounds(modAdsr.removeFromLeft(modAdsr.getWidth() / 4).removeFromTop(modAdsrSliderHeight));
        attackLabel2.setBounds(attackSlider2.getX(), attackSlider2.getBottom() + 5,
            attackSlider2.getWidth(), labelHeight);

        decaySlider2.setBounds(modAdsr.removeFromLeft(modAdsr.getWidth() / 3).removeFromTop(modAdsrSliderHeight));
        decayLabel2.setBounds(decaySlider2.getX(), decaySlider2.getBottom() + 5,
            decaySlider2.getWidth(), labelHeight);

        sustainSlider2.setBounds(modAdsr.removeFromLeft(modAdsr.getWidth() / 2).removeFromTop(modAdsrSliderHeight));
        sustainLabel2.setBounds(sustainSlider2.getX(), sustainSlider2.getBottom() + 5,
            sustainSlider2.getWidth(), labelHeight);

        releaseSlider2.setBounds(modAdsr.removeFromTop(modAdsrSliderHeight));
        releaseLabel2.setBounds(releaseSlider2.getX(), releaseSlider2.getBottom() + 5,
            releaseSlider2.getWidth(), labelHeight);
    }
}

void PluginEditor::MatrixPanel::resized()
{
    auto area = getLocalBounds().reduced(10);
    area.removeFromTop(30); // Для заголовка

    const int rowHeight = 30;
    const int labelWidth = 80;
    const int comboWidth = 120;
    const int sliderWidth = 200;
    const int margin = 10;

    for (auto& row : modulationRows) {
        auto rowArea = area.removeFromTop(rowHeight);

        row.sourceLabel->setBounds(rowArea.removeFromLeft(labelWidth));
        row.sourceCombo->setBounds(rowArea.removeFromLeft(comboWidth));
        rowArea.removeFromLeft(margin);

        row.targetLabel->setBounds(rowArea.removeFromLeft(labelWidth));
        row.targetCombo->setBounds(rowArea.removeFromLeft(comboWidth));
        rowArea.removeFromLeft(margin);

        row.amountLabel->setBounds(rowArea.removeFromLeft(labelWidth));
        row.amountSlider->setBounds(rowArea);

        area.removeFromTop(5);
    }
}

PluginEditor::~PluginEditor()
{
    setLookAndFeel(nullptr);
}

PluginEditor::FilterResponseDisplay::FilterResponseDisplay(PluginProcessor& p)
    : processor(p)
{
    startTimerHz(30); 
    magnitudes.resize(300);
    setOpaque(false);
}

PluginEditor::FilterResponseDisplay::~FilterResponseDisplay()
{
    stopTimer();
}

void PluginEditor::FilterResponseDisplay::paint(juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(
        juce::Colours::darkgrey.withAlpha(0.3f), 0.0f, 0.0f,
        juce::Colours::black.withAlpha(0.6f), 0.0f, static_cast<float>(getHeight()), false));
    g.fillAll();

    g.setColour(juce::Colours::white.withAlpha(0.15f));

    float freqs[] = { 20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
    float logMin = std::log10(20.0f);
    float logMax = std::log10(20000.0f);
    float logRange = logMax - logMin;

    for (float freq : freqs) {
        float x = getWidth() * (std::log10(freq) - logMin) / logRange;
        g.drawVerticalLine(static_cast<int>(x), 0, getHeight());
        juce::String freqText = freq >= 1000 ? juce::String(freq / 1000.0, 1) + "k" : juce::String(freq);
        g.drawText(freqText, x - 20, getHeight() - 20, 40, 20, juce::Justification::centred);
    }

    for (int dB = -60; dB <= 24; dB += 6) {
        float y = juce::jmap(static_cast<float>(dB), -60.0f, 24.0f,
            static_cast<float>(getHeight()), 0.0f);
        g.drawHorizontalLine(static_cast<int>(y), 0, getWidth());
        g.drawText(juce::String(dB), 5, y - 10, 30, 20, juce::Justification::left);
    }

    g.setColour(juce::Colours::cyan.withAlpha(0.8f));
    g.strokePath(responsePath, juce::PathStrokeType(3.0f));
    g.setColour(juce::Colours::cyan);
    g.strokePath(responsePath, juce::PathStrokeType(1.5f));

    auto cutoff = processor.getAPVTS().getRawParameterValue("filterCutoff")->load();
    float cutoffX = getWidth() * (std::log10(cutoff) - logMin) / logRange;
    g.setColour(juce::Colours::red.withAlpha(0.7f));
    g.drawVerticalLine(static_cast<int>(cutoffX), 0, getHeight());
}

void PluginEditor::FilterResponseDisplay::timerCallback()
{
    if (shouldUpdate)
    {
        updateResponse();
        shouldUpdate = false;
        repaint();
    }
}

void PluginEditor::FilterResponseDisplay::updateResponse()
{
    constexpr double sampleRate = 44100.0;
    constexpr int numPoints = 800;
    constexpr float minFreq = 20.0f;
    constexpr float maxFreq = 20000.0f;

    auto cutoff = processor.getAPVTS().getRawParameterValue("filterCutoff")->load();
    auto resonance = processor.getAPVTS().getRawParameterValue("filterResonance")->load();
    auto type = static_cast<int>(processor.getAPVTS().getRawParameterValue("filterType")->load());

    responsePath.clear();
    const float width = static_cast<float>(getWidth());
    const float height = static_cast<float>(getHeight());
    bool firstPoint = true;

    const float logMin = std::log10(minFreq);
    const float logMax = std::log10(maxFreq);
    const float logRange = logMax - logMin;

    const float peakGain = resonance * 18.0f;
    const float peakWidthCoef = 0.7f / (resonance + 0.3f);

    for (int i = 0; i < numPoints; ++i)
    {
        const float t = i / static_cast<float>(numPoints - 1);
        const float freq = std::pow(10.0f, logMin + t * logRange);
        const float x = width * (std::log10(freq) - logMin) / logRange;

        float dB = 0.0f;

        switch (type)
        {
        case 0: // Low-pass
        {
            if (freq > cutoff)
            {
                const float octaves = std::log2(freq / cutoff);
                dB = -12.0f * octaves;
            }

            if (resonance > 0.01f)
            {
                const float distFromPeak = freq - cutoff;
                const float peakWidth = cutoff * peakWidthCoef;
                dB += peakGain * std::exp(-(distFromPeak * distFromPeak) / (2.0f * peakWidth * peakWidth));
            }
            break;
        }

        case 1: // High-pass
        {
            if (freq < cutoff)
            {
                const float octaves = std::log2(cutoff / freq);
                dB = -12.0f * octaves;
            }

            if (resonance > 0.01f)
            {
                const float distFromPeak = freq - cutoff;
                const float peakWidth = cutoff * peakWidthCoef;
                dB += peakGain * std::exp(-(distFromPeak * distFromPeak) / (2.0f * peakWidth * peakWidth));
            }
            break;
        }

        case 2: // Band-pass
        {
            const float centerFreq = cutoff;
            const float bandwidth = centerFreq * (.5f + 1.5f * (1.0f - resonance));
            const float normalizedFreq = freq / centerFreq;
            const float logNormFreq = std::log2(normalizedFreq);

            dB = -24.0f * std::abs(logNormFreq);

            if (resonance > 0.1f)
            {
                const float peakGain = 18.0f * resonance;
                const float peakWidth = 0.5f / (resonance + 0.2f);

                dB += peakGain * std::exp(-(logNormFreq * logNormFreq) /
                    (2.0f * peakWidth * peakWidth));
            }

            if (freq < centerFreq) {
                dB *= 0.9f + 0.1f * resonance;
            }
            break;
        }

        case 3: // Notch
        {
            const float centerFreq = cutoff;
            const float normalizedResonance = (resonance - 0.1f) / 1.9f;

            const float qFactor = 0.2f + 1.0f * normalizedResonance * normalizedResonance;
            const float depth = -60.0f * (1.0f - normalizedResonance);
            const float logOffset = std::log2(freq / centerFreq);
            const float bellShape = std::exp(-(logOffset * logOffset) * qFactor * 3.0f);

            dB = depth * bellShape;
            dB = juce::jlimit(-60.0f, 6.0f, dB);
            break;
        }
        }

        const float y = juce::jmap(juce::jlimit(-60.0f, 24.0f, dB), -60.0f, 24.0f, height, 0.0f);

        if (firstPoint)
        {
            responsePath.startNewSubPath(x, y);
            firstPoint = false;
        }
        else
        {
            responsePath.lineTo(x, y);
        }
    }
}