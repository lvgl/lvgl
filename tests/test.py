import os
import glob

def prepare():
  os.system("rm src/test_*_Runner.c")
  os.system("rm src/*.o")
  files = glob.glob("./src/test_*.c")
  for f in files:
    os.system("ruby unity/generate_test_runner.rb " + f + " config.yml") 
  return files  
