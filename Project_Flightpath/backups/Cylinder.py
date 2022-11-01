import numpy as np
#import Draw as drw

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
        #self.surf = drw.ax.plot_surface(self.x_grid, self.y_grid, self.z_grid, linewidth=0, color=self.color)

    def get_init_values(self):
        return self.radius, self.height, self.x_center, self.y_center, self.elevation
    
    def get_plane_values(self):
        return self.z, self.theta, self.theta_grid, self.z_grid, self.x_grid, self.y_grid
    
    def update(self, val):
        #Update the cylinder from slider values.
        #Draw init values cylinder
        z = np.linspace(self.elevation, self.elevation + height_slider.val, self.resolution)
        theta = np.linspace(0, 2*np.pi, self.resolution)
        theta_grid, z_grid = np.meshgrid(theta, z)
        x_grid = drw.radius_slider.val * np.cos(theta_grid) + drw.xcenter_slider.val
        y_grid = drw.radius_slider.val * np.sin(theta_grid) + drw.ycenter_slider.val
        self.surf.remove()
        self.surf = drw.ax.plot_surface(x_grid, y_grid, z_grid, linewidth=0, color=self.color)
        drw.fig.canvas.draw_idle()
        