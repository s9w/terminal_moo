import matplotlib as mpl
import matplotlib.pyplot as plt
import os.path
import numpy as np


color_changes, fps = np.loadtxt("plot_data.txt", delimiter=' ', unpack=True)
color_lifetime = 1.0 / color_changes

fig = plt.figure(figsize=(6, 4))
ax = fig.add_subplot(111)
ax.plot(color_changes, fps, ".-")
ax.set_xlabel("color changes")
ax.set_ylabel("FPS")

fig.tight_layout()
fig.savefig("colors_vs_fps.png")
