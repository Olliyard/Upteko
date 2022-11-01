from matplotlib.widgets import Slider
from matplotlib.widgets import Button
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.proj3d import proj_transform
from matplotlib.patches import FancyArrowPatch
from mpl_toolkits.mplot3d.axes3d import Axes3D
import numpy as np

#Class 3D Arrow
class Arrow3D(FancyArrowPatch):
    def __init__(self, x, y, z, x2, y2, z2, *args, **kwargs):
        super().__init__((0, 0), (0, 0), *args, **kwargs)
        self._xyz = (x, y, z)
        self._x2y2z2 = (x2, y2, z2)

    def draw(self, renderer):
        x1, y1, z1 = self._xyz
        x2, y2, z2 = self._x2y2z2

        xs, ys, zs = proj_transform((x1, x2), (y1, y2), (z1, z2), self.axes.M)
        self.set_positions((xs[0], ys[0]), (xs[1], ys[1]))
        super().draw(renderer)
        
    def do_3d_projection(self, renderer=None):
        x1, y1, z1 = self._xyz
        x2, y2, z2 = self._x2y2z2

        xs, ys, zs = proj_transform((x1, x2), (y1, y2), (z1, z2), self.axes.M)
        self.set_positions((xs[0], ys[0]), (xs[1], ys[1]))

        return np.min(zs) 

def _arrow3D(ax, x, y, z, x2, y2, z2, *args, **kwargs):
    '''Add an 3d arrow to an `Axes3D` instance.'''

    arrow = Arrow3D(x, y, z, x2, y2, z2, *args, **kwargs)
    ax.add_artist(arrow)

#Used for resetting values
def reset(event):
    radius_slider.reset()
    height_slider.reset()
    xcenter_slider.reset()
    ycenter_slider.reset()
    ax.axes.set_xlim3d(left=-5, right=10) 
    ax.axes.set_ylim3d(bottom=-5, top=10) 
    ax.axes.set_zlim3d(bottom=0, top=50) 


#Create arrows following direction of coords.
def draw_Arrows(x, y, z):
    step = 10
    for i in range(0, len(x)-step, step):
        ax.arrow3D(x[i], y[i], z[i], x[i+step], y[i+step], z[i+step], mutation_scale=10, fc="red")

setattr(Axes3D, 'arrow3D', _arrow3D)


def draw_cylinder():
    
    pass

#Create 3D figure
fig = plt.figure()
ax = plt.axes(projection = '3d')

#Set axis labels
ax.set_xlabel('x-axis [m]')
ax.set_ylabel('y-axis [m]')
ax.set_zlabel('z-axis [m]')

