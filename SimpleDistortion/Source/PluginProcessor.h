/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class SimpleDistortionAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    SimpleDistortionAudioProcessor();
    ~SimpleDistortionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    float getRMSValue(int channel);
    float getOutRMSValue(int channel);

    //This allows you to connect the buttons on your GUI to actual change in the audio [STEP 1]
    using APVTS = juce::AudioProcessorValueTreeState;
    static APVTS::ParameterLayout createParamLayout();

    //creates apvts layout, but does not create APVTS
    APVTS apvts{ *this, nullptr, "parameters", createParamLayout() };

private:
    //This is how we create all the pointers to the actual values of our parameters, allows for cached values, runs faster [STEP 3]
    juce::AudioParameterFloat* drive { nullptr };
    juce::AudioParameterFloat* range { nullptr };
    juce::AudioParameterFloat* blend { nullptr };
    juce::AudioParameterFloat* volume { nullptr };

    float rmsLevelLeft, rmsLevelRight;
    float rmsOutLevelLeft, rmsOutLevelRight;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDistortionAudioProcessor)
};
