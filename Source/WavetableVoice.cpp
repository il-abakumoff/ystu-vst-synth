#include "WavetableVoice.h"

WavetableVoice::WavetableVoice(const std::vector<float>* wavetable1, const std::vector<float>* wavetable2)
    : wavetable1(wavetable1), wavetable2(wavetable2) {
    adsr.setSampleRate(44100.0); // Установите правильный sample rate в prepareToPlay
    adsr.setParameters(juce::ADSR::Parameters(0.01f, 0.1f, 0.7f, 0.2f));
}

bool WavetableVoice::canPlaySound(juce::SynthesiserSound* sound) { return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr; }

void WavetableVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound*, int)
{
    adsr.noteOn();
    isNotePlaying = true;

    float baseFrequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);

    // Частота для первого осциллятора
    float freq1 = baseFrequency * std::pow(2.0f, pitchSemitones1 / 12.0f);
    phaseDelta1 = freq1 * wavetable1->size() / getSampleRate();

    // Частота для второго осциллятора
    float freq2 = baseFrequency * std::pow(2.0f, pitchSemitones2 / 12.0f);
    phaseDelta2 = freq2 * wavetable2->size() / getSampleRate();

    phase1 = phase2 = 0.0f;
}

void WavetableVoice::stopNote(float, bool allowTailOff)
{
    if (!allowTailOff)
        clearCurrentNote(); // Резкое отключение
    adsr.noteOff();
    isNotePlaying = false;
}

void WavetableVoice::renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    if (!isNotePlaying) return;

    for (int i = startSample; i < startSample + numSamples; ++i)
    {
        // Осциллятор 1
        int index1 = static_cast<int>(phase1) % wavetable1->size();
        float sample1 = (*wavetable1)[index1] * level1;

        // Осциллятор 2
        int index2 = static_cast<int>(phase2) % wavetable2->size();
        float sample2 = (*wavetable2)[index2] * level2;

        // Смешивание с огибающей
        float envValue = adsr.getNextSample();
        float mixedSample = (sample1 + sample2) * 0.5f * envValue;

        // Плавное добавление в буфер
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            float currentSample = buffer.getSample(channel, i);
            buffer.setSample(channel, i, currentSample + mixedSample);
        }

        // Обновление фазы
        phase1 = fmod(phase1 + phaseDelta1, wavetable1->size());
        phase2 = fmod(phase2 + phaseDelta2, wavetable2->size());
    }
}

void WavetableVoice::pitchWheelMoved(int newPitchWheelValue) {

}   

void WavetableVoice::controllerMoved(int controllerNumber, int newControllerValue) {

}