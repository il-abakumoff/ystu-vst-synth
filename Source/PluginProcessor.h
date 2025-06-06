#pragma once

#include <JuceHeader.h>
#include <vector>
#include "WavetableVoice.h"
#include "SynthSound.h"
#include <juce_dsp/juce_dsp.h> // ��� DSP �������

class NewProjectAudioProcessor  : public juce::AudioProcessor
{
public:
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // ������ � �����������
    static constexpr int kMaxVoices = 24; // ������������ ����� �������
    juce::Synthesiser synth;             // �������� ���������� JUCE

    // ��������� ������� (DAW, Automation)
    juce::AudioParameterInt* voiceCountParam;  // ���������� ������� (1-24)
    juce::AudioParameterChoice* osc1WaveParam;
    juce::AudioParameterChoice* osc2WaveParam;
    juce::AudioParameterFloat* osc1VolumeParam;
    juce::AudioParameterFloat* osc2VolumeParam;
    juce::AudioParameterFloat* osc1PitchParam;
    juce::AudioParameterFloat* osc2PitchParam;
    
    // Wavetable ������
    std::vector<float> sineTable;       // ������� ������
    std::vector<float> sawTable;        // ������� "����� ����"
    std::vector<float> squareTable;     // ������� ��������

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    juce::AudioParameterFloat* attackParam;
    juce::AudioParameterFloat* decayParam;
    juce::AudioParameterFloat* sustainParam;
    juce::AudioParameterFloat* releaseParam;

    juce::AudioParameterChoice* filterTypeParam;
    juce::AudioParameterFloat* filterCutoffParam;
    juce::AudioParameterFloat* filterResonanceParam;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)

    void initWavetables();              // ������������� ������
    void setupSynth();                  // ��������� �����������

    void updateFilterParameters(float cutoff, float resonance, int type);

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    juce::dsp::StateVariableTPTFilter<float> filter;
    juce::dsp::ProcessSpec filterSpec;
};
