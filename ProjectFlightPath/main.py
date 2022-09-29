# %%
# Includes
from locale import normalize
import Draw as drw
import CSV as sve

# Get .csv file and put it into x,y,z list

#-------------------------------------Draw--------------------------------------------#
def main():
    sve1 = sve.DroneCSV()
    time, x, y, z, xroll, ypitch, zyaw, w = sve1.read_csv("get_dlp")
    time_start = sve1.read_csv("get_start")
    
    #offset = sve1.find_start(time_start, time)

    #x, y, z, xroll, ypitch, zyaw, w = sve1.offset(x, y, z, xroll, ypitch, zyaw, w, offset)
   
    xroll, ypitch, zyaw = sve1.euler_from_quaternion(xroll, ypitch, zyaw, w, degrees=0)
    print("-----------------------debug (rad): ",(zyaw))
    
    xroll, ypitch, zyaw = sve1.euler_from_quaternion(xroll, ypitch, zyaw, w, degrees=1)
    print("-----------------------debug (deg): ",(zyaw))
    draw1 = drw.Draw()
    draw1.draw_cylinder()
    draw1.draw_dronepath(x, y, z, 100)
    draw1.draw_figure()
    # print("The value of __name__ is:", repr(__name__))

if __name__ == "__main__":
    main()
