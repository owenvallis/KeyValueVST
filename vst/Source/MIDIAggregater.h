/*
  ==============================================================================

    MIDIAggregater.h
    Created: 7 Feb 2012 9:06:45pm
    Author:  Owen Vallis

  ==============================================================================
*/

#ifndef __MIDIAGGREGATER_H_5BE1F691__
#define __MIDIAGGREGATER_H_5BE1F691__

#include "../JuceLibraryCode/JuceHeader.h"
#include "BackgroundThread.h"

//==============================================================================
/**
 */
class MIDIAggregater
{
public:
    MIDIAggregater (BackgroundThread& midiSequenceProcessor_);
    ~MIDIAggregater();
    
    void addMidiBuffer (const MidiBuffer& buffer, 
                       const int processBlockSizeInSamples, 
                       const AudioPlayHead::CurrentPositionInfo& newTime, 
                       const double sampleRate);
        
    void setMidiChannelA (int channelA_);
    void setMidiChannelB (int channelB_);
    void setMode (String mode_);
    
    String getMode() { return mode; }
    
private:    
    MidiMessageSequence oneBarMidiSequence; 
    
    // our worker thread to convert teh data to JSON
    // and pass along to the IPC bus thread
    // this happens outside of our VST/host thread
    BackgroundThread& midiSequenceProcessor;
    
    // Parts per Quater Note 
    // ths should be determined by the host
    // but we'll hard set it for now at Ableton's 96 ppq
    double ppqn;
    double ticksPerBar;
    
    // timestamp variables
    double framesPerTick;
    double previousPPQPosOfLastBarStart;
    
    // midi channels
    int channelA, channelB;
    // mode
    String mode;
        
    // send our data out the IPC bus
    void sendBarOfMidi();  
    
};

#endif  // __MIDIAGGREGATER_H_5BE1F691__
