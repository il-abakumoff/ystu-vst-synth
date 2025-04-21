#include "PluginProcessor.h"
#include "PluginEditor.h"

NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Осциллятор 1
    addAndMakeVisible(osc1WaveSelector);
    osc1WaveSelector.addItem("Sine", 1);
    osc1WaveSelector.addItem("Saw", 2);
    osc1WaveSelector.addItem("Square", 3);
    osc1WaveAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        audioProcessor.getAPVTS(), "osc1Wave", osc1WaveSelector));


    addAndMakeVisible(osc1VolumeSlider);
    osc1VolumeSlider.setSliderStyle(juce::Slider::Rotary);
    osc1VolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    osc1VolumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "osc1Volume", osc1VolumeSlider));

    addAndMakeVisible(osc1PitchSlider);
    osc1PitchSlider.setSliderStyle(juce::Slider::Rotary);
    osc1PitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    osc1PitchAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "osc1Pitch", osc1PitchSlider));

    // Осциллятор 2
    addAndMakeVisible(osc2WaveSelector);
    osc2WaveSelector.addItem("Sine", 1);
    osc2WaveSelector.addItem("Saw", 2);
    osc2WaveSelector.addItem("Square", 3);
    osc2WaveAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        audioProcessor.getAPVTS(), "osc2Wave", osc2WaveSelector));

    addAndMakeVisible(osc2VolumeSlider);
    osc2VolumeSlider.setSliderStyle(juce::Slider::Rotary);
    osc2VolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    osc2VolumeAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "osc2Volume", osc2VolumeSlider));

    addAndMakeVisible(osc2PitchSlider);
    osc2PitchSlider.setSliderStyle(juce::Slider::Rotary);
    osc2PitchSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    osc2PitchAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "osc2Pitch", osc2PitchSlider));

    // Подписи
    addAndMakeVisible(osc1VolumeLabel);
    osc1VolumeLabel.setText("Volume", juce::dontSendNotification);
    osc1VolumeLabel.attachToComponent(&osc1VolumeSlider, false);
    osc1VolumeLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(osc1PitchLabel);
    osc1PitchLabel.setText("Pitch", juce::dontSendNotification);
    osc1PitchLabel.attachToComponent(&osc1PitchSlider, false);
    osc1PitchLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(osc2VolumeLabel);
    osc2VolumeLabel.setText("Volume", juce::dontSendNotification);
    osc2VolumeLabel.attachToComponent(&osc2VolumeSlider, false);
    osc2VolumeLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(osc2PitchLabel);
    osc2PitchLabel.setText("Pitch", juce::dontSendNotification);
    osc2PitchLabel.attachToComponent(&osc2PitchSlider, false);
    osc2PitchLabel.setJustificationType(juce::Justification::centred);

    // AMP ADSR
    // Attack
    addAndMakeVisible(attackSlider);
    attackSlider.setSliderStyle(juce::Slider::Rotary);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    attackAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "attack", attackSlider));

    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.attachToComponent(&attackSlider, false);
    attackLabel.setJustificationType(juce::Justification::centred);

    // Decay
    addAndMakeVisible(decaySlider);
    decaySlider.setSliderStyle(juce::Slider::Rotary);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    decayAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "decay", decaySlider));

    addAndMakeVisible(decayLabel);
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.attachToComponent(&decaySlider, false);
    decayLabel.setJustificationType(juce::Justification::centred);

    // Sustain
    addAndMakeVisible(sustainSlider);
    sustainSlider.setSliderStyle(juce::Slider::Rotary);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    sustainAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "sustain", sustainSlider));

    addAndMakeVisible(sustainLabel);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.attachToComponent(&sustainSlider, false);
    sustainLabel.setJustificationType(juce::Justification::centred);

    // Release
    addAndMakeVisible(releaseSlider);
    releaseSlider.setSliderStyle(juce::Slider::Rotary);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    releaseAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "release", releaseSlider));

    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.attachToComponent(&releaseSlider, false);
    releaseLabel.setJustificationType(juce::Justification::centred);

    // FILTER
    addAndMakeVisible(filterTypeSelector);
    filterTypeSelector.addItem("Low-pass", 1);
    filterTypeSelector.addItem("High-pass", 2);
    filterTypeAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        audioProcessor.getAPVTS(), "filterType", filterTypeSelector));

    addAndMakeVisible(filterTypeLabel);
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    filterTypeLabel.attachToComponent(&filterTypeSelector, false);
    filterTypeLabel.setJustificationType(juce::Justification::centred);

    // Filter Cutoff
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    filterCutoffAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "filterCutoff", filterCutoffSlider));

    addAndMakeVisible(filterCutoffLabel);
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    filterCutoffLabel.attachToComponent(&filterCutoffSlider, false);
    filterCutoffLabel.setJustificationType(juce::Justification::centred);

    // Filter Resonance
    addAndMakeVisible(filterResonanceSlider);
    filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    filterResonanceAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        audioProcessor.getAPVTS(), "filterResonance", filterResonanceSlider));

    addAndMakeVisible(filterResonanceLabel);
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    filterResonanceLabel.attachToComponent(&filterResonanceSlider, false);
    filterResonanceLabel.setJustificationType(juce::Justification::centred);


    // Обновляем размер окна
    setSize(600, 400);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Разделительная линия
    g.setColour(juce::Colours::grey);
    g.drawLine(0, getHeight() - 160, getWidth(), getHeight() - 160, 2);
    g.drawLine(getWidth() / 2, getHeight() - 160, getWidth() / 2, getHeight(), 2);

    // Заголовки
    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(16.0f));
    g.drawText("OSCILLATORS", getLocalBounds().removeFromTop(30), juce::Justification::centred, false);
    g.drawText("FILTER", getLocalBounds().removeFromBottom(160).removeFromLeft(getWidth() / 2).removeFromTop(20),
        juce::Justification::centred, false);
    g.drawText("ADSR ENVELOPE", getLocalBounds().removeFromBottom(160).removeFromRight(getWidth() / 2).removeFromTop(20),
        juce::Justification::centred, false);
}

void NewProjectAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto bottomArea = area.removeFromBottom(150).reduced(10);

    // Фильтр - левая половина
    auto filterArea = bottomArea.removeFromLeft(bottomArea.getWidth() / 2).reduced(5);

    // Тип фильтра
    filterTypeSelector.setBounds(filterArea.removeFromTop(30));

    // Cutoff и Resonance
    int filterSliderWidth = filterArea.getWidth() / 2;
    filterCutoffSlider.setBounds(filterArea.removeFromLeft(filterSliderWidth).reduced(5));
    filterResonanceSlider.setBounds(filterArea.reduced(5));

    // ADSR - правая половина
    auto adsrArea = bottomArea.reduced(5);

    // ADSR-параметры
    int sliderWidth = adsrArea.getWidth() / 4;
    attackSlider.setBounds(adsrArea.removeFromLeft(sliderWidth).reduced(5));
    decaySlider.setBounds(adsrArea.removeFromLeft(sliderWidth).reduced(5));
    sustainSlider.setBounds(adsrArea.removeFromLeft(sliderWidth).reduced(5));
    releaseSlider.setBounds(adsrArea.removeFromLeft(sliderWidth).reduced(5));

    // Оставшееся пространство для осцилляторов
    auto oscArea = area.reduced(10);
    auto osc1Area = oscArea.removeFromLeft(oscArea.getWidth() / 2).reduced(5);
    auto osc2Area = oscArea.reduced(5);

    // Осциллятор 1
    osc1WaveSelector.setBounds(osc1Area.removeFromTop(30));
    osc1VolumeSlider.setBounds(osc1Area.removeFromTop(80).reduced(5));
    osc1PitchSlider.setBounds(osc1Area.removeFromTop(80).reduced(5));
    
    // Осциллятор 2
    osc2WaveSelector.setBounds(osc2Area.removeFromTop(30));
    osc2VolumeSlider.setBounds(osc2Area.removeFromTop(80).reduced(5));
    osc2PitchSlider.setBounds(osc2Area.removeFromTop(80).reduced(5));
}