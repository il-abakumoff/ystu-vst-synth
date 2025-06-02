#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WavetableVoice.h"
#include "SynthSound.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_core/juce_core.h>
#include <math.h>
#include "GlobalSettings.h"


PluginProcessor::PluginProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "Parameters", createParameters())
{
    for (int i = 0; i < kMaxVoices; ++i)
    {
        synth.addVoice(new WavetableVoice(&fallbackWaveform, &fallbackWaveform));
    }
    synth.addSound(new SynthSound());
    setupSynth();
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameters()
{
    initWavetables();

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
   
    juce::StringArray waveNames;
    for (const auto& pair : wavetableList)
        waveNames.add(pair.first);

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "osc1Wave", "Oscillator 1 Wave", waveNames, 0));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "osc2Wave", "Oscillator 2 Wave", waveNames, 0));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc1Volume", "Oscillator 1 Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc2Volume", "Oscillator 2 Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc1Pitch", "Oscillator 1 Pitch",
        juce::NormalisableRange<float>(-36.0f, 36.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc2Pitch", "Oscillator 2 Pitch",
        juce::NormalisableRange<float>(-36.0f, 36.0f, 1.0f), 0.0f));    
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc1Fine", "Oscillator 1 Fine",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "osc2Fine", "Oscillator 2 Fine",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 1.0f), 0.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>( // Amp ADSR params
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

    layout.add(std::make_unique<juce::AudioParameterChoice>(   // FILTER
        "filterType", "Filter Type",
        juce::StringArray{ "Low-pass", "High-pass", "Band-pass", "Notch" }, 0));

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

void PluginProcessor::loadCustomWavetables()
{
    auto folder = GlobalSettings::getInstance().getWavetableDirectory();
    auto files = folder.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.wtxt");

    wavetableList.clear();

    for (const auto& file : files)
    {
        std::vector<float> table;
        juce::FileInputStream stream(file);

        while (!stream.isExhausted())
        {
            auto line = stream.readNextLine();
            table.push_back(line.getFloatValue());
        }

        if (!table.empty())
            wavetableList.emplace_back(file.getFileNameWithoutExtension(), std::move(table));
    }

    maxWaveIndex = (int)wavetableList.size();
    fallbackWaveform.assign(1024, 0.0f);
}

void PluginProcessor::initWavetables()
{
    loadCustomWavetables();
}

PluginProcessor::~PluginProcessor()
{
}

const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginProcessor::getProgramName (int index)
{
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
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

    mainFilter.prepare({ sampleRate, (juce::uint32)samplesPerBlock, 2 });
    notchFilter.prepare(filterSpec);
    *notchFilter.state = *juce::dsp::IIR::Coefficients<float>::makeNotch(
        sampleRate, 1000.0f, 0.707f);

    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(getTotalNumOutputChannels()) };
    for (auto& fx : effects)
        fx.prepare(spec);

    updateFilterParameters(1000.0f, 0.7f, 0);
}

void PluginProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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
            float osc1Volume = apvts.getRawParameterValue("osc1Volume")->load();
            float osc1Pitch = apvts.getRawParameterValue("osc1Pitch")->load();
            float osc1Fine = apvts.getRawParameterValue("osc1Fine")->load();
            float osc2Volume = apvts.getRawParameterValue("osc2Volume")->load();
            float osc2Pitch = apvts.getRawParameterValue("osc2Pitch")->load();
            float osc2Fine = apvts.getRawParameterValue("osc2Fine")->load();

            voice->setVolume(osc1Volume, osc2Volume);
            voice->setPitch(osc1Pitch, osc2Pitch, osc1Fine, osc2Fine);
            voice->setAdsrParameters(attack, decay, sustain, release);

            int osc1WaveIndex = static_cast<int>(apvts.getRawParameterValue("osc1Wave")->load());
            int osc2WaveIndex = static_cast<int>(apvts.getRawParameterValue("osc2Wave")->load());

            const std::vector<float>* table1 = nullptr;
            const std::vector<float>* table2 = nullptr;

            if (osc1WaveIndex >= 0 && osc1WaveIndex < maxWaveIndex) {
                table1 = &wavetableList[osc1WaveIndex].second;
                DBG(osc1WaveIndex);
            }
            else
            {
                table1 = &fallbackWaveform;
            }
               

            if (osc2WaveIndex >= 0 && osc2WaveIndex < maxWaveIndex)
                table2 = &wavetableList[osc2WaveIndex].second;
            else
                table2 = &fallbackWaveform;

            voice->setWaveform1(table1);
            voice->setWaveform2(table2);
        }
    }

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    float cutoff = apvts.getRawParameterValue("filterCutoff")->load();
    float resonance = apvts.getRawParameterValue("filterResonance")->load();
    currentFilterType = apvts.getRawParameterValue("filterType")->load();
    updateFilterParameters(cutoff, resonance, currentFilterType);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    if (currentFilterType != 3)
    {
        mainFilter.process(context);
    }
    else
    {
        notchFilter.process(context);
    }

    for (auto& fx : effects)
        fx.process(buffer);
}

bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}


void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState(); // Автоматически сохраняет все параметры APVTS
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

void PluginProcessor::savePresetToFile(const juce::File& file)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());

    if (xml)
    {
        xml->setTagName("STATE"); 
        xml->setAttribute("version", JucePlugin_VersionString);
        xml->writeTo(file);
    }
}

void PluginProcessor::loadPresetFromFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;

    std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(file));
    if (xml && xml->hasTagName("STATE"))
    {
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}

void PluginProcessor::setupSynth()
{
    synth.clearVoices();
    synth.clearSounds();

    for (int i = 0; i < kMaxVoices; ++i)
        synth.addVoice(new WavetableVoice(&fallbackWaveform, &fallbackWaveform));

    synth.addSound(new SynthSound());
}

void PluginProcessor::updateFilterParameters(float cutoff, float resonance, int type)
{ 
    if (type != 3) { // LP/HP/BP
        mainFilter.setCutoffFrequency(cutoff);
        mainFilter.setResonance(resonance);
        
        switch (type) {
            case 0: mainFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass); break;
            case 1: mainFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass); break;
            case 2: mainFilter.setType(juce::dsp::StateVariableTPTFilterType::bandpass); break;
        }
    }
    else // Notch
    {
        *notchFilter.state = *juce::dsp::IIR::Coefficients<float>::makeNotch(
            filterSpec.sampleRate, cutoff, resonance);
    }
}

void PluginProcessor::EffectProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(spec.sampleRate, 500);
    toneFilterDry.state = coeffs;
    toneFilterWet.state = coeffs;
    toneFilterDry.prepare(spec);
    toneFilterWet.prepare(spec);
    toneFilterDry.reset();
    toneFilterWet.reset();
    distortion.functionToUse = [this](float x) { return std::tanh(x * distortionDrive * 2.0f); };
    distortion.reset();

    chorus.prepare(spec);
    chorus.reset();
    chorus.setRate(1.5f);
    chorus.setDepth(0.3f); 
    chorus.setCentreDelay(10.0f);
    chorus.setFeedback(0.0f);
    chorus.setMix(1.0f);

    phaser.prepare(spec);
    phaser.reset();
    phaser.setRate(0.5f);  
    phaser.setDepth(0.8f); 
    phaser.setCentreFrequency(1000.0f); 
    phaser.setFeedback(0.0f);
    phaser.setMix(1.0f);
    
    reverb.prepare(spec);
    reverb.reset();
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = reverbMix;
    reverbParams.dryLevel = 0.0f;
    reverb.setParameters(reverbParams);

    delay.prepare(spec);
    delay.reset();
    delay.setDelay(delayTimeMs);
}

void PluginProcessor::EffectProcessor::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    switch (type)
        {
        case EffectType::None: break;

        case EffectType::Distortion:
        {
            juce::AudioBuffer<float> dryBuffer;
            juce::AudioBuffer<float> wetBuffer;
            dryBuffer.makeCopyOf(buffer);
            wetBuffer.makeCopyOf(buffer);

            juce::dsp::AudioBlock<float> wetBlock(wetBuffer);
            juce::dsp::AudioBlock<float> dryBlock(dryBuffer);

            juce::dsp::ProcessContextReplacing<float> wetCtx(wetBlock);
            juce::dsp::ProcessContextReplacing<float> dryCtx(dryBlock);
     
            distortion.process(wetCtx);
            toneFilterDry.process(dryCtx);
            toneFilterWet.process(wetCtx);

            float gainComp = 1.0f / std::sqrt(distortionDrive); // можно подстроить формулу
            wetBuffer.applyGain(gainComp);

            auto numCh = buffer.getNumChannels();
            auto numSamples = buffer.getNumSamples();
            auto* wet = wetBuffer.getArrayOfReadPointers();
            auto* dry = dryBuffer.getArrayOfReadPointers();
            auto* out = buffer.getArrayOfWritePointers();

            for (int ch = 0; ch < numCh; ++ch)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    out[ch][i] = wet[ch][i] * distortionMix + dry[ch][i] * (1.0f - distortionMix);
                }
            }

            break;
        }

        case EffectType::Chorus:
        {
            juce::AudioBuffer<float> dryBuffer;
            dryBuffer.makeCopyOf(buffer);
            juce::dsp::AudioBlock<float> block(buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            chorus.process(context);

            auto* dry = dryBuffer.getArrayOfReadPointers();
            auto* wet = buffer.getArrayOfWritePointers();
            auto numChannels = buffer.getNumChannels();
            auto numSamples = buffer.getNumSamples();

            for (int ch = 0; ch < numChannels; ++ch)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    wet[ch][i] = wet[ch][i] * chorusMix + dry[ch][i] * (1.0f - chorusMix);
                }
            }
            break;
        }

        case EffectType::Phaser:
        {
            juce::AudioBuffer<float> dryBuffer;
            dryBuffer.makeCopyOf(buffer);
            juce::dsp::AudioBlock<float> block(buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            phaser.process(context);
            auto* dry = dryBuffer.getArrayOfReadPointers();
            auto* wet = buffer.getArrayOfWritePointers();
            auto numChannels = buffer.getNumChannels();
            auto numSamples = buffer.getNumSamples();

            for (int ch = 0; ch < numChannels; ++ch)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    wet[ch][i] = wet[ch][i] * phaserMix + dry[ch][i] * (1.0f - phaserMix);
                }
            }
            break;
        }

        case EffectType::Reverb:
        {
            juce::AudioBuffer<float> dryBuffer;
            dryBuffer.makeCopyOf(buffer);

            juce::dsp::AudioBlock<float> block(buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            reverb.process(context);

            auto* dry = dryBuffer.getArrayOfReadPointers();
            auto* wet = buffer.getArrayOfWritePointers();
            auto numChannels = buffer.getNumChannels();
            auto numSamples = buffer.getNumSamples();

            for (int ch = 0; ch < numChannels; ++ch)
            {
                for (int i = 0; i < numSamples; ++i)
                {
                    wet[ch][i] = wet[ch][i] * reverbMix + dry[ch][i] * (1.0f - reverbMix);
                }
            }
            break;
        }

        case EffectType::Delay:
        {
            const int numSamples = buffer.getNumSamples();
            const int numChannels = buffer.getNumChannels();

            juce::AudioBuffer<float> dryBuffer;
            dryBuffer.makeCopyOf(buffer);

            for (int ch = 0; ch < numChannels; ++ch)
            {
                auto* channelData = buffer.getWritePointer(ch);
                for (int i = 0; i < numSamples; ++i)
                {
                    float in = channelData[i];
                    float delayed = delay.popSample(ch); // читаем задержанный
                    float fbSample = in + delayed * delayFeedback;
                    delay.pushSample(ch, fbSample);
                    channelData[i] = delayed * delayMix + in * (1.0f - delayMix);
                }
            }
            break;
        }
    }
}