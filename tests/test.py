import os
import glob

def prepare():
  os.system("rm test_*_Runner.c")
  files = glob.glob("./test_*.c")
  for f in files:
    os.system("ruby unity/generate_test_runner.rb " + f + " config.yml") 
  return files  
