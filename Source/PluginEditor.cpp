#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include "CustomLookAndFeel.h"

NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor(NewProjectAudioProcessor& p)
    : AudioProcessorEditor(&p),
    audioProcessor(p),
    tabs(juce::TabbedButtonBar::TabsAtTop),
    customLookAndFeel(std::make_unique<CustomLookAndFeel>())
{
    setLookAndFeel(customLookAndFeel.get());

    mainPanel = std::make_unique<MainPanel>();
    matrixPanel = std::make_unique<MatrixPanel>();

    setupMainPanel();
    setupMatrixPanel();

    tabs.addTab("Main", juce::Colours::lightgrey, mainPanel.get(), false);
    tabs.addTab("Matrix", juce::Colours::lightgrey, matrixPanel.get(), false);

    addAndMakeVisible(tabs);

    setSize(800, 600);
}

void NewProjectAudioProcessorEditor::setupMainPanel()
{
    // OSC 1
    {
        
        mainPanel->addAndMakeVisible(mainPanel->osc1WaveSelector);
        mainPanel->osc1WaveSelector.addItem("Sine", 1);
        mainPanel->osc1WaveSelector.addItem("Saw", 2);
        mainPanel->osc1WaveSelector.addItem("Square", 3);
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
        mainPanel->addAndMakeVisible(mainPanel->osc2WaveSelector);
        mainPanel->osc2WaveSelector.addItem("Sine", 1);
        mainPanel->osc2WaveSelector.addItem("Saw", 2);
        mainPanel->osc2WaveSelector.addItem("Square", 3);
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
}

void NewProjectAudioProcessorEditor::setupMatrixPanel()
{
    constexpr int numRows = 4;
    const int rowHeight = 30;
    const int comboBoxWidth = 150;
    const int sliderWidth = 200;

    for (int i = 0; i < numRows; ++i)
    {
        MatrixPanel::ModulationRow row;

        // Источник модуляции
        row.sourceCombo = std::make_unique<juce::ComboBox>();
        matrixPanel->addAndMakeVisible(row.sourceCombo.get());
        row.sourceCombo->addItem("None", 1);
        row.sourceCombo->addItem("Env 1", 2);
        row.sourceCombo->addItem("Env 2", 3);
        row.sourceCombo->setSelectedId(1);

        // Цель модуляции
        row.targetCombo = std::make_unique<juce::ComboBox>();
        matrixPanel->addAndMakeVisible(row.targetCombo.get());
        row.targetCombo->addItem("None", 1);
        row.targetCombo->addItem("Filter Cutoff", 2);
        row.targetCombo->addItem("Osc1 Pitch", 3);
        row.targetCombo->addItem("Osc2 Pitch", 4);
        row.targetCombo->setSelectedId(1);

        // Глубина модуляции
        row.amountSlider = std::make_unique<juce::Slider>();
        matrixPanel->addAndMakeVisible(row.amountSlider.get());
        row.amountSlider->setSliderStyle(juce::Slider::LinearHorizontal);
        row.amountSlider->setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        row.amountSlider->setRange(-1.0, 1.0, 0.01);
        row.amountSlider->setValue(0.0);

        // Подписи
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

void NewProjectAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Рисуем заголовки в зависимости от активной вкладки
    if (tabs.getCurrentTabIndex() == 0)
    {
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f));
        g.drawText("OSCILLATORS", mainPanel->getLocalBounds().removeFromTop(30),
            juce::Justification::centred, false);

        auto bottomArea = mainPanel->getLocalBounds().removeFromBottom(160);
        g.drawText("FILTER", bottomArea.removeFromLeft(bottomArea.getWidth() / 2).removeFromTop(20),
            juce::Justification::centred, false);
        g.drawText("ADSR ENVELOPES", bottomArea.removeFromTop(20),
            juce::Justification::centred, false);
    }
    else if (tabs.getCurrentTabIndex() == 1)
    {
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(16.0f));
        g.drawText("MODULATION MATRIX", matrixPanel->getLocalBounds().removeFromTop(30),
            juce::Justification::centred, false);
    }
}

void NewProjectAudioProcessorEditor::resized()
{
    tabs.setBounds(getLocalBounds());
}

void NewProjectAudioProcessorEditor::MainPanel::resized()
{
    auto area = getLocalBounds();
    const int margin = 10; // Общий отступ
    const int sliderHeight = 80; // Высота слайдеров
    const int labelHeight = 20; // Высота лейблов

    // 1. Осцилляторы (верхняя часть)
    auto oscArea = area.removeFromTop(200);
    {
        // Осциллятор 1
        auto osc1Area = oscArea.removeFromLeft(getWidth() / 2).reduced(margin);
        osc1WaveSelector.setBounds(osc1Area.removeFromTop(30));
        osc1Area.removeFromTop(margin); // Добавляем отступ между выбором волны и слайдерами

        auto osc1SliderArea = osc1Area.removeFromTop(sliderHeight);
        osc1VolumeSlider.setBounds(osc1SliderArea.removeFromLeft(osc1SliderArea.getWidth() / 3));
        osc1PitchSlider.setBounds(osc1SliderArea.removeFromLeft(osc1SliderArea.getWidth() / 2));
        osc1FineSlider.setBounds(osc1SliderArea);

        // Смещаем лейблы вниз от слайдеров
        osc1VolumeLabel.setBounds(osc1VolumeSlider.getX(), osc1VolumeSlider.getBottom() + 5,
            osc1VolumeSlider.getWidth(), labelHeight);
        osc1PitchLabel.setBounds(osc1PitchSlider.getX(), osc1PitchSlider.getBottom() + 5,
            osc1PitchSlider.getWidth(), labelHeight);
        osc1FineLabel.setBounds(osc1FineSlider.getX(), osc1FineSlider.getBottom() + 5,
            osc1FineSlider.getWidth(), labelHeight);

        // Осциллятор 2
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

    // 2. Фильтр и ADSR (нижняя часть)
    auto bottomArea = area.reduced(margin);
    {
        // Левая часть - фильтр
        auto filterArea = bottomArea.removeFromLeft(bottomArea.getWidth() / 2);
        filterTypeSelector.setBounds(filterArea.removeFromTop(30));

        auto filterSliderArea = filterArea.removeFromTop(120);
        auto filterCutoffArea = filterSliderArea.removeFromLeft(filterSliderArea.getWidth() / 2);
        auto filterResonanceArea = filterSliderArea;

        filterCutoffSlider.setBounds(filterCutoffArea.removeFromTop(150));
        filterCutoffLabel.setBounds(filterCutoffArea.removeFromTop(labelHeight));

        filterResonanceSlider.setBounds(filterResonanceArea.removeFromTop(150));
        filterResonanceLabel.setBounds(filterResonanceArea.removeFromTop(labelHeight));

        // Правая часть - ADSR
        auto adsrArea = bottomArea;
        auto adsrHeight = adsrArea.getHeight() / 2;

        // Основной ADSR
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

        // ADSR модуляции
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

void NewProjectAudioProcessorEditor::MatrixPanel::resized()
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

        // Источник
        row.sourceLabel->setBounds(rowArea.removeFromLeft(labelWidth));
        row.sourceCombo->setBounds(rowArea.removeFromLeft(comboWidth));
        rowArea.removeFromLeft(margin);

        // Цель
        row.targetLabel->setBounds(rowArea.removeFromLeft(labelWidth));
        row.targetCombo->setBounds(rowArea.removeFromLeft(comboWidth));
        rowArea.removeFromLeft(margin);

        // Глубина
        row.amountLabel->setBounds(rowArea.removeFromLeft(labelWidth));
        row.amountSlider->setBounds(rowArea);

        area.removeFromTop(5); // Отступ между строками
    }
}


NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}