#%%
from mpl_toolkits.mplot3d import Axes3D
import mpl_toolkits.mplot3d.art3d as art3d
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Circle
import statistics as st
import csv

def update_plot(plot, new_data):
    plot.set_xdata(np.append(plot.get_xdata(), new_data))
    plot.set_ydata(np.append(plot.get_ydata(), new_data))
    plt.draw()

    
def map_range(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min

def printcsv():
    with open('data/drone_local_position.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        line_count = 0
        for row in csv_reader:
            x.append(row[0])
            y.append(row[1])
            z.append(row[2])
            #print(f'x: {row[0]}, y: {row[1]}, z: {row[2]}')
            line_count += 1
        return x, y, z

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

    def map_coords(coordinate):
        """
        map the y-plane
        
        y: list of y-coordinates
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
    
        max_upper = max(upper_coordinate)
        min_upper = min(upper_coordinate)
        max_lower = max(lower_coordinate)
        min_lower = min(lower_coordinate)

        for data in coordinate:
            if(data >= mean_coordinate):
                new_x.append(map_range(data, min_upper,max_upper, self., 7))
            elif (data < mean_x):
                new_x.append(map_range(data, min_lower,max_lower,-6, -7))
        
        return new_x
        
        

    def map_z(self, z):
        min_z = min(z)
        max_z = max(z)
        median_z = st.median(z)
        
        
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
