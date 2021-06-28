import os
import glob

def prepare():
  os.system("rm src/*.o")
  os.system("rm -rdf src/test_runners")
  os.system("mkdir src/test_runners")
  files = glob.glob("./src/test_cases/test_*.c")
  
  for index, item in enumerate(files):
    if item == "./src/test_cases/test_config.c":
        files.pop(index)
        break
        
  files.insert(0, "./src/test_cases/test_config.c")
  
  for f in files:
    r = f[:-2] + "_Runner.c"
    r = r.replace("/test_cases/", "/test_runners/")
    cmd = "ruby unity/generate_test_runner.rb " + f + " " + r + " config.yml"
    os.system(cmd) 
  return files  
