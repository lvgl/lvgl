#!/usr/bin/env python3
"""Generate the glTF assets used by the tests in tests/src/test_cases/3d."""

import base64
import json
import os
import struct
import zlib
from pathlib import Path

BASE_PATH = os.path.abspath(os.path.dirname(__file__))
OUT_DIR = os.path.join(BASE_PATH, "src", "test_assets", "gltf", "generated")

# glTF component types
BYTE_UNSIGNED_SHORT = 5123
FLOAT = 5126

# glTF primitive modes
MODE_TRIANGLES = 4


class Buffer:
    """Accumulates binary data and creates the matching accessors."""

    def __init__(self):
        self.data = bytearray()
        self.views = []
        self.accessors = []

    def _align(self):
        while len(self.data) % 4:
            self.data.append(0)

    def _add_view(self, payload, target=None):
        self._align()
        offset = len(self.data)
        self.data.extend(payload)
        view = {"buffer": 0, "byteOffset": offset, "byteLength": len(payload)}
        if target is not None:
            view["target"] = target
        self.views.append(view)
        return len(self.views) - 1

    def add_floats(self, values, type_name, count, minmax=False):
        payload = struct.pack("<%df" % len(values), *values)
        view = self._add_view(payload, target=34962)  # ARRAY_BUFFER
        accessor = {
            "bufferView": view,
            "componentType": FLOAT,
            "count": count,
            "type": type_name,
        }
        if minmax:
            comps = len(values) // count
            cols = [values[i::comps] for i in range(comps)]
            accessor["min"] = [min(c) for c in cols]
            accessor["max"] = [max(c) for c in cols]
        self.accessors.append(accessor)
        return len(self.accessors) - 1

    def add_indices(self, values):
        payload = struct.pack("<%dH" % len(values), *values)
        view = self._add_view(payload, target=34963)  # ELEMENT_ARRAY_BUFFER
        self.accessors.append(
            {
                "bufferView": view,
                "componentType": BYTE_UNSIGNED_SHORT,
                "count": len(values),
                "type": "SCALAR",
            }
        )
        return len(self.accessors) - 1

    def add_ushorts(self, values, type_name, count):
        payload = struct.pack("<%dH" % len(values), *values)
        view = self._add_view(payload, target=34962)
        self.accessors.append(
            {
                "bufferView": view,
                "componentType": BYTE_UNSIGNED_SHORT,
                "count": count,
                "type": type_name,
            }
        )
        return len(self.accessors) - 1

    def finish(self, gltf, embed=True):
        self._align()
        gltf["bufferViews"] = self.views
        gltf["accessors"] = self.accessors
        if embed:
            uri = (
                "data:application/octet-stream;base64,"
                + base64.b64encode(bytes(self.data)).decode()
            )
            gltf["buffers"] = [{"byteLength": len(self.data), "uri": uri}]
        else:
            gltf["buffers"] = [{"byteLength": len(self.data)}]
        return bytes(self.data)


def png_bytes(width, height, rgba):
    """Minimal PNG writer so the common case needs no third party module."""

    def chunk(tag, payload):
        return (
            struct.pack(">I", len(payload))
            + tag
            + payload
            + struct.pack(">I", zlib.crc32(tag + payload) & 0xFFFFFFFF)
        )

    raw = bytearray()
    for y in range(height):
        raw.append(0)  # no filter
        for x in range(width):
            raw.extend(rgba(x, y))
    return (
        b"\x89PNG\r\n\x1a\n"
        + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
        + chunk(b"IDAT", zlib.compress(bytes(raw), 9))
        + chunk(b"IEND", b"")
    )


def pil_image_bytes(width, height, rgba, fmt, **kwargs):
    from PIL import Image

    img = Image.new("RGBA", (width, height))
    img.putdata([tuple(rgba(x, y)) for y in range(height) for x in range(width)])
    if fmt == "JPEG":
        img = img.convert("RGB")
    import io

    out = io.BytesIO()
    img.save(out, fmt, **kwargs)
    return out.getvalue()


def checker(x, y):
    return (255, 64, 64, 255) if (x + y) % 2 else (64, 64, 255, 255)


def data_uri(payload, mime):
    return "data:%s;base64,%s" % (mime, base64.b64encode(payload).decode())


def base_gltf(**extra):
    gltf = {"asset": {"version": "2.0", "generator": "lvgl gen_gltf_assets.py"}}
    gltf.update(extra)
    return gltf


# ----------------------------------------------------------------------------
# Geometry helpers
# ----------------------------------------------------------------------------

TRIANGLE_POS = [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.5, 1.0, 0.0]
TRIANGLE_NRM = [0.0, 0.0, 1.0] * 3
TRIANGLE_UV = [0.0, 0.0, 1.0, 0.0, 0.5, 1.0]

# Unit cube centered on the origin
CUBE_POS = []
CUBE_NRM = []
CUBE_UV = []
CUBE_IDX = []
_FACES = [
    (
        (0, 0, 1),
        [(-0.5, -0.5, 0.5), (0.5, -0.5, 0.5), (0.5, 0.5, 0.5), (-0.5, 0.5, 0.5)],
    ),
    (
        (0, 0, -1),
        [(0.5, -0.5, -0.5), (-0.5, -0.5, -0.5), (-0.5, 0.5, -0.5), (0.5, 0.5, -0.5)],
    ),
    (
        (1, 0, 0),
        [(0.5, -0.5, 0.5), (0.5, -0.5, -0.5), (0.5, 0.5, -0.5), (0.5, 0.5, 0.5)],
    ),
    (
        (-1, 0, 0),
        [(-0.5, -0.5, -0.5), (-0.5, -0.5, 0.5), (-0.5, 0.5, 0.5), (-0.5, 0.5, -0.5)],
    ),
    (
        (0, 1, 0),
        [(-0.5, 0.5, 0.5), (0.5, 0.5, 0.5), (0.5, 0.5, -0.5), (-0.5, 0.5, -0.5)],
    ),
    (
        (0, -1, 0),
        [(-0.5, -0.5, -0.5), (0.5, -0.5, -0.5), (0.5, -0.5, 0.5), (-0.5, -0.5, 0.5)],
    ),
]
for _face, (_normal, _corners) in enumerate(_FACES):
    for _corner, _uv in zip(_corners, [(0, 0), (1, 0), (1, 1), (0, 1)]):
        CUBE_POS.extend(_corner)
        CUBE_NRM.extend(_normal)
        CUBE_UV.extend(_uv)
    _b = _face * 4
    CUBE_IDX.extend([_b, _b + 1, _b + 2, _b, _b + 2, _b + 3])


def add_triangle(buf, uv=True):
    attrs = {
        "POSITION": buf.add_floats(TRIANGLE_POS, "VEC3", 3, minmax=True),
        "NORMAL": buf.add_floats(TRIANGLE_NRM, "VEC3", 3),
    }
    if uv:
        attrs["TEXCOORD_0"] = buf.add_floats(TRIANGLE_UV, "VEC2", 3)
    return {"attributes": attrs, "mode": MODE_TRIANGLES}


def add_cube(buf, uv=True):
    attrs = {
        "POSITION": buf.add_floats(CUBE_POS, "VEC3", 24, minmax=True),
        "NORMAL": buf.add_floats(CUBE_NRM, "VEC3", 24),
    }
    if uv:
        attrs["TEXCOORD_0"] = buf.add_floats(CUBE_UV, "VEC2", 24)
    return {
        "attributes": attrs,
        "indices": buf.add_indices(CUBE_IDX),
        "mode": MODE_TRIANGLES,
    }


def write_gltf(name, gltf):
    path = os.path.join(OUT_DIR, name)
    with open(path, "w") as f:
        json.dump(gltf, f, indent=1, sort_keys=True)
        f.write("\n")


def write_glb(name, gltf, binary):
    """GLB with a JSON chunk and a BIN chunk, as exported by 3D tools."""
    json_chunk = json.dumps(gltf, sort_keys=True).encode()
    json_chunk += b" " * (-len(json_chunk) % 4)
    bin_chunk = binary + b"\0" * (-len(binary) % 4)

    total = 12 + 8 + len(json_chunk) + 8 + len(bin_chunk)
    out = struct.pack("<III", 0x46546C67, 2, total)
    out += struct.pack("<II", len(json_chunk), 0x4E4F534A) + json_chunk
    out += struct.pack("<II", len(bin_chunk), 0x004E4942) + bin_chunk

    path = os.path.join(OUT_DIR, name)
    with open(path, "wb") as f:
        f.write(out)
    print("wrote %s (%d bytes)" % (name, os.path.getsize(path)))


# ----------------------------------------------------------------------------
# Assets
# ----------------------------------------------------------------------------


def gen_minimal_triangle():
    """1 mesh, 1 primitive, 1 material, 1 node, 1 scene, no textures."""
    buf = Buffer()
    prim = add_triangle(buf, uv=False)
    gltf = base_gltf(
        materials=[
            {
                "name": "red",
                "doubleSided": True,
                "pbrMetallicRoughness": {
                    "baseColorFactor": [0.9, 0.1, 0.1, 1.0],
                    "metallicFactor": 0.0,
                    "roughnessFactor": 0.5,
                },
            }
        ],
        meshes=[{"name": "triangle", "primitives": [dict(prim, material=0)]}],
        nodes=[{"name": "triangle", "mesh": 0}],
        scenes=[{"name": "scene", "nodes": [0]}],
        scene=0,
    )
    binary = buf.finish(gltf)
    write_gltf("minimal_triangle.gltf", gltf)

    # Same model as GLB: the buffer has no URI and lives in the BIN chunk
    glb = json.loads(json.dumps(gltf))
    glb["buffers"] = [{"byteLength": len(binary)}]
    write_glb("minimal_triangle.glb", glb, binary)


def gen_pbr_textures():
    """A quad using the full metallic-roughness texture set: 5 images, 5 textures,
    2 samplers and KHR_texture_transform on the base color texture."""
    buf = Buffer()
    prim = add_cube(buf)

    images = [
        ("base_color", png_bytes(4, 4, checker)),
        ("metallic_roughness", png_bytes(2, 2, lambda x, y: (0, 128, 255, 255))),
        ("normal", png_bytes(2, 2, lambda x, y: (128, 128, 255, 255))),
        ("occlusion", png_bytes(2, 2, lambda x, y: (200, 200, 200, 255))),
        ("emissive", png_bytes(2, 2, lambda x, y: (0, 255, 0, 255))),
    ]
    gltf = base_gltf(
        extensionsUsed=["KHR_texture_transform"],
        images=[{"name": n, "uri": data_uri(p, "image/png")} for n, p in images],
        samplers=[
            {"magFilter": 9729, "minFilter": 9987, "wrapS": 10497, "wrapT": 10497},
            {"magFilter": 9728, "minFilter": 9728, "wrapS": 33071, "wrapT": 33071},
        ],
        textures=[
            {"name": n, "source": i, "sampler": 0 if i else 1}
            for i, (n, _) in enumerate(images)
        ],
        materials=[
            {
                "name": "pbr",
                "pbrMetallicRoughness": {
                    "baseColorTexture": {
                        "index": 0,
                        "extensions": {
                            "KHR_texture_transform": {
                                "offset": [0.1, 0.1],
                                "scale": [2.0, 2.0],
                            }
                        },
                    },
                    "metallicRoughnessTexture": {"index": 1},
                    "metallicFactor": 1.0,
                    "roughnessFactor": 1.0,
                },
                "normalTexture": {"index": 2, "scale": 0.8},
                "occlusionTexture": {"index": 3, "strength": 0.9},
                "emissiveTexture": {"index": 4},
                "emissiveFactor": [0.2, 0.2, 0.2],
            }
        ],
        meshes=[{"name": "cube", "primitives": [dict(prim, material=0)]}],
        nodes=[{"name": "cube", "mesh": 0}],
        scenes=[{"nodes": [0]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("pbr_textures.gltf", gltf)


def gen_compressed_textures():
    """JPEG and WebP (EXT_texture_webp) base color textures, both embedded.

    The WebP image is referenced through the extension only, which is the code path
    that resolves a texture through `webpImageIndex`.
    """
    buf = Buffer()
    prim = add_cube(buf)
    jpeg = pil_image_bytes(8, 8, checker, "JPEG", quality=90)
    webp = pil_image_bytes(8, 8, checker, "WEBP", lossless=True)

    gltf = base_gltf(
        extensionsUsed=["EXT_texture_webp"],
        extensionsRequired=["EXT_texture_webp"],
        images=[
            {"name": "jpeg", "uri": data_uri(jpeg, "image/jpeg")},
            {"name": "webp", "uri": data_uri(webp, "image/webp")},
        ],
        textures=[
            {"name": "jpeg", "source": 0},
            {"name": "webp", "extensions": {"EXT_texture_webp": {"source": 1}}},
        ],
        materials=[
            {
                "name": "jpeg",
                "pbrMetallicRoughness": {"baseColorTexture": {"index": 0}},
            },
            {
                "name": "webp",
                "pbrMetallicRoughness": {"baseColorTexture": {"index": 1}},
            },
        ],
        meshes=[
            {
                "name": "cubes",
                "primitives": [dict(prim, material=0), dict(prim, material=1)],
            }
        ],
        nodes=[{"name": "cubes", "mesh": 0}],
        scenes=[{"nodes": [0]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("compressed_textures.gltf", gltf)


def gen_materials():
    """One primitive per documented material feature: opaque, alpha mask, alpha blend,
    transmission + volume + ior, clearcoat, emissive strength and unlit."""
    buf = Buffer()
    prim = add_cube(buf)

    materials = [
        {
            "name": "opaque",
            "pbrMetallicRoughness": {"baseColorFactor": [0.8, 0.8, 0.8, 1.0]},
        },
        {
            "name": "mask",
            "alphaMode": "MASK",
            "alphaCutoff": 0.4,
            "pbrMetallicRoughness": {"baseColorFactor": [1.0, 0.0, 0.0, 0.5]},
        },
        {
            "name": "blend",
            "alphaMode": "BLEND",
            "pbrMetallicRoughness": {"baseColorFactor": [0.0, 1.0, 0.0, 0.35]},
        },
        {
            "name": "transmission",
            "extensions": {
                "KHR_materials_transmission": {"transmissionFactor": 0.9},
                "KHR_materials_volume": {
                    "thicknessFactor": 0.5,
                    "attenuationDistance": 1.0,
                    "attenuationColor": [0.9, 0.9, 1.0],
                },
                "KHR_materials_ior": {"ior": 1.5},
                "KHR_materials_dispersion": {"dispersion": 0.1},
            },
        },
        {
            "name": "clearcoat",
            "extensions": {
                "KHR_materials_clearcoat": {
                    "clearcoatFactor": 1.0,
                    "clearcoatRoughnessFactor": 0.1,
                }
            },
        },
        {
            "name": "emissive",
            "emissiveFactor": [1.0, 0.6, 0.2],
            "extensions": {
                "KHR_materials_emissive_strength": {"emissiveStrength": 4.0}
            },
        },
        {
            "name": "unlit",
            "extensions": {"KHR_materials_unlit": {}},
            "pbrMetallicRoughness": {"baseColorFactor": [0.2, 0.4, 1.0, 1.0]},
        },
        {
            "name": "specular_sheen",
            "extensions": {
                "KHR_materials_specular": {"specularFactor": 0.7},
                "KHR_materials_sheen": {"sheenColorFactor": [0.5, 0.5, 0.0]},
                "KHR_materials_anisotropy": {"anisotropyStrength": 0.6},
                "KHR_materials_iridescence": {"iridescenceFactor": 0.4},
            },
        },
    ]

    gltf = base_gltf(
        extensionsUsed=[
            "KHR_materials_transmission",
            "KHR_materials_volume",
            "KHR_materials_ior",
            "KHR_materials_dispersion",
            "KHR_materials_clearcoat",
            "KHR_materials_emissive_strength",
            "KHR_materials_unlit",
            "KHR_materials_specular",
            "KHR_materials_sheen",
            "KHR_materials_anisotropy",
            "KHR_materials_iridescence",
        ],
        materials=materials,
        meshes=[
            {
                "name": "materials",
                "primitives": [dict(prim, material=i) for i in range(len(materials))],
            }
        ],
        nodes=[{"name": "materials", "mesh": 0}],
        scenes=[{"nodes": [0]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("materials.gltf", gltf)


def gen_sheen():
    """A cube whose only light response is the sheen lobe: the base color is black and
    the dielectric specular is switched off, so anything visible comes from sheen.

    This is what makes a broken sheen path show up as a black render.
    """
    buf = Buffer()
    prim = add_cube(buf)
    gltf = base_gltf(
        extensionsUsed=["KHR_materials_sheen", "KHR_materials_specular"],
        materials=[
            {
                "name": "sheen_only",
                "pbrMetallicRoughness": {
                    "baseColorFactor": [0.0, 0.0, 0.0, 1.0],
                    "metallicFactor": 0.0,
                    "roughnessFactor": 1.0,
                },
                "extensions": {
                    "KHR_materials_sheen": {
                        "sheenColorFactor": [1.0, 1.0, 1.0],
                        "sheenRoughnessFactor": 0.3,
                    },
                    "KHR_materials_specular": {"specularFactor": 0.0},
                },
            }
        ],
        meshes=[{"name": "cube", "primitives": [dict(prim, material=0)]}],
        nodes=[{"name": "cube", "mesh": 0}],
        scenes=[{"nodes": [0]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("sheen.gltf", gltf)


def gen_cameras():
    """A perspective and an orthographic camera, each on its own node, plus the mesh."""
    buf = Buffer()
    prim = add_cube(buf)
    gltf = base_gltf(
        cameras=[
            {
                "name": "perspective",
                "type": "perspective",
                "perspective": {
                    "yfov": 0.8,
                    "znear": 0.1,
                    "zfar": 100.0,
                    "aspectRatio": 1.5,
                },
            },
            {
                "name": "orthographic",
                "type": "orthographic",
                "orthographic": {"xmag": 2.0, "ymag": 2.0, "znear": 0.1, "zfar": 100.0},
            },
        ],
        materials=[
            {
                "name": "grey",
                "pbrMetallicRoughness": {"baseColorFactor": [0.7, 0.7, 0.7, 1.0]},
            }
        ],
        meshes=[{"name": "cube", "primitives": [dict(prim, material=0)]}],
        nodes=[
            {"name": "cube", "mesh": 0},
            {"name": "cam_persp", "camera": 0, "translation": [0.0, 0.0, 4.0]},
            {
                "name": "cam_ortho",
                "camera": 1,
                "translation": [4.0, 0.0, 0.0],
                "rotation": [0.0, 0.7071068, 0.0, 0.7071068],
            },
        ],
        scenes=[{"nodes": [0, 1, 2]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("cameras.gltf", gltf)


def gen_animation():
    """Three animations over one node: translation (LINEAR), rotation (LINEAR) and
    scale (STEP). Two seconds long so a test can sample it at several times."""
    buf = Buffer()
    prim = add_cube(buf)

    times = buf.add_floats([0.0, 1.0, 2.0], "SCALAR", 3, minmax=True)
    translations = buf.add_floats(
        [0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 2.0, 0.0, 0.0], "VEC3", 3
    )
    rotations = buf.add_floats(
        [0.0, 0.0, 0.0, 1.0, 0.0, 0.7071068, 0.0, 0.7071068, 0.0, 0.0, 0.0, -1.0],
        "VEC4",
        3,
    )
    scales = buf.add_floats([1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 0.5, 0.5, 0.5], "VEC3", 3)

    gltf = base_gltf(
        animations=[
            {
                "name": "move",
                "samplers": [
                    {"input": times, "output": translations, "interpolation": "LINEAR"}
                ],
                "channels": [
                    {"sampler": 0, "target": {"node": 0, "path": "translation"}}
                ],
            },
            {
                "name": "spin",
                "samplers": [
                    {"input": times, "output": rotations, "interpolation": "LINEAR"}
                ],
                "channels": [{"sampler": 0, "target": {"node": 0, "path": "rotation"}}],
            },
            {
                "name": "pulse",
                "samplers": [
                    {"input": times, "output": scales, "interpolation": "STEP"}
                ],
                "channels": [{"sampler": 0, "target": {"node": 0, "path": "scale"}}],
            },
        ],
        materials=[
            {
                "name": "blue",
                "pbrMetallicRoughness": {"baseColorFactor": [0.2, 0.3, 0.9, 1.0]},
            }
        ],
        meshes=[{"name": "cube", "primitives": [dict(prim, material=0)]}],
        nodes=[{"name": "animated", "mesh": 0}],
        scenes=[{"nodes": [0]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("animation.gltf", gltf)


def gen_skin():
    """A two joint skinned strip with an animation on the second joint. This is the
    single skeleton skinned character case from the documentation."""
    buf = Buffer()

    # A strip of 6 vertices along Y, bound to 2 joints
    positions = []
    joints = []
    weights = []
    for i in range(3):
        y = i * 0.5
        w = 0.0 if i == 0 else (0.5 if i == 1 else 1.0)
        for x in (-0.25, 0.25):
            positions.extend([x, y, 0.0])
            joints.extend([0, 1, 0, 0])
            weights.extend([1.0 - w, w, 0.0, 0.0])
    indices = [0, 1, 2, 1, 3, 2, 2, 3, 4, 3, 5, 4]

    pos = buf.add_floats(positions, "VEC3", 6, minmax=True)
    nrm = buf.add_floats([0.0, 0.0, 1.0] * 6, "VEC3", 6)
    joint_acc = buf.add_ushorts(joints, "VEC4", 6)
    weight_acc = buf.add_floats(weights, "VEC4", 6)
    idx = buf.add_indices(indices)

    # Inverse bind matrices (column major identity / translate -0.5 on Y)
    ibm = buf.add_floats(
        [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]
        + [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, -0.5, 0, 1],
        "MAT4",
        2,
    )

    times = buf.add_floats([0.0, 1.0], "SCALAR", 2, minmax=True)
    rotations = buf.add_floats(
        [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.3826834, 0.9238795], "VEC4", 2
    )

    gltf = base_gltf(
        materials=[
            {
                "name": "skin",
                "doubleSided": True,
                "pbrMetallicRoughness": {"baseColorFactor": [0.9, 0.7, 0.5, 1.0]},
            }
        ],
        meshes=[
            {
                "name": "strip",
                "primitives": [
                    {
                        "attributes": {
                            "POSITION": pos,
                            "NORMAL": nrm,
                            "JOINTS_0": joint_acc,
                            "WEIGHTS_0": weight_acc,
                        },
                        "indices": idx,
                        "material": 0,
                        "mode": MODE_TRIANGLES,
                    }
                ],
            }
        ],
        skins=[
            {
                "name": "skeleton",
                "joints": [1, 2],
                "inverseBindMatrices": ibm,
                "skeleton": 1,
            }
        ],
        nodes=[
            {"name": "skinned", "mesh": 0, "skin": 0},
            {"name": "root_joint", "children": [2]},
            {"name": "tip_joint", "translation": [0.0, 0.5, 0.0]},
        ],
        animations=[
            {
                "name": "bend",
                "samplers": [
                    {"input": times, "output": rotations, "interpolation": "LINEAR"}
                ],
                "channels": [{"sampler": 0, "target": {"node": 2, "path": "rotation"}}],
            }
        ],
        scenes=[{"nodes": [0, 1]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("skin.gltf", gltf)


def gen_lights():
    """KHR_lights_punctual: a directional, a point and a spot light, with the point
    light animated so the animated-light path is covered too."""
    buf = Buffer()
    prim = add_cube(buf)
    times = buf.add_floats([0.0, 1.0], "SCALAR", 2, minmax=True)
    translations = buf.add_floats([0.0, 1.0, 1.0, 0.0, -1.0, 1.0], "VEC3", 2)

    gltf = base_gltf(
        extensionsUsed=["KHR_lights_punctual"],
        extensions={
            "KHR_lights_punctual": {
                "lights": [
                    {
                        "name": "sun",
                        "type": "directional",
                        "color": [1.0, 1.0, 0.95],
                        "intensity": 3.0,
                    },
                    {
                        "name": "bulb",
                        "type": "point",
                        "color": [1.0, 0.8, 0.6],
                        "intensity": 20.0,
                        "range": 10.0,
                    },
                    {
                        "name": "spot",
                        "type": "spot",
                        "color": [0.6, 0.8, 1.0],
                        "intensity": 15.0,
                        "spot": {"innerConeAngle": 0.2, "outerConeAngle": 0.6},
                    },
                ]
            }
        },
        materials=[
            {
                "name": "lit",
                "pbrMetallicRoughness": {
                    "baseColorFactor": [0.8, 0.8, 0.8, 1.0],
                    "roughnessFactor": 0.4,
                },
            }
        ],
        meshes=[{"name": "cube", "primitives": [dict(prim, material=0)]}],
        nodes=[
            {"name": "cube", "mesh": 0},
            {
                "name": "sun",
                "extensions": {"KHR_lights_punctual": {"light": 0}},
                "rotation": [-0.3826834, 0.0, 0.0, 0.9238795],
            },
            {
                "name": "bulb",
                "extensions": {"KHR_lights_punctual": {"light": 1}},
                "translation": [0.0, 1.0, 1.0],
            },
            {
                "name": "spot",
                "extensions": {"KHR_lights_punctual": {"light": 2}},
                "translation": [1.0, 1.0, 1.0],
            },
        ],
        animations=[
            {
                "name": "light_move",
                "samplers": [
                    {"input": times, "output": translations, "interpolation": "LINEAR"}
                ],
                "channels": [
                    {"sampler": 0, "target": {"node": 2, "path": "translation"}}
                ],
            }
        ],
        scenes=[{"nodes": [0, 1, 2, 3]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("lights.gltf", gltf)


def gen_hierarchy():
    """A named node tree for the path lookup API plus unnamed nodes for numeric paths.

    /body
    /body/arm
    /body/arm/hand
    /cursor
    /rot_x
    /rot_y
    and one unnamed child of /body, reachable as a numeric path only.

    /cursor, /rot_x and /rot_y are each rotated around a single axis, which is what
    pins the axis order the Euler getter reports.
    """
    buf = Buffer()
    prim = add_triangle(buf, uv=False)
    gltf = base_gltf(
        materials=[
            {
                "name": "grey",
                "doubleSided": True,
                "pbrMetallicRoughness": {"baseColorFactor": [0.6, 0.6, 0.6, 1.0]},
            }
        ],
        meshes=[{"name": "tri", "primitives": [dict(prim, material=0)]}],
        nodes=[
            {
                "name": "body",
                "mesh": 0,
                "children": [1, 4],
                "translation": [1.0, 2.0, 3.0],
            },
            {
                "name": "arm",
                "mesh": 0,
                "children": [2],
                "translation": [0.5, 0.0, 0.0],
                "scale": [2.0, 2.0, 2.0],
            },
            {"name": "hand", "mesh": 0, "translation": [0.25, 0.0, 0.0]},
            {
                "name": "cursor",
                "mesh": 0,
                "translation": [0.0, 0.0, -1.0],
                # 45 degrees around Z
                "rotation": [0.0, 0.0, 0.3826834, 0.9238795],
            },
            {"mesh": 0, "translation": [0.0, 1.0, 0.0]},
            {
                "name": "rot_x",
                "mesh": 0,
                "translation": [3.0, 0.0, 0.0],
                # 30 degrees around X
                "rotation": [0.2588190, 0.0, 0.0, 0.9659258],
            },
            {
                "name": "rot_y",
                "mesh": 0,
                "translation": [4.0, 0.0, 0.0],
                # 60 degrees around Y
                "rotation": [0.0, 0.5, 0.0, 0.8660254],
            },
        ],
        scenes=[{"name": "main", "nodes": [0, 3, 5, 6]}],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("hierarchy.gltf", gltf)


def gen_multi_scene():
    """Two scenes, the second one holding more nodes than the default one."""
    buf = Buffer()
    prim = add_triangle(buf, uv=False)
    gltf = base_gltf(
        materials=[
            {
                "name": "a",
                "doubleSided": True,
                "pbrMetallicRoughness": {"baseColorFactor": [1.0, 0.5, 0.0, 1.0]},
            }
        ],
        meshes=[{"name": "tri", "primitives": [dict(prim, material=0)]}],
        nodes=[
            {"name": "first", "mesh": 0},
            {"name": "second", "mesh": 0, "translation": [2.0, 0.0, 0.0]},
            {"name": "third", "mesh": 0, "translation": [-2.0, 0.0, 0.0]},
        ],
        scenes=[
            {"name": "default_scene", "nodes": [0]},
            {"name": "other_scene", "nodes": [1, 2]},
        ],
        scene=0,
    )
    buf.finish(gltf)
    write_gltf("multi_scene.gltf", gltf)


def gen_environment():
    """A tiny equirectangular Radiance (.hdr) image for the IBL tests, plus the same
    gradient as a JPEG since both formats are supported."""
    width, height = 32, 16

    def color(x, y):
        u = x / (width - 1.0)
        v = y / (height - 1.0)
        return (0.2 + 2.0 * u, 0.3 + 1.5 * v, 1.2 - u * v)

    rows = []
    for y in range(height):
        row = bytearray()
        for x in range(width):
            r, g, b = color(x, y)
            top = max(r, g, b)
            if top < 1e-32:
                row.extend((0, 0, 0, 0))
                continue
            mantissa, exponent = 0.0, 0
            m = top
            while m >= 1.0:
                m /= 2.0
                exponent += 1
            while m < 0.5:
                m *= 2.0
                exponent -= 1
            mantissa = m * 256.0 / top
            row.extend(
                (
                    int(r * mantissa) & 0xFF,
                    int(g * mantissa) & 0xFF,
                    int(b * mantissa) & 0xFF,
                    (exponent + 128) & 0xFF,
                )
            )
        rows.append(bytes(row))

    header = (
        "#?RADIANCE\n"
        "# generated by lvgl gen_gltf_assets.py\n"
        "FORMAT=32-bit_rle_rgbe\n\n"
        "-Y %d +X %d\n" % (height, width)
    ).encode()
    path = os.path.join(OUT_DIR, "environment.hdr")
    with open(path, "wb") as f:
        f.write(header)
        for row in rows:
            f.write(row)
    print("wrote environment.hdr (%d bytes)" % os.path.getsize(path))

    def rgba(x, y):
        r, g, b = color(x, y)
        return (
            min(int(r * 128), 255),
            min(int(g * 128), 255),
            min(int(b * 128), 255),
            255,
        )

    path = os.path.join(OUT_DIR, "environment.jpg")
    with open(path, "wb") as f:
        f.write(pil_image_bytes(width, height, rgba, "JPEG", quality=90))
    print("wrote environment.jpg (%d bytes)" % os.path.getsize(path))


def gen_invalid():
    """Not a glTF file at all, for the failure paths of the loaders."""
    path = os.path.join(OUT_DIR, "invalid.gltf")
    with open(path, "w") as f:
        f.write('{"asset": {"version": "2.0"}, "meshes": [ this is not valid json\n')
    print("wrote invalid.gltf (%d bytes)" % os.path.getsize(path))


def main():
    if not os.path.exists(OUT_DIR):
        Path(OUT_DIR).mkdir(parents=True, exist_ok=True)

    gen_minimal_triangle()
    gen_pbr_textures()
    gen_compressed_textures()
    gen_materials()
    gen_sheen()
    gen_cameras()
    gen_animation()
    gen_skin()
    gen_lights()
    gen_hierarchy()
    gen_multi_scene()
    gen_environment()
    gen_invalid()


if __name__ == "__main__":
    main()
