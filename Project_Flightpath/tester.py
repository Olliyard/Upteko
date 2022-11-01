import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
xfrom mpl_toolkits.mplot3d import Axes3D


fig = plt.figure()
ax = Axes3D(fig)

x = [0, 1, 1, 0], [0, 1, 1, 0]
y = [0, 0, 1, 1], [0, 0, 1, 1]
z = [0, 0, 0, 0], [1, 1, 1, 1]

surfaces = []

for i in range(len(x)):
    surfaces.append([list(zip(x[i], y[i], z[i]))])

for surface in surfaces:
    ax.add_collection3d(Poly3DCollection(surface))

plt.show()

"""
    # ---------------------- MAKE CYAN VECTORS ------------------- #
    variation = 0.3
    new_u = r*np.cos(yaw_z[i]+variation)
    new_w = r*np.sin(yaw_z[i]+variation)
    self.ax.quiver(xp[i], yp[i], zp[i], new_u, new_w, 0, color='c')

    variation = -variation
    new_u = r*np.cos(yaw_z[i]+variation)
    new_w = r*np.sin(yaw_z[i]+variation)
    self.ax.quiver(xp[i], yp[i], zp[i], new_u, new_w, 0, color='c')

    # ---------------------- MAKE MAGENTA VECTORS ------------------- #
    variation = 0
    self.ax.quiver(xp[i]+u, yp[i]+w, zp[i], np.cos(variation), np.sin(variation), 0, color='m')

    variation = -np.pi
    self.ax.quiver(xp[i]+u, yp[i]+w, zp[i], np.cos(variation), np.sin(variation), 0, color='m')

    # ---------------------- NEEDS TESTING ------------------- #
    """
