#!/usr/bin/python3

import sys, getopt
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def get_args(argv) -> tuple:
  ''' Возвращает значение аргументов '''
  input_f = '/var/log/load_cpu_iowait.csv'
  output_f = '/tmp/foo.png'
  level = 3
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
  x = df['time']
  y = df['cpu_iowait']
 
  # Plot Line1 (Left Y Axis)
  fig, ax = plt.subplots(1,1,figsize=(16,9), dpi= 80)
  ax.plot(x, y, color='tab:red')
  ax.ticklabel_format(style='plain', useOffset=False, axis='y')
  
  # Decorations X axis
  ax.set_xlabel('time', fontsize=20)
  ax.tick_params(axis='x', rotation=90, labelsize=1)
  # Decorations left Y1 axis
  ax.set_ylabel('max cpu core iowait, %', color='tab:red', fontsize=20)
  ax.tick_params(axis='y', rotation=0, labelcolor='tab:red', labelsize=12)
  
  # Decorations
  ax.set_title("Load CPU (iowait)", fontsize=22)
  ax.grid(alpha=.4)
  fig.tight_layout()

  # Save plot
  plt.savefig(output_f)

if __name__ == "__main__":
   main(sys.argv[1:])
