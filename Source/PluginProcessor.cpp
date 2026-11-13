#include "PluginProcessor.h"
#include "PluginEditor.h"

RusyaProcessor::RusyaProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    driveParam = apvts.getRawParameterValue ("drive");
}

juce::AudioProcessorValueTreeState::ParameterLayout RusyaProcessor::createLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "drive", 1 }, "Drive",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f), 0.35f));
    return layout;
}

bool RusyaProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    auto out = l.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo())
           && out == l.getMainInputChannelSet();
}

void RusyaProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    oversampling.initProcessing ((size_t) samplesPerBlock);
    oversampling.reset();
    setLatencySamples ((int) std::round (oversampling.getLatencyInSamples()));

    smoothedDrive.reset (sampleRate * 2.0, 0.03);   // smoothing runs at the 2x rate
    smoothedDrive.setCurrentAndTargetValue (driveParam->load());

    for (int i = 0; i < 2; ++i) dcX1[i] = dcY1[i] = 0.f;
}

void RusyaProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numCh = juce::jmin (buffer.getNumChannels(), 2);
    const int numSamples = buffer.getNumSamples();

    smoothedDrive.setTargetValue (driveParam->load());

    juce::dsp::AudioBlock<float> block (buffer);
    auto sub = block.getSubsetChannelBlock (0, (size_t) numCh);
    auto osBlock = oversampling.processSamplesUp (sub);

    const size_t osN = osBlock.getNumSamples();
    for (size_t i = 0; i < osN; ++i)
    {
        const float d = smoothedDrive.getNextValue();

        // 0..1 -> 0..40 dB of pre-gain
        const float g    = juce::Decibels::decibelsToGain (d * 40.0f);
        const float bias = 0.12f * d;                          // asymmetry = even harmonics
        const float comp = 0.6f / std::tanh (0.6f * g);        // loudness compensation
        const float dcB  = std::tanh (bias);

        for (int ch = 0; ch < numCh; ++ch)
        {
            auto* x = osBlock.getChannelPointer ((size_t) ch);
            x[i] = (std::tanh (g * x[i] + bias) - dcB) * comp;
        }
    }

    oversampling.processSamplesDown (sub);

    // DC blocker (~20 Hz), because of the asymmetry
    for (int ch = 0; ch < numCh; ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        for (int i = 0; i < numSamples; ++i)
        {
            const float in = x[i];
            const float out = in - dcX1[ch] + 0.995f * dcY1[ch];
            dcX1[ch] = in; dcY1[ch] = out;
            x[i] = out;
        }
    }
}

juce::AudioProcessorEditor* RusyaProcessor::createEditor() { return new RusyaEditor (*this); }

void RusyaProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary (*xml, dest);
}

void RusyaProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new RusyaProcessor(); }
