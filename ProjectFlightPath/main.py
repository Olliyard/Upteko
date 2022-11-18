#!/usr/bin/env python

from locale import normalize
import Draw as drw
import DataHandler as dh

# Get .csv file and put it into x,y,z list

''' how to use:
1. 
cp ~/rosbag2/rosbag2_1667387040.0855098/rosbag2_1667387040.0855098_0.db/navigation/activate_offboard.csv data/activate_offboard.csv
cp ~/rosbag2/rosbag2_1667387040.0855098/rosbag2_1667387040.0855098_0.db/drone_info/local_pos/position.csv data/drone_local_position_unformated.csv
cp ~/rosbag2/rosbag2_1667387040.0855098/rosbag2_1667387040.0855098_0.db/global_planner/request.csv data/global_plan_genneration_request.csv
cp ~/rosbag2/rosbag2_1667387040.0855098/rosbag2_1667387040.0855098_0.db/navigation/add_mission_point/local.csv data/local_position_targets.csv
2. 

'''
#-------------------------------------Draw--------------------------------------------#


def main():
    dh1 = dh.DataHandler()

    # Read the drone_local_position csv and save the timestamps, the (x,y,z)-coordinates and the quaternions (x,y,z,w)
    time, xp, yp, zp, xo, yo, zo, wo = dh1.read_csv("get_dlp")
    time_start = dh1.read_csv("get_start")

    offset = dh1.find_start(time_start, time)
    
    draw1 = drw.Draw()

    # draw1.draw_cylinder(radius=4, height=32, x_center=4 , y_center=4, elevation=2)
    # def draw_cylinder(self, radius=3, height=20, x_center=4, y_center=0, elevation=10, color='b'):

    draw1.draw_dronepath(xp, yp, zp, 100, c='g')

    # TEST
    roll_x, pitch_y, yaw_z = dh1.euler_from_quaternion(xo, yo, zo, wo)
    draw1.camera_plots(xp, yp, zp, roll_x, pitch_y, yaw_z)
    draw1.draw_figure()

    # drw.drawPyramid()


if __name__ == "__main__":
    main()


