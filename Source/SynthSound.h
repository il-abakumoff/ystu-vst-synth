#pragma once
#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    SynthSound() = default;

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};