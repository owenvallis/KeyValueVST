/*
  ==============================================================================

    MIDIAggregater.h
    Created: 7 Feb 2012 9:06:45pm
    Author:  Owen Vallis

  ==============================================================================
*/

#ifndef __MIDIAGGREGATER_H_5BE1F691__
#define __MIDIAGGREGATER_H_5BE1F691__

#include "../JuceLibraryCode/JuceHeader.h"
#include "S2MP.h"

class S2MPThreadJob  :  public S2MP,
                        public ThreadPoolJob
{
public:
    S2MPThreadJob(int ID)
    : ThreadPoolJob (String(ID))
    
    {
        isCompareFinished = true;
    }
    
    ~S2MPThreadJob()
    {
    }
    
    JobStatus runJob()
    {
        
            isCompareFinished = false;
            compareSequences();
            isCompareFinished = true;  
        
        return jobHasFinished;
    }
    
    bool compareComplete() { return isCompareFinished; }
    
private:
    bool isCompareFinished;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (S2MPThreadJob);
};

//==============================================================================
/**
 */
class MIDIAggregater
{
public:
    MIDIAggregater ();
    ~MIDIAggregater();
    
    void addMidiBuffer (MidiBuffer& buffer, 
                        const AudioPlayHead::CurrentPositionInfo& newTime, 
                        const double sampleRate,
                        const int numberSamplesInProcessBlock);
        
    void resetValues();
        
    void setMidiChannelA (int channelA_);
    void setMidiChannelB (int channelB_);
    void setMode (String mode_);
    
    String getMode() { return mode; }
    
    void saveData();
    void loadData();
        
private:
    ThreadPool simThreads;
    
    // send our data out the IPC bus
    void processMidi (const String mode_, 
                      SortedSet<int>* perfA_,
                      SortedSet<int>& perfB_);
    
    SortedSet<int> getMidiNextItemSet();
    
    OwnedArray<SortedSet<int>> seq1, seq2;
    SortedSet<int> returnedSet;
    OwnedArray<S2MPThreadJob> simMatches;   
    
    HashMap<int, int>   midiEventHolderPerfA, midiEventHolderPerfB;
    SortedSet<int>          midiEventHolderPerfAKey, midiEventHolderPerfBKey;
    
    HashMap<int, int>   midiEventHolderPerfAOverFlow, midiEventHolderPerfBOverFlow;
    SortedSet<int>          midiEventHolderPerfAOverFlowKey, midiEventHolderPerfBOverFlowKey;
    
    int numberOfSamplesSinceLastSnapshot;
    int numberOfSamplesInSnapshot;
        
    // midi channels
    int channelA, channelB;
    // mode
    String mode;
    
    double similarityScore;
    double bestMatchScore;
    int posBestMatchScore;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDIAggregater);

};

#endif  // __MIDIAGGREGATER_H_5BE1F691__
