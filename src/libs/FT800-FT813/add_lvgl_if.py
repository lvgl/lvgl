import subprocess

ccpp = subprocess.check_output("find . -name '*.c' -or -name '*.cpp'", shell=True).splitlines()

for fname in ccpp:
    with open(fname) as f:
        cont = f.read()
    with open(fname, 'w') as f:
        f.write('#include "../../lv_conf_internal.h"\n#if LV_USE_DRAW_EVE\n')
        f.write(cont)
        f.write('\n#endif /*LV_USE_DRAW_EVE*/\n')
