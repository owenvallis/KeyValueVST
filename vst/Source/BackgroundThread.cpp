/*
  ==============================================================================

    BackgroundThread.cpp
    Created: 8 Feb 2012 2:45:49pm
    Author:  Owen Vallis

  ==============================================================================
*/

#include "BackgroundThread.h"

BackgroundThread::BackgroundThread()
{
    firstMeasure = true;
}

BackgroundThread::~BackgroundThread()
{
}

// update our data
void BackgroundThread::processMidi (const String mode_, 
                                    const SortedSet<int>& perfA_, 
                                    const MidiMessageSequence& perfB_)
{
    perfAPrevious = perfA;
    perfA = perfA_;
    perfB = perfB_;
    
    DBG (perfB.getNumEvents());
    
    mode = mode_;
    
    if (firstMeasure) {
        firstMeasure = false;
    } else {
        parseMidi();
    }
}

void BackgroundThread::parseMidi()
{    
    DBG("in");

    if (mode == "Learning") {
        DBG("learn");
        seq2.add (new KeyValueMIDIPair(perfAPrevious, perfB));
        
    } else if (mode == "Performance") {
        DBG("perf");
         
        if (seq1.size() < 2) {
            DBG("adding first seq");
            seq1.add (new KeyValueMIDIPair(perfAPrevious, emptyMidiSeq));
            DBG("added first seq");
        } else {
            DBG("removing first seq");
            seq1.remove (0);
            DBG("adding new seq");
            seq1.add (new KeyValueMIDIPair(perfAPrevious, emptyMidiSeq));  
            DBG("added new seq");
            
            int i = 0;
            double similarityScore = 0;
            double previousSimilarityScore = 0;
            int posMaxSimilarityScore = 0;
            
            for (int i = 0; i < (seq2.size() - 2); i++) {
                
                similarityScore = s2mp.compareSequences(seq1, seq2, 0, i, 2, 2);
                
                if (similarityScore > previousSimilarityScore) {
                    posMaxSimilarityScore = i;
                }
                previousSimilarityScore = similarityScore;
            }
            
            outputSequences.add(&(seq2[i]->getMIDISequence()));
        }
    }
    DBG("out");
}

MidiMessageSequence BackgroundThread::getMidiNextMidiSequence()
{
    MidiMessageSequence seq;
    
    if(outputSequences.size() > 0) {
       seq = *outputSequences.removeAndReturn(0);
    }

    return seq;
}
