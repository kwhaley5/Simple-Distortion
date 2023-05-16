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

    //this is where you make the slider types and make them visible. MAKE SURE TO MAKE BOUNDING BOXES OTHERWISE THEY WILL NOT SHOW UP
    drive.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    drive.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20); //Still need to look into textbox.
    addAndMakeVisible(drive);

    range.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    range.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(range);

    blend.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    blend.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(blend);

    volume.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    volume.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(volume);

    setSize (600, 200);
}

SimpleDistortionAudioProcessorEditor::~SimpleDistortionAudioProcessorEditor()
{
}

//==============================================================================
void SimpleDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto bounds = getLocalBounds();
    auto driveArea = bounds.removeFromLeft(bounds.getWidth() * .25);
    auto rangeArea = bounds.removeFromLeft(bounds.getWidth() * .33);
    auto blendArea = bounds.removeFromLeft(bounds.getWidth() * .5);
    auto volumeArea = bounds.removeFromLeft(bounds.getWidth());

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText("Drive", driveArea, juce::Justification::centred, 1);
    g.drawFittedText("Range", rangeArea, juce::Justification::centred, 1);
    g.drawFittedText("Blend", blendArea, juce::Justification::centred, 1);
    g.drawFittedText("Volume", volumeArea, juce::Justification::centred, 1);
}

void SimpleDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();

    auto driveArea = bounds.removeFromLeft(bounds.getWidth() * .25);
    drive.setBounds(driveArea);

    auto rangeArea = bounds.removeFromLeft(bounds.getWidth() * .33);
    range.setBounds(rangeArea);

    auto blendArea = bounds.removeFromLeft(bounds.getWidth() * .5);
    blend.setBounds(blendArea);

    auto volumeArea = bounds.removeFromLeft(bounds.getWidth());
    volume.setBounds(volumeArea);
}
