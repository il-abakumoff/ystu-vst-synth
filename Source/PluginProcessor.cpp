#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WavetableVoice.h"
#include "SynthSound.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>

NewProjectAudioProcessor::NewProjectAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "Parameters", createParameters())
{

    // Инициализация таблиц
    initWavetables();

    for (int i = 0; i < kMaxVoices; ++i)
    {
        synth.addVoice(new WavetableVoice(&sineTable, &sineTable));
    }
    synth.addSound(new SynthSound());

    // Настройка синтезатора
    setupSynth();
}

juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // osc params
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "osc1Wave", "Oscillator 1 Wave",
        juce::StringArray{ "Sine", "Saw", "Square" }, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "osc2Wave", "Oscillator 2 Wave",
        juce::StringArray{ "Sine", "Saw", "Square" }, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc1Volume", "Oscillator 1 Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc2Volume", "Oscillator 2 Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc1Pitch", "Oscillator 1 Pitch",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc2Pitch", "Oscillator 2 Pitch",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f), 0.0f));

    // Amp ADSR params
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "attack", "Attack",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f), 0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "decay", "Decay",
        juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "sustain", "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "release", "Release",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f), 0.2f));

    // FILTER
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "filterType", "Filter Type",
        juce::StringArray{ "Low-pass", "High-pass" }, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f), 1000.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "filterResonance", "Filter Resonance",
        juce::NormalisableRange<float>(0.1f, 2.0f, 0.01f), 0.7f));


    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modAttack", "Mod Attack",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f), 0.1f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modDecay", "Mod Decay",
        juce::NormalisableRange<float>(0.0f, 3.0f, 0.01f), 0.3f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modSustain", "Mod Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "modRelease", "Mod Release",
        juce::NormalisableRange<float>(0.0f, 5.0f, 0.01f), 0.2f));
    return layout;
}

void NewProjectAudioProcessor::initWavetables()
{
    const int tableSize = 1024;

    sineTable.resize(tableSize);
    for (int i = 0; i < tableSize; ++i)
        sineTable[i] = std::sin(2.0f * juce::MathConstants<float>::pi * i / tableSize);

    sawTable.resize(tableSize);
    for (int i = 0; i < tableSize; ++i)
    {
        float saw = 0.0f;
        for (int harmonic = 1; harmonic <= 8; ++harmonic)
            saw += std::sin(2.0f * juce::MathConstants<float>::pi * i * harmonic / tableSize) / harmonic;
        sawTable[i] = saw * 0.5f;
    }

    squareTable.resize(tableSize);
    for (int i = 0; i < tableSize; ++i)
        squareTable[i] = (i < tableSize / 2) ? 1.0f : -1.0f;
}

NewProjectAudioProcessor::~NewProjectAudioProcessor()
{
}

const juce::String NewProjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool NewProjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool NewProjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double NewProjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int NewProjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String NewProjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void NewProjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void NewProjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<WavetableVoice*>(synth.getVoice(i)))
        {
            voice->setSampleRate(sampleRate);
        }
    }

    filterSpec.sampleRate = sampleRate;
    filterSpec.maximumBlockSize = samplesPerBlock;
    filterSpec.numChannels = getTotalNumOutputChannels();

    filter.prepare(filterSpec);
    filter.reset();

    // Начальные параметры
    updateFilterParameters(1000.0f, 0.7f, 0);
}

void NewProjectAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool NewProjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void NewProjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    float attack = apvts.getRawParameterValue("attack")->load();
    float decay = apvts.getRawParameterValue("decay")->load();
    float sustain = apvts.getRawParameterValue("sustain")->load();
    float release = apvts.getRawParameterValue("release")->load();

    modEnvelope.setParameters(
        apvts.getRawParameterValue("modAttack")->load(),
        apvts.getRawParameterValue("modDecay")->load(),
        apvts.getRawParameterValue("modSustain")->load(),
        apvts.getRawParameterValue("modRelease")->load()
    );

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto* voice = dynamic_cast<WavetableVoice*>(synth.getVoice(i)))
        {
            // Обновляем параметры для ОСЦИЛЛЯТОРА 1
            float osc1Volume = apvts.getRawParameterValue("osc1Volume")->load();
            float osc1Pitch = apvts.getRawParameterValue("osc1Pitch")->load();

            // Обновляем параметры для ОСЦИЛЛЯТОРА 2
            float osc2Volume = apvts.getRawParameterValue("osc2Volume")->load();
            float osc2Pitch = apvts.getRawParameterValue("osc2Pitch")->load();

            // Устанавливаем параметры
            voice->setVolume(osc1Volume, osc2Volume);
            voice->setPitch(osc1Pitch, osc2Pitch);
            voice->setAdsrParameters(
                apvts.getRawParameterValue("attack")->load(),
                apvts.getRawParameterValue("decay")->load(),
                apvts.getRawParameterValue("sustain")->load(),
                apvts.getRawParameterValue("release")->load()
            );

            // Устанавливаем разные волновые таблицы для каждого осциллятора
            int osc1WaveIndex = static_cast<int>(apvts.getRawParameterValue("osc1Wave")->load());
            int osc2WaveIndex = static_cast<int>(apvts.getRawParameterValue("osc2Wave")->load());

            const std::vector<float>* table1 = nullptr;
            const std::vector<float>* table2 = nullptr;
            switch (osc1WaveIndex) {
            case 0: table1 = &sineTable; break;
            case 1: table1 = &sawTable; break;
            case 2: table1 = &squareTable; break;
            }

            switch (osc2WaveIndex) {
            case 0: table2 = &sineTable; break;
            case 1: table2 = &sawTable; break;
            case 2: table2 = &squareTable; break;
            }

            voice->setWaveform1(table1);
            voice->setWaveform2(table2); // Теперь разные таблицы
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    // Обновление параметров фильтра
    float cutoff = apvts.getRawParameterValue("filterCutoff")->load();
    float resonance = apvts.getRawParameterValue("filterResonance")->load();
    int filterType = apvts.getRawParameterValue("filterType")->load();

    // Обновляем параметры фильтра
    updateFilterParameters(cutoff, resonance, filterType);

    // Обработка фильтром
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);
}

bool NewProjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor()
{
    return new NewProjectAudioProcessorEditor (*this);
}


void NewProjectAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Создаем XML-структуру для сохранения
    auto state = apvts.copyState(); // Автоматически сохраняет все параметры APVTS
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    // Дополнительно можно сохранить другие данные (если есть)
    // xml->setAttribute("customData", someValue);

    // Копируем XML в бинарный блок
    copyXmlToBinary(*xml, destData);
}

void NewProjectAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Восстанавливаем из бинарных данных
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        // Восстанавливаем параметры APVTS
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));

        // Дополнительно можно восстановить другие данные
        // someValue = xmlState->getDoubleAttribute("customData", defaultValue);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new NewProjectAudioProcessor();
}

void NewProjectAudioProcessor::setupSynth()
{
    synth.clearVoices();
    synth.clearSounds();

    // Добавляем голоса
    for (int i = 0; i < kMaxVoices; ++i)
        synth.addVoice(new WavetableVoice(&sineTable, &sineTable));

    // Добавляем звук (обязательно)
    synth.addSound(new SynthSound());
}

void NewProjectAudioProcessor::updateFilterParameters(float cutoff, float resonance, int type)
{
    // Устанавливаем базовые параметры
    filter.setCutoffFrequency(cutoff);
    filter.setResonance(resonance);

    // Настраиваем тип фильтра через параметры
    switch (type) {
    case 0: // Low-pass
        filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
        break;
    case 1: // High-pass
        filter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
        break;
    }
}