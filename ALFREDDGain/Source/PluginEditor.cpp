#include "PluginEditor.h"

using namespace AlfreddColours;

//==============================================================================
AlfreddLookAndFeel::AlfreddLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId,       text);
    setColour (juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId,  accent.withAlpha (0.4f));
    setColour (juce::CaretComponent::caretColourId,     accent);
}

void AlfreddLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float startAngle, float endAngle,
                                           juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (8.0f);
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto  centre = bounds.getCentre();
    const float arcR   = radius - 4.0f;
    const float angle  = startAngle + sliderPos * (endAngle - startAngle);
    const float zeroPos   = (float) slider.valueToProportionOfLength (0.0);
    const float zeroAngle = startAngle + zeroPos * (endAngle - startAngle);

    // Pista
    juce::Path trackPath;
    trackPath.addCentredArc (centre.x, centre.y, arcR, arcR, 0.0f, startAngle, endAngle, true);
    g.setColour (track);
    g.strokePath (trackPath, juce::PathStrokeType (6.0f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

    // Arco de valor (desde 0 dB hasta el valor actual)
    juce::Path valuePath;
    valuePath.addCentredArc (centre.x, centre.y, arcR, arcR, 0.0f,
                             juce::jmin (zeroAngle, angle), juce::jmax (zeroAngle, angle), true);
    g.setColour (slider.isEnabled() ? accent : textDim);
    g.strokePath (valuePath, juce::PathStrokeType (6.0f, juce::PathStrokeType::curved,
                                                   juce::PathStrokeType::rounded));

    // Cuerpo de la perilla
    const float bodyR = arcR - 14.0f;
    juce::ColourGradient body (juce::Colour (0xff2c2f36), centre.x, centre.y - bodyR,
                               juce::Colour (0xff17181c), centre.x, centre.y + bodyR, false);
    g.setGradientFill (body);
    g.fillEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f);
    g.setColour (juce::Colour (0xff3a3e47));
    g.drawEllipse (centre.x - bodyR, centre.y - bodyR, bodyR * 2.0f, bodyR * 2.0f, 1.0f);

    // Indicador
    const float inner = bodyR * 0.45f;
    const float outer = bodyR * 0.92f;
    juce::Path pointer;
    pointer.startNewSubPath (centre.x + inner * std::sin (angle), centre.y - inner * std::cos (angle));
    pointer.lineTo          (centre.x + outer * std::sin (angle), centre.y - outer * std::cos (angle));
    g.setColour (accent);
    g.strokePath (pointer, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved,
                                                 juce::PathStrokeType::rounded));
}

void AlfreddLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                           bool highlighted, bool)
{
    auto b = button.getLocalBounds().toFloat().reduced (1.0f);
    const bool on = button.getToggleState();

    g.setColour (on ? warn.withAlpha (0.18f) : panel);
    g.fillRoundedRectangle (b, 6.0f);

    g.setColour (on ? warn : (highlighted ? textDim : track));
    g.drawRoundedRectangle (b, 6.0f, 1.2f);

    g.setColour (on ? warn : textDim);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText (button.getButtonText(), b.toNearestInt(), juce::Justification::centred);
}

//==============================================================================
void LevelMeter::setLevel (float linearPeak)
{
    const float db = juce::Decibels::gainToDecibels (linearPeak, -100.0f);

    if (db > shownDb)
        shownDb = db;
    else
        shownDb = juce::jmax (db, shownDb - 1.5f); // caída ~45 dB/s a 30 Hz

    repaint();
}

void LevelMeter::paint (juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();

    g.setColour (panel);
    g.fillRoundedRectangle (b, 3.0f);

    auto proportion = [] (float db)
    {
        return juce::jmap (juce::jlimit (-60.0f, 6.0f, db), -60.0f, 6.0f, 0.0f, 1.0f);
    };

    const float prop = proportion (shownDb);
    if (prop > 0.002f)
    {
        auto inner = b.reduced (1.5f);
        auto fill  = inner.withTrimmedTop (inner.getHeight() * (1.0f - prop));

        juce::ColourGradient grad (juce::Colour (0xffff5252), 0.0f, inner.getY(),
                                   accent,                    0.0f, inner.getBottom(), false);
        grad.addColour (0.09, juce::Colour (0xffffd24a));
        grad.addColour (0.22, accent);
        g.setGradientFill (grad);
        g.fillRoundedRectangle (fill, 2.0f);
    }

    // Marca de 0 dB
    const float zeroY = b.getBottom() - b.getHeight() * proportion (0.0f);
    g.setColour (text.withAlpha (0.35f));
    g.drawHorizontalLine ((int) zeroY, b.getX(), b.getRight());
}

//==============================================================================
AlfreddGainEditor::AlfreddGainEditor (AlfreddGainProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&lookAndFeel);
    setSize (420, 360);
    setResizable (false, false);

    gainSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    gainSlider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f,
                                    juce::MathConstants<float>::pi * 2.75f, true);
    gainSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 110, 28);
    gainSlider.setTextValueSuffix (" dB");
    gainSlider.setNumDecimalPlacesToDisplay (1);
    gainSlider.setDoubleClickReturnValue (true, 0.0);
    gainSlider.setLookAndFeel (&lookAndFeel);
    addAndMakeVisible (gainSlider);

    bypassButton.setLookAndFeel (&lookAndFeel);
    addAndMakeVisible (bypassButton);

    addAndMakeVisible (inMeter);
    addAndMakeVisible (outMeter);

    gainAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
                           processor.apvts, ParamIDs::gain, gainSlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
                           processor.apvts, ParamIDs::bypass, bypassButton);

    startTimerHz (30);
}

AlfreddGainEditor::~AlfreddGainEditor()
{
    stopTimer();
    gainSlider.setLookAndFeel (nullptr);
    bypassButton.setLookAndFeel (nullptr);
    setLookAndFeel (nullptr);
}

void AlfreddGainEditor::timerCallback()
{
    inMeter.setLevel  (processor.inputPeak.exchange  (0.0f));
    outMeter.setLevel (processor.outputPeak.exchange (0.0f));
}

void AlfreddGainEditor::paint (juce::Graphics& g)
{
    g.setGradientFill (juce::ColourGradient (bgTop, 0.0f, 0.0f, bgBottom, 0.0f, (float) getHeight(), false));
    g.fillAll();

    auto area = getLocalBounds().reduced (20);

    // Marca
    auto header = area.removeFromTop (46);
    g.setColour (text);
    g.setFont (juce::FontOptions (28.0f, juce::Font::bold));
    g.drawText ("A L F R E D D", header.removeFromLeft (230), juce::Justification::centredLeft);

    g.setColour (accent);
    g.setFont (juce::FontOptions (13.0f, juce::Font::bold));
    g.drawText ("G A I N", header, juce::Justification::centredRight);

    g.setColour (accent.withAlpha (0.6f));
    g.fillRect (area.getX(), area.getY() + 2, 36, 2);

    // Etiquetas de medidores
    g.setColour (textDim);
    g.setFont (juce::FontOptions (11.0f, juce::Font::bold));
    g.drawText ("IN",  inMeter.getX()  - 8, inMeter.getBottom()  + 4, inMeter.getWidth()  + 16, 14, juce::Justification::centred);
    g.drawText ("OUT", outMeter.getX() - 8, outMeter.getBottom() + 4, outMeter.getWidth() + 16, 14, juce::Justification::centred);
}

void AlfreddGainEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    area.removeFromTop (56); // cabecera

    auto bottom = area.removeFromBottom (36);
    bypassButton.setBounds (bottom.withSizeKeepingCentre (120, 32));

    area.removeFromBottom (10);

    auto meterColumnL = area.removeFromLeft (26);
    auto meterColumnR = area.removeFromRight (26);

    inMeter.setBounds  (meterColumnL.withTrimmedBottom (22));
    outMeter.setBounds (meterColumnR.withTrimmedBottom (22));

    gainSlider.setBounds (area.reduced (6));
}
