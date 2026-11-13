#include "PluginEditor.h"
#include "BinaryData.h"

namespace
{
    const juce::Colour sovietRed   { 0xffcc0000 };
    const juce::Colour darkRed     { 0xff5a0000 };
    const juce::Colour gold        { 0xffffd23f };
    const juce::Colour cream       { 0xffeee4cd };

    juce::Image load (const void* data, int size) { return juce::ImageCache::getFromMemory (data, size); }

    // draw image fitted inside a box, centered
    void drawFit (juce::Graphics& g, const juce::Image& img, juce::Rectangle<float> box, float opacity = 1.0f)
    {
        if (img.isNull()) return;
        g.setOpacity (opacity);
        g.drawImage (img, box, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize);
        g.setOpacity (1.0f);
    }
}

//==============================================================================
void SovietLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                          float pos, float startAngle, float endAngle, juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (6.0f);
    const float size = juce::jmin (bounds.getWidth(), bounds.getHeight());
    auto r = juce::Rectangle<float> (size, size).withCentre (bounds.getCentre());
    const auto c = r.getCentre();
    const float angle = startAngle + pos * (endAngle - startAngle);

    // shadow
    g.setColour (juce::Colours::black.withAlpha (0.55f));
    g.fillEllipse (r.translated (0, 6));

    // outer gold ring + ticks
    g.setColour (gold);
    g.fillEllipse (r);
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (float) i / 10.0f * (endAngle - startAngle);
        const float r1 = size * 0.5f + 1.0f, r2 = size * 0.5f - 11.0f;
        g.setColour (darkRed);
        g.drawLine (c.x + std::sin (a) * r1, c.y - std::cos (a) * r1,
                    c.x + std::sin (a) * r2, c.y - std::cos (a) * r2, i % 5 == 0 ? 4.0f : 2.0f);
    }

    // red progress arc
    auto arcR = r.reduced (16.0f);
    juce::Path arc;
    arc.addCentredArc (c.x, c.y, arcR.getWidth() * 0.5f, arcR.getHeight() * 0.5f, 0.0f, startAngle, angle, true);
    g.setColour (sovietRed);
    g.strokePath (arc, juce::PathStrokeType (9.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // body
    auto body = r.reduced (32.0f);
    g.setGradientFill (juce::ColourGradient (juce::Colour (0xff3a3a3a), body.getX(), body.getY(),
                                             juce::Colour (0xff0d0d0d), body.getRight(), body.getBottom(), false));
    g.fillEllipse (body);
    g.setColour (gold.withAlpha (0.9f));
    g.drawEllipse (body, 2.5f);

    // red star
    juce::Path star;
    star.addStar (c, 5, body.getWidth() * 0.11f, body.getWidth() * 0.27f, -juce::MathConstants<float>::halfPi + angle);
    g.setColour (sovietRed);
    g.fillPath (star);
    g.setColour (gold);
    g.strokePath (star, juce::PathStrokeType (2.0f));

    // pointer
    const float p1 = body.getWidth() * 0.5f - 4.0f, p2 = body.getWidth() * 0.5f - 24.0f;
    g.setColour (gold);
    g.drawLine (c.x + std::sin (angle) * p1, c.y - std::cos (angle) * p1,
                c.x + std::sin (angle) * p2, c.y - std::cos (angle) * p2, 6.0f);
}

//==============================================================================
RusyaEditor::RusyaEditor (RusyaProcessor& p)
    : AudioProcessorEditor (&p), proc (p), attachment (p.apvts, "drive", knob)
{
    moscow    = load (BinaryData::moscow_jpg,              BinaryData::moscow_jpgSize);
    kddezis   = load (BinaryData::kddezis_logo_png,        BinaryData::kddezis_logo_pngSize);
    rusyaLogo = load (BinaryData::rusya_mixtape_logo_png,  BinaryData::rusya_mixtape_logo_pngSize);
    sukaBlyat = load (BinaryData::suka_blyat_png,          BinaryData::suka_blyat_pngSize);
    bottle    = load (BinaryData::bottle_png,              BinaryData::bottle_pngSize);
    vnzla     = load (BinaryData::vnzla_png,               BinaryData::vnzla_pngSize);

    knob.setLookAndFeel (&laf);
    knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    knob.setRotaryParameters (juce::degreesToRadians (225.0f), juce::degreesToRadians (495.0f), true);
    knob.setDoubleClickReturnValue (true, 0.35);
    knob.setTooltip ("Drive");
    knob.onValueChange = [this] { repaint(); };   // bottle tilts + readout follow the knob
    addAndMakeVisible (knob);

    setSize (560, 720);
}

RusyaEditor::~RusyaEditor() { knob.setLookAndFeel (nullptr); }

void RusyaEditor::resized()
{
    knobArea = { 140, 300, 280, 280 };
    knob.setBounds (knobArea);
}

void RusyaEditor::paint (juce::Graphics& g)
{
    const auto area = getLocalBounds().toFloat();

    // --- background: Moscow, darkened and tinted red
    g.fillAll (juce::Colours::black);
    if (moscow.isValid())
    {
        g.drawImage (moscow, area, juce::RectanglePlacement::fillDestination | juce::RectanglePlacement::centred);
    }
    g.setGradientFill (juce::ColourGradient (juce::Colours::black.withAlpha (0.55f), 0, 0,
                                             darkRed.withAlpha (0.90f), 0, area.getBottom(), false));
    g.fillRect (area);

    // --- watermark RUSYA MIXTAPE behind the knob
    drawFit (g, rusyaLogo, { 40.0f, 270.0f, 480.0f, 340.0f }, 0.13f);

    // --- header: kddezis y el quepaso / RUSYA MIXTAPE sign
    drawFit (g, kddezis, { 60.0f, 10.0f, 440.0f, 270.0f });

    // --- bottle (tilts with the knob like you're pouring it)
    if (bottle.isValid())
    {
        const float v = (float) knob.valueToProportionOfLength (knob.getValue());
        const float tilt = juce::jmap (v, -0.35f, 0.55f);                 // radians
        const float bh = 330.0f;
        const float s = bh / (float) bottle.getHeight();
        const float cx = 70.0f, cy = 440.0f;
        g.drawImageTransformed (bottle,
            juce::AffineTransform::scale (s)
                .translated (cx - bottle.getWidth() * s * 0.5f, cy - bh * 0.5f)
                .rotated (tilt, cx, cy + bh * 0.5f));
    }

    // --- SUKA BLYAT as the knob label + readout
    drawFit (g, sukaBlyat, { 130.0f, 585.0f, 300.0f, 54.0f });
    g.setColour (gold);
    g.setFont (juce::Font (juce::FontOptions (18.0f, juce::Font::bold)));
    g.drawText (juce::String (juce::roundToInt (knob.getValue() * 100.0)) + " %",
                getLocalBounds().withY (640).withHeight (24).withTrimmedLeft (0), juce::Justification::centredTop);

    // --- bottom banner: red stripe with the plugin name
    auto banner = juce::Rectangle<float> (0.0f, 664.0f, area.getWidth(), 56.0f);
    g.setColour (sovietRed);
    g.fillRect (banner);
    g.setColour (gold);
    g.fillRect (banner.removeFromTop (3.0f));
    g.setColour (cream);
    g.setFont (juce::Font (juce::FontOptions (30.0f, juce::Font::bold)));
    g.drawText ("MAMAGUEVO", banner.withTrimmedLeft (70.0f).withTrimmedBottom (14.0f), juce::Justification::centred);
    g.setFont (juce::Font (juce::FontOptions (12.0f, juce::Font::plain)));
    g.setColour (gold);
    g.drawText (juce::CharPointer_UTF8 ("\xd0\x9c\xd0\x90\xd0\x9c\xd0\x90\xd0\x93\xd0\xa3\xd0\x95\xd0\x92\xd0\x9e  \xc2\xb7  SATURATION  \xc2\xb7  VST3"),
                banner.withTrimmedLeft (70.0f).withTop (banner.getBottom() - 20.0f), juce::Justification::centred);

    // --- VNZLA badge
    if (vnzla.isValid())
        g.drawImage (vnzla, juce::Rectangle<float> (10.0f, 666.0f, 52.0f, 52.0f),
                     juce::RectanglePlacement::centred);

    // --- gold frame
    g.setColour (gold);
    g.drawRect (area, 3.0f);
}
