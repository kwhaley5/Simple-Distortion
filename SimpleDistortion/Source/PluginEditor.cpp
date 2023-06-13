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
    //allow me to overide knobs and create meters
    setLookAndFeel(&laf);

    //create level Meters
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
    auto bounds = getLocalBounds();
    auto grad = juce::ColourGradient::ColourGradient(juce::Colour(186u, 34u, 34u), bounds.toFloat().getTopLeft(), juce::Colour(186u, 34u, 34u), bounds.toFloat().getBottomRight(), false);
    grad.addColour(.5f, juce::Colours::transparentBlack);
    //grad.addColour(.7f, juce::Colour(186u, 34u, 34u));

    g.setGradientFill(grad);
    //g.fillAll (juce::Colour(50u,30u,30u)); //create background
    g.fillAll();
    

    auto inputMeter = bounds.removeFromLeft(bounds.getWidth() * .125);
    inputMeter = inputMeter.removeFromBottom(bounds.getHeight() * .1);

    auto outputMeter = bounds.removeFromRight(bounds.getWidth() * .14);
    outputMeter = outputMeter.removeFromBottom(bounds.getHeight() * .1);

    //Making space for logo and text without distorting
    auto infoSpace = bounds.removeFromTop(bounds.getHeight() * .35);
    auto logoSpace = infoSpace.removeFromLeft(bounds.getWidth() * .425);
    auto textSpace = infoSpace.removeFromRight(bounds.getWidth() * .425);

    //Make space for Text. Note that knobs are defined by the APVTS create layout in the plugin processor, and they can be edited in resized
    auto driveArea = bounds.removeFromLeft(bounds.getWidth() * .25);
    driveArea = driveArea.removeFromBottom(bounds.getHeight() * .4);

    auto rangeArea = bounds.removeFromLeft(bounds.getWidth() * .33);
    rangeArea = rangeArea.removeFromBottom(bounds.getHeight() * .4);

    auto blendArea = bounds.removeFromLeft(bounds.getWidth() * .5);
    blendArea = blendArea.removeFromBottom(bounds.getHeight() * .4);

    auto volumeArea = bounds.removeFromLeft(bounds.getWidth());
    volumeArea = volumeArea.removeFromBottom(bounds.getHeight() * .4);

    //add logo
    logo = juce::ImageCache::getFromMemory(BinaryData::KITIK_LOGO_NO_BKGD_png, BinaryData::KITIK_LOGO_NO_BKGD_pngSize);
    g.drawImage(logo, infoSpace.toFloat());
    //g.drawRect(infoSpace, 2.f);

    //Add Text
    newFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::OFFSHORE_TTF, BinaryData::OFFSHORE_TTFSize));
    g.setColour (juce::Colours::whitesmoke);
    g.setFont(newFont);
    g.setFont(30.f);
    g.drawFittedText("Simple", logoSpace, juce::Justification::centredRight, 1);
    g.drawFittedText("Distortion", textSpace, juce::Justification::centredLeft, 1);

    g.setFont(juce::Font::Font()); //Return to original Font
    g.setFont(15.f);
    g.drawFittedText("Drive", driveArea, juce::Justification::centred, 1);
    g.drawFittedText("Range", rangeArea, juce::Justification::centred, 1);
    g.drawFittedText("Blend", blendArea, juce::Justification::centred, 1);
    g.drawFittedText("Volume", volumeArea, juce::Justification::centred, 1);
    g.drawFittedText("In", inputMeter, juce::Justification::centred, 1);
    g.drawFittedText("Out", outputMeter, juce::Justification::centred, 1);

}

void SimpleDistortionAudioProcessorEditor::resized()
{
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


