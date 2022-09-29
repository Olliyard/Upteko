# %%
# Includes
from locale import normalize
import Draw as drw
import CSV as sve

# Get .csv file and put it into x,y,z list

#-------------------------------------Draw--------------------------------------------#

def main():
    x, y, z = sve.printcsv()
    draw1 = drw.Draw()
    draw1.draw_cylinder()
    draw1.draw_dronepath(x, y, z)
    draw1.draw_figure()
    print("The value of __name__ is:", repr(__name__))

if __name__ == "__main__":
    main()
