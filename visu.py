import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import sys
import os
import time

fig, ax = plt.subplots()
fig.canvas.set_window_title('Radar')
xdata, ydata = [], []
ln = plt.scatter([], [], animated=True)
# ln.set_offsets([[0.5], [0.4]])
# print(ln.get_offsets())
location_file = "/tmp/locs.csv"

def getData(i):
  try :
    #Format: id,hp,team,x,y,z
    data = np.loadtxt(location_file, delimiter=",")
  except:
    return ln,
  if(len(data) == 0):
    return ln,
  if(len(data.shape) == 1):
    data = data.reshape((1, 6))
  local_player_index = int(data[0,0])
  data = data[1:]
  # data = data[data[:, 1] != 0]
  xs = data[:, 3]
  # ys = []
  zs = data[:, 5]
  cs = [ 'b' if c == 3 else 'r' for c in data[:,2]]
  if (local_player_index != -1 and local_player_index < len(data)):
    cs[local_player_index] = 'g'
  hps = data[:, 1].astype(int, copy=False)
  ln.set_offsets(list(zip(zs, xs)))
  ln.set_color(cs)
  txt = [ax.annotate(str(hps[i]),(zs[i], xs[i]), color=cs[i]) for i in range(len(xs))]
  txt.insert(0, ln)
  # return ln,
  return tuple(txt)

def blit_init():
  if(map_name == "de_dust2"):
    xlims = (-2400, 1954)
    ylims = (-1155, 3383)
  elif(map_name == "de_inferno"):
    xlims = (-2087, 2950)
    ylims = (-1140, 3870)
  elif(map_name == "de_mirage"):
    xlims = (-3230, 1855)
    ylims = (-3385, 1713)
  elif(map_name == "de_cache"):
    xlims = (-2000, 3653)
    ylims = (-2420, 3250)
  elif(map_name == "de_overpass"):
    xlims = (-4831, 525)
    ylims = (-3583, 1781)
  elif(map_name == "de_cbble"):
    xlims = (-3840, 2325)
    ylims = (-3100, 3072)
  elif(map_name == "de_train"):
    xlims = (-2477, 2365)
    ylims = (-2445, 2392)
  elif(map_name == "de_nuke"):
    xlims = (-3453, 3750)
    ylims = (-4290, 2887)
  ax.imshow(img, extent=[ax.get_xlim()[0], ax.get_xlim()[1], ax.get_ylim()[0], ax.get_ylim()[1]])
  ax.set_xlim(xlims)
  ax.set_ylim(ylims)
  return ln,

# map_name = "de_dust2"
# map_name = "de_inferno"
# map_name = "de_mirage"
# map_name = "de_cache"

map_name = sys.argv[1]


has_plotted = False
while(not has_plotted):
  if (os.path.isfile(location_file)):
    if (os.path.getsize(location_file) > 0):
      has_plotted = True
      img = plt.imread("overviews/{}_radar.jpg".format(map_name))
      ani = animation.FuncAnimation(fig, getData, init_func=blit_init, interval=200, blit=True)
      plt.show()
  time.sleep(1)
