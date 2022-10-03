# %%
# Includes
from locale import normalize
import Draw as drw
import DataHandler as dh

# Get .csv file and put it into x,y,z list

#-------------------------------------Draw--------------------------------------------#
def main():
    dh1 = dh.DataHandler()
    
    #Read the drone_local_position csv and save the timestamps, the (x,y,z)-coordinates and the quaternions (x,y,z,w)
    time, x, y, z, xq, yq, zq, wq = dh1.read_csv("get_dlp")
    time_start = dh1.read_csv("get_start")
    layout, data_x, data_y, data_z, data_yaw = dh1.read_csv("get_camera")
    
    # for i in range(0, len(layout)):
    #     print("Index ", i, " -- Layout: ", layout[i])
    #     print("Index ", i, " -- Data_x: ", data_x[i])

    # for i in range(0, len(data)):
    #     print("Index ", i, " -- Data: ", data[i])
    
    offset = dh1.find_start(time_start, time)

    x, y, z, xq, yq, zq, wq = dh1.offset(x, y, z, xq, yq, zq, wq, offset)
    xroll, ypitch, zyaw = dh1.euler_from_quaternion(xq, yq, zq, wq, degrees=0)

    draw1 = drw.Draw()
    draw1.draw_cylinder()
    draw1.draw_dronepath(x, y, z, 100, c='b')
    x_cam, y_cam, z_cam = draw1.spherical_to_rectangular_coordinates(data_x, data_y, data_z, ypitch, data_yaw, l=1)
    draw1.draw_dronepath(x_cam, y_cam, z_cam, 100, c='y')
    draw1.draw_figure()

if __name__ == "__main__":
    main()
