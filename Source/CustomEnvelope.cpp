#include "CustomEnvelope.h"

CustomEnvelope::CustomEnvelope() = default;

void CustomEnvelope::setSampleRate(double rate)
{
    sampleRate = rate;
}

void CustomEnvelope::setParameters(float attackSec, float decaySec,
    float sustainLvl, float releaseSec)
{
    attackTime = attackSec;
    decayTime = decaySec;
    sustainLevel = sustainLvl;
    releaseTime = releaseSec;
}

void CustomEnvelope::noteOn()
{
    state = Attack;
    currentValue = 0.0f;
    samplesProcessed = 0;
}

void CustomEnvelope::noteOff()
{
    if (state != Idle && state != Release)
    {
        releaseStartValue = currentValue; // Сохраняем текущее значение
        state = Release;
        samplesProcessed = 0;
    }
}

float CustomEnvelope::getNextValue()
{
    switch (state)
    {
    case Attack:
    {
        if (attackTime <= 0.0f)
        {
            currentValue = 1.0f;
            state = Decay;
            break;
        }

        float attackSamples = attackTime * sampleRate;
        currentValue = juce::jmin(samplesProcessed / attackSamples, 1.0f);

        if (currentValue >= 1.0f)
        {
            currentValue = 1.0f;
            state = Decay;
            samplesProcessed = 0;
        }
        break;
    }

    case Decay:
    {
        if (decayTime <= 0.0f)
        {
            currentValue = sustainLevel;
            state = Sustain;
            break;
        }

        float decaySamples = decayTime * sampleRate;
        float t = samplesProcessed / decaySamples;
        currentValue = 1.0f + (sustainLevel - 1.0f) * t;

        if (t >= 1.0f)
        {
            currentValue = sustainLevel;
            state = Sustain;
        }
        break;
    }

    case Sustain:
        currentValue = sustainLevel;
        break;

    case Release:
    {
        if (releaseTime <= 0.0f)
        {
            currentValue = 0.0f;
            state = Idle;
            break;
        }

        float releaseSamples = releaseTime * sampleRate;
        float t = samplesProcessed / releaseSamples;
        currentValue = releaseStartValue * (1.0f - t); // Затухаем от текущего значения

        if (t >= 1.0f)
        {
            currentValue = 0.0f;
            state = Idle;
        }
        break;
    }

    case Idle:
        currentValue = 0.0f;
        break;
    }

    samplesProcessed++;
    return currentValue;
}

bool CustomEnvelope::isActive() const
{
    return state != Idle;
}