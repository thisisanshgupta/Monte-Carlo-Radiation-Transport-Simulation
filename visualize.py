import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt('detector_data.txt')
x = data[:, 0]
y = data[:, 1]

plt.figure(figsize=(10, 6))
plt.plot(x, y, '-')
plt.fill_between(x, y, alpha=0.3)
plt.title('CT Scan Detector Readings')
plt.xlabel('Detector Position')
plt.ylabel('Detected Energy')
plt.grid(True, linestyle='--', alpha=0.7)


#plt.style.use('seaborn')
plt.savefig('ct_scan_graph.png', dpi=300, bbox_inches='tight')
print("Graph saved as ct_scan_graph.png")

#plt.show()