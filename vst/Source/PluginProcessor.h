/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#ifndef __PLUGINPROCESSOR_H_EB520863__
#define __PLUGINPROCESSOR_H_EB520863__

#include "../JuceLibraryCode/JuceHeader.h"
#include "MIDIAggregater.h"

//==============================================================================
/**
*/
class Key_valueAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Key_valueAudioProcessor();
    ~Key_valueAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    
    void connectToDataBus();
    void setMidiChannelPerformerA (int channel);
    void setMidiChannelPerformerB (int channel);
    
    void setMode (String mode_);
    String getMode() { return midiAggregater.getMode(); }

private:
    MIDIAggregater midiAggregater;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Key_valueAudioProcessor);
};

#endif  // __PLUGINPROCESSOR_H_EB520863__
