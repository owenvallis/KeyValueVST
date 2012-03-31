/*
  ==============================================================================

    S2MP.cpp
    Created: 12 Mar 2012 12:32:19am
    Author:  Owen Vallis

  ==============================================================================
*/

#import "S2MP.h"

S2MP::S2MP()
{
    // similarity calculation coefficients
    _mappingCo = 0.5;
    _orderCo = 0.5; 
    
}

S2MP::~S2MP()
{
    
}

void S2MP::setParams (OwnedArray<SortedSet<int>> &sp1_, OwnedArray<SortedSet<int>> &sp2_, 
                      int sp1frst, int sp2frst, 
                      int sp1len, int sp2len)
{
    const ScopedLock sl (lock);
    
    // set ranges from sequences
    _sp1frst = sp1frst;
    
    // check bounds
    if ((sp1len > -1) && (_sp1frst + sp1len) <= sp1_.size()) {
        _sp1len = _sp1frst + sp1len;
    } else {
        _sp1len = sp1_.size() - _sp1frst;
    }
    
    _sp2frst = sp2frst;
    
    // check bounds
    if ((sp2len > -1) && (_sp2frst + sp2len) <= sp2_.size()) {
        _sp2len = _sp2frst + sp2len;
    } else {
        _sp2len = sp2_.size() - _sp2frst;
    }
    
    sp1 = &sp1_;
    sp2 = &sp2_;
    
}

void S2MP::compareSequences() 
{
    // do calculations
    calcWeightMatrix(*sp1, *sp2);
    calcMappingScore();
    calcOrderScore();
    calcSimilarity();
}

// Calculates similarity between two lists of lists
float S2MP::compareSequences (OwnedArray<SortedSet<int>> &sp1_, OwnedArray<SortedSet<int>> &sp2_, 
                               int sp1frst, int sp2frst, 
                               int sp1len, int sp2len)
{    
    // set ranges from sequences
    _sp1frst = sp1frst;
    
    // check bounds
    if ((sp1len > -1) && (_sp1frst + sp1len) <= sp1_.size()) {
        _sp1len = _sp1frst + sp1len;
    } else {
        _sp1len = sp1_.size() - _sp1frst;
    }
    
    _sp2frst = sp2frst;
    
    // check bounds
    if ((sp2len > -1) && (_sp2frst + sp2len) <= sp2_.size()) {
        _sp2len = _sp2frst + sp2len;
    } else {
        _sp2len = sp2_.size() - _sp2frst;
    }
    
    // do calculations
    calcWeightMatrix(sp1_, sp2_);
    calcMappingScore();
    calcOrderScore();
    calcSimilarity();
    
    return _similarity;
}

// ==================================================================
// calculate the mapping score
// ==================================================================

// Calulate the matching weights between each pair of itemsets in the sequences, stores in a matrix
void S2MP::calcWeightMatrix(OwnedArray<SortedSet<int>> &sp1, OwnedArray<SortedSet<int>> &sp2)
{
    //DBG("sp1 size: " + String(sp1.size()));
    //DBG("sp2 size: " + String(sp2.size()));

    // clear the weight matrix
    _w.clearQuick();
    
    // for each ith itemset in seq1, calculate matching weight for each jth item in seq2
    for (int i = _sp1frst; i < _sp1len; i++)
    {
        // add a new row for itemset i
        _w.add(Array<float>());
        
        for (int j = _sp2frst; j < _sp2len; j++) {
            // find interection
            int intersection = numberOfIntersections (sp1[i], sp2[j]);
            
            // append row with jth: matching weight = (num elements in intersection)/(avg num of elements in itemsets)		
            _w.getReference(i - _sp1frst).add (intersection/(_sp1len + _sp2len/2.0));
        }   
        //DBG("Size of row " + String(i) + " " + String(_w.getReference(i - _sp1frst).size()) + "   number of jth items tested " + String(_sp2len));
    }
    
}

int S2MP::numberOfIntersections (SortedSet<int>* set1, SortedSet<int>* set2)
{
    int i = 0, j = 0;
    int m = set1->size();
    int n = set2->size();
    
    int intersectionCount = 0;
    
    while (i < m && j < n)
    {
        if (set1->getUnchecked(i) < set2->getUnchecked(j))
        {
            i++;
        }
        else if (set2->getUnchecked(j) < set1->getUnchecked(i))
        {
            j++;
        }
        else /* if set1[i] == set2[j] */
        {
            intersectionCount++;
            i++;
            j++;
        }
    }  
    
    return intersectionCount;
}

// Calculate the mapping order and mapping score
void S2MP::calcMappingScore()
{
    _mappingOrder.clearQuick();
    
    // keep track of the number of matching weights equal to zero
    int coutnMinusOnes = 0;
  
    // find the position of each row's max matching weight
    for (int i = 0; i < _w.size(); i++)
    {

        // in the case of two matching Max weights, we take the first one
        int weightPos = indexOfMax(_w.getReference(i));
 
        // set all mappings associated with a meanininglessly small weight to -1
        if (_w.getReference(i).getUnchecked(weightPos) < 0.00001)
        {
            weightPos = -1;
            coutnMinusOnes += 1;
        }
        
        // store matching weight position i.e. max weight in this row
        _mappingOrder.add(weightPos);

    }
    
    // If there are no mappings, return with mappingScore = 0
    if (coutnMinusOnes == _mappingOrder.size())
    {
        _mappingScore = 0;
        return;
    }
    
    // Now detect conflicts/duplicates (two itemsets in sp1 mapped to the same itemset in sp2)
    for (int i = 0; i < (_sp1len - _sp1frst - 1); i++)
    {        

        //if we have a none zero matching weight, find conflict and resolve
        if (_mappingOrder.getUnchecked(i) > -1)
        {
            for (int k = i + 1; k < (_sp1len - _sp1frst); k++)
            {
                if (_mappingOrder.getUnchecked(i) == _mappingOrder.getUnchecked(k))
                    solveConflict(i, k);
            }
        }
    }
    
    // set _mapping_score to the mean of the weights of the mapped itemsets and return
    float count = 0.0;
    float total = 0.0;
    
    for (int i = 0; i < _mappingOrder.size(); i++)
    {
        if (_mappingOrder.getUnchecked(i) > -1)
        {
            total += _w.getReference(i)[_mappingOrder.getUnchecked(i)];
            count += 1.0;
        }
    }
          
    if (count != 0)
    {
        _mappingScore = total/count;
    } else {
        _mappingScore = 0.0;
    }

}

int S2MP::indexOfMax(Array<float>& weights)
{
    int mx = 0;
    
    for (int i = 1; i < weights.size(); i++)
	{
		if(weights.getUnchecked(mx) < weights.getUnchecked(i))
		{
			mx = i;
		}
	}
    
    return mx;
}

// The conflict: _mapping_order[i] and _mapping_order[k] are equal.
// Need to find possible resolutions to this conflict and pick the best one.
void S2MP::solveConflict (int i_, int k_)
{
    // First find what itemsets in sp2 are still available to be mapped to
    
    // Vector to hold indices of available sp2 itemsets
    // Find which of these ids are NOT in _mapping_order (and store in avbl)
    SortedSet<int> avbl; 
    
    for (int i = 0; i < _mappingOrder.size(); i++) 
    {
        avbl.add(i);
    }
    
    SortedSet<int> sortedMappingOrder;
    sortedMappingOrder.addArray(_mappingOrder.begin(), _mappingOrder.size());
    
    // Now find which of these ids are NOT in *_mappingOrder (and store in avbl)
    avbl.removeValuesIn(sortedMappingOrder);
        
    // Now evaluate all the scores for keeping _mapping_order[i] the same and changing _mapping_order[k]
    bool noAlternativeFound = true;
    float score = 0.0;
    float sc1 = 0.0;
    int sc1id = 0;
    
    for (int j = 0; j < avbl.size(); j++)
    {
        score = calcLocalSim (i_,
                              _mappingOrder.getUnchecked(i_),
                              _w.getReference(i_).getUnchecked(_mappingOrder.getUnchecked(i_)),
                              k_,
                              avbl.getUnchecked(j),
                              _w.getReference(k_).getUnchecked(avbl.getUnchecked(j)));
        if (score > sc1)
        {
            sc1 = score;
            sc1id = avbl.getUnchecked(j);
            noAlternativeFound = false;
        }
    }
    
    // Now evaluate all the scores for keeping _mapping_order[k] the same and changing _mapping_order[i]
    float sc2 = 0.0;
    int sc2id = 0;
    
    for (int j = 0; j < avbl.size(); j++)
    {        
        score = calcLocalSim (i_, 
                              avbl.getUnchecked(j), 
                              _w.getReference(i_).getUnchecked(avbl.getUnchecked(j)), 
                              k_,
                              _mappingOrder.getUnchecked(k_),
                              _w.getReference(k_).getUnchecked(_mappingOrder.getUnchecked(k_)));
        if (score > sc2)
        {
            sc2 = score;
            sc2id = avbl.getUnchecked(j);
            noAlternativeFound = false;
        }
    }
    
    // Pick the best resolution to the conflict
    
    // If no alternatives were found, remove the mapping associated with the lower weight
    if (noAlternativeFound)
    {
        // If matching weight associated with _mapping_order[i] is greater, 
        // Then keep i
        if (_w.getReference(i_).getUnchecked(_mappingOrder.getUnchecked(i_)) > 
            _w.getReference(k_).getUnchecked(_mappingOrder.getUnchecked(k_)))
        {
            _mappingOrder.set(k_, -1);
        }
        // Else keep k
        else
        {
            _mappingOrder.set(i_, -1);
        }
    }
    // If the best resolution was found by changing the _mapping_order[k], then implement this change
    else if (sc1 > sc2)
    {
        _mappingOrder.set(k_, sc1id);
    }
    // Else the best resolution is implemented by changing _mapping_order[i]
    else
    {
        _mappingOrder.set(k_, sc2id);
    }
}

float S2MP::calcLocalSim (int id1, int cid1, float w1, int id2, int cid2, float w2)
{
    if (((id1 > id2) && (cid1 > cid2)) || ((id1 < id2) && (cid1 < cid2)))
        return (w1+w2)/2.0;
    else
        return (w1+w2)/4.0;
}

// ==================================================================
// calculate the order score
// ==================================================================

// Calulate the order between each pair of matched itemsets in the sequences
void S2MP::calcOrderScore()
{
    Array <int> subs;
    Array <int> subsids;
	float totalOrder;
	float meanlen = ((float)_sp1len + (float)_sp2len)/2.0;
	float positionOrder;
	float score;
	float sm;
    
	_orderScore = 0.0;
    
    // Create an augmented version of mappingOrder with a 0 on the end
    // (necessary for method used to detect increasing subsequences)
    Array <int> tempMappingOrder(_mappingOrder);
    tempMappingOrder.add(0);
    
	for (int i = 0; i <= _sp1len; i++)
    {
		if (i == 0 || subs.size() == 0)
        {
			if (tempMappingOrder.getUnchecked(i) > -1)
            {
                subs.add(tempMappingOrder.getUnchecked(i));
                subsids.add(i);
            }
        }
        else 
        {
            // If this mapOrder value is less than the previous one, need to end subsequence
            if (tempMappingOrder.getUnchecked(i) < tempMappingOrder.getUnchecked(i - 1))
            {
                // Do calculations on current subsequence
                totalOrder = subs.size()/meanlen;
                
                // Calculate position order
				if(subs.size() ==1)
				{
					positionOrder = 0.0;
				}
                else 
                {
                    sm = 0.0;
                    
					for (int j = 1; j < subs.size(); j++)
					{
						sm += fabs(subs.getUnchecked(j) - subs.getUnchecked(j - 1)) - 
                        fabs(subsids.getUnchecked(j) - subsids.getUnchecked(j - 1));
					}
					positionOrder = sm/meanlen;
                }
                // Calculate score for this subsequence
				score = totalOrder*(1.0-positionOrder);
				
				// If this is the best score yet, then use it.
				if(score>_orderScore)
				{
					_orderScore = score;
				}
				
				/*
				 * Restart subsequence
				 */
				subs.clear();
				subsids.clear();
				
				if(tempMappingOrder.getUnchecked(i) > -1)
				{
					subs.add(tempMappingOrder.getUnchecked(i));
					subsids.add(i);
				}
			}
			else 
			{
				if(tempMappingOrder.getUnchecked(i) > -1)
				{
					subs.add(tempMappingOrder.getUnchecked(i));
					subsids.add(i);
				}
			}            
        }
    }                             
}

void S2MP::calcSimilarity()
{
    _similarity = (_mappingCo * _mappingScore) + (_orderCo * _orderScore);
}
