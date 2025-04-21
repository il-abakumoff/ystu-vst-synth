#pragma once
#include <JuceHeader.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
	
class WavetableVoice : public juce::SynthesiserVoice
{
public:
    WavetableVoice(const std::vector<float>* wavetable1, const std::vector<float>* wavetable2);

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    void setWaveform1(const std::vector<float>* newTable) { wavetable1 = newTable; }
    void setWaveform2(const std::vector<float>* newTable) { wavetable2 = newTable; }

    void setVolume(float vol1, float vol2) {
        level1 = vol1;
        level2 = vol2;
    }

    void setPitch(float pitch1, float pitch2) {
        pitchSemitones1 = pitch1;
        pitchSemitones2 = pitch2;
    }

    void setSampleRate(double sr) { adsr.setSampleRate(sr); }


private:
    const std::vector<float>* wavetable1;
    const std::vector<float>* wavetable2;

    float phase1 = 0.0f, phase2 = 0.0f;
    float phaseDelta1 = 0.0f, phaseDelta2 = 0.0f;
    float level1 = 0.0f, level2 = 0.0f; // ��������� ���������� ������
    bool isNotePlaying = false;

    float pitchSemitones1 = 0.0f; // Pitch ��� ������� �����������
    float pitchSemitones2 = 0.0f; // Pitch ��� ������� �����������

    juce::ADSR adsr;
};

