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

    setSize (400, 300);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void NewProjectAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto osc1Area = area.removeFromLeft(getWidth() / 2).reduced(10);
    auto osc2Area = area.reduced(10);

    osc1WaveSelector.setBounds(osc1Area.removeFromTop(30));
    osc1VolumeSlider.setBounds(osc1Area.removeFromTop(100).reduced(5));
    osc1PitchSlider.setBounds(osc1Area.removeFromTop(100).reduced(5));

    osc2WaveSelector.setBounds(osc2Area.removeFromTop(30));
    osc2VolumeSlider.setBounds(osc2Area.removeFromTop(100).reduced(5));
    osc2PitchSlider.setBounds(osc2Area.removeFromTop(100).reduced(5));
}