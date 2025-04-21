#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class NewProjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    NewProjectAudioProcessor& audioProcessor;

    // Осциллятор 1
    juce::ComboBox osc1WaveSelector;
    juce::Slider osc1VolumeSlider;
    juce::Slider osc1PitchSlider;
    juce::Label osc1VolumeLabel, osc1PitchLabel;

    // Осциллятор 2
    juce::ComboBox osc2WaveSelector;
    juce::Slider osc2VolumeSlider;
    juce::Slider osc2PitchSlider;
    juce::Label osc2VolumeLabel, osc2PitchLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc1WaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1VolumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1PitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc2WaveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2VolumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2PitchAttachment;

    // Amp ADSR elements
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;


    juce::ComboBox filterTypeSelector;
    juce::Slider filterCutoffSlider;
    juce::Slider filterResonanceSlider;
    juce::Label filterCutoffLabel, filterResonanceLabel, filterTypeLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
