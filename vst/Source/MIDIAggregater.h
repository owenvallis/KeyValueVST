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
    MIDIAggregater ();
    ~MIDIAggregater();
    
    void addMidiBuffer (const MidiBuffer& buffer, 
                        const AudioPlayHead::CurrentPositionInfo& newTime, 
                        const double sampleRate);
    
    void getMidiBuffer (MidiBuffer& buffer, const int numberSamplesInProcessBlock);
    
    void resetValues();
        
    void setMidiChannelA (int channelA_);
    void setMidiChannelB (int channelB_);
    void setMode (String mode_);
    
    String getMode() { return mode; }
    
private: 
    
    CriticalSection lock;
    
    SortedSet<int>      perfASortedSet;
    SortedSet<int>      overflowSet;
    
    MidiMessageSequence perfBMidi, overflowSequence, outputSequence;
    
    // our worker thread 
    BackgroundThread midiSequenceProcessor;
    
    // Parts per Quater Note 
    // ths should be determined by the host
    // but we'll hard set it for now at Ableton's 96 ppq
    double ppqn;
    double ticksPerBar;
    
    // timestamp variables
    double framesPerTick;
    double currentTickPosFromLastBar;
    double ppqPositionOfLastBarStart, previousPPQPosOfLastBarStart;
    
    int numberOfSamplesPerBar;
    int currentMidiMessageInSequence;
    int samplePosInCurrentBarSinceLastBlock;
    
    // midi channels
    int channelA, channelB;
    // mode
    String mode;
};

#endif  // __MIDIAGGREGATER_H_5BE1F691__
