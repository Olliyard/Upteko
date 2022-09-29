import csv
import json

def get_drone_positions():
    with open("drone_local_position_unformated.csv", 'r') as file:
        headers = ['time', 'header', 'pose']
        timestamps = []
        header = []
        
        #----- Position ----
        x = []
        y = []
        z = []
        
        #----- Orientation ----
        xroll = []
        ypitch = []
        zyaw = []
        w = []
        csvreader = csv.DictReader(file, delimiter=',',  fieldnames=headers)
        for i, row in enumerate(csvreader):
            if i == 0:
                continue 

            if i > 1:
                #----- Times -----
                timestamps.append(row['time'])
                
                #----- Header -----
                header.append(row['header'])
                
                #----- Position -----
                pose = row['pose'].replace("'", "\"")
                cdict_pose = json.loads(pose)
                x.append(cdict_pose['position']['x'])
                y.append(cdict_pose['position']['y'])
                z.append(cdict_pose['position']['z'])
                
                #----- Orientation -----
                xroll.append(cdict_pose['orientation']['x'])
                ypitch.append(cdict_pose['orientation']['y'])
                zyaw.append(cdict_pose['orientation']['z'])
                w.append(cdict_pose['orientation']['w'])
    
        #----- Position -----
        x = list(map(float, x))
        y = list(map(float, y))
        z = list(map(float, z))
        
        
        #----- Orientation -----
        xroll = list(map(float, xroll))
        ypitch = list(map(float, ypitch))
        zyaw = list(map(float, zyaw))
        w = list(map(float, w))

    return timestamps, x, y, z, xroll, ypitch, zyaw, w