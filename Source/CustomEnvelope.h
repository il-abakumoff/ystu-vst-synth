#pragma once
#include <JuceHeader.h>

class CustomEnvelope
{
public:
    CustomEnvelope();

    void setSampleRate(double sampleRate);
    void setParameters(float attackSec, float decaySec, float sustainLevel, float releaseSec);

    void noteOn();
    void noteOff();
    float getNextValue();

    bool isActive() const;

private:
    enum State { Idle, Attack, Decay, Sustain, Release };

    double sampleRate;
    State state = Idle;

    float attackTime = 0.1f;
    float decayTime = 0.1f;
    float sustainLevel = 1.0f;
    float releaseTime = 0.1f;

    float currentValue = 0.0f;
    int samplesProcessed = 0;

    float releaseStartValue = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomEnvelope)
};