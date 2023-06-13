/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDistortionAudioProcessor::SimpleDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{

}

SimpleDistortionAudioProcessor::~SimpleDistortionAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    //This is the preprocessor for audio running through. I don't think I'll need this, we shall see.
    //Didn't use this, still need to figure out what this is for in the future, because I bet reverb and delay are going to need it
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
}

void SimpleDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //rms levels for meters
    rmsLevelLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    rmsLevelRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

    //added to fix graphical bug, rms levels when no music was playing was below -60
    if (rmsLevelLeft < -60) {
        rmsLevelLeft = -60;
    }
    if (rmsLevelRight < -60) {
        rmsLevelRight = -60;
    }

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    //get the paremeters, that will be attached to the knobs, and do something with it.
    auto drive = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Drive"));
    auto range = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Range"));
    auto blend = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Blend"));
    auto volume = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("Volume"));

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); sample++) {

            auto cleanSig = *channelData; //gets the clean signal

            *channelData *= drive->get() * range->get(); //the get method and pointing at the data achive the same result

            *channelData = ((((2.f / juce::float_Pi) * tanh(*channelData) * (1.f - * blend)) + (cleanSig * *blend)) / 2) * *volume; //clips back down to one, and mixes the cleansig, if present

            channelData++;
        }
    }

    //Looks like it works...
    rmsOutLevelLeft = juce::Decibels::gainToDecibels(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    rmsOutLevelRight = juce::Decibels::gainToDecibels(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

    //added to fix graphical bug, rms levels when no music was playing was below -60
    if (rmsOutLevelLeft < -60) {
        rmsOutLevelLeft = -60;
    }
    if (rmsOutLevelRight < -60) {
        rmsOutLevelRight = -60;
    }

}

//==============================================================================
bool SimpleDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleDistortionAudioProcessor::createEditor()
{
    return new SimpleDistortionAudioProcessorEditor (*this);
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    // Use this to get state information, [STEP 4A]
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void SimpleDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    //And set the state [STEP 4B]
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
    }
}

float SimpleDistortionAudioProcessor::getRMSValue(int channel) //get function to provide the rms levels for the editor
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0) {
        return rmsLevelLeft;
    }
    else if (channel == 1) {
        return rmsLevelRight;
    }
    else {
        return 0.0f;
    }
}

float SimpleDistortionAudioProcessor::getOutRMSValue(int channel) //same as above
{
    jassert(channel == 0 || channel == 1);
    if (channel == 0) {
        return rmsOutLevelLeft;
    }
    else if (channel == 1) {
        return rmsOutLevelRight;
    }
    else {
        return 0.0f;
    }
}

//This is where we create the actual layout [STEP 2]
juce::AudioProcessorValueTreeState::ParameterLayout SimpleDistortionAudioProcessor::createParamLayout() 
{
    APVTS::ParameterLayout layout;

    using namespace juce;

    auto driveRange = NormalisableRange<float>(1, 10, .01, 1);
    auto blendRange = NormalisableRange<float>(.01, 1, .01, 1);
    auto volumeRange = NormalisableRange<float>(0, 1, .01, 1);

    layout.add(std::make_unique<AudioParameterFloat>("Drive", "Drive", driveRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("Range", "Range", blendRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("Blend", "Blend", blendRange, 0));
    layout.add(std::make_unique<AudioParameterFloat>("Volume", "Volume", volumeRange, 0));

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDistortionAudioProcessor();
}
