
import os

base_path = os.path.dirname(__file__)
dst_config = os.path.join(base_path, 'lv_conf.h')
src_config = os.path.abspath(os.path.join(
    base_path,
    '..',
    'lv_conf_template.h'
))


def run():
    with open(src_config, 'r') as f:
        data = f.read()

    data = data.split('\n')

    for i, line in enumerate(data):
        if 'LV_USE' in line or 'LV_FONT' in line:
            line = [item for item in line.split(' ') if item]
            for j, item in enumerate(line):
                if item == '0':
                    line[j] = '1'
            line = ' '.join(line)
            data[i] = line
    data = '\n'.join(data)

    with open(dst_config, 'w') as f:
        f.write(data)


def cleanup():
    if os.path.exists(dst_config):
        os.remove(dst_config)