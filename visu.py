import os
import sys
import time
from select import select

import matplotlib
import matplotlib.animation as animation
import matplotlib.pyplot as plt
import numpy as np

# import _thread

SEPERATOR = "|"
matplotlib.rcParams['toolbar'] = 'None'
fig, ax = plt.subplots()
xdata, ydata = [], []
ln = plt.scatter([], [], animated=True)
# ln.set_offsets([[0.5], [0.4]])
# print(ln.get_offsets())
# location_file = "/tmp/locs.csv"
xlims, ylims = [], []

def weapon(wid):
  if wid == 0:
    return "NONE"
  if wid == 1:
    return "DEAGLE"
  if wid == 2:
    return "DUALS"
  if wid == 3:
    return "FIVE7"
  if wid == 4:
    return "GLOCK"
  if wid == 7:
    return "AK47"
  if wid == 8:
    return "AUG"
  if wid == 9:
    return "AWP"
  if wid == 10:
    return "FAMAS"
  if wid == 11:
    return "G3SG1"
  if wid == 13:
    return "GALIL"
  if wid == 14:
    return "M249"
  if wid == 16:
    return "M4A4"
  if wid == 17:
    return "MAC10"
  if wid == 19:
    return "P90"
  if wid == 23:
    return "MP5"
  if wid == 24:
    return "UMP"
  if wid == 25:
    return "XM1014"
  if wid == 26:
    return "BIZON"
  if wid == 27:
    return "MAG7"
  if wid == 28:
    return "NEGEV"
  if wid == 29:
    return "SAWED"
  if wid == 30:
    return "TEC9"
  if wid == 31:
    return "ZEUS"
  if wid == 32:
    return "P2000"
  if wid == 33:
    return "MP7"
  if wid == 34:
    return "MP9"
  if wid == 35:
    return "NOVA"
  if wid == 36:
    return "P250"
  if wid == 38:
    return "SCAR"
  if wid == 39:
    return "SG556"
  if wid == 40:
    return "SCOUT"
  if wid == 42:
    return "KNIFE"
  if wid == 43:
    return "FLASH"
  if wid == 44:
    return "HE"
  if wid == 45:
    return "SMOKE"
  if wid == 46:
    return "MOLO"
  if wid == 47:
    return "DEC"
  if wid == 48:
    return "INC"
  if wid == 49:
    return "C4"
  if wid == 59:
    return "KNIFE"
  if wid == 60:
    return "M4A1"
  if wid == 61:
    return "USP"
  if wid == 63:
    return "CZ75"
  if wid == 64:
    return "REV"
  if wid in range(500, 520):
    return "KNIFE"
  return "UNK"

def readFromInput():
  ready, _, _ = select([sys.stdin], [], [])
  while (len(ready) > 0):
    inp = input()
    ready, _, _ = select([sys.stdin], [], [], 0)
    if (inp == "quit"):
      plt.close()

  lines = inp.split(SEPERATOR)
  arr = []
  for line in lines:
    arr.append(np.fromstring(line, sep=','))
  arr = np.asarray(arr)
  return arr

def getData(i):
  try :
    #Format: id,hp,team,weapon,defusing,x,y,z,rotation
    data = readFromInput()
  except:
    return ln,
  if(len(data) == 0):
    return ln,
  if(len(data.shape) == 1):
    data = data.reshape((1, 9))
  # parse input data
  local_player_index = int(data[0,0])
  rotation = int(data[0,8])
  data = data[1:]
  # data = data[data[:, 1] != 0]
  zs = data[:, -4]
  xs = data[:, -3]
  # ys = data[:, -2]
  player_count = len(xs)
  cs = [ 'b' if c == 3 else 'r' for c in data[:,2]]
  vecs_tmp = np.dstack((zs, xs))[0]

  # filter out thos points not inside the limits
  filter_indices = []
  for j,vec in enumerate(vecs_tmp):
    if xlims[0] <= vec[0] <= xlims[1] and  ylims[0] <= vec[1] <= ylims[1]:
      filter_indices.append(j)
  vecs = vecs_tmp[filter_indices]

  # transformation for generic radar
  if generic and local_player_index != -1 and local_player_index < len(data):
    # center around local player
    vecs -= vecs[local_player_index]
    # rotate according to rotation
    angle = np.radians(rotation - 90)
    rotation_matrix = np.array([[np.cos(angle), -np.sin(angle)], [np.sin(angle), np.cos(angle)]])
    vecs = np.dot(vecs, rotation_matrix)

  if (local_player_index != -1 and local_player_index < len(data)):
    cs[local_player_index] = 'g'

  hps = data[:, 1].astype(int, copy=False)
  weapons = data[:, 3]
  defusings = (data[:, 4] == 1)
  ln.set_offsets(vecs[:,:2])
  ln.set_color(cs)
  strings = []
  for i in range(len(vecs)):
    strings.append(str(hps[i]))
    strings[-1] += " " + weapon(weapons[i])
    if defusings[i]:
      strings[-1] += " def"
  txt = [ax.annotate(strings[i], vecs[i], color=cs[i]) for i in range(len(vecs))]
  txt.insert(0, ln)
  # return ln,
  return tuple(txt)

def blit_init():
  global xlims, ylims
  if(map_name == "de_dust2"):
    xlims = (-2400, 2040)
    ylims = (-1287, 3232)
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
  elif(map_name == "dz_blacksite"):
    xlims = (-8604, 8350)
    ylims = (-8300, 8804)
  else:
    xlims = (-2400, 2400)
    ylims = (-1500, 1500)
  ax.imshow(img, extent=[ax.get_xlim()[0], ax.get_xlim()[1], ax.get_ylim()[0], ax.get_ylim()[1]])
  ax.set_xlim(xlims)
  ax.set_ylim(ylims)
  ax.set_frame_on(False)
  ax.axis('off')
  fig.tight_layout(pad=0.01)
  return ln,

def catch_term_sig():
  while (True):
    inp = input()
    if (inp == "quit"):
      plt.close()
      break
# map_name = "de_dust2"
# map_name = "de_inferno"
# map_name = "de_mirage"
# map_name = "de_cache"

map_name = sys.argv[1]
interval = int(sys.argv[2])
fig.canvas.set_window_title("Radar - {}".format(map_name))

has_plotted = False
# _thread.start_new_thread(catch_term_sig, ())

while(not has_plotted):
  time.sleep(1)
  has_plotted = True
  generic = False
  try:
    img = plt.imread("overviews/{}_radar.jpg".format(map_name))
  except:
    white = [255, 255, 255]
    img = np.array([white*400]*400)
    img = img.reshape((400,400,3))
    generic = True
  ani = animation.FuncAnimation(fig, getData, init_func=blit_init, interval=interval, blit=True)
  plt.show()
