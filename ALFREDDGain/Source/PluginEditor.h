#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

namespace AlfreddColours
{
    const juce::Colour bgTop    { 0xff15161a };
    const juce::Colour bgBottom { 0xff0b0c0e };
    const juce::Colour panel    { 0xff1d1f24 };
    const juce::Colour track    { 0xff2a2d34 };
    const juce::Colour accent   { 0xff3dd6c6 };
    const juce::Colour warn     { 0xffff9f43 };
    const juce::Colour text     { 0xffe8eaed };
    const juce::Colour textDim  { 0xff7c818c };
}

// Aspecto oscuro y minimalista para la perilla y el botón de bypass.
class AlfreddLookAndFeel : public juce::LookAndFeel_V4
{
public:
    AlfreddLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                           float sliderPos, float startAngle, float endAngle,
                           juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override;
};

// Medidor vertical de pico con caída suave (-60 dB a +6 dB).
class LevelMeter : public juce::Component
{
public:
    void setLevel (float linearPeak);
    void paint (juce::Graphics&) override;

private:
    float shownDb = -100.0f;
};

class AlfreddGainEditor : public juce::AudioProcessorEditor,
                          private juce::Timer
{
public:
    explicit AlfreddGainEditor (AlfreddGainProcessor&);
    ~AlfreddGainEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    AlfreddGainProcessor& processor;

    AlfreddLookAndFeel lookAndFeel;
    juce::Slider       gainSlider;
    juce::ToggleButton bypassButton { "BYPASS" };
    LevelMeter         inMeter, outMeter;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlfreddGainEditor)
};
