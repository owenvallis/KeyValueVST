import os
import socket
import sys
import json

# ======= config =======

DATA_FOLDER = 'data/'
SERVER_PORT = 10001

''' example messages
'{"action": "learn", "data": {"perf_b": [6, 34, 96, 376.234558], "perf_a": [1, 56, 120, 374.234558]}}'
'{"action": "playback", "data": [1, 56, 120, 374.234558]}'
'{"action": "save_database", "data": "training_set_1.json"}'
'{"action": "list_databases"}'
'{"action": "load_database", "data": "training_set_1.json"}'
'''

class MessageHandler:

    def __init__(self):
        self.database = []

    def router(self, message):
        actions = {
            'learn': self.learn,
            'playback': self.playback,
            'Performance': self.playback,
            'save_database': self.save_database,
            'list_databases': self.list_databases,
            'load_database': self.load_database,
        }

        if message['action'] not in actions:
            print "Recieved unknown action: %s" % message['action']
            return 1

        print "Recieved action: %s " % message['action']
        return actions[message['action']](message['data'])

    # ======= actions =======

    def learn(self, data):
        self.database.append(data)
        return 0

    def playback(self, data):
        print "Playback data: %s" % data
        # algorithm goes here
        perf_b = [  6, 34, 96, 376.234558 ]
        return perf_b

    def save_database(self, data):
        print "Saving database to: %s" % data

        f = open(DATA_FOLDER + data,"w")
        f.writelines(json.dumps(self.database))
        f.close()
        return 0

    def list_databases(self, data):
        return [filename for filename in os.listdir(DATA_FOLDER)]

    def load_database(self, data):
        print "Loading database from: %s" % data

        f = open(DATA_FOLDER + data, 'r')
        self.database = json.loads(f.read())
        f.close()

        print "Loaded %s (%s items)" % (data, len(self.database))
        return 0


class SocketListener:

    def __init__(self, message_handler):
        # Create a TCP/IP socket and listen on it
        self.sock = socket.create_connection(('127.0.0.1', SERVER_PORT))
        self.message_handler = message_handler

    def run(self):
        while True:
            try:
                # listen for next line - blocks and waits for newline
                data = self.sock.makefile().readline()
                response = self.message_handler.router(json.loads(data))
                response = json.dumps(response)
                print "Response: %s" % response
                self.sock.send(response)
            except ValueError, e:
                print e
                exit()


message_handler = MessageHandler()
socket_listener = SocketListener(message_handler)
socket_listener.run()