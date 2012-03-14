/*
  ==============================================================================

    BackgroundThread.cpp
    Created: 8 Feb 2012 2:45:49pm
    Author:  Owen Vallis

  ==============================================================================
*/

#include "BackgroundThread.h"

BackgroundThread::BackgroundThread() :  Thread ("MIDI processor")
{
    startThread();
}

BackgroundThread::~BackgroundThread()
{
    // allow the thread 2 seconds to stop cleanly - should be plenty of time.
    stopThread (2000);
}

// update our data
void BackgroundThread::processMidi (const String mode, 
                                    const MidiMessageSequence& perfA, 
                                    const MidiMessageSequence& perfB)
{
    notify();
}

void BackgroundThread::parseMidi()
{
   
}

void BackgroundThread::run()
{
    // this is the code that runs this thread - we'll loop continuously,
    // updating the co-ordinates of our blob.
    
    // threadShouldExit() returns true when the stopThread() method has been
    // called, so we should check it often, and exit as soon as it gets flagged.
    while (! threadShouldExit())
    {
        // sleep until we get an event
        wait (-1);
            
        DBG ("parsing midi");
        
        parseMidi();
    }
}
