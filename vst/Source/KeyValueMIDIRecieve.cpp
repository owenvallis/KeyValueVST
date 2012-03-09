/*
 ==============================================================================
 
 KeyValueMIDIRecieve.cpp
 Created: 10 Feb 2012 4:13:11pm
 Author:  Owen Vallis
 
 ==============================================================================
 */

#include "KeyValueMIDIRecieve.h"

KeyValueMIDIRecieve::KeyValueMIDIRecieve()
{
    
}

KeyValueMIDIRecieve::~KeyValueMIDIRecieve()
{
    
}

void KeyValueMIDIRecieve::addData (const MemoryBlock& message)
{
    var midiData = JSON::parse(message.toString());
    
    if(midiData != var::null)
    {
        addDatatoFifoBuffer (midiData);
    }
}

void KeyValueMIDIRecieve::getBlockOfMidiData (MidiBuffer& currentMidiBlock)
{
    
}

void KeyValueMIDIRecieve::addDatatoFifoBuffer (const var& midiData)
{
    for (int i = 0; i < midiData.size(); i++)
    {
        var child = midiData[i];
        
        for (int j = 0; j < child.size(); j++) 
        {
            // unpack midi
            DBG((int)child[j]);
        }
        
    }
    
    DBG("unpacked");
}