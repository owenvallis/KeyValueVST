import json
import time
import random
from pprint import pprint
import SocketServer

# ======= config =======

SERVER_HOST = 'localhost'
SERVER_PORT = 10001

# =================================================================
# generate messages
# =================================================================

# ======= generete messages in memory =======

def generate_learn_message():
    perf_a = [  1,
        random.randint(0,127),
        random.randint(0,255),
        random.uniform(0,1000) # float
    ]

    perf_b = [  6,
        random.randint(0,127),
        random.randint(0,255),
        random.uniform(0,1000)
    ]

    return {
        'action':'learn',
        'data' : {
            'perf_a': perf_a,
            'perf_b': perf_b,
        }
    }


def generate_message_queue():
    message_queue = []

    # list databases
    message_queue.append({
        'action':'list_databases',
        'data': None
    })

    # load db
    message_queue.append({
        'action':'load_database',
        'data': 'training_set_1.json'
    })

    # test sending an invalid message
    message_queue.append({
        'action':'invalid',
        'data': 'test sending an invalid message'
    })

    # learning data
    for i in range(0, 5):
        message_queue.append(generate_learn_message())

    # save db
    message_queue.append({
        'action':'save_database',
        'data': 'training_set_1.json'
    })

    return message_queue


# ======= generete messages from files =======

def generate_message_queue_from_datafile(filename):
    jdata = json.loads(open(filename, 'r').read())
    mode = jdata[0]
    return [{'action': mode, 'data': item} for item in jdata[1:]]

def generate_message_queues_from_datafiles():
    import os

    event_fields = ['performer', 'MIDI channel', 'cc number', 'cc value', 'MIDI timestamp']
    folder = 'data/training_data/'

    return [generate_message_queue_from_datafile(folder + filename) for filename in os.listdir(folder) if 'json' in filename.lower()]

message_queues = generate_message_queues_from_datafiles()
message_queue = message_queues[0]

# ======= server =======

class VSTSimulator(SocketServer.BaseRequestHandler):
    daemon_threads = True
    allow_reuse_address = True

    def setup(self):
        print 'Client connected: %s:%s' % self.client_address

    def handle(self):
        for msg in message_queue:
            print "Sending data"
            jdata = json.dumps(msg)
            self.request.send(jdata + '\n')

            print "Listening...",
            data = self.request.recv(1024)
            jdata = json.loads(data)
            print "received: %s " % jdata

            print "Sleeping for a second"
            time.sleep(1)

        return

    def finish(self):
        print 'Client disconnected: %s:%s' % self.client_address

server = SocketServer.ThreadingTCPServer((SERVER_HOST, SERVER_PORT), VSTSimulator)
server.serve_forever()
