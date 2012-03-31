/*
  ==============================================================================

    S2MP.h
    Created: 12 Mar 2012 12:32:19am
    Author:  Owen Vallis and Ollie Glass
    Website: www.flipmu.com
 
    based off S2MP code by Aengus Martin
    Copyright 2010 am-process.org. All rights reserved.
 
    This class and the SeqPat class provide a C++ implementation 
    of the S2MP similarity measure as described in:
 
    Saneifar et al. S2mp: Similarity measure for sequential patterns. 
    Proceedings of the Seventh Australasian Data Mining Conference (2008)

  ==============================================================================
*/

#ifndef __S2MP_H_3C6A6B70__
#define __S2MP_H_3C6A6B70__

#include "../JuceLibraryCode/JuceHeader.h"

class S2MP {
public:
    S2MP();
    ~S2MP();
    
    void setParams (OwnedArray<SortedSet<int>> &sp1_, OwnedArray<SortedSet<int>> &sp2_, 
                    int sp1frst = 0, int sp2frst = 0, 
                    int sp1len = -1, int sp2len = -1);
    
    void compareSequences();
    
    float getSimilarityScore() { return _similarity; }
    
    // Calculates similarity between two lists of lists
    float compareSequences (OwnedArray<SortedSet<int>> &sp1_, OwnedArray<SortedSet<int>> &sp2_, 
                             int sp1frst = 0, int sp2frst = 0, 
                             int sp1len = -1, int sp2len = -1);
    
private:
    CriticalSection lock;
    
    OwnedArray<SortedSet<int>>* sp1; 
    OwnedArray<SortedSet<int>>* sp2;
	/*
	 * Private variables
	 */
	float _mappingCo;                 // Mapping coefficient
	float _orderCo;                   // Order coefficient
	
	int _sp1frst;                      // Element of _sp1 to start first subsequence
	int _sp1len;                       // Length of first subsequence
	int _sp2frst;                      // Element of _sp2 to start second subsequence
	int _sp2len;                       // Length of second subsequence
	
	float _mappingScore;              // Average weight score
	float _orderScore;                // Order score
	Array <int> _mappingOrder;         // Mapping order
	Array <Array <float> > _w;        // Weight matrix
	float _similarity;                // Overall similarity
	
	/*
	 * Private functions for calculating the similarity
	 */
	void calcWeightMatrix (OwnedArray<SortedSet<int>> &sp1, OwnedArray<SortedSet<int>> &sp2);
    int numberOfIntersections (SortedSet<int>* set1, SortedSet<int>* set2);

	void calcMappingScore();
    int indexOfMax (Array<float> &weights);
    void solveConflict (int i_, int k_);
    float calcLocalSim (int id1, int cid1, float w1, int id2, int cid2, float w2);
    
	void calcOrderScore();
    
	void calcSimilarity();
    
    
};

#endif  // __S2MP_H_3C6A6B70__
