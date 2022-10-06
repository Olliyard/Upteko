import csv
import json
import math
from operator import index
from unittest.mock import CallableMixin

class DataHandler():
    def __init__(self):
        self.header = []
        self.layout = []
        # ----- Times -----
        self.times = []
        self.time_start = []
        # ----- Position ----
        self.x = []
        self.y = []
        self.z = []
        # ----- Orientation ----
        self.xroll = []
        self.ypitch = []
        self.zyaw = []
        self.w = []
        # ----- Camera pos -----
        self.cam = []

    def read_csv(self, id):
        match id:
            case "get_dlp":
                with open("drone_local_position_unformated.csv", 'r') as file:
                    headers = ['time', 'header', 'pose']
                    csvreader = csv.DictReader(
                        file, delimiter=',',  fieldnames=headers)
                    for i, row in enumerate(csvreader):
                        if i == 0:
                            continue

                        if i > 0:
                            # ----- Times -----
                            self.times.append(row['time'])

                            # ----- Header -----
                            self.header.append(row['header'])

                            # ----- Position -----
                            pose = row['pose'].replace("'", "\"")
                            cdict_pose = json.loads(pose)
                            self.x.append(cdict_pose['position']['x'])
                            self.y.append(cdict_pose['position']['y'])
                            self.z.append(cdict_pose['position']['z'])

                            # ----- Orientation -----
                            self.xroll.append(cdict_pose['orientation']['x'])
                            self.ypitch.append(cdict_pose['orientation']['y'])
                            self.zyaw.append(cdict_pose['orientation']['z'])
                            self.w.append(cdict_pose['orientation']['w'])

                    # ----- Position -----
                    self.x = list(map(float, self.x))
                    self.y = list(map(float, self.y))
                    self.z = list(map(float, self.z))

                    # ----- Orientation -----
                    self.xroll = list(map(float, self.xroll))
                    self.ypitch = list(map(float, self.ypitch))
                    self.zyaw = list(map(float, self.zyaw))
                    self.w = list(map(float, self.w))

                return self.times, self.x, self.y, self.z, self.xroll, self.ypitch, self.zyaw, self.w

            case "get_start":
                with open("activate_offboard.csv", 'r') as file:
                    headers = ['time', 'data']
                    csvreader = csv.DictReader(
                        file, delimiter=',',  fieldnames=headers)
                    for i, row in enumerate(csvreader):
                        if i == 0:
                            continue

                        if i > 0:
                            # ----- Times -----
                            self.time_start.append(row['time'])

                return self.time_start
        
            case "get_camera":
                with open("local_position_targets.csv", 'r') as file:
                    headers = ['time', 'layout', 'data']
                    data_x = []
                    data_y = []
                    data_z = []
                    data_w = []
                    csvreader = csv.DictReader(file, delimiter=',', fieldnames=headers)
                    for i, row in enumerate(csvreader):
                        if i == 0:
                            continue
                        if i > 0:
                            layout = row['layout'].replace("'", "\"")
                            cdict_layout = json.loads(layout)
                            self.layout.append(cdict_layout['data_offset'])
                
                            cam = row['data'].strip('][').split(', ')
                            data_x.append(cam[0])
                            data_y.append(cam[1])
                            data_z.append(cam[2])
                            data_w.append(cam[3])
                            
                        self.layout = list(map(float, self.layout))
                        data_x = list(map(float, data_x))
                        data_y = list(map(float, data_y))
                        data_z = list(map(float, data_z))
                        data_w = list(map(float, data_w))
                    
                    print("cam: data_x: ", len(data_x))
                    print("cam: data_y: ", len(data_y))
                    print("cam: data_z: ", len(data_z))
                    print("cam: data_yaw: ", len(data_w))
                    
                return self.layout, data_x, data_y, data_z, data_w
        
            case _:
                print("ERROR on ID: ", id, " -- No case with given ID")
                exit(1)

    def find_start(self, startime, alltime):
        for i in range(len(alltime)):
            if (startime[0][0:19] == alltime[i][0:19]):
                return i

    def offset(self, x, y, z, xr, yp, zy, w, offset, lead_up=10):
        x = x[offset:]
        y = y[offset:]
        z = z[offset:]

        xr = xr[offset:]
        yp = yp[offset:]
        zy = zy[offset:]
        w = w[offset:]
        return x, y, z, xr, yp, zy, w

    def euler_from_quaternion(self, xo, yo, zo, wo):
        """
        Convert a quaternion into euler angles (roll, pitch, yaw)
        roll is rotation around x in radians (counterclockwise)
        pitch is rotation around y in radians (counterclockwise)
        yaw is rotation around z in radians (counterclockwise)
        """
        roll_x = []
        pitch_y = []
        yaw_z = []

        for i in range(len(wo)):
            t0 = +2.0 * (wo[i] * xo[i] + yo[i] * zo[i])
            t1 = +1.0 - 2.0 * (xo[i] * xo[i] + yo[i] * yo[i])
            
            t2 = +2.0 * (wo[i] * yo[i] - zo[i] * xo[i])
            t2 = +1.0 if t2 > +1.0 else t2
            t2 = -1.0 if t2 < -1.0 else t2
            
            t3 = +2.0 * (wo[i] * zo[i] + xo[i] * yo[i])
            t4 = +1.0 - 2.0 * (yo[i] * yo[i] + zo[i] * zo[i])
            
            roll_x.append(math.atan2(t0, t1))
            pitch_y.append(math.asin(t2))
            yaw_z.append(math.atan2(t3, t4))
                
        roll_x = list(map(float, roll_x))
        pitch_y = list(map(float, pitch_y))
        yaw_z = list(map(float, yaw_z))     

        return roll_x, pitch_y, yaw_z  # in radians

