/*
  ==============================================================================

    SeqPat.h
    Created: 13 Mar 2012 1:57:34pm
    Author:  Owen Vallis and Ollie Glass
    Website: www.flipmu.com
 
    based off S2MP code by Aengus Martin
    Copyright 2010 am-process.org. All rights reserved.

  ==============================================================================
*/

#ifndef __SEQPAT_H_92E0F5B2__
#define __SEQPAT_H_92E0F5B2__

#include "../JuceLibraryCode/JuceHeader.h"
#include "KeyValueMIDIPair.h"

class SeqPat
{
public:
    SeqPat();
    ~SeqPat();
    
    void addKeyValueMIDIPair (const KeyValueMIDIPair* const item)
    {
        seq.add (item);
    }

    inline int size() const noexcept
    {
        return seq.size();
    }
        
    const SortedSet<int>& getItemSet (int itemSet) const
    {        
        return seq[itemSet]->getItemSet();
    }
    
    const MidiBuffer& getMidiBuffer (int midiBuffer) const
    {
        return seq[midiBuffer]->getMIDIBuffer();
    }
    
private:    
    OwnedArray <KeyValueMIDIPair> seq;
};

#endif  // __SEQPAT_H_92E0F5B2__
