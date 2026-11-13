#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

// Soviet-style knob: dark body, red ring, gold ticks, gold pointer, red star in the middle
class SovietLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h,
                           float pos, float startAngle, float endAngle, juce::Slider&) override;
};

class RusyaEditor : public juce::AudioProcessorEditor
{
public:
    explicit RusyaEditor (RusyaProcessor&);
    ~RusyaEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    RusyaProcessor& proc;
    SovietLookAndFeel laf;
    juce::Slider knob;
    juce::AudioProcessorValueTreeState::SliderAttachment attachment;

    juce::Image moscow, kddezis, rusyaLogo, sukaBlyat, bottle, vnzla;
    juce::Rectangle<int> knobArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RusyaEditor)
};
