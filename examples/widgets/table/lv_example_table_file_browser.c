/**
 * @file lv_example_table_file_browser.c
 */

#include "../../lv_examples.h"

#if LV_USE_TABLE && (LV_USE_FS_STDIO || LV_USE_FS_POSIX || LV_USE_FS_WIN32 || LV_USE_FS_FATFS) && LV_BUILD_EXAMPLES

#include <stdlib.h>
#include <string.h>

/*A file explorer is a quick-access sidebar plus a path header and a table of
 *directory entries read with the `lv_fs` API. The static helpers below build one on
 *top of base widgets, styled by hand to give it a flat look.*/

#if LV_USE_FS_WIN32
    #define BROWSER_ROOT "C:C:/"
#else
    #define BROWSER_ROOT "A:/"
#endif

#define COLOR_BG    lv_color_hex(0xf2f1f6)   /*sidebar + window background*/
#define COLOR_PANEL lv_color_white()         /*browser area*/

static lv_obj_t * quick_access;
static lv_obj_t * browser_path_label;
static lv_obj_t * browser_table;
static char browser_path[LV_FS_MAX_PATH_LENGTH];

static lv_style_t style_bg;       /*window + sidebar background, fully flat*/
static lv_style_t style_flat;     /*strip a base object's card (pad/border/radius)*/
static lv_style_t style_panel;    /*white browser area*/
static lv_style_t style_sidebar;
static lv_style_t style_qa_header;
static lv_style_t style_qa_btn;
static lv_style_t style_header;
static lv_style_t style_table;
static lv_style_t style_pressed;

static void styles_init(void)
{
    lv_style_init(&style_bg);
    lv_style_set_bg_opa(&style_bg, LV_OPA_COVER);
    lv_style_set_bg_color(&style_bg, COLOR_BG);
    lv_style_set_pad_all(&style_bg, 0);
    lv_style_set_pad_gap(&style_bg, 0);
    lv_style_set_radius(&style_bg, 0);
    lv_style_set_border_width(&style_bg, 0);

    lv_style_init(&style_flat);
    lv_style_set_bg_opa(&style_flat, LV_OPA_TRANSP);
    lv_style_set_pad_all(&style_flat, 0);
    lv_style_set_pad_gap(&style_flat, 0);
    lv_style_set_radius(&style_flat, 0);
    lv_style_set_border_width(&style_flat, 0);

    lv_style_init(&style_panel);
    lv_style_set_bg_opa(&style_panel, LV_OPA_COVER);
    lv_style_set_bg_color(&style_panel, COLOR_PANEL);
    lv_style_set_pad_all(&style_panel, 0);
    lv_style_set_pad_gap(&style_panel, 0);
    lv_style_set_radius(&style_panel, 0);
    lv_style_set_border_width(&style_panel, 0);

    /*Sidebar: grey background with a thin divider on its right edge*/
    lv_style_init(&style_sidebar);
    lv_style_set_bg_opa(&style_sidebar, LV_OPA_COVER);
    lv_style_set_bg_color(&style_sidebar, COLOR_BG);
    lv_style_set_pad_all(&style_sidebar, 6);
    lv_style_set_pad_row(&style_sidebar, 4);
    lv_style_set_radius(&style_sidebar, 0);
    lv_style_set_border_width(&style_sidebar, 1);
    lv_style_set_border_color(&style_sidebar, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_side(&style_sidebar, LV_BORDER_SIDE_RIGHT);

    /*Section header (DEVICE / PLACES): a small full-width coloured chip*/
    lv_style_init(&style_qa_header);
    lv_style_set_bg_opa(&style_qa_header, LV_OPA_COVER);
    lv_style_set_text_color(&style_qa_header, lv_color_white());
    lv_style_set_pad_hor(&style_qa_header, 6);
    lv_style_set_pad_ver(&style_qa_header, 2);
    lv_style_set_radius(&style_qa_header, 3);

    /*Quick-access entry: a flat transparent row so the grey sidebar shows through*/
    lv_style_init(&style_qa_btn);
    lv_style_set_bg_opa(&style_qa_btn, LV_OPA_TRANSP);
    lv_style_set_shadow_opa(&style_qa_btn, LV_OPA_TRANSP);
    lv_style_set_text_color(&style_qa_btn, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_pad_all(&style_qa_btn, 4);
    lv_style_set_pad_gap(&style_qa_btn, 6);
    lv_style_set_radius(&style_qa_btn, 0);

    /*Path header above the table*/
    lv_style_init(&style_header);
    lv_style_set_bg_opa(&style_header, LV_OPA_COVER);
    lv_style_set_bg_color(&style_header, COLOR_PANEL);
    lv_style_set_text_color(&style_header, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_pad_all(&style_header, 6);
    lv_style_set_radius(&style_header, 0);
    lv_style_set_border_width(&style_header, 1);
    lv_style_set_border_color(&style_header, lv_palette_lighten(LV_PALETTE_GREY, 1));
    lv_style_set_border_side(&style_header, LV_BORDER_SIDE_BOTTOM);

    lv_style_init(&style_table);
    lv_style_set_bg_opa(&style_table, LV_OPA_COVER);
    lv_style_set_bg_color(&style_table, COLOR_PANEL);
    lv_style_set_pad_all(&style_table, 0);
    lv_style_set_radius(&style_table, 0);
    lv_style_set_border_width(&style_table, 0);

    lv_style_init(&style_pressed);
    lv_style_set_bg_opa(&style_pressed, LV_OPA_20);
    lv_style_set_bg_color(&style_pressed, lv_palette_main(LV_PALETTE_GREY));
}

/**
 * List the entries of `path` into the table. Does nothing if `path` is not a
 * readable directory, so a failed navigation simply leaves the view unchanged.
 */
static void browser_open(const char * path)
{
    lv_fs_dir_t dir;
    if(lv_fs_dir_open(&dir, path) != LV_FS_RES_OK) return;

    lv_snprintf(browser_path, sizeof(browser_path), "%s", path);
    lv_label_set_text(browser_path_label, browser_path);

    lv_table_set_row_count(browser_table, 0);

    /*First row goes up to the parent directory*/
    uint32_t row = 0;
    lv_table_set_cell_value(browser_table, row, 0, LV_SYMBOL_LEFT);
    lv_table_set_cell_value(browser_table, row, 1, "..");
    row++;

    /*lv_fs_dir_read prefixes directory names with '/'; an empty name ends the list*/
    char fn[LV_FS_MAX_PATH_LENGTH];
    while(lv_fs_dir_read(&dir, fn, sizeof(fn)) == LV_FS_RES_OK && fn[0] != '\0') {
        bool is_dir = fn[0] == '/';
        const char * name = is_dir ? fn + 1 : fn;
        lv_table_set_cell_value(browser_table, row, 0, is_dir ? LV_SYMBOL_DIRECTORY : LV_SYMBOL_FILE);
        lv_table_set_cell_value(browser_table, row, 1, name);
        row++;
    }

    lv_fs_dir_close(&dir);
}

/**
 * Navigate to the parent directory by dropping the last path component.
 */
static void browser_up(void)
{
    char buf[LV_FS_MAX_PATH_LENGTH];
    lv_snprintf(buf, sizeof(buf), "%s", browser_path);

    size_t len = strlen(buf);
    if(len > 0 && buf[len - 1] == '/') buf[len - 1] = '\0';   /*drop a trailing slash*/

    char * slash = strrchr(buf, '/');
    if(slash == NULL) return;        /*already at the drive root*/
    slash[1] = '\0';                 /*keep the slash, cut the last component*/
    browser_open(buf);
}

static void table_event(lv_event_t * e)
{
    LV_UNUSED(e);
    uint32_t row, col;
    lv_table_get_selected_cell(browser_table, &row, &col);
    if(row == LV_TABLE_CELL_NONE) return;

    const char * icon = lv_table_get_cell_value(browser_table, row, 0);
    const char * name = lv_table_get_cell_value(browser_table, row, 1);

    if(strcmp(name, "..") == 0) {
        browser_up();
        return;
    }

    /*Only folders can be opened; clicking a file just logs it*/
    if(strcmp(icon, LV_SYMBOL_DIRECTORY) != 0) {
        LV_LOG_USER("Selected file: %s%s", browser_path, name);
        return;
    }

    char child[LV_FS_MAX_PATH_LENGTH];
    const char * sep = (browser_path[strlen(browser_path) - 1] == '/') ? "" : "/";
    lv_snprintf(child, sizeof(child), "%s%s%s", browser_path, sep, name);
    browser_open(child);
}

static void quick_access_event(lv_event_t * e)
{
    const char * path = lv_event_get_user_data(e);
    browser_open(path);
}

/**
 * Add a coloured section header (e.g. "PLACES") to the quick-access sidebar.
 */
static void quick_add_header(const char * txt, lv_color_t color)
{
    lv_obj_t * label = lv_label_create(quick_access);
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_add_style(label, &style_qa_header, 0);
    lv_obj_set_style_bg_color(label, color, 0);
    lv_label_set_text(label, txt);
}

/**
 * Add a clickable shortcut row that opens `path` when tapped.
 */
static void quick_add_shortcut(const char * icon, const char * name, const char * path)
{
    lv_obj_t * btn = lv_button_create(quick_access);
    lv_obj_set_width(btn, lv_pct(100));
    lv_obj_add_style(btn, &style_qa_btn, 0);
    lv_obj_add_style(btn, &style_pressed, LV_STATE_PRESSED);

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text_fmt(label, "%s %s", icon, name);

    /*lv_strdup keeps the path alive for the lifetime of the example*/
    lv_obj_add_event_cb(btn, quick_access_event, LV_EVENT_CLICKED, lv_strdup(path));
}

/**
 * @title File browser built from a table
 * @brief Build a file browser with a sidebar, path header and table.
 *
 * A grey quick-access sidebar holds DEVICE/PLACES shortcuts; the white browser area
 * shows the current path over an `lv_table` filled from a directory read with the
 * `lv_fs` API (`lv_fs_dir_open`/`lv_fs_dir_read`/`lv_fs_dir_close`). Clicking a
 * folder row opens the child directory, the ".." row walks back up, and the sidebar
 * shortcuts jump straight to a path. Requires a file-system driver in `lv_conf.h`.
 */
void lv_example_table_file_browser(void)
{
    styles_init();

    lv_obj_t * root = lv_obj_create(lv_screen_active());
    lv_obj_set_size(root, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(root, &style_bg, 0);

    /*Quick-access sidebar (fixed width so the browser gets a predictable size)*/
    quick_access = lv_obj_create(root);
    lv_obj_set_size(quick_access, 100, lv_pct(100));
    lv_obj_set_flex_flow(quick_access, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(quick_access, &style_sidebar, 0);

    char home[LV_FS_MAX_PATH_LENGTH];
#if LV_USE_FS_WIN32
    lv_snprintf(home, sizeof(home), "C:C:/Users/Public");
#else
    const char * env = getenv("HOME");
    lv_snprintf(home, sizeof(home), "A:%s", env ? env : "");
#endif

    char buf[LV_FS_MAX_PATH_LENGTH];
    quick_add_header("PLACES", lv_palette_main(LV_PALETTE_LIME));
    quick_add_shortcut(LV_SYMBOL_HOME, "Home", home);
    lv_snprintf(buf, sizeof(buf), "%s/Documents", home);
    quick_add_shortcut(LV_SYMBOL_FILE, "Docs", buf);
    lv_snprintf(buf, sizeof(buf), "%s/Pictures", home);
    quick_add_shortcut(LV_SYMBOL_IMAGE, "Pics", buf);
    lv_snprintf(buf, sizeof(buf), "%s/Music", home);
    quick_add_shortcut(LV_SYMBOL_AUDIO, "Music", buf);

    quick_add_header("DEVICE", lv_palette_main(LV_PALETTE_ORANGE));
    quick_add_shortcut(LV_SYMBOL_DRIVE, "Files", BROWSER_ROOT);

    /*Browser area: path header above the file table*/
    lv_obj_t * browser = lv_obj_create(root);
    lv_obj_set_height(browser, lv_pct(100));
    lv_obj_set_flex_grow(browser, 1);
    lv_obj_set_flex_flow(browser, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(browser, &style_panel, 0);

    browser_path_label = lv_label_create(browser);
    lv_obj_set_width(browser_path_label, lv_pct(100));
    lv_label_set_long_mode(browser_path_label, LV_LABEL_LONG_MODE_DOTS);
    lv_obj_add_style(browser_path_label, &style_header, 0);

    browser_table = lv_table_create(browser);
    lv_obj_set_width(browser_table, lv_pct(100));
    lv_obj_set_flex_grow(browser_table, 1);
    lv_obj_add_style(browser_table, &style_table, 0);
    /*Tighten the cell padding; the theme default is large enough to clip the icon*/
    lv_obj_set_style_pad_hor(browser_table, 6, LV_PART_ITEMS);
    lv_obj_set_style_pad_ver(browser_table, 8, LV_PART_ITEMS);

    /*Icon column fits a symbol; name column fills the rest of the browser area*/
    int32_t icon_col = 30;
    int32_t name_col = lv_display_get_horizontal_resolution(NULL) - 100 - icon_col - 16;
    lv_table_set_column_count(browser_table, 2);
    lv_table_set_column_width(browser_table, 0, icon_col);
    lv_table_set_column_width(browser_table, 1, name_col);
    lv_obj_add_event_cb(browser_table, table_event, LV_EVENT_VALUE_CHANGED, NULL);

    browser_open(BROWSER_ROOT);
}

#endif
