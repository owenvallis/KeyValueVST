/*
 ==============================================================================
 
 MIDIAggregater.cpp
 Created: 7 Feb 2012 9:06:45pm
 Author:  Owen Vallis
 
 ==============================================================================
 */
#include "MIDIAggregater.h"

MIDIAggregater::MIDIAggregater (BackgroundThread& midiSequenceProcessor_) : midiSequenceProcessor (midiSequenceProcessor_),  
mode("Learning")
{    
    // Parts per Quater Note 
    // ths should be determined by the host
    // but we'll hard set it for now at Ableton's 96 ppq
    ppqn = 96;   
    ticksPerBar = ppqn * 4;
    
    // initialize our timestamp variables
    framesPerTick = 0;
    previousPPQPosOfLastBarStart = 0;    
}

MIDIAggregater::~MIDIAggregater()
{
    
}

void MIDIAggregater::addMidiBuffer (const MidiBuffer& buffer, 
                                    const int processBlockSizeInSamples, 
                                    const AudioPlayHead::CurrentPositionInfo& newTime, 
                                    const double sampleRate)
{       
    // calculate the current number of samples per Bar
    double secPerQuarterNote = 60.0/newTime.bpm;
    double samplesPerBeat = sampleRate * secPerQuarterNote;
    
    // calculate the current number of frames (samples) per tick
    framesPerTick = samplesPerBeat/ppqn;
    
    // calculate the tick timestamp at the start of this block
    double currentTickPosFromLastBar = (newTime.ppqPosition - newTime.ppqPositionOfLastBarStart) * ppqn;
    
    // create TimeStamps as Ticks
    MidiBuffer::Iterator i (buffer);
    MidiMessage message (0xf4, 0.0);
    int currentFrame;
    
    // iterate over our new MIDI events and create their tick timestamps
    while (i.getNextEvent (message, currentFrame))
    {
        double currentTickPos = currentTickPosFromLastBar + (currentFrame/framesPerTick);
        message.setTimeStamp(currentTickPos);
        oneBarMidiSequence.addEvent(message);
    }
    
    // lets see if we've collected a full Bar of midi
    if(previousPPQPosOfLastBarStart != newTime.ppqPositionOfLastBarStart)
    {
        sendBarOfMidi();
        
        previousPPQPosOfLastBarStart = newTime.ppqPositionOfLastBarStart;
    }
}

void MIDIAggregater::setMidiChannelA (int channelA_)
{
    channelA = channelA_;
}

void MIDIAggregater::setMidiChannelB (int channelB_)
{
    channelB = channelB_;
}

void MIDIAggregater::setMode (String mode_)
{
    mode = mode_;
}



void MIDIAggregater::sendBarOfMidi()
{
    MidiMessageSequence perfA;
    MidiMessageSequence perfB;
    
    oneBarMidiSequence.updateMatchedPairs();
    
    MidiMessageSequence overflowSequence;
    
    MidiMessageSequence::MidiEventHolder* midiEvent;
    
    for (int i = 0; i < oneBarMidiSequence.getNumEvents(); i++)
    {
        midiEvent = oneBarMidiSequence.getEventPointer(i);
        
        // and its a control message
        if (midiEvent->message.isController())
        {
            // if we are within our single Bar
            if (midiEvent->message.getTimeStamp() <= ticksPerBar)
            {
                
                if (midiEvent->message.getChannel() == channelA)
                {
                    perfA.addEvent(midiEvent->message);
                } 
                else if (midiEvent->message.getChannel() == channelB) 
                {
                    perfB.addEvent(midiEvent->message);
                }
                
            } else { 
                // if a tick timestamp is > ticksPerBar ticks, then it must be from the next bar
                overflowSequence.addEvent(midiEvent->message);
            }
        }
    }
    
    midiEvent = nullptr;
    
    midiSequenceProcessor.processMidi (mode, perfA, perfB);
    
    // clear our midibuffer and then add back in the leftover midi events
    oneBarMidiSequence.clear();
    oneBarMidiSequence.addSequence (overflowSequence, -ticksPerBar, 0, ticksPerBar);
}
