# Includes
from __future__ import unicode_literals
import sys
import subprocess
import os
import shutil
import Draw as drw
import DataHandler as dh

'''
The format for main is ./main.py <desired rosbag>
If things are not working, comment the 'else' statement in '__main__' and use the following guides:
Put name of <desired_rosbag> here, called "dr" for short below, this is to easily copy paste below

1.
ros2bag-convert ~/rosbag2/dr/dr_0.dp

2. 
cp ~/rosbag2/dr/dr_0.dp/navigation/activate_offboard.csv        $pwd/data/activate_offboard.csv
cp ~/rosbag2/dr/dr_0.dp/drone_info/local_pos/position.csv       $pwd/data/drone_local_position_unformated.csv
cp ~/rosbag2/dr/dr_0.dp/global_planner/request.csv              $pwd/data/global_plan_genneration_request.csv
cp ~/rosbag2/dr/dr_0.dp/navigation/add_mission_point/local.csv  $pwd/data/local_position_targets.csv
3. 
./main.py

'''

def main():
    # Create instances of class Datahandler and Draw
    dh1 = dh.DataHandler()
    drw1 = drw.Draw()
    
    # Get data from datasets
    time, xp, yp, zp, xo, yo, zo, wo = dh1.read_csv(cmd = "get_dlp")
    elevation, x_center, diameter, height = dh1.read_csv(cmd = "get_cval")
    time_start = dh1.read_csv(cmd = "get_start")
    #data_x, data_y, data_z = dh1.read_csv(cmd = "get_camera")

    # Data handling
    roll_x, pitch_y, yaw_z = dh1.euler_from_quaternion(xo, yo, zo, wo)
    x_dif, y_dif, index_max_z, index_min_z = dh1.get_maxmin(xp, yp, zp)
    offset = dh1.find_start(time_start, time)

    # Draw figures
    drw1.draw_cylinder(diameter=diameter, elevation=elevation, x_center=x_center, height=height)
    drw1.draw_dronepath(xp, yp, zp, offset, with_arrow=1, c='g', step=20)
    #drw1.draw_dronepath(data_x, data_y, data_z, 0, c='c')
    drw1.camera_plots(xp, yp, zp, roll_x, pitch_y, yaw_z)
    #drw1.insert_image('C:/Users/revil/Desktop/Git_Repos/Upteko/ProjectFlightPath/data/Windmill.jpeg', 'jpeg')

    # Annotate figure
    drw1.annotate(0.1, 0.8, "x", x_dif, c='b')
    drw1.annotate(0.1, 0.77, "y", y_dif, c='r')
    drw1.annotate_maxmin_z(time, xp, yp, zp, index_max_z, index_min_z)
    
    # Draw figure
    drw1.draw_figure(title = f"Date of inspection: {time[0][0:9]}")


if __name__ == "__main__":
    if not sys.argv[1]:
        print(f"\nError! Not enough arguments, expected a desired rossbag.")
    else:
        # Get the rosbag argument from the command line
        dr = sys.argv[1]

        # Get the current working directory
        current_dir = os.getcwd()

        # Use subprocess to run the 'cp' command
        path_nav = f"~\\rosbag2\\{dr}\\{dr}_0.db\\navigation\\activate_offboard.csv"
        path_pos = f"~\\rosbag2\\{dr}\\{dr}_0.db\\drone_info\\local_pos\\position.csv"
        path_req = f"~\\rosbag2\\{dr}\\{dr}_0.db\\global_planner\\request.csv"
        path_local = f"~\\rosbag2\\{dr}\\{dr}_0.db\\navigation\\add_mission_point\\local.csv"

        print("Copying files...")
        shutil.copy2(path_nav, f"{current_dir}\\data\\activate_offboard.csv")
        print("activate_offboard.csv copied!")
        shutil.copy2(path_pos, f"{current_dir}\\data\\drone_local_position_unformated.csv")
        print("position.csv copied!")
        shutil.copy2(path_req, f"{current_dir}\\data\\global_plan_genneration_request.csv")
        print("request.csv copied!")
        shutil.copy2(path_local, f"{current_dir}\\data\\local_position_targets.csv")
        print("local.csv copied!")
        print("All files copied -- Starting main")
        main()