#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    SynthSound() = default;

    // ќб€зательный метод (должен возвращать true дл€ всех нот)
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }

    // ќб€зательный метод (звук может примен€тьс€ к каналу)
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};