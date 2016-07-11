from EventStore import EventStore
import numpy as np

events = EventStore(['output.root'])
print 'number of events: ', len(events)
pos = []
for store in events:
    clusters = store.get('clusters')
    
    for c in clusters:
        cor = c.Core().position
        pos.append([cor.X, cor.Y, cor.Z])
pos = np.array(pos)
print "number of hits: ", len(pos)
np.savetxt('hit_positions.dat', pos)
