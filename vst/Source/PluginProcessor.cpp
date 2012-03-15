/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Key_valueAudioProcessor::Key_valueAudioProcessor()
{
}

Key_valueAudioProcessor::~Key_valueAudioProcessor()
{
}

//==============================================================================
const String Key_valueAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int Key_valueAudioProcessor::getNumParameters()
{
    return 0;
}

float Key_valueAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void Key_valueAudioProcessor::setParameter (int index, float newValue)
{    
}

const String Key_valueAudioProcessor::getParameterName (int index)
{    
    return String::empty;
}

const String Key_valueAudioProcessor::getParameterText (int index)
{
    return String::empty;
}

const String Key_valueAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String Key_valueAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool Key_valueAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool Key_valueAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool Key_valueAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Key_valueAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

int Key_valueAudioProcessor::getNumPrograms()
{
    return 0;
}

int Key_valueAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Key_valueAudioProcessor::setCurrentProgram (int index)
{
}

const String Key_valueAudioProcessor::getProgramName (int index)
{
    return String::empty;
}

void Key_valueAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Key_valueAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Key_valueAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void Key_valueAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // get the number of samples for this block
    int blockSizeInSamples = buffer.getNumSamples();
    
    // ask the host for the current time so we can calculate the size of our measure...
    AudioPlayHead::CurrentPositionInfo newTime;

    if (getPlayHead() != 0 && getPlayHead()->getCurrentPosition (newTime))
    {
        if(newTime.isPlaying) {
            midiAggregater.addMidiBuffer(midiMessages, newTime, getSampleRate());  
            
            if (midiAggregater.getMode() == "Performance") {
                midiAggregater.getMidiBuffer(midiMessages, blockSizeInSamples);
            }
        }
    }
    
}

//==============================================================================
bool Key_valueAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Key_valueAudioProcessor::createEditor()
{
    return new Key_valueAudioProcessorEditor (this);
}

//==============================================================================
void Key_valueAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Key_valueAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void Key_valueAudioProcessor::connectToDataBus()
{
}

void Key_valueAudioProcessor::setMidiChannelPerformerA (int channel)
{
    midiAggregater.setMidiChannelA(channel);
}

void Key_valueAudioProcessor::setMidiChannelPerformerB (int channel)
{
    midiAggregater.setMidiChannelB(channel);
}

void Key_valueAudioProcessor::setMode (String mode_)
{
    midiAggregater.setMode (mode_);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Key_valueAudioProcessor();
}


