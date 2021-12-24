import numpy as np
import librosa
import math
import sys

print("Loading file")
audio, sample_rate = librosa.load(sys.argv[1], duration=60, offset=0, sr=15360)

print("Getting spectrum")
spectrum = librosa.stft(audio)
S = np.abs(spectrum)

fout = open("spectrum.h", "w")

print("Writing file")
fn = 36
fs = int(len(S) / fn)
fout.write("const uint16_t spectrum[][4] = {\n")
for t in range(0,len(S[0]-1)):
  fout.write("{ ")
  f_prev = 0 
  for f in [8, 45, 300, 600]:
    v = 0
    for i in range(f_prev, f): v += S[i][t]
    if v != 0: v = int(v/30)
    if v < 0:  v = 0
    f_prev = f
    fout.write(str(int(v)) + ", ")
  fout.write("},\n")
fout.write("};\n")
fout.close()

print("Finished")