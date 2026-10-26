#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>

namespace ParamIDs
{
    inline constexpr const char* gain   = "gain";
    inline constexpr const char* bypass = "bypass";
}

class AlfreddGainProcessor : public juce::AudioProcessor
{
public:
    AlfreddGainProcessor();
    ~AlfreddGainProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "ALFREDD Gain"; }
    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // Picos de nivel (lineal) leídos por la interfaz; la GUI los pone a 0 al leerlos.
    std::atomic<float> inputPeak  { 0.0f };
    std::atomic<float> outputPeak { 0.0f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    std::atomic<float>* gainParam   = nullptr;
    std::atomic<float>* bypassParam = nullptr;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedGain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AlfreddGainProcessor)
};
