# %%
# Includes
from locale import normalize
import Draw as drw
import DataHandler as dh

# Get .csv file and put it into x,y,z list

#-------------------------------------Draw--------------------------------------------#


def main():
    dh1 = dh.DataHandler()
    draw1 = drw.Draw()

    # Read the drone_local_position csv and save the timestamps, the (x,y,z)-coordinates and the quaternions (x,y,z,w)
    elevation, x_center, diameter, height = dh1.read_csv("get_cylinder_values")
    draw1.draw_cylinder(diameter=float(diameter), elevation=elevation, x_center=x_center, height=height)

    
    time, xp, yp, zp, xo, yo, zo, wo = dh1.read_csv("get_dlp")
    time_start = dh1.read_csv("get_start")
    print(time_start)

    offset = dh1.find_start(time_start, time)
    
    draw1.draw_dronepath(xp, yp, zp, 100, c='g')

    # TEST
    roll_x, pitch_y, yaw_z = dh1.euler_from_quaternion(xo, yo, zo, wo)
    #draw1.camera_plots(xp, yp, zp, roll_x, pitch_y, yaw_z)

    layout, data_x, data_y, data_z, w = dh1.read_csv("get_camera")
    print(data_x)
    draw1.ax.scatter(data_x, data_y, data_z, color='c')
    draw1.ax.plot(data_x, data_y, data_z, color='c')
    
    
    
    draw1.draw_figure()

if __name__ == "__main__":
    main()


