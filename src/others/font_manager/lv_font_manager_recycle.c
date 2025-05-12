/**
 * @file lv_font_manager_recycle.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_manager_recycle.h"

#if LV_USE_FONT_MANAGER

#include "../../font/lv_font.h"
#include "../../misc/lv_ll.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_font_manager_recycle_t {
    lv_ll_t recycle_ll;
    uint32_t max_size;
};

typedef struct {
    lv_font_info_t ft_info;
    char name[LV_FONT_MANAGER_NAME_MAX_LEN];
    lv_font_t * font;
} lv_font_recycle_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_font_recycle_close(lv_font_manager_recycle_t * manager, lv_font_recycle_t * recycle);
static void lv_font_manager_recycle_remove_tail(lv_font_manager_recycle_t * manager);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_font_manager_recycle_t * lv_font_manager_recycle_create(uint32_t max_size)
{
    lv_font_manager_recycle_t * manager = lv_malloc_zeroed(sizeof(lv_font_manager_recycle_t));
    LV_ASSERT_MALLOC(manager);
    if(!manager) {
        LV_LOG_ERROR("malloc failed for lv_font_manager_recycle_t");
        return NULL;
    }

    lv_ll_init(&manager->recycle_ll, sizeof(lv_font_recycle_t));
    manager->max_size = max_size;

    LV_LOG_INFO("success");
    return manager;
}

void lv_font_manager_recycle_delete(lv_font_manager_recycle_t * manager)
{
    LV_ASSERT_NULL(manager);

    lv_ll_t * recycle_ll = &manager->recycle_ll;

    lv_font_recycle_t * recycle = lv_ll_get_head(recycle_ll);

    /* clear all recycle */
    while(recycle != NULL) {
        lv_font_recycle_t * recycle_next = lv_ll_get_next(recycle_ll, recycle);
        lv_font_recycle_close(manager, recycle);
        recycle = recycle_next;
    }

    lv_free(manager);

    LV_LOG_INFO("success");
}

lv_font_t * lv_font_manager_recycle_get_reuse(lv_font_manager_recycle_t * manager, const lv_font_info_t * ft_info)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(ft_info);

    lv_ll_t * recycle_ll = &manager->recycle_ll;

    LV_LOG_INFO("font: %s(%d) searching...", ft_info->name, ft_info->size);

    lv_font_recycle_t * recycle;
    LV_LL_READ(recycle_ll, recycle) {
        /* match font */
        if(lv_font_info_is_equal(ft_info, &recycle->ft_info)) {
            lv_font_t * font = recycle->font;
            LV_LOG_INFO("found font: %p", (void *)font);

            /* remove reused font */
            lv_ll_remove(recycle_ll, recycle);
            lv_free(recycle);
            return font;
        }
    }

    LV_LOG_INFO("NOT found");

    return false;
}

void lv_font_manager_recycle_set_reuse(lv_font_manager_recycle_t * manager, lv_font_t * font,
                                       const lv_font_info_t * ft_info)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(ft_info);

    lv_ll_t * recycle_ll = &manager->recycle_ll;

    /* check recycled size */
    if(lv_ll_get_len(recycle_ll) >= manager->max_size) {
        LV_LOG_INFO("recycle full, remove tail font...");
        lv_font_manager_recycle_remove_tail(manager);
    }

    /* record reuse font */
    lv_font_recycle_t * recycle = lv_ll_ins_head(recycle_ll);
    LV_ASSERT_MALLOC(recycle);
    lv_memzero(recycle, sizeof(lv_font_recycle_t));

    lv_strncpy(recycle->name, ft_info->name, sizeof(recycle->name));
    recycle->name[sizeof(recycle->name) - 1] = '\0';

    recycle->font = font;
    recycle->ft_info = *ft_info;
    recycle->ft_info.name = recycle->name;

    LV_LOG_INFO("insert font: %s(%d) to reuse list", ft_info->name, ft_info->size);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_font_recycle_close(lv_font_manager_recycle_t * manager, lv_font_recycle_t * recycle)
{
    LV_ASSERT_NULL(manager);
    LV_ASSERT_NULL(recycle);

    LV_LOG_INFO("font: %s(%d) close", recycle->ft_info.name, recycle->ft_info.size);
    recycle->ft_info.class_p->delete_cb(recycle->font);
    recycle->font = NULL;

    lv_ll_remove(&manager->recycle_ll, recycle);
    lv_free(recycle);
}

static void lv_font_manager_recycle_remove_tail(lv_font_manager_recycle_t * manager)
{
    lv_font_recycle_t * tail = lv_ll_get_tail(&manager->recycle_ll);
    LV_ASSERT_NULL(tail);
    lv_font_recycle_close(manager, tail);
}

#endif /* LV_USE_FONT_MANAGER */
