/*
  ==============================================================================

    KeyValueMIDIRecieve.h
    Created: 10 Feb 2012 4:13:11pm
    Author:  Owen Vallis

  ==============================================================================
*/

#ifndef __KEYVALUEMIDIRECIEVE_H_9DB76BEC__
#define __KEYVALUEMIDIRECIEVE_H_9DB76BEC__

#include "../JuceLibraryCode/JuceHeader.h"


//==============================================================================
class KeyValueMIDIRecieve
{
public:
    KeyValueMIDIRecieve();
    ~KeyValueMIDIRecieve();
    
    void addData (const MemoryBlock& message);
    
    void getBlockOfMidiData (MidiBuffer& currentMidiBlock);
    
private:
    MidiBuffer fifoMidiBuffer; 
    
    void addDatatoFifoBuffer (const var& midiData);
    

};

#endif  // __KEYVALUEMIDIRECIEVE_H_9DB76BEC__
