# -*- coding: utf-8 -*-

import zipfile
import io
import os


SDL2_URL = 'https://github.com/libsdl-org/SDL/releases/download/release-2.26.5/SDL2-devel-2.26.5-VC.zip'  # NOQA


def get_path(name: str, p: str) -> str:
    for file in os.listdir(p):
        file = os.path.join(p, file)

        if file.endswith(name):
            return file

        if os.path.isdir(file):
            if res := get_path(name, file):
                return res


def get_sdl2(path, url=SDL2_URL):
    import requests  # NOQA

    stream = io.BytesIO()

    with requests.get(url, stream=True) as r:
        r.raise_for_status()

        content_length = int(r.headers['Content-Length'])
        chunks = 0
        # print()
        # sys.stdout.write('\r' + str(chunks) + '/' + str(content_length))
        # sys.stdout.flush()

        for chunk in r.iter_content(chunk_size=1024):
            stream.write(chunk)
            chunks += len(chunk)
            # sys.stdout.write('\r' + str(chunks) + '/' + str(content_length))
            # sys.stdout.flush()

    # print()
    stream.seek(0)
    zf = zipfile.ZipFile(stream)

    for z_item in zf.infolist():
        for ext in ('.h', '.dll', '.lib'):
            if not z_item.filename.endswith(ext):
                continue

            zf.extract(z_item, path=path)
            break

    include_path = get_path('include', path)
    lib_path = get_path('lib\\x64', path)
    dll_path = get_path('SDL2.dll', lib_path)

    sdl_include_path = os.path.split(include_path)[0]
    if not os.path.exists(os.path.join(sdl_include_path, 'SDL2')):
        os.rename(include_path, os.path.join(sdl_include_path, 'SDL2'))

    zf.close()
    stream.close()

    return os.path.abspath(sdl_include_path), dll_path
