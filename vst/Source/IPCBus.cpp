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
    
    // and try to open the socket or pipe...
    bool openedOk = false;
    
    // connect to our external pipe
    openedOk = connectToSocket("127.0.0.1", 5555, 1001);
    
    if (! openedOk)
    {        
        AlertWindow::showMessageBoxAsync (AlertWindow::WarningIcon,
                                          "VST",
                                          "Failed to open the socket or pipe...");
    }
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