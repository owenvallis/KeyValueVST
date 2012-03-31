/*
 ==============================================================================
 
 MIDIAggregater.cpp
 Created: 7 Feb 2012 9:06:45pm
 Author:  Owen Vallis
 
 ==============================================================================
 */
#include "MIDIAggregater.h"





//==============================================================================
MIDIAggregater::MIDIAggregater () : simThreads (400),
                                    mode ("Learning")
                                    
{    
    // same as Ableton's internal CC rate of 100hrz
    numberOfSamplesInSnapshot = 441;
    
    resetValues();    
}

MIDIAggregater::~MIDIAggregater()
{
    simThreads.removeAllJobs (true, 2000);
}

void MIDIAggregater::addMidiBuffer (MidiBuffer& buffer, 
                                    const AudioPlayHead::CurrentPositionInfo& newTime, 
                                    const double sampleRate,
                                    const int numberSamplesInProcessBlock)
{    
    MidiBuffer currentBuffer = buffer;
    
    similarityScore = 0;
    bestMatchScore = -1;
    posBestMatchScore = -1;
    
    if (numberOfSamplesSinceLastSnapshot >= numberOfSamplesInSnapshot)
    {
        SortedSet <int>* itemSetA = new SortedSet<int>();
        SortedSet <int> itemSetB;
        
        // we have to add 1 to the CC number as they are 0 - 127 and we need 1 - 128 for this to work
        for (int i = 0; i < midiEventHolderPerfAKey.size(); i++) 
        {
            int key = midiEventHolderPerfAKey[i];
            int ccValue = (midiEventHolderPerfA[key]) + (key * 128);
            itemSetA->add (ccValue);
        }
        
        // we have to add 1 to the CC number as they are 0 - 127 and we need 1 - 128 for this to work
        for (int i = 0; i < midiEventHolderPerfB.size(); i++)
        {
            int key = midiEventHolderPerfBKey[i];
            int ccValue = (midiEventHolderPerfB[key]) + ((key + 128) * 128);
            itemSetB.add (ccValue);
        }
        
        // add our overflow to the hasmap for the next snapshot
        midiEventHolderPerfA.swapWith (midiEventHolderPerfAOverFlow);
        midiEventHolderPerfAOverFlow.clear();
        midiEventHolderPerfAKey = midiEventHolderPerfAOverFlowKey;
        midiEventHolderPerfAOverFlowKey.clearQuick();
        
        // add our overflow to the hasmap for the next snapshot
        midiEventHolderPerfB.swapWith (midiEventHolderPerfAOverFlow);
        midiEventHolderPerfBOverFlow.clear();
        midiEventHolderPerfBKey = midiEventHolderPerfBOverFlowKey;
        midiEventHolderPerfBOverFlowKey.clearQuick();
        
        // modulo the current sample position at the start of this block by our snapshot size
        // use the remainder as our starting offset for the next snapshot
        numberOfSamplesSinceLastSnapshot = numberOfSamplesSinceLastSnapshot % numberOfSamplesInSnapshot;
        
        // lets process our data
        if (mode == "Learning") {
            DBG("learn");
            
            for (int i = 0; i < itemSetB.size(); i++) {
                itemSetA->add (itemSetB[i]);
                DBG ("added: " + String(itemSetB[i]) );
            }
            seq2.add (itemSetA);
            
            DBG (seq2.size());
            
        } else if (mode == "Performance") { 
            //DBG("perform");
            
            if (seq1.size() > 96) {
                seq1.remove (0);
            }    
                
            DBG ("start sim match: " + String(Time::getMillisecondCounter()));
            DBG (simMatches.size());
            
            DBG (seq2.size() - seq1.size());
            
            for (int i = 0; i < seq2.size() - seq1.size(); i++) {
                
                if (i < simMatches.size()) {
                    simMatches[i]->setParams (seq1, seq2, 0, i, seq1.size(), seq1.size());
                    simThreads.addJob (simMatches[i], false);
                    // DBG("already added");
                } else {
                    DBG("adding new S2MP");
                    S2MPThreadJob* newMatch = new S2MPThreadJob (i);
                    simMatches.add (newMatch);
                    simMatches[i]->setParams (seq1, seq2, 0, i, seq1.size(), seq1.size());
                    simThreads.addJob (newMatch, false);
                }
                                
            }
            DBG ("threads running: " + String(Time::getMillisecondCounter()));
            DBG (simMatches.size());
            
            while (simMatches.getLast()->compareComplete() != true) {
                DBG("stuck");
            }
            
            DBG ("end sim match: " + String(Time::getMillisecondCounter()));
            
            DBG (simMatches.size());
            
            for (int i = 0; i < simMatches.size(); i++) {
                
                similarityScore = simMatches[i]->getSimilarityScore();
                if (similarityScore > bestMatchScore) {
                    posBestMatchScore = i;
                    bestMatchScore = similarityScore;
                }
            }
            
            DBG ("found sim match: " + String (posBestMatchScore) + ",  " + String (bestMatchScore) + ",  " + String(Time::getMillisecondCounter()));
            
            if (posBestMatchScore != -1) {
                
                buffer.clear();
                
                for (int i = 0; i < seq2[posBestMatchScore]->size(); i++) { 
                    
                    // only add items above our first performers range
                    if ((*seq2[posBestMatchScore])[i] > (128 * 128)) {
                        itemSetA->add ((*seq2[posBestMatchScore])[i]);
                        
                        double storedValue = ((*seq2[posBestMatchScore])[i]/128.00) - 128;
                        int controllerType = storedValue;
                        int value = (storedValue - controllerType) * 128.00;
                        //DBG ("stored value: " + String((*seq2[posBestMatchScore])[i]) + "  controllerType: " + String (controllerType) + "  value: " + String (value));
                        buffer.addEvent (MidiMessage::controllerEvent (channelB, controllerType, value), numberOfSamplesSinceLastSnapshot);
                    }
                } 
                
            }   

            seq1.add (itemSetA);
        }
    }

    // create TimeStamps as Ticks
    MidiBuffer::Iterator i (currentBuffer);
    MidiMessage message (0xf4, 0.0);
    int currentFrame = -1 ;
        
    // iterate over our new MIDI events and add them to our HashMap
    while (i.getNextEvent (message, currentFrame))
    {
        if (message.isController()) 
        {
            if (message.getChannel() == channelA) 
            {
                if (numberOfSamplesSinceLastSnapshot + currentFrame < numberOfSamplesInSnapshot)
                {
                    midiEventHolderPerfA.set (message.getControllerNumber(), message.getControllerValue());
                    midiEventHolderPerfAKey.add (message.getControllerNumber());
                } else {
                    midiEventHolderPerfAOverFlow.set (message.getControllerNumber(), message.getControllerValue());
                    midiEventHolderPerfAOverFlowKey.add (message.getControllerNumber());
                }
            } else if (message.getChannel() == channelB) {
                if (numberOfSamplesSinceLastSnapshot + currentFrame < numberOfSamplesInSnapshot)
                {
                    midiEventHolderPerfB.set (message.getControllerNumber(), message.getControllerValue());
                    midiEventHolderPerfBKey.add (message.getControllerNumber());
                } else {
                    midiEventHolderPerfBOverFlow.set (message.getControllerNumber(), message.getControllerValue());
                    midiEventHolderPerfBOverFlowKey.add (message.getControllerNumber());
                }            
            }
        }
    }
    
    numberOfSamplesSinceLastSnapshot += numberSamplesInProcessBlock;
    
    if (posBestMatchScore == -1) {
        buffer.clear();
    }
}

void MIDIAggregater::resetValues()
{    
    numberOfSamplesSinceLastSnapshot = 0;
    
    midiEventHolderPerfA.clear();
    midiEventHolderPerfB.clear();
    midiEventHolderPerfAKey.clearQuick();
    midiEventHolderPerfBKey.clearQuick();
    
    midiEventHolderPerfAOverFlow.clear();
    midiEventHolderPerfBOverFlow.clear();
    midiEventHolderPerfAOverFlowKey.clearQuick();
    midiEventHolderPerfBOverFlowKey.clearQuick();
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

void MIDIAggregater::saveData() {
    
}

void MIDIAggregater::loadData() {
    seq1.clear();
    seq2.clear();
}
