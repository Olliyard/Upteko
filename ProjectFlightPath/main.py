#%%
from mpl_toolkits.mplot3d import Axes3D
import mpl_toolkits.mplot3d.art3d as art3d
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Circle
import statistics as st
import csv


def printcsv():
    """
    Convert a csv file to x, y and z coordinates.
    """
    x = []
    y = []
    z = []
    with open('drone_local_position.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        line_count = 0
        for row in csv_reader:
            x.append(row[0])
            y.append(row[1])
            z.append(row[2])
            #print(f'x: {row[0]}, y: {row[1]}, z: {row[2]}')
            line_count += 1
        x = list(map(float, x))
        y = list(map(float, y))
        z = list(map(float, z))
        return x, y, z


def map_range(x, in_min, in_max, out_min, out_max):
    """
    Map the lower and upper values of dataset to a specified boundary and adjust accordingly.
    x: data list
    in_min: the lowest value of the x dataset
    in_max: the highest value of the x dataset
    out_min: the user defined lower bound
    out_max: the user defined upper bound
    """
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


"""
Class: Cylinder
Description: Initialize cylinder object for print
Param: radius - the radius of the cylinder
Param: height - the height of the cylinder
Param: color - the color of the cylinder
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
    
    def print_fig(self):
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
        self.radius = cylinder.radius
        self.height = cylinder.height
        self.resolution = 1
        self.max_upper = self.radius+2
        self.min_upper = self.radius+1
        self.max_lower = -self.radius-2
        self.min_lower = -self.radius-1

    def map_coords(self, coordinate):
        """
        map the coordinates
        
        coordinate: list of coordinates
        """
        mean_coordinate = st.mean(coordinate)
        new_coordinate = []
        upper_coordinate = []
        lower_coordinate =[]

        for data in coordinate:
            if(data >= mean_coordinate):
                upper_coordinate.append(data)
            else:
                lower_coordinate.append(data)
    
        max_upper_coord = max(upper_coordinate)
        min_upper_coord = min(upper_coordinate)
        max_lower_coord = max(lower_coordinate)
        min_lower_coord = min(lower_coordinate)

        for data in coordinate:
            if(data >= mean_coordinate):
                new_coordinate.append(map_range(data, min_upper_coord, max_upper_coord, self.min_upper, self.max_upper))
            elif (data < mean_coordinate):
                new_coordinate.append(map_range(data, min_lower_coord, max_lower_coord, self.min_lower, self.max_lower))
        
        return new_coordinate
        
    def update_coordinates(x_coord, y_coord, z_coord):
        pass
        


fig=plt.figure()
ax = Axes3D(fig)

#Create cylinder object with radius 2, height 5 and color 'blue'
cylinder1 = Cylinder(3, 20, 'b')

#Create drone object which inherits the beforementioned cylinder object values.
drone1 = Drone(cylinder1)

#Print the cylinder object
cylinder1.print_fig()

x, y, z  =printcsv()
ax.scatter(x,y,z)
"""
new_x = drone1.map_coords(x)
new_y = drone1.map_coords(y)
ax.scatter(new_x, new_y, z)
"""

#Print the drone flight path
#drone1.flight_print()

ax.set_xlabel('x-axis')
ax.set_ylabel('y-axis')
ax.set_zlabel('z-axis')
plt.show()

#Thinking of implementing an update loop which updates the drones flight path live every x seconds. 

# %%
