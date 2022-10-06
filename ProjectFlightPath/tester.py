from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm

def f(x, y):
    return np.sqrt(x ** 2 + y ** 2)

fig = plt.figure()
ax = plt.axes(projection='3d')
# Can manipulate with 100j and 80j values to make your cone looks different
u, v = np.mgrid[0:2*np.pi:100j, 0:np.pi:80j]
x = np.cos(u)*np.sin(v)
y = np.sin(u)*np.sin(v)
z = f(x, y)

ax.plot_surface(x, y, z, cmap=cm.coolwarm)

# Some other effects you may want to try based on your needs:
# ax.plot_surface(x, y, -z, cmap=cm.coolwarm)
# ax.scatter3D(x, y, z, color="b")
# ax.plot_wireframe(x, y, z, color="b")
# ax.plot_wireframe(x, y, -z, color="r")

# Can set your view from different angles. 
ax.view_init(azim=15, elev=15)
ax.set_xlabel("x")
ax.set_ylabel("y")
ax.set_zlabel("z")
plt.show()
ax.set_ylabel("y")
ax.set_zlabel("z")
plt.show()

# Actually not sure about the math here though:
u, v = np.mgrid[0:2*np.pi:100j, 0:np.pi:20j]
x = np.cos(u)*np.sin(v)
y = np.sin(u)*np.sin(v)