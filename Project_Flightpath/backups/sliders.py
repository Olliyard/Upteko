#-------------------------------------Sliders--------------------------------------------#
# Create sliders to set cylinder values
ax_radius = drw.fig.add_axes([0.2, 0.25, 0.0225, 0.63])
ax_height = drw.fig.add_axes([0.15, 0.25, 0.0225, 0.63])
ax_xcenter = drw.fig.add_axes([0.1, 0.25, 0.0225, 0.63])
ax_ycenter = drw.fig.add_axes([0.05, 0.25, 0.0225, 0.63])
radius_slider = Slider(
    ax=ax_radius, label='Radius [m]', valmin=1, valmax=30, valinit=radius, valstep=1, orientation='vertical')
height_slider = Slider(
    ax=ax_height, label='Height [m]', valmin=1, valmax=30, valinit=height, valstep=1, orientation='vertical')
xcenter_slider = Slider(
    ax=ax_xcenter, label='x-offset [m]', valmin=0, valmax=30, valinit=x_center, valstep=1, orientation='vertical')
ycenter_slider = Slider(
    ax=ax_ycenter, label='y-offset [m]', valmin=0, valmax=30, valinit=y_center, valstep=1, orientation='vertical')


def update(val, surf, resolution):
    # Update the cylinder from slider values.
    z = drw.np.linspace(self.elevation, self.elevation +
                    height_slider.val, self.resolution)
    theta = drw.np.linspace(0, 2*np.pi, self.resolution)
    theta_grid, z_grid = np.meshgrid(theta, z)
    x_grid = radius_slider.val * np.cos(theta_grid) + xcenter_slider.val
    y_grid = radius_slider.val * np.sin(theta_grid) + ycenter_slider.val
    surf.remove()
    surf = drw.ax.plot_surface(
        x_grid, y_grid, z_grid, linewidth=0, color=self.color)
    drw.fig.canvas.draw_idle()

# Update the cylinder values from sliders
radius_slider.on_changed(cylinder1.update(lambda new_val: update(new_val, surf, resolution)))
height_slider.on_changed(cylinder1.update)
xcenter_slider.on_changed(cylinder1.update)
ycenter_slider.on_changed(cylinder1.update)


#-------------------------------------Buttons--------------------------------------------#
# Create a button to reset the sliders to initial values.
ax_reset = drw.fig.add_axes([0.8, 0.015, 0.15, 0.04])
ax_map = drw.fig.add_axes([0.8, 0.075, 0.15, 0.04])
resetbutton = Button(ax_reset, 'Reset', hovercolor='0.975')
mapbutton = Button(ax_map, 'Map coords', hovercolor='0.975')

# Used for resetting values
def reset(event):
    radius_slider.reset()
    height_slider.reset()
    xcenter_slider.reset()
    ycenter_slider.reset()
    drw.ax.axes.set_xlim3d(left=-5, right=10)
    drw.ax.axes.set_ylim3d(bottom=-5, top=10)
    drw.ax.axes.set_zlim3d(bottom=0, top=50)

# Reset the slider values
resetbutton.on_clicked(reset)
mapbutton.on_clicked(drone1.map_coords(x))
mapbutton.on_clicked(drone1.map_coords(y))