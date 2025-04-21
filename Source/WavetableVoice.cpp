#include "WavetableVoice.h"

WavetableVoice::WavetableVoice(const std::vector<float>* wavetable1, const std::vector<float>* wavetable2)
    : wavetable1(wavetable1), wavetable2(wavetable2) {
    envelope.setParameters(0.01f, 0.1f, 0.7f, 0.2f);
}

bool WavetableVoice::canPlaySound(juce::SynthesiserSound* sound) { return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr; }

void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    envelope.noteOn();
    isNotePlaying = true;

    float baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    phaseDelta1 = baseFrequency * std::pow(2.0f, pitchSemitones1 / 12.0f) * wavetable1->size() / getSampleRate();
    phaseDelta2 = baseFrequency * std::pow(2.0f, pitchSemitones2 / 12.0f) * wavetable2->size() / getSampleRate();

    // Сбрасываем фазы
    phase1 = phase2 = 0.0f;
}

void WavetableVoice::stopNote(float, bool allowTailOff)
{
    if (allowTailOff)
    {
        envelope.noteOff();
        isNotePlaying = false; // Помечаем ноту как остановленную
    }
    else
    {
        envelope.setParameters(0.0f, 0.0f, 0.0f, 0.0f);
        clearCurrentNote();
        isNotePlaying = false;
    }
}

void WavetableVoice::renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (!envelope.isActive())
    {
        clearCurrentNote();
        return;
    }


    for (int i = startSample; i < startSample + numSamples; ++i)
    {
        float envValue = envelope.getNextValue();

        // Осцилляторы
        int index1 = static_cast<int>(phase1) % wavetable1->size();
        float sample1 = (*wavetable1)[index1] * level1;

        int index2 = static_cast<int>(phase2) % wavetable2->size();
        float sample2 = (*wavetable2)[index2] * level2;

        // Смешивание с огибающей
        float mixedSample = (sample1 + sample2) * 0.5f * envValue;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            buffer.addSample(channel, i, mixedSample);
        }

        phase1 = fmod(phase1 + phaseDelta1, wavetable1->size());
        phase2 = fmod(phase2 + phaseDelta2, wavetable2->size());

        if (!envelope.isActive())
        {
            clearCurrentNote();
            return;
        }
    }
}

void WavetableVoice::pitchWheelMoved(int newPitchWheelValue) {

}   

void WavetableVoice::controllerMoved(int controllerNumber, int newControllerValue) {

}


void WavetableVoice::setAdsrParameters(float attack, float decay, float sustain, float release)
{
    envelope.setParameters(attack, decay, sustain, release);
}