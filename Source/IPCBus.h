/*
  ==============================================================================

    IPCBus.h
    Created: 8 Feb 2012 3:55:31pm
    Author:  Owen Vallis

  ==============================================================================
*/

#ifndef __IPCBUS_H_BD9C46F4__
#define __IPCBUS_H_BD9C46F4__

#include "../JuceLibraryCode/JuceHeader.h"
#include "KeyValueMIDIRecieve.h"

//==============================================================================
class IPCBus  : public InterprocessConnection
{
public:
    IPCBus (KeyValueMIDIRecieve& keyValueMIDIRecieve_);    
    ~IPCBus();
    
    void sendData (var& data);
    
    void connect ();
    
    void messageReceived (const MemoryBlock& message);
    void connectionLost();
    void connectionMade();
    
private:
    KeyValueMIDIRecieve& keyValueMIDIRecieve;
};



#endif  // __IPCBUS_H_BD9C46F4__
