import csv
import json
import math

class DataHandler():
    def __init__(self):
        self.header = []
        self.layout = []
        # ----- Times -----
        self.times = []
        self.time_start1 = []
        self.time_start2 = []
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

    def read_csv(self, cmd, csv_file=""):
        if cmd == "get_dlp":            
            if not csv_file:
                csv_file = "data/drone_local_position_unformated.csv"
            with open(csv_file, 'r') as file:
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

        elif cmd == "get_cval":
            if not csv_file:
                csv_file = "data/global_plan_genneration_request.csv"
            
            cylinder_height = 0
            cylinder_elevation = 0
            cylinder_xcenter = 0
            cylinder_ycenter = 0
            cylinder_diameter = 0
            with open(csv_file, 'r') as file:
                headers = ['time','current_pose','current_pos_global','dist_blade_current','dist_blade_target','blade_length','first_blade_rotation','first_blade','inspect_targets','overlap_procentage']
                csvreader = csv.DictReader(
                    file, delimiter=',', fieldnames=headers)
                for i, row in enumerate(csvreader):
                    if i == 0:
                        continue
                    
                    if i > 0:
                        cur_pose = row['current_pose'].replace("'", "\"")
                        cdict_cur_pose = json.loads(cur_pose)
                        cylinder_elevation = cdict_cur_pose['position']['z']
                        cylinder_xcenter = row['dist_blade_current']
                        cylinder_diameter = row['dist_blade_target']
                        cylinder_height = row['blade_length']
                    
                cylinder_xcenter = float(cylinder_xcenter)
                cylinder_diameter = float(cylinder_diameter)
                cylinder_height = float(cylinder_height)
            return cylinder_elevation, cylinder_xcenter, cylinder_diameter, cylinder_height


        elif cmd == "get_start":
            if not csv_file:
                csv_file = "data/activate_offboard.csv"
            with open(csv_file, 'r') as file:
                headers = ['time', 'data']
                csvreader = csv.DictReader(
                    file, delimiter=',',  fieldnames=headers)
                for i, row in enumerate(csvreader):
                    if i == 0:
                        continue
                    if i > 0:
                        # ----- Times -----
                        self.time_start1.append(row['time'])
            return self.time_start1
        
        elif cmd == "get_camera":
            if not csv_file:
                csv_file = "data/local_position_targets.csv"
            with open(csv_file, 'r') as file:
                headers = ['time', 'layout', 'data']
                data_x = []
                data_y = []
                data_z = []
                data_w = []
                csvreader = csv.DictReader(file, delimiter=',', fieldnames=headers)
                for i, row in enumerate(csvreader):
                    if i == 2:
                        continue
                    if i > 2:
                        # layout = row['layout'].replace("'", "\"")
                        # cdict_layout = json.loads(layout)
                        # self.layout.append(cdict_layout['data_offset'])
            
                        cam = row['data'].strip('][').split(', ')
                        data_x.append(cam[0])
                        data_y.append(cam[1])
                        data_z.append(cam[2])
                        # data_w.append(cam[3])
                        
                    # self.layout = list(map(float, self.layout))
                    data_x = list(map(float, data_x))
                    data_y = list(map(float, data_y))
                    data_z = list(map(float, data_z))
                    # data_w = list(map(float, data_w))
            return data_x, data_y, data_z
        
        else:
            return f"ERROR on ID: {cmd}"

    def find_start(self, start_time, all_time):
        for i in range(len(all_time)):
            if (start_time[0][0:19] == all_time[i][0:19]):
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
    
    def get_maxmin(self, x, y, z):
        # ----- Min/Max ----- 
        x_dif = str(round((max(x) - min(x)), 2))
        y_dif = str(round((max(y) - min(y)), 2))

        index_max_z = z.index(max(z))
        index_min_z = z.index(min(z))
        return x_dif, y_dif, index_max_z, index_min_z