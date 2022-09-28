import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.proj3d import proj_transform
from mpl_toolkits.mplot3d.axes3d import Axes3D
from matplotlib.patches import FancyArrowPatch


class Arrow3D(FancyArrowPatch):

    def __init__(self, x, y, z, x2, y2, z2, *args, **kwargs):
        super().__init__((0, 0), (0, 0), *args, **kwargs)
        self._xyz = (x, y, z)
        #self._x2y2z2 = (x2, y2, z2)
        self._x2y2z2 = (x2, y2, z2)

    def draw(self, renderer):
        x1, y1, z1 = self._xyz
        #x2, y2, z2 = self._x2y2z2
        x2, y2, z2 = self._x2y2z2
        #x2, y2, z2 = (x1 + x2, y1 + y2, z1 + z2)

        xs, ys, zs = proj_transform((x1, x2), (y1, y2), (z1, z2), self.axes.M)
        self.set_positions((xs[0], ys[0]), (xs[1], ys[1]))
        super().draw(renderer)
        
    def do_3d_projection(self, renderer=None):
        x1, y1, z1 = self._xyz
        #x2, y2, z2 = self._x2y2z2
        x2, y2, z2 = self._x2y2z2
        #x2, y2, z2 = (x1 + x2, y1 + y2, z1 + z2)

        xs, ys, zs = proj_transform((x1, x2), (y1, y2), (z1, z2), self.axes.M)
        self.set_positions((xs[0], ys[0]), (xs[1], ys[1]))

        return np.min(zs) 

def _arrow3D(ax, x, y, z, x2, y2, z2, *args, **kwargs):
    '''Add an 3d arrow to an `Axes3D` instance.'''

    arrow = Arrow3D(x, y, z, x2, y2, z2, *args, **kwargs)
    ax.add_artist(arrow)


setattr(Axes3D, 'arrow3D', _arrow3D)


fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.arrow3D(1,1,1,
           1,2,3,
           mutation_scale=10,
           ec ='green',
           fc='blue')
ax.arrow3D(1,0,0,
           1,1,1,
           mutation_scale=20,
           ec ='green',
           fc='red')
ax.set_title('3D Arrows Demo')
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.set_zlabel('z')
plt.show()