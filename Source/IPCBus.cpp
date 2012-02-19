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

void IPCBus::sendData (const MemoryBlock& message)
{
    if (isConnected()) 
    {
        DBG ("sending data");
        sendMessage (message);
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