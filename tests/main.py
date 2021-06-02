#!/usr/bin/env python3

import defines
import build
import test


files = test.prepare()

for f in files:
  name = f[:-2] #test_foo.c -> test_foo 
  build.build("Minimal config, 16 bit color depth", defines.minimal_16bit, name)
  

#build("Minimal config monochrome", minimal_monochrome)
#build.build("Minimal config, 16 bit color depth", defines.minimal_16bit, "test_obj_tree")
#build("Minimal config, 16 bit color depth swapped", minimal_16bit_swap)
#build("Full config, 32 bit color depth", full_32bit)
