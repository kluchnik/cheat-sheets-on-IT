#!/usr/bin/python3

import sys, getopt
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def get_args(argv) -> tuple:
  ''' Возвращает значение аргументов '''
  input_f = '/var/log/load_hw.csv'
  output_f = '/tmp/foo.png'
  opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
  for opt, arg in opts:
    if opt == '-h':
      print ('draw_load_hw.py -i <input_file> -o <output_file>')
      sys.exit()
    elif opt in ("-i", "--ifile"):
      input_f = arg
    elif opt in ("-o", "--ofile"):
      output_f = arg
  return input_f, output_f
    
def main(argv):
  input_f, output_f = get_args(argv)
  # Import Data
  df = pd.read_csv(input_f)
  x = df['date']
  y1 = df['cpu_%']
  y2 = df['mem_MB']
  y3 = df['disk_MB']
  
  # Plot Line1 (Left Y Axis)
  fig, ax1 = plt.subplots(1,1,figsize=(16,9), dpi= 80)
  ax1.plot(x, y1, color='tab:red')
  ax1.ticklabel_format(style='plain', useOffset=False, axis='y')
  
  # Plot Line2 (Right Y Axis)
  ax2 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
  ax2.plot(x, y2, color='tab:blue')
  ax2.ticklabel_format(style='plain', useOffset=False, axis='y')
  
  # Plot Line3 (Right Y Axis)
  ax3 = ax1.twinx()  # instantiate a second axes that shares the same x-axis
  ax3.spines.right.set_position(("axes", 1.1))
  ax3.plot(x, y3, color='tab:green')
  ax3.ticklabel_format(style='plain', useOffset=False, axis='y')
  
  # Decorations X axis
  ax1.set_xlabel('date', fontsize=20)
  ax1.tick_params(axis='x', rotation=90, labelsize=1)
  # Decorations left Y1 axis
  ax1.set_ylabel('cpu, %', color='tab:red', fontsize=20)
  ax1.tick_params(axis='y', rotation=0, labelcolor='tab:red', labelsize=12)
  # Decorations right Y2 axis
  ax2.set_ylabel('memory, MB', color='tab:blue', fontsize=20)
  ax2.tick_params(axis='y', labelcolor='tab:blue', labelsize=12)
  # Decorations right Y3 axis
  ax3.set_ylabel('disk, MB', color='tab:green', fontsize=20)
  ax3.tick_params(axis='y', labelcolor='tab:green', labelsize=12)
  
  # Decorations
  ax1.set_title("Load HW", fontsize=22)
  ax1.grid(alpha=.4)
  fig.tight_layout()

  # Save plot
  plt.savefig(output_f)

if __name__ == "__main__":
   main(sys.argv[1:])
