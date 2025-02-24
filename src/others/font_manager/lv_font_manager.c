/**
 * @file lv_font_manager.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_manager.h"

#if LV_USE_FONT_MANAGER

#include "lv_font_manager_recycle.h"
#include "lv_font_manager_utils.h"
#include "../../lvgl.h"

/*********************
 *      DEFINES
 *********************/

#define IS_FONT_FAMILY_NAME(name) (lv_strchr((name), ',') != NULL)
#define IS_FONT_HAS_FALLBACK(font) ((font)->fallback != NULL)

/**********************
 *      TYPEDEFS
 **********************/

/* font manager object */
struct _lv_font_manager_t {
    lv_ll_t refer_ll; /* freetype font record list */
    lv_ll_t rec_ll; /* lvgl font record list */
    lv_ll_t path_ll; /* font path record list */
    lv_font_manager_recycle_t * recycle_manager;
};

/* freetype font reference node */
typedef struct _lv_font_refer_node_t {
    lv_font_t * font_p; /* lv_freetype gen font */
    lv_freetype_info_t ft_info; /* freetype font info */
    char name[LV_FONT_MANAGER_NAME_MAX_LEN]; /* name buffer */
    int ref_cnt; /* reference count */
} lv_font_refer_node_t;

/* lvgl font record node */
typedef struct _lv_font_rec_node_t {
    lv_font_t font; /* lvgl font info */
    const lv_font_refer_node_t * refer_node_p; /* referenced freetype resource */
} lv_font_rec_node_t;

typedef struct _lv_font_path_t {
    char * name;
    char * path;
    bool is_static;
} lv_font_path_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static const char * lv_font_manager_get_path(lv_font_manager_t * manager, const char * name);

static bool lv_font_manager_check_resource(lv_font_manager_t * manager);
static lv_font_rec_node_t * lv_font_manager_search_rec_node(lv_font_manager_t * manager, lv_font_t * font);

static const lv_font_refer_node_t * lv_font_manager_get_freetype_font(lv_font_manager_t * manager,
                                                                      const lv_freetype_info_t * ft_info);
static bool lv_font_manager_reset_freetype_font(lv_font_manager_t * manager, const lv_font_refer_node_t * node);

static lv_font_t * lv_font_manager_create_font_single(lv_font_manager_t * manager, const lv_freetype_info_t * ft_info);
static bool lv_font_manager_delete_font_single(lv_font_manager_t * manager, lv_font_t * font);

static lv_font_t * lv_font_manager_create_font_family(lv_font_manager_t * manager, const lv_freetype_info_t * ft_info);
static void lv_font_manager_delete_font_family(lv_font_manager_t * manager, lv_font_t * font);

static void lv_font_manager_add_path_core(lv_font_manager_t * manager, const char * name, const char * path,
                                          bool is_static);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_font_manager_t * lv_font_manager_create(uint32_t recycle_cache_size)
{
    LV_ASSERT_MSG(recycle_cache_size > 0, "recycle_cache_size should be greater than 0");
    lv_font_manager_t * manager = lv_malloc_zeroed(sizeof(lv_font_manager_t));
    LV_ASSERT_MALLOC(manager);
    if(!manager) {
        LV_LOG_ERROR("malloc failed for lv_font_manager_t");
        return NULL;
    }

    lv_ll_init(&manager->refer_ll, sizeof(lv_font_refer_node_t));
    lv_ll_init(&manager->rec_ll, sizeof(lv_font_rec_node_t));
    lv_ll_init(&manager->path_ll, sizeof(lv_font_path_t));

    manager->recycle_manager = lv_font_manager_recycle_create(recycle_cache_size);

    LV_LOG_INFO("success");
    return manager;
}

bool lv_font_manager_delete(lv_font_manager_t * manager)
{
    LV_ASSERT_NULL(manager);

    /* Resource leak check */
    if(lv_font_manager_check_resource(manager)) {
        LV_LOG_ERROR("unfreed resource detected, delete failed!");
        return false;
    }

    /* clean recycle_manager */
    lv_font_manager_recycle_delete(manager->recycle_manager);

    /* clean path map */
    lv_font_path_t * font_path;
    LV_LL_READ(&manager->path_ll, font_path) {
        if(!font_path->is_static) {
            lv_free(font_path->name);
            lv_free(font_path->path);
        }

        font_path->name = NULL;
        font_path->path = NULL;
    }
    lv_ll_clear(&manager->path_ll);

    lv_free(manager);

    LV_LOG_INFO("success");
    return true;
}

void lv_font_manager_add_path(lv_font_manager_t * manager, const char * name, const char * path)
{
    lv_font_manager_add_path_core(manager, name, path, false);
}

void lv_font_manager_add_path_static(lv_font_manager_t * manager, const char * name, const char * path)
{
    lv_font_manager_add_path_core(manager, name, path, true);
}

bool lv_font_manager_remove_path(lv_font_manager_t * manager, const char * name)
{
    lv_font_path_t * font_path;
    LV_LL_READ(&manager->path_ll, font_path) {
        if(lv_strcmp(name, font_path->name) == 0) {
            break;
        }
    }

    if(!font_path) {
        LV_LOG_WARN("name: %s not found", name);
        return false;
    }

    lv_ll_remove(&manager->path_ll, font_path);

    if(!font_path->is_static) {
        lv_free(font_path->name);
        lv_free(font_path->path);
    }

    font_path->name = NULL;
    font_path->path = NULL;

    lv_free(font_path);

    LV_LOG_WARN("name: %s remove success", name);
    return true;
}

lv_font_t * lv_font_manager_create_font(lv_font_manager_t * manager, const char * font_family, uint16_t render_mode,
                                        uint32_t size, uint16_t style)
{

    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(font_family);

    lv_freetype_info_t ft_info;
    lv_memzero(&ft_info, sizeof(ft_info));
    ft_info.name = font_family;
    ft_info.render_mode = render_mode;
    ft_info.size = size;
    ft_info.style = style;

    lv_font_t * ret_font;

    if(IS_FONT_FAMILY_NAME(ft_info.name)) {
        ret_font = lv_font_manager_create_font_family(manager, &ft_info);
    }
    else {
        ret_font = lv_font_manager_create_font_single(manager, &ft_info);
    }

    /* Append fallback font to make LV_SYMBOL displayable */
    lv_font_t * cur_font = ret_font;
    while(cur_font) {
        if(cur_font->fallback == NULL) {
            cur_font->fallback = LV_FONT_DEFAULT;
            break;
        }
        cur_font = (lv_font_t *)cur_font->fallback;
    }

    return ret_font;
}

void lv_font_manager_delete_font(lv_font_manager_t * manager, lv_font_t * font)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(font);

    if(IS_FONT_HAS_FALLBACK(font)) {
        lv_font_manager_delete_font_family(manager, font);
        return;
    }

    lv_font_manager_delete_font_single(manager, font);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_font_t * lv_font_manager_create_font_single(lv_font_manager_t * manager, const lv_freetype_info_t * ft_info)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(ft_info);

    /* get freetype font */
    const lv_font_refer_node_t * refer_node = lv_font_manager_get_freetype_font(manager, ft_info);
    if(!refer_node) {
        return NULL;
    }

    /* add font record node */
    lv_font_rec_node_t * rec_node = lv_ll_ins_head(&manager->rec_ll);
    LV_ASSERT_MALLOC(rec_node);
    lv_memzero(rec_node, sizeof(lv_font_rec_node_t));

    /* copy freetype_font data */
    rec_node->font = *refer_node->font_p;

    /* record reference freetype_font */
    rec_node->refer_node_p = refer_node;

    LV_LOG_INFO("success");
    return &rec_node->font;
}

static bool lv_font_manager_delete_font_single(lv_font_manager_t * manager, lv_font_t * font)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(font);

    if(font == LV_FONT_DEFAULT) {
        LV_LOG_INFO("LV_FONT_DEFAULT can not be deleted");
        return false;
    }

    /* check font is created by font manager */
    lv_font_rec_node_t * rec_node = lv_font_manager_search_rec_node(manager, font);
    if(!rec_node) {
        LV_LOG_WARN("NO record found for font: %p(%d),"
                    " it was not created by font manager",
                    (void *)font, (int)font->line_height);
        return false;
    }

    /* reset freetype font resource */
    bool retval = lv_font_manager_reset_freetype_font(manager, rec_node->refer_node_p);
    LV_ASSERT(retval);

    /* free rec_node */
    lv_ll_remove(&manager->rec_ll, rec_node);
    lv_free(rec_node);

    LV_LOG_INFO("success");
    return retval;
}

static const char * strncpy_until(char * dest, const char * src, size_t n, char c)
{
    LV_ASSERT_NULL(dest);
    LV_ASSERT_NULL(src);

    size_t i = 0;
    while(i < n && *src != '\0' && *src != c) {
        *dest++ = *src++;
        i++;
    }

    if(i < n) {
        *dest = '\0';
    }

    return src;
}

static lv_font_t * lv_font_manager_create_font_family(lv_font_manager_t * manager, const lv_freetype_info_t * ft_info)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(ft_info);

    lv_font_t * first_font = NULL;
    lv_font_t * pre_font = NULL;

    const char * family_str = ft_info->name;
    LV_LOG_INFO("font-family: %s", family_str);

    char tmp_name[LV_FONT_MANAGER_NAME_MAX_LEN] = { 0 };
    lv_freetype_info_t tmp_ft_info = *ft_info;
    tmp_ft_info.name = tmp_name;

    while(1) {
        family_str = strncpy_until(tmp_name, family_str, sizeof(tmp_name) - 1, ',');

        lv_font_t * cur_font = lv_font_manager_create_font_single(manager, &tmp_ft_info);

        if(cur_font) {
            /* save first font pointer */
            if(!first_font) {
                first_font = cur_font;
            }

            /* append font fallback */
            if(pre_font) {
                pre_font->fallback = cur_font;
            }

            pre_font = cur_font;
        }

        /* stop */
        if(*family_str == '\0') {
            break;
        }

        if(*family_str != ',') {
            LV_LOG_ERROR("font name buffer is too small, please increase FONT_NAME_MAX");
            break;
        }

        /* skip ',' */
        family_str++;
    }

    return first_font;
}

static void lv_font_manager_delete_font_family(lv_font_manager_t * manager, lv_font_t * font)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(font);

    lv_font_t * f = font;
    while(f) {
        lv_font_t * fallback = (lv_font_t *)f->fallback;
        lv_font_manager_delete_font_single(manager, f);
        f = fallback;
    }
}

static void lv_font_manager_add_path_core(lv_font_manager_t * manager, const char * name, const char * path,
                                          bool is_static)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(name);
    LV_ASSERT_NULL(path);

    const char * old_path = lv_font_manager_get_path(manager, name);
    if(old_path) {
        LV_LOG_WARN("name: %s, path: %s already exists", name, old_path);
        return;
    }

    lv_font_path_t * font_path = lv_ll_ins_tail(&manager->path_ll);
    LV_ASSERT_MALLOC(font_path);
    font_path->is_static = is_static;

    if(is_static) {
        font_path->name = (char *)name;
        font_path->path = (char *)path;
    }
    else {
        font_path->name = lv_strdup(name);
        LV_ASSERT_MALLOC(font_path->name);

        font_path->path = lv_strdup(path);
        LV_ASSERT_MALLOC(font_path->path);
    }

    LV_LOG_INFO("name: %s, path: %s add success", name, path);
}

static const char * lv_font_manager_get_path(lv_font_manager_t * manager, const char * name)
{
    lv_font_path_t * font_path;
    LV_LL_READ(&manager->path_ll, font_path) {
        if(lv_strcmp(name, font_path->name) == 0) {
            return font_path->path;
        }
    }

    return NULL;
}

static bool lv_font_manager_check_resource(lv_font_manager_t * manager)
{
    LV_ASSERT_NULL(manager);

    /* Check the recorded font */
    lv_ll_t * rec_ll = &manager->rec_ll;
    uint32_t rec_ll_len = lv_ll_get_len(rec_ll);
    if(rec_ll_len) {
        LV_LOG_WARN("lvgl font resource[%" LV_PRIu32 "]:", rec_ll_len);

        lv_font_rec_node_t * node;
        LV_LL_READ(rec_ll, node) {
            LV_LOG_WARN("font: %p(%d) -> ref: %s(%d)",
                        (void *)node,
                        (int)node->font.line_height,
                        node->refer_node_p->ft_info.name,
                        node->refer_node_p->ft_info.size);
        }
    }

    /* Check the recorded font resources created by freetype */
    lv_ll_t * refer_ll = &manager->refer_ll;
    uint32_t refer_ll_len = lv_ll_get_len(refer_ll);
    if(refer_ll_len) {
        LV_LOG_WARN("freetype font resource[%" LV_PRIu32 "]:", refer_ll_len);

        lv_font_refer_node_t * node;
        LV_LL_READ(refer_ll, node) {
            LV_LOG_WARN("font: %s(%d), ref_cnt = %d",
                        node->ft_info.name,
                        node->ft_info.size,
                        node->ref_cnt);
        }
    }

    /* Check resource leak */
    bool has_resource = (rec_ll_len || refer_ll_len);

    return has_resource;
}

static lv_font_rec_node_t * lv_font_manager_search_rec_node(lv_font_manager_t * manager, lv_font_t * font)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(font);

    lv_font_rec_node_t * rec_node;
    LV_LL_READ(&manager->rec_ll, rec_node) {
        if(font == &rec_node->font) {
            LV_LOG_INFO("font: %p(%d) matched", (void *)font, (int)font->line_height);
            return rec_node;
        }
    }

    return NULL;
}

static lv_font_refer_node_t * lv_font_manager_search_refer_node(lv_font_manager_t * manager,
                                                                const lv_freetype_info_t * ft_info)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(ft_info);

    lv_font_refer_node_t * refer_node;
    LV_LL_READ(&manager->refer_ll, refer_node) {
        if(lv_freetype_info_is_equal(ft_info, &refer_node->ft_info)) {
            LV_LOG_INFO("font: %s(%d) matched", ft_info->name, ft_info->size);
            return refer_node;
        }
    }

    return NULL;
}

static lv_font_t * lv_font_manager_create_font_warpper(lv_font_manager_t * manager, const lv_freetype_info_t * ft_info)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(ft_info);

    lv_font_t * font;

    /* create freetype font */
    font = lv_font_manager_recycle_get_reuse(manager->recycle_manager, ft_info);

    /* get reuse font from recycle */
    if(font) {
        return font;
    }

    /* cache miss */

    /* generate full file path */
    const char * path = lv_font_manager_get_path(manager, ft_info->name);
    if(!path) {
        LV_LOG_ERROR("name: %s not found path", ft_info->name);
        return NULL;
    }

    font = lv_freetype_font_create(path, ft_info->render_mode, ft_info->size, ft_info->style);
    if(!font) {
        LV_LOG_ERROR("Freetype font init failed, name: %s, render_mode: %d, size: %d, style: %d",
                     ft_info->name, ft_info->render_mode, ft_info->size, ft_info->style);
        return NULL;
    }

    return font;
}

static void lv_font_manager_delete_font_warpper(lv_font_manager_t * manager, lv_font_refer_node_t * refer_node)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(refer_node);
    lv_font_manager_recycle_set_reuse(manager->recycle_manager, refer_node->font_p, &refer_node->ft_info);
}

static const lv_font_refer_node_t * lv_font_manager_get_freetype_font(lv_font_manager_t * manager,
                                                                      const lv_freetype_info_t * ft_info)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(ft_info);

    /* check refer_node is existed */
    lv_font_refer_node_t * refer_node = lv_font_manager_search_refer_node(manager, ft_info);
    if(refer_node) {
        refer_node->ref_cnt++;
        LV_LOG_INFO("refer_node existed, ref_cnt++ = %d", refer_node->ref_cnt);
        return refer_node;
    }

    /* not found refer_node, start to create font */

    lv_font_t * font = lv_font_manager_create_font_warpper(manager, ft_info);

    if(!font) {
        return NULL;
    }

    /* add refer_node to refer_ll */
    refer_node = lv_ll_ins_head(&manager->refer_ll);
    LV_ASSERT_MALLOC(refer_node);
    lv_memzero(refer_node, sizeof(lv_font_refer_node_t));

    lv_strncpy(refer_node->name, ft_info->name, sizeof(refer_node->name) - 1);

    /* copy font data */
    refer_node->font_p = font;
    refer_node->ft_info = *ft_info;
    refer_node->ft_info.name = refer_node->name;
    refer_node->ref_cnt = 1;

    LV_LOG_INFO("success");
    return refer_node;
}

static bool lv_font_manager_reset_freetype_font(lv_font_manager_t * manager, const lv_font_refer_node_t * node)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(node);

    /* Check refer_node is existed */
    lv_font_refer_node_t * refer_node = lv_font_manager_search_refer_node(manager, &node->ft_info);
    if(!refer_node) {
        LV_LOG_WARN("NO record found for font: %s(%d),"
                    " it was not created by font manager",
                    node->ft_info.name, node->ft_info.size);
        return false;
    }

    refer_node->ref_cnt--;

    /* If ref_cnt is > 0, no need to delete font */
    if(refer_node->ref_cnt > 0) {
        LV_LOG_INFO("refer_node existed, ref_cnt-- = %d", refer_node->ref_cnt);
        return true;
    }

    /* if ref_cnt is about to be 0, free font resource */
    lv_font_manager_delete_font_warpper(manager, refer_node);

    /* free refer_node */
    lv_ll_remove(&manager->refer_ll, refer_node);
    lv_free(refer_node);

    LV_LOG_INFO("success");
    return true;
}

#endif /* LV_USE_FONT_MANAGER */
