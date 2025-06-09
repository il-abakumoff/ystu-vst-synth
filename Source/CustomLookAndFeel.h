#pragma once
#include <juce_gui_extra/juce_gui_extra.h>
#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel();
    void applyThemeColours();

    void setTheme(int themeId);

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
        juce::Slider& slider) override;
};

