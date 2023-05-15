#!/usr/bin/python3

import sys, getopt, re
import numpy as np
import matplotlib.pyplot as plt

def get_args(argv) -> tuple:
  ''' Возвращает значение аргументов '''
  input_f = '/var/log/load_io_disk.log'
  output_f = '/tmp/foo.png'
  disk_name = 'sda'
  level = 3
  opts, args = getopt.getopt(argv,"hi:o:d:l:",["ifile=","ofile=","disk_name="])
  for opt, arg in opts:
    if opt == '-h':
      print ('draw_load_hw.py -i <input_file> -o <output_file> -d <disk_name> -l <draw_level>')
      sys.exit()
    elif opt in ("-i", "--ifile"):
      input_f = arg
    elif opt in ("-o", "--ofile"):
      output_f = arg
    elif opt in ("-d", "--disk_name"):
      disk_name = arg
    elif opt in ("-l", "--level"):
      level = arg
  return input_f, output_f, disk_name, level

def read_log_file(input_f:str, disk_name:str) -> tuple:
  time = np.array([])
  r_MBps = np.array([])
  w_MBps = np.array([])
  svctm_ms = np.array([])
  util_pct = np.array([])

  with open(input_f, "r") as file:
    tmp_time = []
    for line in file:
      find_data_time = re.findall(r'^\d+/\d+/\d+ \d+:\d+:\d+', line.strip())
      if find_data_time != tmp_time:
        tmp_time = find_data_time
	find_header = []
	find_metering = []
      else:
        find_header = re.findall(r'^Device.*', line.strip())
        find_metering = re.findall(r'^{}.*'.format(disk_name), line.strip())
      if find_metering and find_header:
        header = re.sub(r'[ ]+', ',', find_header[0]).split(',')
	number_r_MBps = header.index('rMB/s')
	number_w_MBps = header.index('wMB/s')
	number_svctm_ms = header.index('svctm')
	number_util_pct = header.index('%util')
        metering = find_metering[0]
        metering = re.sub(r',', '.', metering)
        metering = re.sub(r'[ ]+', ',', metering)
        metering = metering.split(',')
        if tmp_time:
          time = np.append(time, tmp_time[0].split(' ')[1])
          r_MBps = np.append(r_MBps, metering[number_r_MBps])
          w_MBps = np.append(w_MBps, metering[number_w_MBps])
          svctm_ms = np.append(svctm_ms, metering[number_svctm_ms])
          util_pct = np.append(util_pct, metering[number_util_pct])
  
  return (time,
          util_pct.astype(np.float64),
          r_MBps.astype(np.float64),
          w_MBps.astype(np.float64),
          util_pct.astype(np.float64))

def main(argv):
  input_f, output_f, disk_name, level = get_args(argv)
  # Import Data
  x, y1, y2, y3, y4 = read_log_file(input_f, disk_name)

  # Plot Line1 (Left Y Axis)
  fig, ax1 = plt.subplots(1,1,figsize=(16,9), dpi= 80)
  ax1.plot(x, y1, color='tab:red')
  ax1.ticklabel_format(style='plain', useOffset=False, axis='y')

  # Plot Line2 (Right Y Axis)
  if level >= 2:
    ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
    ax2.plot(x, y2, color='tab:blue')
    ax2.ticklabel_format(style='plain', useOffset=False, axis='y')

  # Plot Line3 (Right Y Axis)
  if level >= 3:
    ax3 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
    # new version matplotlib:
    # ax3.spines.right.set_position(("axes", 1.1))
    # old version matplotlib:
    ax3.spines['right'].set_position(('axes', 1.1))
    ax3.plot(x, y3, color='tab:green')
    ax3.ticklabel_format(style='plain', useOffset=False, axis='y')

  # Plot Line4 (Right Y Axis)
  if level >= 4:
    ax4 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
    # new version matplotlib:
    # ax4.spines.right.set_position(("axes", 1.2))
    # old version matplotlib:
    ax4.spines['right'].set_position(('axes', 1.2))
    ax4.plot(x, y4, color='tab:orange')
    ax4.ticklabel_format(style='plain', useOffset=False, axis='y')

  # Decorations X axis
  ax1.set_xlabel('time', fontsize=20)
  ax1.tick_params(axis='x', rotation=90, labelsize=1)
  # Decorations left Y1 axis
  ax1.set_ylabel('util, %', color='tab:red', fontsize=20)
  ax1.tick_params(axis='y', rotation=0, labelcolor='tab:red', labelsize=12)
  # Decorations right Y2 axis
  if level >= 2:
    ax2.set_ylabel('read, MB/s', color='tab:blue', fontsize=20)
    ax2.tick_params(axis='y', labelcolor='tab:blue', labelsize=12)
  # Decorations right Y3 axis
  if level >= 3:
    ax3.set_ylabel('write, MB/s', color='tab:green', fontsize=20)
    ax3.tick_params(axis='y', labelcolor='tab:green', labelsize=12)
  # Decorations right Y4 axis
  if level >= 4:
    ax4.set_ylabel('svctm (Average IO time), ms', color='tab:orange', fontsize=20)
    ax4.tick_params(axis='y', labelcolor='tab:orange', labelsize=12)

  # Decorations
  ax1.set_title("IO disk", fontsize=22)
  ax1.grid(alpha=.4)
  fig.tight_layout()

  # Save plot
  plt.savefig(output_f)

if __name__ == "__main__":
   main(sys.argv[1:])
