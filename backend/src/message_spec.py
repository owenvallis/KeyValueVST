from pprint import pprint
import json

# '{"action": "learn", "data": {"perf_b": [6, 34, 96, 376.234558], "perf_a": [1, 56, 120, 374.234558]}}'
# '{"action": "playback", "data": [1, 56, 120, 374.234558]}'
# '{"action": "save_database", "data": "training_set_1.json"}'
# '{"action": "list_databases", "data": null}'
# '{"action": "load_database", "data": "training_set_1.json"}'

# ====== learning ======

perf_a = [  1,
  56,
  120,
  374.234558
]

perf_b = [  6,
  34,
  96,
  376.234558
]

example_msg = {
	'action':'learn',
	'data' : {
		'perf_a': perf_a,
		'perf_b': perf_b,
	}
}

pprint(json.dumps(example_msg))


# ====== playback ======

perf_a = [  1,
  56,
  120,
  374.234558
]

example_msg = {
	'action':'playback',
	'data': perf_a,
}

pprint(json.dumps(example_msg))

# ====== save and load database ======

example_msg = {
	'action':'save_database',
	'data': 'training_set_1.json'
}

pprint(json.dumps(example_msg))


example_msg = {
	'action': 'list_databases',
    'data': None
}

pprint(json.dumps(example_msg))


example_msg = {
	'action': 'load_database',
	'data': 'training_set_1.json'
}

pprint(json.dumps(example_msg))



# ====== example data from email ======


'''
[ "Learning
[
  "PerfA",
  1,
  56,
  120,
  374.234558
],
[
  "PerfB",
  6,
  34,
  96,
  376.234558
]
]
'''