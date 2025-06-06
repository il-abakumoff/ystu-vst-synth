#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    SynthSound() = default;

    // ������������ ����� (������ ���������� true ��� ���� ���)
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }

    // ������������ ����� (���� ����� ����������� � ������)
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};