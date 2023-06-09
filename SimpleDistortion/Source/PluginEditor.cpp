/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

SimpleDistortionAudioProcessorEditor::SimpleDistortionAudioProcessorEditor (SimpleDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), driveAT(audioProcessor.apvts, "Drive", drive), 
    rangeAT(audioProcessor.apvts, "Range", range), 
    blendAT(audioProcessor.apvts, "Blend", blend), 
    volumeAT(audioProcessor.apvts, "Volume", volume) //better to use a map as we get more parameters
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    setLookAndFeel(&laf);

    addAndMakeVisible(meterL);
    addAndMakeVisible(meterR);
    addAndMakeVisible(outMeterL);
    addAndMakeVisible(outMeterR);

    //this is where you make the slider types and make them visible. MAKE SURE TO MAKE BOUNDING BOXES OTHERWISE THEY WILL NOT SHOW UP
    drive.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    drive.setTextBoxStyle (juce::Slider::NoTextBox, false, 100, 20);
    addAndMakeVisible(drive);

    range.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    range.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    addAndMakeVisible(range);

    blend.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    blend.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    addAndMakeVisible(blend);

    volume.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volume.setTextBoxStyle(juce::Slider::NoTextBox, false, 100, 20);
    addAndMakeVisible(volume);

    setSize (800, 250);

    startTimerHz(24); //render adjustment at 24 hz
}

SimpleDistortionAudioProcessorEditor::~SimpleDistortionAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void SimpleDistortionAudioProcessorEditor::timerCallback()
{
    //these get our rms level, and the set level function tells you how much of the rect you want
    meterL.setLevel(audioProcessor.getRMSValue(0));
    meterR.setLevel(audioProcessor.getRMSValue(1));

    meterL.repaint();
    meterR.repaint();

    outMeterL.setLevel(audioProcessor.getOutRMSValue(0));
    outMeterR.setLevel(audioProcessor.getOutRMSValue(1));

    outMeterL.repaint();
    outMeterR.repaint();
}

//==============================================================================
void SimpleDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    //This is where I'm making the text visible
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto bounds = getLocalBounds();

    auto inputMeter = bounds.removeFromLeft(bounds.getWidth() * .125);
    inputMeter = inputMeter.removeFromBottom(bounds.getHeight() * .1);

    auto outputMeter = bounds.removeFromRight(bounds.getWidth() * .14);
    outputMeter = outputMeter.removeFromBottom(bounds.getHeight() * .1);

    auto logoSpace = bounds.removeFromTop(bounds.getHeight() * .2);

    auto driveArea = bounds.removeFromLeft(bounds.getWidth() * .25);
    driveArea = driveArea.removeFromBottom(bounds.getHeight() * .4);

    auto rangeArea = bounds.removeFromLeft(bounds.getWidth() * .33);
    rangeArea = rangeArea.removeFromBottom(bounds.getHeight() * .4);

    auto blendArea = bounds.removeFromLeft(bounds.getWidth() * .5);
    blendArea = blendArea.removeFromBottom(bounds.getHeight() * .4);

    auto volumeArea = bounds.removeFromLeft(bounds.getWidth());
    volumeArea = volumeArea.removeFromBottom(bounds.getHeight() * .4);

    g.setColour (juce::Colours::whitesmoke);
    g.setFont (15.0f);
    g.drawFittedText("Drive", driveArea, juce::Justification::centred, 1);
    g.drawFittedText("Range", rangeArea, juce::Justification::centred, 1);
    g.drawFittedText("Blend", blendArea, juce::Justification::centred, 1);
    g.drawFittedText("Volume", volumeArea, juce::Justification::centred, 1);
    g.drawFittedText("In", inputMeter, juce::Justification::centred, 1);
    g.drawFittedText("Out", outputMeter, juce::Justification::centred, 1);
}

void SimpleDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    //this is the actual dial/meter

    auto bounds = getLocalBounds();

    auto inputMeter = bounds.removeFromLeft(bounds.getWidth() * .125);
    auto meterLSide = inputMeter.removeFromLeft(inputMeter.getWidth() * .5);
    meterL.setBounds(meterLSide);
    meterR.setBounds(inputMeter);

    auto outputMeter = bounds.removeFromRight(bounds.getWidth() * .14);
    auto outMeterLSide = outputMeter.removeFromLeft(outputMeter.getWidth() * .5);
    outMeterL.setBounds(outMeterLSide);
    outMeterR.setBounds(outputMeter);

    auto logoSpace = bounds.removeFromTop(bounds.getHeight() * .2);

    auto driveArea = bounds.removeFromLeft(bounds.getWidth() * .25);
    drive.setBounds(driveArea);

    auto rangeArea = bounds.removeFromLeft(bounds.getWidth() * .33);
    range.setBounds(rangeArea);

    auto blendArea = bounds.removeFromLeft(bounds.getWidth() * .5);
    blend.setBounds(blendArea);

    auto volumeArea = bounds.removeFromLeft(bounds.getWidth());
    volume.setBounds(volumeArea);
}


