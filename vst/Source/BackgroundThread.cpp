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
    firstMeasure = true;
    startThread();
}

BackgroundThread::~BackgroundThread()
{
    // allow the thread 2 seconds to stop cleanly - should be plenty of time.
    stopThread (2000);
}

// update our data
void BackgroundThread::processMidi (const String mode_, 
                                    const SortedSet<int>& perfA_, 
                                    const MidiMessageSequence& perfB_)
{
    perfAPrevious = perfA;
    perfA = perfA_;
    perfB = perfB_;
    
    mode = mode_;
    
    if (firstMeasure) {
        firstMeasure = false;
    } else {
        notify();
    }
}

void BackgroundThread::parseMidi()
{    
    if (mode == "Learning") {
        seq2.add (new KeyValueMIDIPair(perfAPrevious, perfB));
        
    } else if (mode == "Performance") {
        
        if (seq1.size() < 8) {
            seq1.add (new KeyValueMIDIPair(perfAPrevious, emptyMidiSeq));
        } else {
            seq1.remove (0);
            seq1.add (new KeyValueMIDIPair(perfAPrevious, emptyMidiSeq));   
            
            int i = 0;
            double similarityScore, previousSimilarityScore;
            int posMaxSimilarityScore;
            
            while ((i + 8) < seq2.size()) {
                similarityScore = s2mp.compareSequences(seq1, seq2, 0, i, 8, 8);
                
                if (similarityScore > previousSimilarityScore) {
                    posMaxSimilarityScore = i;
                }
                
                previousSimilarityScore = similarityScore;
                
                i++;
            }
            
            outputSequences.add(&(seq2[i]->getMIDISequence()));
        }
    }
}

const MidiMessageSequence& BackgroundThread::getMidiNextMidiSequence()
{
    return *outputSequences.removeAndReturn(0);
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
