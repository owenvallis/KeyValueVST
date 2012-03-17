/*
 ==============================================================================
 
 MIDIAggregater.cpp
 Created: 7 Feb 2012 9:06:45pm
 Author:  Owen Vallis
 
 ==============================================================================
 */
#include "MIDIAggregater.h"

MIDIAggregater::MIDIAggregater () : mode("Learning")
{    
    // Parts per Quater Note 
    // ths should be determined by the host
    // but we'll hard set it for now at Ableton's 96 ppq
    ppqn = 96;   
    ticksPerBar = ppqn * 4;
    
    // initialize our timestamp variables
    framesPerTick = 0;
    previousPPQPosOfLastBarStart = 0; 
    
    numberOfSamplesPerBar = 0;
    
    samplePosInCurrentBarSinceLastBlock = 0;
    currentMidiMessageInSequence = -1;
    
}

MIDIAggregater::~MIDIAggregater()
{
    
}

void MIDIAggregater::addMidiBuffer (const MidiBuffer& buffer, 
                                    const AudioPlayHead::CurrentPositionInfo& newTime, 
                                    const double sampleRate)
{       
    // calculate the current number of samples per Bar
    double secPerQuarterNote = 60.0/newTime.bpm;
    double samplesPerBeat = sampleRate * secPerQuarterNote;
    numberOfSamplesPerBar = samplesPerBeat * 4;
    
    // calculate the current number of frames (samples) per tick
    framesPerTick = samplesPerBeat/ppqn;
    
    ppqPositionOfLastBarStart = newTime.ppqPositionOfLastBarStart;
    
    // calculate the tick timestamp at the start of this block
    currentTickPosFromLastBar = (newTime.ppqPosition - ppqPositionOfLastBarStart) * ppqn;
    
    // create TimeStamps as Ticks
    MidiBuffer::Iterator i (buffer);
    MidiMessage message (0xf4, 0.0);
    int currentFrame;
    
    const ScopedLock sl (lock);
    
    // iterate over our new MIDI events and create their tick timestamps
    while (i.getNextEvent (message, currentFrame))
    {
        if (message.isController()) 
        {
            // get our timestamp in ticks
            double currentTickPos = currentTickPosFromLastBar + (currentFrame/framesPerTick);
            
            // if we are within our single Bar
            if (currentTickPos < ticksPerBar)
            {
                if (message.getChannel() == channelA)
                {
                    perfASortedSet.add (message.getControllerValue());
                } 
                else if (message.getChannel() == channelB) 
                {
                    message.setTimeStamp (currentTickPos);
                    perfBMidi.addEvent (message);
                }                
            } 
            else // we have overflowed into the next bar
            { 
                if (message.getChannel() == channelA)
                {
                    overflowSet.add (message.getControllerValue());
                } 
                else if (message.getChannel() == channelB) 
                {
                    // if a tick timestamp is > ticksPerBar ticks, then it must be from the next bar
                    message.setTimeStamp (currentTickPos);
                    overflowSequence.addEvent (message);
                }
            }
        }
    }
    
    // lets see if we've collected a full Bar of midi
    if(previousPPQPosOfLastBarStart != newTime.ppqPositionOfLastBarStart)
    {
        // if so, lets process our data
        DBG("process");
        midiSequenceProcessor.processMidi (mode, perfASortedSet, perfBMidi);
        
        if (getMode() == "Performance")
        {
            outputSequence.clear();
            outputSequence = midiSequenceProcessor.getMidiNextMidiSequence();
            DBG("gotMIDI");
            
            if(currentMidiMessageInSequence == -1)
            {
                samplePosInCurrentBarSinceLastBlock = 0;
            } else {
                samplePosInCurrentBarSinceLastBlock = samplePosInCurrentBarSinceLastBlock - numberOfSamplesPerBar;
            }
            
            currentMidiMessageInSequence = 0;
        }
        
        // clear our ordered set and then add back in the overflow
        perfASortedSet = overflowSet;
        overflowSet.clearQuick();
        
        // clear our midiSequence and then add back in the overflow midi events
        perfBMidi.clear();
        perfBMidi.addSequence (overflowSequence, -ticksPerBar, 0, ticksPerBar);
        overflowSequence.clear();
        
        previousPPQPosOfLastBarStart = newTime.ppqPositionOfLastBarStart;
    }
}

void MIDIAggregater::getMidiBuffer (MidiBuffer& buffer, const int numberSamplesInProcessBlock)
{
    
    buffer.clear();
    int currentSample = 0;
    
    while (currentSample < numberSamplesInProcessBlock && currentMidiMessageInSequence < outputSequence.getNumEvents())
    {
        double timeStamp = outputSequence.getEventPointer(currentMidiMessageInSequence)->message.getTimeStamp();
        
        currentSample = (timeStamp * framesPerTick) - samplePosInCurrentBarSinceLastBlock;

        if (currentSample > 0 && currentSample < numberSamplesInProcessBlock) {
            buffer.addEvent (outputSequence.getEventPointer(currentMidiMessageInSequence)->message, currentSample);
        }
        
        if (currentSample < numberSamplesInProcessBlock)
        {
            currentMidiMessageInSequence++;
        }
    }
    
    samplePosInCurrentBarSinceLastBlock += numberSamplesInProcessBlock;

}

void MIDIAggregater::resetValues()
{
    // initialize our timestamp variables
    framesPerTick = 0;
    previousPPQPosOfLastBarStart = 0; 
    
    numberOfSamplesPerBar = 0;
    
    samplePosInCurrentBarSinceLastBlock = 0;
    currentMidiMessageInSequence = -1;
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
