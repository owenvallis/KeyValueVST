/*
  ==============================================================================

    BackgroundThread.h
    Created: 8 Feb 2012 2:45:49pm
    Author:  Owen Vallis

  ==============================================================================
*/

#ifndef __BACKGROUNDTHREAD_H_E82F8E88__
#define __BACKGROUNDTHREAD_H_E82F8E88__

#include "../JuceLibraryCode/JuceHeader.h"
#include "S2MP.h"
#include "KeyValueMIDIPair.h"

//==============================================================================
/**
 */
class BackgroundThread : public Thread
{
public:
    BackgroundThread();
    ~BackgroundThread();
    
    // send our data out the IPC bus
    void processMidi (const String mode_, 
                      const SortedSet<int>& perfA_, 
                      const MidiMessageSequence& perfB_);
    
    const MidiMessageSequence& getMidiNextMidiSequence();
    
private:
    OwnedArray<KeyValueMIDIPair> seq1, seq2;
    S2MP s2mp;
    
    String mode;
    
    Boolean firstMeasure;
    
    SortedSet<int> perfA, perfAPrevious;
    MidiMessageSequence perfB, emptyMidiSeq;
    OwnedArray<MidiMessageSequence> outputSequences;
    
    void parseMidi();
    
    void run();
            
};

#endif  // __BACKGROUNDTHREAD_H_E82F8E88__
