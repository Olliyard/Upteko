#%%
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Circle
import statistics as st
import csv

def printcsv():
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
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


class Cylinder:
    def __init__(self, radius = 1, height = 2, x_center = 0, y_center = 0, elevation = 0, color = 'b'):
        self.radius = radius
        self.height = height
        self.color = color
        self.x_center = x_center
        self.y_center = y_center
        self.resolution = 100
        self.elevation = elevation
    
    def print_fig(self):
        #simpler implementation
        z = np.linspace(self.elevation, self.elevation + self.height, self.resolution)    #Create a figure with x height around z-axis
        theta = np.linspace(0, 2*np.pi, self.resolution)    #Create a theta value to mesh with 
        theta_grid, z_grid = np.meshgrid(theta, z)
        x_grid = self.radius * np.cos(theta_grid) + self.x_center
        y_grid = self.radius * np.sin(theta_grid) + self.y_center
        ax.plot_surface(x_grid, y_grid, z_grid, linewidth=0)
    
    
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
        

#Create 3D figure
fig=plt.figure()
ax = Axes3D(fig)

#Create cylinder object with radius 3, height 20, x center value of 4, y center value of 0, elevation of 15 and color 'blue'
radius = 3
height = 20
x_center = 4
y_center = 0
elevation = 15
color = 'b'
cylinder1 = Cylinder(radius, height, x_center, y_center, elevation, color)

#Create drone object which inherits the beforementioned cylinder object values.
drone1 = Drone(cylinder1)

#Print the cylinder object
cylinder1.print_fig()

#Set x,y,z values from .csv file
x, y, z =printcsv()
ax.scatter(x,y,z)
ax.plot(x, y, z)

#Set axis labels
ax.set_xlabel('x-axis')
ax.set_ylabel('y-axis')
ax.set_zlabel('z-axis')
plt.show()

# %%





"""
new_x = drone1.map_coords(x)
new_y = drone1.map_coords(y)
ax.scatter(new_x, new_y, z)
"""