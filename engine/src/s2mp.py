import math

class S2MP:

    def __init__(self):
        # similarity calculation coefficients
        self._mapping_co = 0.5
        self._order_co = 0.5

    def compare_sequences(self, sp1, sp2, sp1frst=0, sp1len=None, sp2frst=0, sp2len=None):
        '''Calculates similarity between two lists of lists'''

        # set ranges from sequences
        self._sp1 = sp1
        self._sp1range = range(sp1frst, sp1len if sp1len else len(sp1))

        self._sp2 = sp2
        self._sp2range = range(sp2frst, sp2len if sp2len else len(sp2))

        # do calculations
        self._weights = self._calc_weight_matrix()
        self._calc_mapping_score()
        self._calc_order_score()

        similarity = (self._mapping_co * self._mapping_score) + (self._order_co * self._order_score)
        return similarity

    # ==================================================================
    # calculate the mapping score
    # ==================================================================

    def _calc_weight_matrix(self):
        '''Calulate the matching weights between each pair of itemsets in the sequences, stores in a matrix'''
        weights = []

        # for each ith itemset in seq1, calculate matching weight for each jth item in seq2
        for itemset_i in self._sp1:
            row = []

            for itemset_j in self._sp2:
                # matching weight  = (num elements in intersection)/(avg num of elements in itemsets)
                intersection = list(set(itemset_i).intersection(itemset_j))
                row.append(
                    len(intersection) / (len(itemset_i) + len(itemset_j) / 2.0)
                )

            weights.append(row)

        return weights

    def _calc_mapping_score(self):
        '''Calculate the mapping order and mapping score'''
        self._mapping_order = []

        # keep track of the number of matching weights equal to zero
        count_minus_ones = 0
        row = 0

        # find the position of each row's max matching weight
        for itemset_i in self._weights:

            # in the case of two matching Max weights, we take the first one
            weight_pos = itemset_i.index(max(itemset_i))

            # set all mappings associated with a meanininglessly small weight to -1
            if self._weights[row][weight_pos] < 0.00001:
                weight_pos = -1
                count_minus_ones += 1

            # store matching weight - max weight in this row
            self._mapping_order.append(weight_pos)

            row += 1

        # If there are no mappings, return with mappingScore = 0
        if count_minus_ones == self._mapping_order:
            self._mapping_score = 0
            return

        # Now detect conflicts/duplicates (two itemsets in sp1 mapped to the same itemset in sp2)
        for i in range(0, len(self._mapping_order)):

            # if we have a none zero matching weight, find conflict and resolve
            if self._mapping_order[i] > -1:
                for k in range(i + 1, len(self._mapping_order)):
                    if self._mapping_order[i] == self._mapping_order[k]:
                        self._solve_conflict(i, k)

        # set _mapping_score to the mean of the weights of the mapped itemsets and return
        count = 0
        total = 0.0

        for i in range(0, len(self._mapping_order)):
            if self._mapping_order[i] > -1:
                total += self._weights[i][self._mapping_order[i]]
                count += 1

        if count == 0:
            self._mapping_score = 0
        else:
            self._mapping_score = total/count

    def _solve_conflict(self, i, k):
        '''The conflict: _mapping_order[i] and _mapping_order[k] are equal.
        Need to find possible resolutions to this conflict and pick the best one.'''

        # First find what itemsets in sp2 are still available to be mapped to

        # Vector to hold indices of available sp2 itemsets
        # Find which of these ids are NOT in _mapping_order (and store in avbl)
        avbl = list(set(range(len(self._sp2range))) - set(self._mapping_order))

        # Now evaluate all the scores for keeping _mapping_order[i] the same and changing _mapping_order[k]
        no_alternative_found = True
        score = 0
        sc1 = 0.0
        sc1id = 0

        for j in range(len(avbl)):
            score = self._calc_local_sim(i,
                self._mapping_order[i],
                self._weights[i][self._mapping_order[i]],
                k,
                avbl[j],
                self._weights[k][avbl[j]])
            if score > sc1:
                sc1 = score
                sc1id = avbl[j]
                no_alternative_found = False

        # Now evaluate all the scores for keeping _mapping_order[j] the same and changing _mapping_order[i]
        sc2 = 0.0
        sc2id = 0

        for j in range(len(avbl)):
            score = self._calc_local_sim(i, 
                avbl[j], 
                self._weights[i][avbl[j]], 
                k,
                self._mapping_order[k],
                self._weights[k][self._mapping_order[k]])
            if score > sc2:
                sc2 = score
                sc2id = avbl[j]
                no_alternative_found = False

        # Pick the best resolution to the conflict

        # If no alternatives were found, remove the mapping associated with the lower weight
        if no_alternative_found:

            # If matching weight associated with _mapping_order[i] is greater, then keep i
            if self._weights[i][self._mapping_order[i]] > self._weights[k][self._mapping_order[k]]:
                self._mapping_order[k] = -1
            # Else keep k
            else:
                self._mapping_order[i] = -1

        # If the best resolution was found by changing the _mapping_order[k], then implement this change
        elif sc1 > sc2:
            self._mapping_order[k] = sc1id

        # Else the best resolution is implemented by changing _mapping_order[i]
        else:
            self._mapping_order[i] = sc2id

    def _calc_local_sim(self, id1, cid1, w1, id2, cid2, w2):
        if ((id1 > id2) and (cid1 > cid2)) or ((id1 < id2) and (cid1 < cid2)):
            return (w1+w2)/2.0
        else:
            return (w1+w2)/4.0

    # ==================================================================
    # calculate the order score
    # ==================================================================

    def _calc_order_score(self):
        subs = []
        subsids = []
        total_order = None
        meanlen = (float(len(self._sp1range)) + float(len(self._sp2range)))/2.0
        position_order = None
        score = None
        lSubs = None
        sm = None

        self._order_score = 0.0

        # Create an augmented version of mappingOrder with a 0 on the end
        # (necessary for method used to detect increasing subsequences)
        temp_mapping_order = self._mapping_order
        temp_mapping_order.append(0)

        for i in range(len(self._sp1range) + 1):
            if i == 0 or len(subs) == 0:
                if temp_mapping_order[i] > -1:
                    subs.append(temp_mapping_order[i])
                    subsids.append(i)

                continue

            # If this mapOrder value is less than the previous one, need to end subsequence
            if temp_mapping_order[i] < temp_mapping_order[i-1]:

                # Do calculations on current subsequence
                total_order = len(subs)/meanlen

                # Calculate position order
                if len(subs) == 1:
                    position_order = 0.0
                else:
                    sm = 0.0

                    for j in range(1, len(subs)):
                        sm += math.fabs(subs[j] - subs[j-1]) - math.fabs(subsids[j] - subsids[j-1])

                    position_order = sm/meanlen

                # Calculate score for this subsequence
                score = total_order * (1.0 - position_order)

                # If this is the best score yet, then use it.
                if score > self._order_score:
                    self._order_score = score

                # Restart subsequence
                del subs[:]
                del subsids[:]

                if temp_mapping_order[i] > -1:
                    subs.append(temp_mapping_order[i])
                    subsids.append(i)

            else:
                if temp_mapping_order[i] > -1:
                    subs.append(temp_mapping_order[i])
                    subsids.append(i)


def test_datafiles():
    import os
    import json

    event_fields = ['performer', 'MIDI channel', 'cc number', 'cc value', 'MIDI timestamp']

    folder = 'data/training_data/'
    filenames = [filename for filename in os.listdir(folder) if 'json' in filename.lower()]
    for filename in filenames:
        jdata = json.loads(open(folder + filename, 'r').read())

        mode = jdata[0]
        print mode

        # for item in jdata[1:]:
        for item in jdata[1:10]:
            # event = dict(zip(event_fields, item))
            print item
            # MIDI Channel, the CC number, the CC value, and the MIDI timestamp

        exit()


    exit()

def generate_sequence(itemsets, itemset_length):
    import random
    return [
        [random.randint(0,255) for i in range(itemset_length)]
        for j in range(itemsets)
    ]

def test_performance():
    s2mp = S2MP()

    sequence_length = 10
    itemset_length = 10
    step = 10

    while True:
        seq1 = generate_sequence(sequence_length, itemset_length)
        seq2 = generate_sequence(sequence_length, itemset_length)

        print "Comparing sequence length %s, itemset length %s..." % (sequence_length, itemset_length)
        print s2mp.compare_sequences(seq1, seq2)

        sequence_length += step
        itemset_length += step


def test_hardcoded():
    seq2 = [
        [1,2,3],
        [4,5,6]
    ]

    tests = [
        [[1,2,3],[4,5,6]],
        [[4,5,6],[1,2,3]],
        [[1,2],[4,5,6]],
        [[4,4,4]],
        [[4,5,6]],
        [[6,5,4]],
        [[7,6,5]],
        [[5,6]],
        [[-1,-2,-3]]
    ]

    s2mp = S2MP()

    print "Sequence: %s\n" % (seq2)
    for seq in tests:
        print "Comparison sequence: %s\nresult: %s" % (seq, s2mp.compare_sequences(seq, seq2))


if __name__ == '__main__':
    # test_hardcoded()
    test_datafiles()
    # test_performance()

