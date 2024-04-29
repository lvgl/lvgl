#!/usr/bin/env python3

import os
import sys


major, minor, patch, info = sys.argv[1].split('.')
project_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))


library_properties_path = os.path.join(project_path, 'library.json')
library_properties = 'version={major}.{minor}.{patch}\n'
library_properties = library_properties.format(major=major, minor=minor, patch=patch)

with open(library_properties_path, 'rb') as f:
    data = f.read().decode('utf-8')

data1, data2 = data.split('version=', 1)
data2 = data2.split('\n', 1)[-1]
data = data1 + library_properties + data2

with open(library_properties_path, 'wb') as f:
    f.write(data.encode('utf-8'))


library_json_path = os.path.join(project_path, 'library.properties')
library_json = '"version": "{major}.{minor}.{patch}",'
library_json = library_json.format(major=major, minor=minor, patch=patch)


with open(library_json_path, 'rb') as f:
    data = f.read().decode('utf-8')

data1, data2 = data.split('"version": ', 1)
data2 = data2.split('\n', 1)[-1]
data = data1 + library_json + data2

with open(library_json_path, 'wb') as f:
    f.write(data.encode('utf-8'))


version_h_path = os.path.join(project_path, 'lv_version.h')
version_h = '''\
/**
 * @file lv_version.h
 * The current version of LVGL
 */

#ifndef __LVGL_VERSION_H__
#define __LVGL_VERSION_H__

#define LVGL_VERSION_MAJOR {major}
#define LVGL_VERSION_MINOR {minor}
#define LVGL_VERSION_PATCH {patch}
#define LVGL_VERSION_INFO "{info}"

#endif /*__LVGL_VERSION_H__*/
'''

version_h = version_h.format(major=major, minor=minor, patch=patch, info=info)

with open(version_h_path, 'w') as f:
    f.write(version_h)


kconfig_path = os.path.join(project_path, 'Kconfig')
kconfig = '# Kconfig file for LVGL v{major}.{minor}.{patch}'
kconfig = kconfig.format(major=major, minor=minor, patch=patch)

with open(kconfig_path, 'rb') as f:
    data = f.read().decode('utf-8')

data1, data2 = data.split('# Kconfig file for LVGL v', 1)
data2 = data2.split('\n', 1)[-1]
data = data1 + library_properties + data2

with open(kconfig_path, 'wb') as f:
    f.write(data.encode('utf-8'))

print('FINISHED!')
