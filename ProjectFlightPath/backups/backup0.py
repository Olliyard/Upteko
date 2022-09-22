        
        
    def flight_print(self):
        #implementation based on the written documentation found in "Documentation_FlightControlProject" on OneDrive
        
        #Flight up (x: 0, y: -3, z: 0-6)
        self.x_coord = [0, 0, 0, 0, 0, 0, 0]
        self.y_coord = [-3, -3, -3, -3, -3, -3, -3]
        self.z_coord = list(range(0, self.height+1, self.resolution))
        ax.scatter(self.x_coord, self.y_coord, self.z_coord)
        ax.plot(self.x_coord, self.y_coord, self.z_coord, color='r')
        #ax.arrow()
        
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