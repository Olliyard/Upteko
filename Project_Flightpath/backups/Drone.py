import statistics as st
from backups.Cylinder import Cylinder

#Map the values of the coordinates to pre-set boundaries
def map_range(x, in_min, in_max, out_min, out_max):
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min

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