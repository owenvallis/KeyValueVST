/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-11 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#ifndef __JUCE_MESSAGE_JUCEHEADER__
#define __JUCE_MESSAGE_JUCEHEADER__

class MessageListener;
class MessageManager;


//==============================================================================
/** The base class for objects that can be delivered to a MessageListener.

    If you want to send a message that carries some kind of custom data, just
    create a subclass of Message with some appropriate member variables to hold
    your data.

    @see MessageListener, MessageManager, ActionListener, ChangeListener
*/
class JUCE_API  Message  : public ReferenceCountedObject
{
public:
    //==============================================================================
    /** Creates an uninitialised message.

        The class's variables will also be left uninitialised.
    */
    Message() noexcept;

    /** Destructor. */
    virtual ~Message();

    /** A typedef for pointers to messages. */
    typedef ReferenceCountedObjectPtr <Message> Ptr;

    //==============================================================================
private:
    friend class MessageListener;
    friend class MessageManager;
    MessageListener* messageRecipient;

    // Avoid the leak-detector because for plugins, the host can unload our DLL with undelivered
    // messages still in the system event queue. These aren't harmful, but can cause annoying assertions.
    JUCE_DECLARE_NON_COPYABLE (Message);
};


#endif   // __JUCE_MESSAGE_JUCEHEADER__
