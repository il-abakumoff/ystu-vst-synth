#include "EffectsTab.h"

EffectBlock::EffectBlock()
{
    effectSelector.addItem("None", 1);
    effectSelector.addItem("Distortion", 2);
    effectSelector.addItem("Chorus", 3);
    effectSelector.addItem("Phaser", 4);
    effectSelector.addItem("Reverb", 5);
    effectSelector.addItem("Delay", 6);
    effectSelector.setSelectedId(1); 

    effectSelector.onChange = [this]() {
        updateControlsForEffect(effectSelector.getText());
        };
    addAndMakeVisible(effectSelector);

    updateControlsForEffect("Distortion");
}

void EffectsTab::connectEffectCallbacks(std::function<void(int, EffectType)> handler)
{
    block1.onEffectChanged = [handler](EffectType type) { handler(0, type); };
    block2.onEffectChanged = [handler](EffectType type) { handler(1, type); };
    block3.onEffectChanged = [handler](EffectType type) { handler(2, type); };
}

void EffectBlock::updateControlsForEffect(const juce::String& effectName)
{
    clearSliders();

    if (onEffectChanged)
    {
        if (effectName == "Distortion") onEffectChanged(EffectType::Distortion);
        else if (effectName == "Chorus")     onEffectChanged(EffectType::Chorus);
        else if (effectName == "Flanger")    onEffectChanged(EffectType::Flanger);
        else if (effectName == "Phaser")     onEffectChanged(EffectType::Phaser);
        else if (effectName == "Reverb")     onEffectChanged(EffectType::Reverb);
        else if (effectName == "Delay")      onEffectChanged(EffectType::Delay);
        else                                 onEffectChanged(EffectType::None);
    }

    if (effectName == "None")
    {
        clearSliders();
        resized();
        return;
    }
    if (effectName == "Distortion")
    {
        addParameter("Drive", 1.0f, 10.0f, 0.1f, 2.0f);
        addParameter("Tone", 50.0f, 20000.0f, 1.0f, 2000.0f);
        addParameter("Mix", 0.0f, 1.0f, 0.01f, 0.1f);
    }
    else if (effectName == "Chorus")
    {
        addParameter("Rate", 0.1f, 15.0f, 0.01f, 1.5f);   
        addParameter("Depth", 0.0f, 1.0f, 0.01f, 0.3f);  
        addParameter("Mix", 0.0f, 1.0f, 0.01f, 0.5f);
    }
    else if (effectName == "Phaser")
    {
        addParameter("Rate", 0.1f, 5.0f, 0.01f, 0.5f);      // Ãö
        addParameter("Depth", 0.0f, 1.0f, 0.01f, 0.8f);     // 0–1
        addParameter("Mix", 0.0f, 1.0f, 0.01f, 0.5f);
    }
    else if (effectName == "Reverb")
    {
        addParameter("Room Size", 0.0f, 1.0f, 0.01f, 0.5f);
        addParameter("Damping", 0.0f, 1.0f, 0.01f, 0.5f);
        addParameter("Mix", 0.0f, 1.0f, 0.01f, 0.5f);
    }
    else if (effectName == "Delay")
    {
        addParameter("Time", 2000.0f, 40000.0f, 10.0f, 8000.0f);
        addParameter("Feedback", 0.0f, 0.95f, 0.01f, 0.3f);
        addParameter("Mix", 0.0f, 1.0f, 0.01f, 0.15f);
    }

    resized();
}

void EffectBlock::clearSliders()
{
    parameterSliders.clear();
    parameterLabels.clear();
    removeAllChildren();
    addAndMakeVisible(effectSelector);
}

void EffectBlock::addParameter(const juce::String& label, float min, float max, float step, float defaultValue)
{
    auto slider = std::make_unique<juce::Slider>();
    slider->setSliderStyle(juce::Slider::Rotary);
    slider->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider->setNumDecimalPlacesToDisplay(2);
    slider->setRange(min, max, step);
    slider->setValue(defaultValue);

    int index = (int)parameterSliders.size();
    slider->onValueChange = [this, index]() {
        if (onParameterChanged)
            onParameterChanged(index, parameterSliders[index]->getValue());
        };

    addAndMakeVisible(*slider);

    auto lbl = std::make_unique<juce::Label>();
    lbl->setText(label, juce::dontSendNotification);
    lbl->setJustificationType(juce::Justification::centred);
    addAndMakeVisible(*lbl);

    parameterSliders.push_back(std::move(slider));
    parameterLabels.push_back(std::move(lbl));
}

void EffectBlock::resized()
{
    auto area = getLocalBounds().reduced(8);
    effectSelector.setBounds(area.removeFromTop(30));

    const int sliderWidth = 60;
    const int labelHeight = 20;
    const int spacing = 10;

    for (size_t i = 0; i < parameterSliders.size(); ++i)
    {
        auto sliderArea = area.removeFromLeft(sliderWidth).removeFromTop(60);
        parameterSliders[i]->setBounds(sliderArea);

        parameterLabels[i]->setBounds(sliderArea.getX(), sliderArea.getBottom(), sliderWidth, labelHeight);
        area.removeFromLeft(spacing);
    }
}


EffectsTab::EffectsTab()
{
    addAndMakeVisible(block1);
    addAndMakeVisible(block2);
    addAndMakeVisible(block3);
}

void EffectsTab::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto blockWidth = area.getWidth() / 3;

    block1.setBounds(area.removeFromLeft(blockWidth).reduced(5));
    block2.setBounds(area.removeFromLeft(blockWidth).reduced(5));
    block3.setBounds(area.reduced(5));
}

void EffectsTab::connectParameterCallbacks(std::function<void(int, int, float)> handler)
{
    block1.onParameterChanged = [handler](int param, float val) { handler(0, param, val); };
    block2.onParameterChanged = [handler](int param, float val) { handler(1, param, val); };
    block3.onParameterChanged = [handler](int param, float val) { handler(2, param, val); };
}
