#pragma once

#include <JuceHeader.h>
#include <vector>
#include "WavetableVoice.h"
#include "SynthSound.h"
#include <juce_dsp/juce_dsp.h> // Для DSP модулей
#include "EffectTypes.h"

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

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void savePresetToFile(const juce::File& file);
    void loadPresetFromFile(const juce::File& file);

    enum FilterType {
        LowPass = 0,
        HighPass = 1,
        BandPass = 2,
        Notch = 3
    };

    struct EffectProcessor
    {
        EffectType type = EffectType::None;

        // distortion
        float distortionDrive = 1.0f;
        float distortionMix = 1.0f; // 0 = dry, 1 = wet
        float distortionTone = 20000.0f; // cutoff для LPF

        juce::dsp::WaveShaper<float, std::function<float(float)>> distortion;
        juce::dsp::ProcessorDuplicator<
            juce::dsp::IIR::Filter<float>,
            juce::dsp::IIR::Coefficients<float>
        > toneFilterDry, toneFilterWet;

        juce::dsp::Chorus<float> chorus;
        float chorusMix = 0.5f;
        
        juce::dsp::Phaser<float> phaser;
        float phaserMix = 0.5f;
        
        juce::dsp::Reverb reverb;
        juce::dsp::Reverb::Parameters reverbParams;
        float reverbMix = 0.5f;

        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delay{ 96000 };
        juce::AudioBuffer<float> delayBuffer;        
        float delayTimeMs = 300.0f;
        float delayFeedback = 0.4f;
        float delayMix = 0.5f;

        void prepare(const juce::dsp::ProcessSpec& spec);
        void process(juce::AudioBuffer<float>& buffer);
    };

    std::array<EffectProcessor, 3> effects;

    static constexpr int kMaxVoices = 12; // Максимальное число голосов
    juce::Synthesiser synth;             // Основной синтезатор JUCE

    // Параметры плагина (DAW, Automation)
    juce::AudioParameterInt* voiceCountParam;  // Количество голосов (1-24)
    juce::AudioParameterChoice* osc1WaveParam;
    juce::AudioParameterChoice* osc2WaveParam;
    juce::AudioParameterFloat* osc1VolumeParam;
    juce::AudioParameterFloat* osc2VolumeParam;
    juce::AudioParameterFloat* osc1PitchParam;
    juce::AudioParameterFloat* osc2PitchParam;    
    juce::AudioParameterFloat* osc1FineParam;
    juce::AudioParameterFloat* osc2FineParam;
    
    std::vector<float> sineTable;       // Таблица синуса
    std::vector<float> sawTable;        // Таблица "супер пилы"
    std::vector<float> squareTable;     // Таблица квадрата

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    juce::AudioParameterFloat* attackParam;
    juce::AudioParameterFloat* decayParam;
    juce::AudioParameterFloat* sustainParam;
    juce::AudioParameterFloat* releaseParam;

    juce::AudioParameterChoice* filterTypeParam;
    juce::AudioParameterFloat* filterCutoffParam;
    juce::AudioParameterFloat* filterResonanceParam;

    juce::AudioParameterFloat* modAttackParam;
    juce::AudioParameterFloat* modDecayParam;
    juce::AudioParameterFloat* modSustainParam;
    juce::AudioParameterFloat* modReleaseParam;

    auto getNotchFilterState() {
        return notchFilter.state;
    }    
    auto getFilterSpec() {
        return filterSpec;
    }

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessor)

    void initWavetables();              // Инициализация таблиц
    void setupSynth();                  // Настройка синтезатора

    void updateFilterParameters(float cutoff, float resonance, int type);

    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    juce::dsp::StateVariableTPTFilter<float> mainFilter;
    juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>> notchFilter;
    
    juce::dsp::ProcessSpec filterSpec;
    int currentFilterType = 0;
    
    CustomEnvelope modEnvelope;
};
