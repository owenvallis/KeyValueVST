/*
  ==============================================================================

    IPCBus.cpp
    Created: 8 Feb 2012 3:55:31pm
    Author:  Owen Vallis

  ==============================================================================
*/

#include "IPCBus.h"

IPCBus::IPCBus (KeyValueMIDIRecieve& keyValueMIDIRecieve_) : InterprocessConnection (true),
                                                             keyValueMIDIRecieve (keyValueMIDIRecieve_)
{
}

IPCBus::~IPCBus()
{
    disconnect();
}

void IPCBus::sendData (var& data)
{
    
    // write the data to the IPC bus
    MemoryBlock m;
    MemoryOutputStream mos (m, false);
    JSON::writeToStream (mos, data);
    //DBG (JSON::toString (data));
    
    if (isConnected()) 
    {
        DBG ("sending data");
        sendMessage (m);
    } else {
        DBG ("not connected");
    }
    
}

void IPCBus::connect()
{
    // connect to our external pipe
    connectToPipe ("keyvaluevst");

}

void IPCBus::messageReceived (const MemoryBlock& message) 
{
    keyValueMIDIRecieve.addData (message);
}
void IPCBus::connectionLost() 
{
    DBG ("connection lost");
}

void IPCBus::connectionMade() 
{
    DBG ("connection made");
}