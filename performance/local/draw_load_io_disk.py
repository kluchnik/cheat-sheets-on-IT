#!/usr/bin/python3

import sys, getopt, re
import numpy as np
import matplotlib.pyplot as plt

def get_args(argv) -> tuple:
  ''' Возвращает значение аргументов '''
  input_f = '/var/log/load_io_disk.log'
  output_f = '/tmp/foo.png'
  disk_name = 'sda'
  opts, args = getopt.getopt(argv,"hi:o:d:",["ifile=","ofile=","disk_name="])
  for opt, arg in opts:
    if opt == '-h':
      print ('draw_load_hw.py -i <input_file> -o <output_file> -d <disk_name>')
      sys.exit()
    elif opt in ("-i", "--ifile"):
      input_f = arg
    elif opt in ("-o", "--ofile"):
      output_f = arg
    elif opt in ("-d", "--disk_name"):
      disk_name = arg
  return input_f, output_f, disk_name

def read_log_file(input_f:str, disk_name:str) -> tuple:
  time = np.array([])
  r_MBps = np.array([])
  w_MBps = np.array([])
  svctm_ms = np.array([])
  util_pct = np.array([])

  with open(input_f, "r") as file:
    for line in file:
      find_data_time = re.findall(r'^\d+/\d+/\d+ \d+:\d+:\d+', line.strip())
      find_metering = re.findall(r'^{}.*'.format(disk_name), line.strip())
      if find_data_time:
        tmp_time = find_data_time[0].split(' ')[1]
      if find_metering:
        metering = re.sub(r'[ ]+', ',', find_metering[0]).split(',')
        if tmp_time:
          time = np.append(time, tmp_time)
          r_MBps = np.append(r_MBps, metering[3])
          w_MBps = np.append(w_MBps, metering[4])
          svctm_ms = np.append(svctm_ms, metering[14])
          util_pct = np.append(util_pct, metering[15])
  
  return (time,
    util_pct.astype(np.float64),
	r_MBps.astype(np.float64),
	w_MBps.astype(np.float64),
	util_pct.astype(np.float64))

def main(argv):
  input_f, output_f, disk_name = get_args(argv)
  # Import Data
  x, y1, y2, y3, y4 = read_log_file(input_f, disk_name)

  # Plot Line1 (Left Y Axis)
  fig, ax1 = plt.subplots(1,1,figsize=(16,9), dpi= 80)
  _ = ax1.plot(x, y1, color='tab:red')
  ax1.ticklabel_format(style='plain', useOffset=False, axis='y')

  # Plot Line2 (Right Y Axis)
  ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
  _ = ax2.plot(x, y2, color='tab:blue')
  ax2.ticklabel_format(style='plain', useOffset=False, axis='y')

  # Plot Line3 (Right Y Axis)
  ax3 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
  # new version matplotlib:
  # ax3.spines.right.set_position(("axes", 1.1))
  # old version matplotlib:
  ax3.spines['right'].set_position(('axes', 1.1))
  _ = ax3.plot(x, y3, color='tab:green')
  ax3.ticklabel_format(style='plain', useOffset=False, axis='y')

  # Plot Line4 (Right Y Axis)
  ax4 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
  # new version matplotlib:
  # ax4.spines.right.set_position(("axes", 1.2))
  # old version matplotlib:
  ax4.spines['right'].set_position(('axes', 1.2))
  _ = ax4.plot(x, y4, color='tab:orange')
  ax4.ticklabel_format(style='plain', useOffset=False, axis='y')

  # Decorations X axis
  _ = ax1.set_xlabel('time', fontsize=20)
  ax1.tick_params(axis='x', rotation=90, labelsize=1)
  # Decorations left Y1 axis
  _ = ax1.set_ylabel('util, %', color='tab:red', fontsize=20)
  ax1.tick_params(axis='y', rotation=0, labelcolor='tab:red', labelsize=12)
  # Decorations right Y2 axis
  _ = ax2.set_ylabel('read, MB/s', color='tab:blue', fontsize=20)
  ax2.tick_params(axis='y', labelcolor='tab:blue', labelsize=12)
  # Decorations right Y3 axis
  _ = ax3.set_ylabel('write, MB/s', color='tab:green', fontsize=20)
  ax3.tick_params(axis='y', labelcolor='tab:green', labelsize=12)
  # Decorations right Y4 axis
  _ = ax4.set_ylabel('svctm (Average IO time), ms', color='tab:orange', fontsize=20)
  ax4.tick_params(axis='y', labelcolor='tab:orange', labelsize=12)

  # Decorations
  _ = ax1.set_title("IO disk", fontsize=22)
  ax1.grid(alpha=.4)
  fig.tight_layout()

  # Save plot
  plt.savefig(output_f)

if __name__ == "__main__":
   main(sys.argv[1:])
