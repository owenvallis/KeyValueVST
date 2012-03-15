/*
  ==============================================================================

    KeyValueMIDIPair.h
    Created: 14 Mar 2012 4:35:28am
    Author:  Owen Vallis

  ==============================================================================
*/

#ifndef __KEYVALUEMIDIPAIR_H_46379F2C__
#define __KEYVALUEMIDIPAIR_H_46379F2C__

#include "../JuceLibraryCode/JuceHeader.h"

class KeyValueMIDIPair 
{    
public:
    KeyValueMIDIPair(const SortedSet<int>& itemSet_, const MidiMessageSequence& midiSeq_)
    {
        itemSet = itemSet_;
        midiSeq = midiSeq_;
    }
    
    ~KeyValueMIDIPair() {}
    
    const SortedSet<int>& getItemSet() const { return itemSet; }
    const MidiMessageSequence& getMIDISequence() const { return midiSeq; }
    
private:
    SortedSet<int> itemSet;
    MidiMessageSequence midiSeq;
};



#endif  // __KEYVALUEMIDIPAIR_H_46379F2C__
