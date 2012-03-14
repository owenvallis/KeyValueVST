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
#include "SeqPat.h"
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
    void processMidi (const String mode, 
                      const MidiMessageSequence& perfA, 
                      const MidiMessageSequence& perfB);
    
private:
    SeqPat seq1, seq2;
    
    void parseMidi();
    
    void run();
            
};

#endif  // __BACKGROUNDTHREAD_H_E82F8E88__
