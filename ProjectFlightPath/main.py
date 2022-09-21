#%%
from mpl_toolkits.mplot3d import Axes3D
import mpl_toolkits.mplot3d.art3d as art3d
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Circle

def update_plot(plot, new_data):
    plot.set_xdata(np.append(plot.get_xdata(), new_data))
    plot.set_ydata(np.append(plot.get_ydata(), new_data))
    plt.draw()
    
"""
Class: Cylinder
Description: Initialize cylinder object for print
Param: radius - the radius of the cylinder
Param: height - the height of the cylinder
Param: color - the color of the cylinder
Return: None
"""
class Cylinder:
    def __init__(self, radius, height, color):
        self.radius = radius
        self.height = height
        self.color = color
        self.x_center = 0
        self.y_center = 0
        self.resolution = 100
        self.elevation = 0
    
    def print_fig(self):    #Is it better to just create a simpler transparent figure?
        #simpler implementation
        z = np.linspace(0, self.height, self.resolution)    #Create a figure with x height around z-axis
        theta = np.linspace(0, 2*np.pi, self.resolution)    #Create a theta value to mesh with 
        theta_grid, z_grid = np.meshgrid(theta, z)
        x_grid = self.radius * np.cos(theta_grid) + self.x_center
        y_grid = self.radius * np.sin(theta_grid) + self.y_center
        ax.plot_surface(x_grid, y_grid, z_grid, linewidth=0)
        
        """
        #Create buttom
        floor = Circle((self.x_center, self.y_center), self.radius, color=self.color)   #Create 2D circle object at x, y centerpoints with given radius
        ax.add_patch(floor)                                                             #Fill in circle with color
        art3d.pathpatch_2d_to_3d(floor, z=self.elevation, zdir='z')                     #Create 3D object and give z offset equal to elevation.

        #Create top
        ceiling = Circle((self.x_center, self.y_center), self.radius, color=self.color)
        ax.add_patch(ceiling)
        art3d.pathpatch_2d_to_3d(ceiling, z=self.elevation+self.height, zdir="z")
        """


class Drone(Cylinder):
    def __init__(self, cylinder):
        self.radius = cylinder.radius + 1
        self.height = cylinder.height + 1
        self.resolution = 1

        
    def flight_print(self):
        #Flight up (x: 0, y: -3, z: 0-6)
        self.x_coord = [0, 0, 0, 0, 0, 0, 0]
        self.y_coord = [-3, -3, -3, -3, -3, -3, -3]
        self.z_coord = list(range(0, self.height+1, self.resolution))
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        ax.plot(self.x_coord, self.y_coord, self.z_coord, color='r')
        
        #Flight over (x: 0, y: (-3)-3, z: 6)
        self.x_coord = [0, 0, 0, 0, 0, 0, 0]
        self.y_coord = list(range(-(self.radius), self.radius+1, self.resolution))
        self.z_coord = [6, 6, 6, 6, 6, 6, 6]
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        
        
        #Flight down (x: 0, y: 3, z: 6-0)
        self.x_coord = [0, 0, 0, 0, 0, 0, 0]
        self.y_coord = [3, 3, 3, 3, 3, 3, 3]
        self.z_coord = list(range(self.height, -1, -self.resolution))
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        
        #Flight around (x: 0-3, y: 3, z: 0)
        self.x_coord = list(range(0, self.radius+1, self.resolution))
        self.y_coord = [3, 3, 3, 3]
        self.z_coord = [0, 0, 0, 0]
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)

        #Flight around contd. (x: 3, y: 3-0, z: 0)
        self.x_coord = [3, 3, 3, 3]
        self.y_coord = list(range(self.radius, -1, -self.resolution))
        self.z_coord = [0, 0, 0, 0]
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        
        
        #Flight up
        self.x_coord = [3, 3, 3, 3, 3, 3, 3]
        self.y_coord = [0, 0, 0, 0, 0, 0, 0]
        self.z_coord = list(range(0, self.height+1, self.resolution))
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        
        
        #Flight over (x: 3-(-3), y: 0, z: 6)
        #self.x_coord = [3, 3, 3, 3, 3, 3, 3]
        self.x_coord = list(range(self.radius, -(self.radius+1), -self.resolution))
        self.y_coord = [0, 0, 0, 0, 0, 0, 0]
        self.z_coord = [6, 6, 6, 6, 6, 6, 6]
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        
        
        #Flight down
        self.x_coord = [-3, -3, -3, -3, -3, -3, -3]
        self.y_coord = [0, 0, 0, 0, 0, 0, 0]
        self.z_coord = list(range(self.height, -1, -self.resolution))
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        
        #Flight around
        self.x_coord = [-3, -3, -3, -3]
        self.y_coord = list(range(0, -(self.radius+1), -self.resolution))
        self.z_coord = [0, 0, 0, 0]
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)

        
        #Flight around contd.
        self.x_coord = list(range(-(self.radius), 1, self.resolution))
        self.y_coord = [-3, -3, -3, -3]
        self.z_coord = [0, 0, 0, 0]
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        
        
    def update_coordinates(x_coord, y_coord, z_coord):
        pass
        
        


fig=plt.figure()
ax = Axes3D(fig)

cylinder1 = Cylinder(2, 5, 'b')
drone1 = Drone(cylinder1)
cylinder1.print_fig()
drone1.flight_print()
ax.set_xlabel('x-axis')
ax.set_ylabel('y-axis')
ax.set_zlabel('z-axis')
plt.show()

#Thinking of implementing an update loop which updates the drones flight path live every x seconds. 


# %%

