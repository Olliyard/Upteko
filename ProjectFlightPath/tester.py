# import
#%%
%matplotlib inline
from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt

fig = plt.figure()
ax = plt.axes(projection = '3d')

# %%
##

# creating 3d plot using matplotlib  
# in python 

  
# for creating a responsive plot 

%matplotlib widget 

  
# importing required libraries 

from mpl_toolkits.mplot3d import Axes3D 

import matplotlib.pyplot as plt 

  
# creating random dataset 

xs = [14, 24, 43, 47, 54, 66, 74, 89, 12, 

      44, 1, 2, 3, 4, 5, 9, 8, 7, 6, 5] 

  

ys = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 6, 3, 

      5, 2, 4, 1, 8, 7, 0, 5] 

  

zs = [9, 6, 3, 5, 2, 4, 1, 8, 7, 0, 1, 2,  

      3, 4, 5, 6, 7, 8, 9, 0] 

  
# creating figure 

fig = plt.figure() 

ax = Axes3D(fig) 

  
# creating the plot 

plot_geeks = ax.scatter(xs, ys, zs, color='green') 

  
# setting title and labels 

ax.set_title("3D plot") 

ax.set_xlabel('x-axis') 

ax.set_ylabel('y-axis') 

ax.set_zlabel('z-axis') 

  
# displaying the plot 
plt.show() 

# %%
# creating 3d bar plot using matplotlib  
# in python 

# to interacte  with plot 
%matplotlib widget 

  
# importing required libraries 
from mpl_toolkits.mplot3d import Axes3D 
import matplotlib.pyplot as plt 
import numpy as np 

  
# creating random dataset 
xs = [2, 3, 4, 5, 1, 6, 2, 1, 7, 2] 
ys = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10] 

zs = np.zeros(10) 

dx = np.ones(10) 

dy = np.ones(10) 

dz = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10] 

  
# creating figure 

figg = plt.figure() 

ax = figg.add_subplot(111, projection='3d') 

  
# creating the plot 

plot_geeks = ax.bar3d(xs, ys, zs, dx, dy, dz, color='green') 

  
# setting title and labels 

ax.set_title("3D bar plot") 

ax.set_xlabel('x-axis') 

ax.set_ylabel('y-axis') 

ax.set_zlabel('z-axis') 

  
# displaying the plot 
plt.show() 

# %%
from mpl_toolkits.mplot3d import Axes3D
import mpl_toolkits.mplot3d.art3d as art3d
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Circle

def plot_3D_cylinder(radius, height, elevation=0, resolution=100, color='r', x_center = 0, y_center = 0):
    fig=plt.figure()
    ax = Axes3D(fig, azim=30, elev=30)

    x = np.linspace(x_center-radius, x_center+radius, resolution)
    z = np.linspace(elevation, elevation+height, resolution)
    X, Z = np.meshgrid(x, z)

    Y = np.sqrt(radius**2 - (X - x_center)**2) + y_center # Pythagorean theorem

    ax.plot_surface(X, Y, Z, linewidth=0, color=color)
    ax.plot_surface(X, (2*y_center-Y), Z, linewidth=0, color=color)

    floor = Circle((x_center, y_center), radius, color=color)
    ax.add_patch(floor)
    art3d.pathpatch_2d_to_3d(floor, z=elevation, zdir="z")

    ceiling = Circle((x_center, y_center), radius, color=color)
    ax.add_patch(ceiling)
    art3d.pathpatch_2d_to_3d(ceiling, z=elevation+height, zdir="z")

    ax.set_xlabel('x-axis')
    ax.set_ylabel('y-axis')
    ax.set_zlabel('z-axis')

    plt.show()

# params
radius = 1
height = 5
elevation = 0
resolution = 100
color = 'r'
x_center = 1
y_center = 1

plot_3D_cylinder(radius, height, elevation=elevation, resolution=resolution, color=color, x_center=x_center, y_center=y_center)
# %%
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Cylinder
x=np.linspace(-1, 1, 100)
z=np.linspace(-2, 2, 100)
Xc, Zc=np.meshgrid(x, z)
Yc = np.sqrt(1-Xc**2)

# Draw parameters
rstride = 20
cstride = 10
ax.plot_surface(Xc, Yc, Zc, alpha=0.2, rstride=rstride, cstride=cstride)
ax.plot_surface(Xc, -Yc, Zc, alpha=0.2, rstride=rstride, cstride=cstride)
# %%
#%%
from mpl_toolkits.mplot3d import Axes3D
import mpl_toolkits.mplot3d.art3d as art3d
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Circle

"""
equation for a circle

x  =  h + r cosθ
y  =  k + r sinθ

where h and k are the co-ordinates of the center

0 <= θ <= 360
"""
fig = plt.figure()
ax = Axes3D(fig)

theta = np.linspace(0, 2 * np.pi, 201)
radius = 10

x = np.linspace(0, 5, 20)

thetas, xs = np.meshgrid(theta, x)

y = radius * np.cos(thetas)
z = radius * np.sin(thetas)

ax.plot_surface(xs, y, z, color='orange')

ax.set_xticks([0, 5])
ax.set_yticks([-radius, 0, radius])
ax.set_zticks([-radius, 0, radius])

plt.show()
# %%
from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
from matplotlib import cm

fig = plt.figure()
ax = fig.gca(projection='3d')
X, Y, Z = axes3d.get_test_data(0.05)
ax.plot_surface(X, Y, Z, rstride=8, cstride=8, alpha=0.3)
cset = ax.contour(X, Y, Z, zdir='z', offset=-100, cmap=cm.coolwarm)
cset = ax.contour(X, Y, Z, zdir='x', offset=-40, cmap=cm.coolwarm)
cset = ax.contour(X, Y, Z, zdir='y', offset=40, cmap=cm.coolwarm)

ax.set_xlabel('X')
ax.set_xlim(-40, 40)
ax.set_ylabel('Y')
ax.set_ylim(-40, 40)
ax.set_zlabel('Z')
ax.set_zlim(-100, 100)

plt.show()
#%%
'''
===========================
More triangular 3D surfaces
===========================

Two additional examples of plotting surfaces with triangular mesh.

The first demonstrates use of plot_trisurf's triangles argument, and the
second sets a Triangulation object's mask and passes the object directly
to plot_trisurf.
'''

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.tri as mtri


fig = plt.figure(figsize=plt.figaspect(0.5))

#============
# First plot
#============

# Make a mesh in the space of parameterisation variables u and v
u = np.linspace(0, 2.0 * np.pi, endpoint=True, num=50)
v = np.linspace(-0.5, 0.5, endpoint=True, num=10)
u, v = np.meshgrid(u, v)
u, v = u.flatten(), v.flatten()

# This is the Mobius mapping, taking a u, v pair and returning an x, y, z
# triple
x = (1 + 0.5 * v * np.cos(u / 2.0)) * np.cos(u)
y = (1 + 0.5 * v * np.cos(u / 2.0)) * np.sin(u)
z = 0.5 * v * np.sin(u / 2.0)

# Triangulate parameter space to determine the triangles
tri = mtri.Triangulation(u, v)

# Plot the surface.  The triangles in parameter space determine which x, y, z
# points are connected by an edge.
ax = fig.add_subplot(1, 2, 1, projection='3d')
ax.plot_trisurf(x, y, z, triangles=tri.triangles, cmap=plt.cm.Spectral)
ax.set_zlim(-1, 1)


#============
# Second plot
#============

# Make parameter spaces radii and angles.
n_angles = 36
n_radii = 8
min_radius = 0.25
radii = np.linspace(min_radius, 0.95, n_radii)

angles = np.linspace(0, 2*np.pi, n_angles, endpoint=False)
angles = np.repeat(angles[..., np.newaxis], n_radii, axis=1)
angles[:, 1::2] += np.pi/n_angles

# Map radius, angle pairs to x, y, z points.
x = (radii*np.cos(angles)).flatten()
y = (radii*np.sin(angles)).flatten()
z = (np.cos(radii)*np.cos(angles*3.0)).flatten()

# Create the Triangulation; no triangles so Delaunay triangulation created.
triang = mtri.Triangulation(x, y)

# Mask off unwanted triangles.
xmid = x[triang.triangles].mean(axis=1)
ymid = y[triang.triangles].mean(axis=1)
mask = np.where(xmid**2 + ymid**2 < min_radius**2, 1, 0)
triang.set_mask(mask)

# Plot the surface.
ax = fig.add_subplot(1, 2, 2, projection='3d')
ax.plot_trisurf(triang, z, cmap=plt.cm.CMRmap)


plt.show()

#%%
fig=plt.figure()
ax = Axes3D(fig, azim=30, elev=30)

def plot_3D_cylinder(radius, height, elevation=0, resolution=100,  x_center = 0, y_center = 0):
    x = np.linspace(x_center-radius, x_center+radius, resolution)
    z = np.linspace(elevation, elevation+height, resolution)
    X, Z = np.meshgrid(x, z)

    Y = np.sqrt(radius**2 - (X - x_center)**2) + y_center # Pythagorean theorem

    ax.plot_surface(X, Y, Z, linewidth=0, color='g')
    ax.plot_surface(X, (2*y_center-Y), Z, linewidth=0, color='r')

    floor = Circle((x_center, y_center), radius, color='b')
    ax.add_patch(floor)
    art3d.pathpatch_2d_to_3d(floor, z=elevation, zdir="z")

    ceiling = Circle((x_center, y_center), radius, color='y')
    ax.add_patch(ceiling)
    art3d.pathpatch_2d_to_3d(ceiling, z=elevation+height, zdir="z")

    ax.set_xlabel('x-axis')
    ax.set_ylabel('y-axis')
    ax.set_zlabel('z-axis')

    plt.show()

def flight_path():
    

# params
radius = 1
height = 5
elevation = 0
resolution = 100

x_center = 1
y_center = 1

plot_3D_cylinder(radius, height, elevation=elevation, resolution=resolution, x_center=x_center, y_center=y_center)
