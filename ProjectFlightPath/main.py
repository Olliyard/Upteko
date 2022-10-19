# Includes
from locale import normalize
import Draw as drw
import DataHandler as dh

def main():
    # Create instances of class Datahandler and Draw
    dh1 = dh.DataHandler()
    drw1 = drw.Draw()
    
    # Get data from datasets
    time, xp, yp, zp, xo, yo, zo, wo = dh1.read_csv("get_dlp")
    elevation, x_center, diameter, height = dh1.read_csv("get_cylinder_values")
    time_start = dh1.read_csv("get_start")
    data_x, data_y, data_z = dh1.read_csv("get_camera")

    # Data handling
    roll_x, pitch_y, yaw_z = dh1.euler_from_quaternion(xo, yo, zo, wo)
    x_dif, y_dif, index_max_z, index_min_z = dh1.get_maxmin(xp, yp, zp)
    offset = dh1.find_start(time_start, time)

    # Draw figures
    drw1.draw_cylinder(diameter=diameter, elevation=elevation, x_center=x_center, height=height)
    drw1.draw_dronepath(xp, yp, zp, offset, with_arrow=0, c='g', step=20)
    drw1.draw_dronepath(data_x, data_y, data_z, 0, c='c')
    drw1.camera_plots(xp, yp, zp, roll_x, pitch_y, yaw_z)
    drw1.insert_image('C:/Users/revil/Desktop/Git_Repos/Upteko/ProjectFlightPath/data/Windmill.jpeg', 'jpeg')

    # Annotate figure
    drw1.annotate(0.1, 0.8, "x", x_dif, c='b')
    drw1.annotate(0.1, 0.77, "y", y_dif, c='r')
    drw1.annotate_maxmin_z(time, xp, yp, zp, index_max_z, index_min_z)
    
    # Draw figure
    drw1.draw_figure(title = f"Date of inspection: {time[0][0:9]}")


if __name__ == "__main__":
    main()