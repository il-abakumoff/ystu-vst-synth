#include "CustomLookAndFeel.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <JuceHeader.h>

CustomLookAndFeel::CustomLookAndFeel()
{
    setColourScheme(getMidnightColourScheme());
    setDefaultSansSerifTypefaceName("Arial");
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
    juce::Slider& slider)
{
    auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
    auto centreX = (float)x + (float)width * 0.5f;
    auto centreY = (float)y + (float)height * 0.5f;
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Fill
    g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
    g.fillEllipse(rx, ry, rw, rw);

    // Outline
    g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
    g.drawEllipse(rx, ry, rw, rw, 2.0f);

    // Pointer
    juce::Path p;
    auto pointerLength = radius * 0.33f;
    auto pointerThickness = 2.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(juce::Colours::white);
    g.fillPath(p);
}

void CustomLookAndFeel::applyThemeColours()
{
    auto bg = findColour(juce::ResizableWindow::backgroundColourId);
    auto fg = findColour(juce::Label::textColourId); // можно базироваться на цвете схемы

    setColour(juce::TabbedButtonBar::tabTextColourId, fg);
    setColour(juce::TabbedButtonBar::frontTextColourId, fg.brighter(0.3f));

    setColour(juce::TextEditor::textColourId, fg);
    setColour(juce::TextEditor::backgroundColourId, bg.darker(0.3f));

    setColour(juce::ComboBox::textColourId, fg);
    setColour(juce::Label::textColourId, fg);
}

void CustomLookAndFeel::setTheme(int themeId)
{
    if (themeId == 1)
        setColourScheme(getMidnightColourScheme());
    else
        setColourScheme(juce::LookAndFeel_V4::getLightColourScheme());

    applyThemeColours();
}