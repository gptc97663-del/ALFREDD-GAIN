#include "PluginProcessor.h"
#include "PluginEditor.h"

AlfreddGainProcessor::AlfreddGainProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    gainParam   = apvts.getRawParameterValue (ParamIDs::gain);
    bypassParam = apvts.getRawParameterValue (ParamIDs::bypass);
}

juce::AudioProcessorValueTreeState::ParameterLayout AlfreddGainProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { ParamIDs::gain, 1 },
        "Gain",
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.01f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel ("dB")));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { ParamIDs::bypass, 1 },
        "Bypass",
        false));

    return layout;
}

bool AlfreddGainProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto& in  = layouts.getMainInputChannelSet();
    const auto& out = layouts.getMainOutputChannelSet();

    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo())
        return false;

    return in == out;
}

void AlfreddGainProcessor::prepareToPlay (double sampleRate, int)
{
    smoothedGain.reset (sampleRate, 0.02); // rampa de 20 ms: sin clics al mover la perilla o activar bypass
    const bool bypassed = bypassParam->load() > 0.5f;
    smoothedGain.setCurrentAndTargetValue (bypassed ? 1.0f
                                                    : juce::Decibels::decibelsToGain (gainParam->load()));
}

void AlfreddGainProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numIn      = getTotalNumInputChannels();
    const int numOut     = getTotalNumOutputChannels();

    for (int ch = numIn; ch < numOut; ++ch)
        buffer.clear (ch, 0, numSamples);

    // Nivel de entrada
    float inPk = 0.0f;
    for (int ch = 0; ch < juce::jmin (numIn, buffer.getNumChannels()); ++ch)
        inPk = juce::jmax (inPk, buffer.getMagnitude (ch, 0, numSamples));
    inputPeak.store (juce::jmax (inputPeak.load(), inPk));

    // Ganancia (con bypass = ganancia unidad, suavizado)
    const bool bypassed = bypassParam->load() > 0.5f;
    const float target  = bypassed ? 1.0f : juce::Decibels::decibelsToGain (gainParam->load());
    smoothedGain.setTargetValue (target);

    if (smoothedGain.isSmoothing())
    {
        auto** data = buffer.getArrayOfWritePointers();
        const int numCh = buffer.getNumChannels();

        for (int s = 0; s < numSamples; ++s)
        {
            const float g = smoothedGain.getNextValue();
            for (int ch = 0; ch < numCh; ++ch)
                data[ch][s] *= g;
        }
    }
    else
    {
        buffer.applyGain (smoothedGain.getTargetValue());
    }

    // Nivel de salida
    float outPk = 0.0f;
    for (int ch = 0; ch < juce::jmin (numOut, buffer.getNumChannels()); ++ch)
        outPk = juce::jmax (outPk, buffer.getMagnitude (ch, 0, numSamples));
    outputPeak.store (juce::jmax (outputPeak.load(), outPk));
}

juce::AudioProcessorEditor* AlfreddGainProcessor::createEditor()
{
    return new AlfreddGainEditor (*this);
}

void AlfreddGainProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary (*xml, destData);
}

void AlfreddGainProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AlfreddGainProcessor();
}
