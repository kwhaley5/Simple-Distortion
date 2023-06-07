/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class SimpleDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    SimpleDistortionAudioProcessorEditor (SimpleDistortionAudioProcessor&);
    ~SimpleDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    struct Laf : juce::LookAndFeel_V4 {
        
        Laf() {

        }

        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
            float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
        {
            using namespace juce;

            auto unfill = Colours::black;
            auto fill = Colours::cyan;

            auto boundsFull = Rectangle<int>(x, y, width, height).toFloat();
            auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

            auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
            auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            auto lineW = jmin(8.0f, radius * 0.5f);
            auto arcRadius = radius - lineW * 0.5f;

            auto rootTwo = MathConstants<float>::sqrt2;

            Path backgroundArc;
            backgroundArc.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius,
                arcRadius,
                0.0f,
                rotaryStartAngle,
                rotaryEndAngle,
                true);

            g.setColour(unfill);
            g.strokePath(backgroundArc, PathStrokeType(lineW/2, PathStrokeType::curved, PathStrokeType::rounded));

            if(slider.isEnabled())
            {
                Path valueArc;
                valueArc.addCentredArc(bounds.getCentreX(),
                    bounds.getCentreY(),
                    arcRadius,
                    arcRadius,
                    0.0f,
                    rotaryStartAngle,
                    toAngle,
                    true);

                g.setColour(fill);
                g.strokePath(valueArc, PathStrokeType(lineW/2, PathStrokeType::curved, PathStrokeType::rounded));
            }

            auto thumbWidth = lineW * 2.0f;
            Point<float> thumbPoint(bounds.getCentreX() + radius/rootTwo * std::cos(toAngle - MathConstants<float>::halfPi),
                bounds.getCentreY() + radius/rootTwo * std::sin(toAngle - MathConstants<float>::halfPi));

            Point<float> shortLine(bounds.getCentreX() + (arcRadius - (arcRadius/ rootTwo)) * std::cos(toAngle - MathConstants<float>::halfPi),
                bounds.getCentreY() + (arcRadius - (arcRadius/ rootTwo)) * std::sin(toAngle - MathConstants<float>::halfPi));

            //make circle

            g.setColour(fill);
            g.fillRoundedRectangle(boundsFull.getCentreX() - (radius * rootTwo / 2), boundsFull.getCentreY() - (radius * rootTwo / 2), radius * rootTwo, radius * rootTwo, radius * .7);

            //make dial line
            g.setColour(Colours::white); //need to set gradient so it actually looks decent

            g.drawLine(shortLine.getX(), shortLine.getY(), thumbPoint.getX(), thumbPoint.getY(), lineW / 2);
            //g.drawLine(bounds.getCentreX(), bounds.getCentreY(), thumbPoint.getX(), thumbPoint.getY(), lineW / 2)
            

        }
    };

private:


    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleDistortionAudioProcessor& audioProcessor;
    Laf laf;

    //create the objects you want to control here. I declared these as sliders, and these are the types they can be: https://docs.juce.com/master/classSlider.html 
    juce::Slider drive, range, blend, volume;

 

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    //This allows us to attach our layout to these sliders
    Attachment driveAT, rangeAT, blendAT, volumeAT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDistortionAudioProcessorEditor)
};
