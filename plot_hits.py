import matplotlib.pyplot as plt
import numpy as np

dat = np.loadtxt('hit_positions.dat')
truth = {'eta': [], 'phi': []}
with open('output.txt') as f:
  for line in f:
      if 'DEBUG particle eta' in line:
        #SimG4Alg.Geanti...  DEBUG particle eta, phi  = -0.0047039 1.42423
        eta, phi = line.split()[-2:]
        eta, phi = float(eta), float(phi)
        truth['eta'].append(eta)
        truth['phi'].append(phi)

def plot_truth_radial(truth, rmax=900, ax=None):
  for phi in truth['phi']:
    if ax:
      ax.plot([0,rmax*np.cos(phi)], [0, rmax* np.sin(phi)], color='blue', alpha=0.3)

def plot_truth_zr(truth, rmax=1000, ax=None):
  for theta in 2 * np.arctan(np.exp(-np.array(truth['eta']))):
      ax.plot([0,rmax*np.cos(theta)], [0, rmax* np.sin(theta)], color='green', alpha=0.3)

fig = plt.figure()
ax = plt.axes()
ax.plot(dat[:,0], dat[:,1], 'o')
#plot_truth_radial(truth, ax=ax)
fig = plt.figure()
ax = plt.axes()
ax.plot(dat[:,2], np.sqrt(dat[:,0]**2 + dat[:,1]**2), 'o')
#plot_truth_zr(truth, ax=ax)
fig = plt.figure()
ax = plt.axes()
ax.plot(dat[:,2], dat[:,1], 'o')
plt.show()
        
