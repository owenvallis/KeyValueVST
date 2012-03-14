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
    KeyValueMIDIPair();
    ~KeyValueMIDIPair();
    
    void addItemSet (const Array <int> &itemSet)
    {
        _itemSet.addArray (itemSet.begin(), itemSet.size());
    }
    
    void addMIDIBuffer (const MidiBuffer &buffer)
    {
        _buffer = buffer;
    }

    
    const SortedSet<int>& getItemSet() const { return _itemSet; }
    const MidiBuffer& getMIDIBuffer() const { return _buffer; }
    
private:
    SortedSet<int> _itemSet;
    MidiBuffer _buffer;
};



#endif  // __KEYVALUEMIDIPAIR_H_46379F2C__
