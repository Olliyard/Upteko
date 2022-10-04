from locale import normalize
import statistics as st
import numpy as np
import matplotlib.pyplot as plt
import Arrow3D as arw3D
import math
import matplotlib



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
        self.x_center = x_center
        self.y_center = y_center
        self.radius = radius
        self.z_center = (0.5 * (height+elevation))
        z = np.linspace(
            elevation, elevation + height, self.resolution)

        # Create a theta value to mesh with
        theta = np.linspace(0, 2*np.pi, self.resolution)
        theta_grid, z_grid = np.meshgrid(theta, z)
        x_grid = radius * np.cos(theta_grid) + x_center
        y_grid = radius * np.sin(theta_grid) + y_center
        self.ax.plot_surface(
            x_grid, y_grid, z_grid, linewidth=0, color=color)

    def draw_dronepath(self, x, y, z, offset, c = 'b', with_arrow=0, step = 10):
        x_off = x[0:offset]
        y_off = y[0:offset]
        z_off = z[0:offset]
        self.ax.scatter(x_off, y_off, z_off, color='r')
        self.ax.plot(x_off, y_off, z_off, color ='r')

        self.ax.scatter(x[offset:], y[offset:], z[offset:], color=c)
        self.ax.plot(x[offset:], y[offset:], z[offset:], color=c)

        if(with_arrow == 1):
            #Create arrows following direction of coords.
            for i in range(0, len(x)-step, step):
                self.ax.arrow3D(x[i], y[i], z[i], x[i+step], y[i+step], z[i+step], mutation_scale=10, fc="red")
            
    def draw_figure(self):
        plt.show()
        
    # ------------------ TESTS ------------------ #
    ## 2d only, for now, sin, cos for three planes.
    #def camera_plots(self, xp, yp, zp, xo, yo, zo):
    def camera_plots(self, xp, yp, zp, xo, yo, zo):
        r=3
        for i in range(0, len(xp), 30):
            self.ax.arrow3D(
            xp[i], 
            yp[i], 
            zp[i],
            xp[i]+r*(np.cos(xo[i])), 
            yp[i]+r*(np.sin(yo[i])),
            zp[i]+r*(np.sin(zo[i])), 
            mutation_scale=10, 
            fc="red")
        """
        ## draw arrow
            r = 1  # or whatever fits you
            for i in range(0,len(xp),1):
                matplotlib.pyplot.arrow(xp[i], yp[i], (r*np.cos(xo[i])), (r*np.sin(xo[i])))
        """
    # Spherical coordinates to rectangular (cartesian) coordinates. Plots in reference to origin
    def spherical_to_rectangular_coordinates(self, data_x, data_y, data_z, pitch, yaw, l=1, step=1):
        cam_x = []
        cam_y = []
        cam_z = []
        for index in range(0, len(data_x)-step, step):
            # x = (self.x_center + ((l * math.sin(yaw[index])) * math.cos(pitch[index])))
            # y = (self.y_center + ((l * math.sin(yaw[index])) * math.sin(pitch[index])))
            # z = (self.z_center + (l * math.cos(yaw[index])))
            # print("Types: ")
            # print("Yaw: ", type(yaw[index]))
            # print("Pitch: ", type(pitch[index]))
            # print("Size of datapoints: ", len(datapoints))            

            cam_x.append((data_x[index] + ((l * math.sin(yaw[index])) * math.cos(pitch[index]))))
            cam_y.append((data_y[index] + ((l * math.sin(yaw[index])) * math.sin(pitch[index]))))
            cam_z.append((data_z[index] + (l * math.cos(yaw[index]))))
            
            #self.ax.arrow3D(x_p[index], y_p[index], z_p[index], x, y, z, mutation_scale=10, fc="yellow")
        

        self.ax.scatter(cam_x, cam_y, cam_z, color='y')
        # self.ax.plot(cam_x,cam_y,cam_z,color='y')
        #self.ax.arrow3D(x_p, y_p, z_p, x, y, z, mutation_scale=10, fc="yellow")
        
        return cam_x,cam_y,cam_z


def map_range(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

