#%%
#Includes
from locale import normalize
import matplotlib.pyplot as plt
import numpy as np
import statistics as st
import csv
from matplotlib.widgets import Slider
from matplotlib.widgets import Button
from mpl_toolkits.mplot3d.proj3d import proj_transform
from mpl_toolkits.mplot3d.axes3d import Axes3D
from matplotlib.patches import FancyArrowPatch


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

setattr(Axes3D, 'arrow3D', _arrow3D)


#Get .csv file and put it into x,y,z list
def printcsv():
    x = []
    y = []
    z = []
    with open('drone_local_position.csv') as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        for row in csv_reader:
            x.append(row[0])
            y.append(row[1])
            z.append(row[2])
        x = list(map(float, x))
        y = list(map(float, y))
        z = list(map(float, z))
        return x, y, z

#Map the values of the coordinates to pre-set boundaries
def map_range(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

#Used for resetting values
def reset(event):
    radius_slider.reset()
    height_slider.reset()
    xcenter_slider.reset()
    ycenter_slider.reset()
    ax.axes.set_xlim3d(left=-5, right=10) 
    ax.axes.set_ylim3d(bottom=-5, top=10) 
    ax.axes.set_zlim3d(bottom=0, top=50) 

#Used for creating and updating cylinder
class Cylinder:
    def __init__(self, radius = 3, height = 20, x_center = 4, y_center = 0, elevation = 10, color = 'b'):
        self.radius = radius
        self.height = height
        self.color = color
        self.x_center = x_center
        self.y_center = y_center
        self.resolution = 20
        self.elevation = elevation
        
        #Create the cylinder
        self.z = np.linspace(self.elevation, self.elevation + self.height, self.resolution)    #Create a figure with x height around z-axis
        self.theta = np.linspace(0, 2*np.pi, self.resolution)    #Create a theta value to mesh with 
        self.theta_grid, self.z_grid = np.meshgrid(self.theta, self.z)
        self.x_grid = self.radius * np.cos(self.theta_grid) + self.x_center
        self.y_grid = self.radius * np.sin(self.theta_grid) + self.y_center
        self.surf = ax.plot_surface(self.x_grid, self.y_grid, self.z_grid, linewidth=0, color=self.color)

    
    def update(self, val):
        #Update the cylinder from slider values.
        z = np.linspace(self.elevation, self.elevation + height_slider.val, self.resolution)
        theta = np.linspace(0, 2*np.pi, self.resolution)
        theta_grid, z_grid = np.meshgrid(theta, z)
        x_grid = radius_slider.val * np.cos(theta_grid) + xcenter_slider.val
        y_grid = radius_slider.val * np.sin(theta_grid) + ycenter_slider.val
        self.surf.remove()
        self.surf = ax.plot_surface(x_grid, y_grid, z_grid, linewidth=0, color=self.color)
        fig.canvas.draw_idle()
    
        
#Used for mapping to cylinder
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
fig = plt.figure()
ax = plt.axes(projection = '3d')

#Set axis labels
ax.set_xlabel('x-axis [m]')
ax.set_ylabel('y-axis [m]')
ax.set_zlabel('z-axis [m]')

#Create cylinder object with radius 3, height 20, x center value of 4, y center value of 0, elevation of 15 and color 'blue'
cylinder1 = Cylinder(color='r')

#Create drone object which inherits the beforementioned cylinder object values.
drone1 = Drone(cylinder1)

#Set x,y,z values from .csv file
x, y, z = printcsv()
ax.scatter(x,y,z)
l1, = ax.plot(x, y, z)

#Create arrows following direction of coords.
step = 10
for i in range(0, len(x)-step, step):
    ax.arrow3D(x[i], y[i], z[i], x[i+step], y[i+step], z[i+step], mutation_scale=10, fc="red")



           
#Create sliders to set cylinder values
ax_radius = fig.add_axes([0.2, 0.25, 0.0225, 0.63])
ax_height = fig.add_axes([0.15, 0.25, 0.0225, 0.63])
ax_xcenter = fig.add_axes([0.1, 0.25, 0.0225, 0.63])
ax_ycenter = fig.add_axes([0.05, 0.25, 0.0225, 0.63])
radius_slider = Slider(ax=ax_radius, label='Radius [m]', valmin=1, valmax=30, valinit=1, valstep=1, orientation='vertical')
height_slider = Slider(ax=ax_height, label='Height [m]', valmin=1, valmax=30, valinit=1, valstep=1, orientation='vertical')
xcenter_slider = Slider(ax=ax_xcenter, label='x-offset [m]', valmin=0, valmax=30, valinit=0, valstep=1, orientation='vertical')
ycenter_slider = Slider(ax=ax_ycenter, label='y-offset [m]', valmin=0, valmax=30, valinit=0, valstep=1, orientation='vertical')

#Update the cylinder values from sliders
radius_slider.on_changed(cylinder1.update)
height_slider.on_changed(cylinder1.update)
xcenter_slider.on_changed(cylinder1.update)
ycenter_slider.on_changed(cylinder1.update)

# Create a button to reset the sliders to initial values.
ax_reset = fig.add_axes([0.8, 0.015, 0.15, 0.04])
ax_map = fig.add_axes([0.8, 0.075, 0.15, 0.04])
resetbutton = Button(ax_reset, 'Reset', hovercolor='0.975')
mapbutton = Button(ax_map, 'Map coords', hovercolor='0.975')


#Reset the slider values
resetbutton.on_clicked(reset)
mapbutton.on_clicked(drone1.map_coords(x))
mapbutton.on_clicked(drone1.map_coords(y))

print(x[0], x[1], y[0], y[1], z[0], z[1])


plt.show()

# %%





"""
new_x = drone1.map_coords(x)
new_y = drone1.map_coords(y)
ax.scatter(new_x, new_y, z)
"""