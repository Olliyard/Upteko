from math import atan2, asin
from locale import normalize
import matplotlib.pyplot as plt
import numpy as np
import Arrow3D
import matplotlib.image as mpimg

class Draw:
    def __init__(self, width=12, height=10):
        # Create 3D figure
        self.fig = plt.figure(figsize=(width, height))
        self.ax = plt.axes(projection='3d')


    def draw_cylinder(self, diameter=6, height=20, x_center=4, y_center=0, elevation=15, color='b', resolution=20):
        z = np.linspace(
            elevation, elevation + height, resolution)
        theta = np.linspace(0, 2*np.pi, resolution)
        theta_grid, z_grid = np.meshgrid(theta, z)
        x_grid = ((0.5*diameter) * np.cos(theta_grid)) + x_center
        y_grid = ((0.5*diameter) * np.sin(theta_grid)) + y_center
        self.ax.plot_surface(
            x_grid, y_grid, z_grid, linewidth=0, color=color)


    def draw_dronepath(self, x, y, z, offset=0, c='b', with_arrow=0, step=10):
        self.ax.scatter(x[0:offset], y[0:offset], z[0:offset], color='y')
        self.ax.plot(x[0:offset], y[0:offset], z[0:offset], color='y')

        self.ax.scatter(x[offset:], y[offset:], z[offset:], color=c)
        self.ax.plot(x[offset:], y[offset:], z[offset:], color=c)

        if (with_arrow == 1):
            # Create arrows following direction of coords.
            for i in range(0, len(x)-step, step):
                self.ax.arrow3D(x[i], y[i], z[i], x[i+step], y[i+step],
                                z[i+step], mutation_scale=10, fc="red")


    def draw_figure(self, title="null"):
        # Set axis labels
        self.x_axis = self.ax.set_xlabel('x-axis [m]')
        self.y_axis = self.ax.set_ylabel('y-axis [m]')
        self.z_axis = self.ax.set_zlabel('z-axis [m]')
        self.ax.set_title(title)
        self.ax.set_xlim(-10, 15)
        self.ax.set_ylim(-10, 15)
        plt.show()

    # ------------------ TESTS ------------------ #
    # 2d only, for now, sin, cos for three planes.
    def camera_plots(self, xp, yp, zp, roll_x, pitch_y, yaw_z, r=1):
        for i in range(0, len(xp), 10):
            u = r*np.cos(yaw_z[i])
            w = r*np.sin(yaw_z[i])

            v = r*(-np.cos(np.pi * roll_x[i]) * np.sin(np.pi * pitch_y[i]) * \
                np.cos(np.pi * yaw_z[i]))
            self.ax.quiver(xp[i], yp[i], zp[i], u, w, v, normalize=True, color='m')


    def map_range(x, in_min, in_max, out_min, out_max):
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
    
    
    def insert_image(self, path_to_image, image_type):
        newax = self.fig.add_axes([0.07, 0.55, 0.2, 0.2])
        try:
            newax.imshow(mpimg.imread(path_to_image, format=image_type))
        except BaseException as ls:
            print(ls)            
        
        newax.axis('off')


    def annotate(self, x_place=0, y_place=0, point='null', text='null', c='b'):
        plt.gcf().text(x_place, y_place, f"Distance max/min of {point} is: {text}", color=c, bbox=dict(facecolor='none', edgecolor=c, boxstyle='round'))
       
     
    def annotate_maxmin_z(self, time, x, y, z, index_z_max, index_z_min):
        max = (time[index_z_max][11:19], round(x[index_z_max], 2), round(y[index_z_max], 2), round(z[index_z_max], 2))
        min = (time[index_z_min][11:19], round(x[index_z_min], 2), round(y[index_z_min], 2), round(z[index_z_min], 2))
        
        self.ax.text(max[1], max[2], max[3], '%s' % (str(max)), size=10, zorder=1, color='k')
        self.ax.text(min[1], min[2], min[3], '%s' % (str(min)), size=10, zorder=1, color='k')
