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
#include "IPCBus.h"

//==============================================================================
/**
 */
class BackgroundThread : public Thread
{
public:
    BackgroundThread (IPCBus& dataBus_);
    ~BackgroundThread();
    
    // send our data out the IPC bus
    void sendBarOfMidi (var& MIDILog_);
    
private:
    
    IPCBus& dataBus;
    
    var MIDILog;
    
    void run();
            
};

#endif  // __BACKGROUNDTHREAD_H_E82F8E88__
