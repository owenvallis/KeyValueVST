/*
 ==============================================================================
 
 MIDIAggregater.cpp
 Created: 7 Feb 2012 9:06:45pm
 Author:  Owen Vallis
 
 ==============================================================================
 */
#include "MIDIAggregater.h"

MIDIAggregater::MIDIAggregater (BackgroundThread& varToJSONConverter_) : varToJSONConverter (varToJSONConverter_),  
                                                                         mode("Learning")
{    
    // Parts per Quater Note 
    // ths should be determined by the host
    // but we'll hard set it for now at Ableton's 96 ppq
    ppqn = 96;   
    ticksPerBar = ppqn * 4;
    
    // initialize our timestamp variables
    samplesPerBar = 0;
    currentSamplePosWithinBar = 0;
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
    samplesPerBar = samplesPerBeat * 4.0;
    
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
        int currentSamplePos = currentSamplePosWithinBar + currentFrame;
        tickTimeStamp.set (currentSamplePos, currentTickPos);
    }
    
    // add current midiBuffer to the aggregate midi buffer
    oneBarMidiBuffer.addEvents (buffer, buffer.getFirstEventTime(), -1, currentSamplePosWithinBar);
    
    // update our current sample position within the Bar
    currentSamplePosWithinBar += processBlockSizeInSamples;
        
    // lets see if we've collected a full Bar of midi
    if(previousPPQPosOfLastBarStart != newTime.ppqPositionOfLastBarStart)
    {
        sendBarOfMidi();
        
        previousPPQPosOfLastBarStart = newTime.ppqPositionOfLastBarStart;
    }
}

void MIDIAggregater::resetCurrentPos(double pos)
{
    currentSamplePosWithinBar = pos;
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
    var MIDILog = mode;
    
    // check for overflow events and place them at the start of the next midiBuffer
    MidiBuffer::Iterator i (oneBarMidiBuffer);
    MidiMessage message (0xf4, 0.0);
    
    int currentFrame;
    double sampleOffset;
    
    MidiBuffer tempBuffer;
    HashMap<int, double> tempTimeStamps;
        
    while (i.getNextEvent (message, currentFrame))
    {     
        // if we are within our single Bar
        if (currentFrame <= samplesPerBar && tickTimeStamp[currentFrame] <= ticksPerBar)
        {
            // and its a control message
            if (message.isController())
            {
                if (message.getChannel() == channelA)
                {
                    var cc;
                    
                    cc.append ("PerfA");
                    cc.append (channelA);
                    cc.append (message.getControllerNumber());
                    cc.append (message.getControllerValue());
                    cc.append (tickTimeStamp[currentFrame]);
                    
                    MIDILog.append (cc);	
                    
                } else if (message.getChannel() == channelB) {
                    
                    var cc;
                    
                    cc.append ("PerfB");
                    cc.append (channelB);
                    cc.append (message.getControllerNumber());
                    cc.append (message.getControllerValue());
                    cc.append (tickTimeStamp[currentFrame]);
                    
                    MIDILog.append (cc);	
                }
            }
        } else {  // if we are over our single Bar

            sampleOffset = currentFrame - samplesPerBar;

            // if a tick timestamp is > ticksPerBar ticks, then it must be from the next bar
            if (tickTimeStamp[currentFrame] > ticksPerBar)
            {         
                double wrappedTimeStamp = tickTimeStamp[currentFrame] - (int)tickTimeStamp[currentFrame];
                tempTimeStamps.set (sampleOffset, wrappedTimeStamp);
            }
            
            // pack any messages after the one Bar point into this temp midibuffer
            tempBuffer.addEvent (message, sampleOffset);
            
            // update the timestamps to the start of a new Bar            
            tempTimeStamps.set (sampleOffset, tickTimeStamp[currentFrame]);
        }
    }
    
    // pass our data to the background thread to send out over the IPC bus
    varToJSONConverter.sendBarOfMidi (MIDILog);
    
    sampleOffset = currentSamplePosWithinBar - samplesPerBar; 
    
    // clear our midibuffer and then add back in the leftover midi events
    oneBarMidiBuffer.clear();
    oneBarMidiBuffer.addEvents (tempBuffer, tempBuffer.getFirstEventTime(), -1, 0);
    
    // clear our timestamp buffer and add back in the leftover timestamps
    tickTimeStamp.clear();
    tickTimeStamp.swapWith (tempTimeStamps);
    
    // reset our current pos to include the offset
    resetCurrentPos (sampleOffset);
}
