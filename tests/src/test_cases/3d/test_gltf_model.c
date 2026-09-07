#if LV_BUILD_TEST
#include "../../lvgl.h"

#if LV_USE_GLTF

#include "unity/unity.h"

#include <stdio.h>

#define ASSET(name) "A:src/test_assets/gltf/generated/" name
#define RAW_ASSET_PATH(name) "src/test_assets/gltf/generated/" name

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static lv_gltf_model_t * load(const char * path)
{
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(path, NULL);
    TEST_ASSERT_NOT_NULL(model);
    return model;
}

/* Reads a whole asset into memory so that the byte array loader can be tested */
static uint8_t * read_asset(const char * path, size_t * size_out)
{
    FILE * f = fopen(path, "rb");
    TEST_ASSERT_NOT_NULL(f);
    TEST_ASSERT_EQUAL(0, fseek(f, 0, SEEK_END));
    long size = ftell(f);
    TEST_ASSERT_GREATER_THAN(0, size);
    rewind(f);

    uint8_t * buf = lv_malloc((size_t)size);
    TEST_ASSERT_NOT_NULL(buf);
    TEST_ASSERT_EQUAL((size_t)size, fread(buf, 1, (size_t)size, f));
    fclose(f);

    *size_out = (size_t)size;
    return buf;
}

void test_gltf_model_load_from_file(void)
{
    lv_gltf_model_t * model = load(ASSET("minimal_triangle.gltf"));

    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_mesh_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_material_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_node_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_scene_count(model));
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_image_count(model));
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_texture_count(model));
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_camera_count(model));
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_animation_count(model));

    lv_gltf_model_delete(model);
}

/* The .glb holds the same model as the .gltf, only in the binary container */
void test_gltf_model_load_glb_matches_gltf(void)
{
    lv_gltf_model_t * gltf = load(ASSET("minimal_triangle.gltf"));
    lv_gltf_model_t * glb = load(ASSET("minimal_triangle.glb"));

    TEST_ASSERT_EQUAL(lv_gltf_model_get_mesh_count(gltf), lv_gltf_model_get_mesh_count(glb));
    TEST_ASSERT_EQUAL(lv_gltf_model_get_material_count(gltf), lv_gltf_model_get_material_count(glb));
    TEST_ASSERT_EQUAL(lv_gltf_model_get_node_count(gltf), lv_gltf_model_get_node_count(glb));
    TEST_ASSERT_EQUAL(lv_gltf_model_get_scene_count(gltf), lv_gltf_model_get_scene_count(glb));

    lv_gltf_model_delete(glb);
    lv_gltf_model_delete(gltf);
}

void test_gltf_model_load_from_bytes(void)
{
    size_t size = 0;
    uint8_t * bytes = read_asset(RAW_ASSET_PATH("minimal_triangle.glb"), &size);

    lv_gltf_model_t * model = lv_gltf_data_load_from_bytes(bytes, size, NULL);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_mesh_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_node_count(model));

    lv_gltf_model_delete(model);
    lv_free(bytes);
}

void test_gltf_model_load_json_from_bytes(void)
{
    size_t size = 0;
    uint8_t * bytes = read_asset(RAW_ASSET_PATH("minimal_triangle.gltf"), &size);

    lv_gltf_model_t * model = lv_gltf_data_load_from_bytes(bytes, size, NULL);
    TEST_ASSERT_NOT_NULL(model);
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_material_count(model));

    lv_gltf_model_delete(model);
    lv_free(bytes);
}

void test_gltf_model_load_missing_file_fails(void)
{
    TEST_ASSERT_NULL(lv_gltf_data_load_from_file(ASSET("this_file_does_not_exist.gltf"), NULL));
}

void test_gltf_model_load_invalid_content_fails(void)
{
    TEST_ASSERT_NULL(lv_gltf_data_load_from_file(ASSET("invalid.gltf"), NULL));

    const uint8_t garbage[] = { 'n', 'o', 't', ' ', 'g', 'l', 't', 'f' };
    TEST_ASSERT_NULL(lv_gltf_data_load_from_bytes(garbage, sizeof(garbage), NULL));
}

/* The documented texture set: one image and one texture per map */
void test_gltf_model_counts_pbr_textures(void)
{
    lv_gltf_model_t * model = load(ASSET("pbr_textures.gltf"));

    TEST_ASSERT_EQUAL(5, lv_gltf_model_get_image_count(model));
    TEST_ASSERT_EQUAL(5, lv_gltf_model_get_texture_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_material_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_mesh_count(model));

    lv_gltf_model_delete(model);
}

/* JPEG and WebP textures, the WebP one through EXT_texture_webp */
void test_gltf_model_counts_compressed_textures(void)
{
    lv_gltf_model_t * model = load(ASSET("compressed_textures.gltf"));

    TEST_ASSERT_EQUAL(2, lv_gltf_model_get_image_count(model));
    TEST_ASSERT_EQUAL(2, lv_gltf_model_get_texture_count(model));
    TEST_ASSERT_EQUAL(2, lv_gltf_model_get_material_count(model));

    lv_gltf_model_delete(model);
}

/* One material per documented material feature */
void test_gltf_model_counts_materials(void)
{
    lv_gltf_model_t * model = load(ASSET("materials.gltf"));

    TEST_ASSERT_EQUAL(8, lv_gltf_model_get_material_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_mesh_count(model));
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_texture_count(model));

    lv_gltf_model_delete(model);
}

void test_gltf_model_counts_cameras(void)
{
    lv_gltf_model_t * model = load(ASSET("cameras.gltf"));

    TEST_ASSERT_EQUAL(2, lv_gltf_model_get_camera_count(model));
    TEST_ASSERT_EQUAL(3, lv_gltf_model_get_node_count(model));

    lv_gltf_model_delete(model);
}

void test_gltf_model_counts_scenes(void)
{
    lv_gltf_model_t * model = load(ASSET("multi_scene.gltf"));

    TEST_ASSERT_EQUAL(2, lv_gltf_model_get_scene_count(model));
    TEST_ASSERT_EQUAL(3, lv_gltf_model_get_node_count(model));

    lv_gltf_model_delete(model);
}

void test_gltf_model_counts_lights_asset(void)
{
    lv_gltf_model_t * model = load(ASSET("lights.gltf"));

    /* The cube plus the three light nodes */
    TEST_ASSERT_EQUAL(4, lv_gltf_model_get_node_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_animation_count(model));

    lv_gltf_model_delete(model);
}

void test_gltf_model_counts_skin_asset(void)
{
    lv_gltf_model_t * model = load(ASSET("skin.gltf"));

    /* The skinned mesh node plus the two joints */
    TEST_ASSERT_EQUAL(3, lv_gltf_model_get_node_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_animation_count(model));
    TEST_ASSERT_EQUAL(1, lv_gltf_model_get_mesh_count(model));

    lv_gltf_model_delete(model);
}

/* The count getters assert on a NULL model, but the functions that document a NULL
 * check must tolerate it. */
void test_gltf_model_null_tolerant_functions(void)
{
    lv_gltf_model_delete(NULL);
    lv_gltf_model_set_animation_speed(NULL, LV_GLTF_ANIM_SPEED_2X);
    TEST_ASSERT_EQUAL(0, lv_gltf_model_get_animation_speed(NULL));
    lv_gltf_model_loader_delete(NULL);
}

/* A loader can be reused for several models. Its texture cache is what makes shared
 * images load once, so models loaded through the same loader stay independent. */
void test_gltf_model_shared_loader(void)
{
    lv_gltf_model_loader_t * loader = lv_gltf_model_loader_create();
    TEST_ASSERT_NOT_NULL(loader);

    lv_gltf_model_t * first = lv_gltf_data_load_from_file(ASSET("pbr_textures.gltf"), loader);
    lv_gltf_model_t * second = lv_gltf_data_load_from_file(ASSET("pbr_textures.gltf"), loader);
    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_NOT_NULL(second);
    TEST_ASSERT_NOT_EQUAL(first, second);

    TEST_ASSERT_EQUAL(lv_gltf_model_get_texture_count(first), lv_gltf_model_get_texture_count(second));

    lv_gltf_model_delete(second);
    lv_gltf_model_delete(first);
    lv_gltf_model_loader_delete(loader);
}

void test_gltf_model_loader_texture_store(void)
{
    lv_gltf_model_loader_t * loader = lv_gltf_model_loader_create();
    TEST_ASSERT_NOT_NULL(loader);

    /* An unknown hash reports "not found" as 0 */
    TEST_ASSERT_EQUAL(0, lv_gltf_model_loader_get_texture(loader, 0x1234u));

    lv_gltf_model_loader_store_texture(loader, 0x1234u, 42u);
    TEST_ASSERT_EQUAL(42u, lv_gltf_model_loader_get_texture(loader, 0x1234u));

    /* Storing another hash does not disturb the first one */
    lv_gltf_model_loader_store_texture(loader, 0x5678u, 43u);
    TEST_ASSERT_EQUAL(42u, lv_gltf_model_loader_get_texture(loader, 0x1234u));
    TEST_ASSERT_EQUAL(43u, lv_gltf_model_loader_get_texture(loader, 0x5678u));

    lv_gltf_model_loader_delete(loader);
}

/* Loading every asset in a row must leave nothing behind */
void test_gltf_model_load_and_delete_all_assets(void)
{
    const char * assets[] = {
        ASSET("minimal_triangle.gltf"),
        ASSET("minimal_triangle.glb"),
        ASSET("pbr_textures.gltf"),
        ASSET("compressed_textures.gltf"),
        ASSET("materials.gltf"),
        ASSET("cameras.gltf"),
        ASSET("animation.gltf"),
        ASSET("skin.gltf"),
        ASSET("lights.gltf"),
        ASSET("hierarchy.gltf"),
        ASSET("multi_scene.gltf"),
    };

    for(uint32_t i = 0; i < sizeof(assets) / sizeof(assets[0]); i++) {
        lv_gltf_model_t * model = lv_gltf_data_load_from_file(assets[i], NULL);
        TEST_ASSERT_NOT_NULL(model);
        lv_gltf_model_delete(model);
    }
}

#else /*LV_USE_GLTF*/

void setUp(void)
{
}

void tearDown(void)
{
}

void test_gltf_model_load_from_file(void)
{
}

void test_gltf_model_load_glb_matches_gltf(void)
{
}

void test_gltf_model_load_from_bytes(void)
{
}

void test_gltf_model_load_json_from_bytes(void)
{
}

void test_gltf_model_load_missing_file_fails(void)
{
}

void test_gltf_model_load_invalid_content_fails(void)
{
}

void test_gltf_model_counts_pbr_textures(void)
{
}

void test_gltf_model_counts_compressed_textures(void)
{
}

void test_gltf_model_counts_materials(void)
{
}

void test_gltf_model_counts_cameras(void)
{
}

void test_gltf_model_counts_scenes(void)
{
}

void test_gltf_model_counts_lights_asset(void)
{
}

void test_gltf_model_counts_skin_asset(void)
{
}

void test_gltf_model_null_tolerant_functions(void)
{
}

void test_gltf_model_shared_loader(void)
{
}

void test_gltf_model_loader_texture_store(void)
{
}

void test_gltf_model_load_and_delete_all_assets(void)
{
}

#endif /*LV_USE_GLTF*/

#endif /*LV_BUILD_TEST*/
