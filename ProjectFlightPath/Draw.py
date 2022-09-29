import statistics as st
import numpy as np
import matplotlib.pyplot as plt
import Arrow3D as arw3D

# Map the values of the coordinates to pre-set boundaries


class Draw:
    def __init__(self):
        # Create 3D figure
        self.fig = plt.figure()
        self.ax = plt.axes(projection='3d')

        # Set axis labels
        self.x_axis = self.ax.set_xlabel('x-axis [m]')
        self.y_axis = self.ax.set_ylabel('y-axis [m]')
        self.z_axis = self.ax.set_zlabel('z-axis [m]')

    def draw_cylinder(self, radius=3, height=20, x_center=4, y_center=0, elevation=10, color='b'):
        self.resolution = 20
        z = np.linspace(
            elevation, elevation + height, self.resolution)

        # Create a theta value to mesh with
        theta = np.linspace(0, 2*np.pi, self.resolution)
        theta_grid, z_grid = np.meshgrid(theta, z)
        x_grid = radius * np.cos(theta_grid) + x_center
        y_grid = radius * np.sin(theta_grid) + y_center
        self.ax.plot_surface(
            x_grid, y_grid, z_grid, linewidth=0, color=color)

    def draw_dronepath(self, x, y, z, step = 10):
        self.ax.scatter(x, y, z)
        self.ax.plot(x, y, z)

        # Create arrows following direction of coords.
        for i in range(0, len(x)-step, step):
            self.ax.arrow3D(x[i], y[i], z[i], x[i+step], y[i+step], z[i+step], mutation_scale=10, fc="red")
            
    def draw_figure(self):
        plt.show()


def map_range(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min