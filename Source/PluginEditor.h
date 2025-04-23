#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

class NewProjectAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor(NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    NewProjectAudioProcessor& audioProcessor;
    juce::TabbedComponent tabs;

    struct MainPanel : public juce::Component
    {
        // Осциллятор 1
        juce::ComboBox osc1WaveSelector;
        juce::Slider osc1VolumeSlider;
        juce::Slider osc1PitchSlider;
        juce::Slider osc1FineSlider;
        juce::Label osc1VolumeLabel;
        juce::Label osc1PitchLabel;
        juce::Label osc1FineLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc1WaveAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1VolumeAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1PitchAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1FineAttachment;

        // Осциллятор 2
        juce::ComboBox osc2WaveSelector;
        juce::Slider osc2VolumeSlider;
        juce::Slider osc2PitchSlider;
        juce::Slider osc2FineSlider;
        juce::Label osc2VolumeLabel;
        juce::Label osc2PitchLabel;
        juce::Label osc2FineLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc2WaveAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2VolumeAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2PitchAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2FineAttachment;

        // ADSR 1
        juce::Slider attackSlider1;
        juce::Slider decaySlider1;
        juce::Slider sustainSlider1;
        juce::Slider releaseSlider1;
        juce::Label attackLabel1;
        juce::Label decayLabel1;
        juce::Label sustainLabel1;
        juce::Label releaseLabel1;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment1;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment1;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment1;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment1;

        // ADSR 2
        juce::Slider attackSlider2;
        juce::Slider decaySlider2;
        juce::Slider sustainSlider2;
        juce::Slider releaseSlider2;
        juce::Label attackLabel2;
        juce::Label decayLabel2;
        juce::Label sustainLabel2;
        juce::Label releaseLabel2;

        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment2;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment2;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment2;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment2;

        // Фильтр
        juce::ComboBox filterTypeSelector;
        juce::Slider filterCutoffSlider;
        juce::Slider filterResonanceSlider;
        juce::Label filterTypeLabel;
        juce::Label filterCutoffLabel;
        juce::Label filterResonanceLabel;

        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> filterTypeAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;

        void resized() override;
    };
    std::unique_ptr<MainPanel> mainPanel;

    struct MatrixPanel : public juce::Component
    {
        struct ModulationRow
        {
            std::unique_ptr<juce::ComboBox> sourceCombo;
            std::unique_ptr<juce::ComboBox> targetCombo;
            std::unique_ptr<juce::Slider> amountSlider;
            std::unique_ptr<juce::Label> sourceLabel;
            std::unique_ptr<juce::Label> targetLabel;
            std::unique_ptr<juce::Label> amountLabel;
        };
        std::vector<ModulationRow> modulationRows;

        void resized() override;
    };
    std::unique_ptr<MatrixPanel> matrixPanel;

    void setupMainPanel();
    void setupMatrixPanel();

    std::unique_ptr<CustomLookAndFeel> customLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessorEditor)
};