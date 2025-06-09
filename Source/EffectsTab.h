#pragma once
#include <JuceHeader.h>
#include "EffectTypes.h"

class EffectBlock : public juce::Component
{
public:
    EffectBlock();

    void resized() override;
    void updateControlsForEffect(const juce::String& effectName);
    std::function<void(EffectType)> onEffectChanged;
    std::function<void(int sliderIndex, float value)> onParameterChanged;

private:
    juce::ComboBox effectSelector;
    std::vector<std::unique_ptr<juce::Slider>> parameterSliders;
    std::vector<std::unique_ptr<juce::Label>> parameterLabels;

    void clearSliders();
    void addParameter(const juce::String& label, float min, float max, float step, float defaultValue);

};

class EffectsTab : public juce::Component
{
public:
    EffectsTab();
    void resized() override;
    void connectEffectCallbacks(std::function<void(int, EffectType)> handler);
    void connectParameterCallbacks(std::function<void(int blockIndex, int paramIndex, float value)> handler);

private:
    EffectBlock block1, block2, block3;
};