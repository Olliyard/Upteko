xfrom mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import matplotlib.pyplot as plt


fig = plt.figure()
ax = Axes3D(fig)

x = [0,1,1,0],[0,1,1,0]
y = [0,0,1,1],[0,0,1,1]
z = [0,0,0,0],[1,1,1,1]

surfaces = []

for i in range(len(x)):
    surfaces.append( [list(zip(x[i],y[i],z[i]))] )

for surface in surfaces:
    ax.add_collection3d(Poly3DCollection(surface))

plt.show()

"""
            """
            var1 = 0.8
            var2 = 1.2
            step = 0.001
            #print([i])
            if xp[i] < 0:
                step = -0.001
            elif xp[i] > 0:
                step = 0.001
            X = np.arange(xp[i]*var1, xp[i]*var2, step)

            if yp[i] < 0:
                step = -0.001
            elif yp[i] > 0:
                step = 0.001
            Y = np.arange(yp[i]*var1, yp[i]*var2, step)

            
            
            #(x, y) = np.meshgrid(np.arange(-1, 3.1, 1), np.arange(-1, 1.1, 1))
            #if i == 0:
            
            test = i
            print(np.arange(xp[test]*0.8, xp[test]*1.2, 0.001))
            (x, y) = np.meshgrid(np.linspace(xp[test]*0.8, xp[test]*1.2, 100), np.linspace(yp[test]*0.8, yp[test]*1.2, 100))
                #print(x)
                #print(y)
            z = x+y
            self.ax.plot_surface(x, y, z, alpha=0.5)
            """
"""