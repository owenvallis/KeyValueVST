import json
import math
from pprint import pprint

class NaiveSequencer:

    def __init__(self):
        pass

    def set_database(self, database):
        for event in database:
            pass
        pass

    def return_next(self, bar):
        pass


def bars_from_datafile(filename, bar_length = 4.0):
    jdata = json.loads(open(filename, 'r').read())
    mode = jdata[0]

    # nb the following algorithm assumes all items are in timestamp order. test this:
    last_timestamp = -1
    for item in jdata[1:]:
        if item[4] < last_timestamp:
            print 'data sequence error'
            exit()
        last_timestamp = item[4]

    # split data into bars
    bar_start_timestamp = 0.0
    bar_end_timestamp = bar_start_timestamp + bar_length

    bars = []
    bar = []

    # for each item...
    for item in jdata[1:]:

        # if we're in the current bar, add it
        if item[4] <= bar_end_timestamp:
            bar.append(item)
            continue

        # else we're in the next bar. save the last bar and make a new one
        bars.append(bar)
        bar = []

        # move the window until we catch up with the item
        # n.b. item's probably in the next bar, but there might have been a pause
        while item[4] > bar_end_timestamp:
            # move the window
            bar_start_timestamp += bar_length
            bar_end_timestamp += bar_length

        # found it. save the item
        bar.append(item)

    return bars

def process_bars(bar1, bar2):
    # maps perfA in bar 1 to perfB in bar 2
    bar1_perf_a = [event for event in bar1 if event[0] == 'PerfA']
    bar2_perf_b = [event for event in bar2 if event[0] == 'PerfB']

    print bar1_perf_a
    print bar2_perf_b


if __name__ == '__main__':
    tfile = 'data/training_data/10_1331192251726.JSON'
    bars = bars_from_datafile(tfile, bar_length = 0.5)

    for i in range(len(bars)-1):
        process_bars(bars[i], bars[i+1])



# ==============================================================================
# old
# ==============================================================================

def generate_message_queues_from_datafiles():
    import os
    folder = 'data/training_data/'
    return [generate_message_queue_from_datafile(folder + filename) for filename in os.listdir(folder) if 'json' in filename.lower()]


def seperate_performers_from_datafile(filename):
    jdata = json.loads(open(filename, 'r').read())
    mode = jdata[0]

    # nb the following algorithm assumes all items are in timestamp order. test this:
    last_timestamp = -1
    for item in jdata[1:]:
        if item[4] < last_timestamp:
            print 'data sequence error'
            exit()
        last_timestamp = item[4]


    # split data into performers
    performers = {
        'PerfA': [],
        'PerfB': []
    }

    # for each item...
    for item in jdata[1:]:
        performers[item[0]].append(item)

    return performers

def message_queue_from_datafile(filename):
    jdata = json.loads(open(filename, 'r').read())
    mode = jdata[0]
    return [item for item in jdata[1:]]


def bars_from_events(events):
    bar_start_timestamp = 0.0
    bar_length = 4.0
    bar_end_timestamp = bar_start_timestamp + bar_length

    bars = []
    bar = []

    # for each item...
    for item in events:

        # if we're in the current bar, add it
        if item[4] <= bar_end_timestamp:
            bar.append(item)
            continue

        # else we're in the next bar. save the last bar and make a new one
        bars.append(bar)
        bar = []

        # move the window until we catch up with the item
        # n.b. item's probably in the next bar, but there might have been a pause
        while item[4] > bar_end_timestamp:
            # move the window
            bar_start_timestamp += bar_length
            bar_end_timestamp += bar_length

        # found it. save the item
        bar.append(item)

    return bars

