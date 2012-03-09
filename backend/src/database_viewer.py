import os
import socket
import sys
import json
import time

# ======= config =======

DATA_FOLDER = 'data/'
SERVER_PORT = 10000

# ======= actions =======

database = []

def list_databases(data):
    return [filename for filename in os.listdir(DATA_FOLDER)]

def load_database(data):
    print "Loading database from: %s" % data

    f = open(DATA_FOLDER + data, 'r')
    database = json.loads(f.read())
    f.close()

    print "Loaded %s (%s items)" % (data, len(database))

print list_databases(None)

for name in list_databases(None):
    load_database(name)