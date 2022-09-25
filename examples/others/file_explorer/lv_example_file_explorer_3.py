import fs_driver
import os

LV_FILE_EXPLORER_QUICK_ACCESS = True
LV_USE_FS_WIN32 = False
LV_FILE_EXPLORER_PATH_MAX_LEN = 128 

def exch_table_item(tb, i, j):
    tmp = tb.get_cell_value(i, 0)
    tb.set_cell_value(0, 2, tmp)
    tb.set_cell_value(i, 0, tb.get_cell_value(j, 0))
    tb.set_cell_value(j, 0, tb.get_cell_value(0, 2))

    tmp = tb.get_cell_value(i, 1)
    tb.set_cell_value(0, 2, tmp)
    tb.set_cell_value(i, 1, tb.get_cell_value(j, 1))
    tb.set_cell_value(j, 1, tb.get_cell_value(0, 2))

# Quick sort 3 way
def sort_by_file_kind(tb, lo, hi) :
    if lo >= hi:
        return;

    lt = lo
    i = lo + 1
    gt = hi
    v = tb.get_cell_value(lo, 1)
    
    while i <= gt :
        
        if tb.get_cell_value(i, 1) < v :
            lt += 1
            i  += 1
            exch_table_item(tb, lt, i)
        elif tb.get_cell_value(i, 1) >  v:
            gt -= 1
            exch_table_item(tb, i, gt)
        else :
            i += 1
    sort_by_file_kind(tb, lo, lt - 1);
    sort_by_file_kind(tb, gt + 1, hi);


def file_explorer_event_handler(e) :

    code = e.get_code()
    obj = e.get_target()

    if code == lv.EVENT.VALUE_CHANGED:
        cur_path =  obj.explorer_get_current_path()
        sel_fn = obj.explorer_get_selected_file_name()
        print(str(cur_path) + str(sel_fn))

    elif code == lv.EVENT.READY :
        tb = obj.explorer_get_file_table()
        sum = tb.get_row_cnt()
        # print("sum: ",sum)
        sort_by_file_kind(tb, 0, (sum - 1));

file_explorer = lv.file_explorer(lv.scr_act())
# Before custom sort, please set the default sorting to NONE. The default is NONE.
file_explorer.explorer_set_sort(lv.EXPLORER_SORT.NONE)

file_explorer = lv.file_explorer(lv.scr_act())

if LV_USE_FS_WIN32 :
    file_explorer.explorer_open_dir("D:")
    if LV_FILE_EXPLORER_QUICK_ACCESS :
        file_explorer.explorer_set_quick_access_path(lv.EXPLORER.HOME_DIR, "C:/Users/Public/Desktop")
        file_explorer.explorer_set_quick_access_path(lv.EXPLORER.VIDEO_DIR, "C:/Users/Public/Videos")
        file_explorer.explorer_set_quick_access_path(lv.EXPLORER_PICTURES_DIR, "C:/Users/Public/Pictures");
        file_explorer.explorer_set_quick_access_path(lv.EXPLORER_MUSIC_DIR, "C:/Users/Public/Music");
        file_explorer.explorer_set_quick_access_path(lv.EXPLORER_DOCS_DIR, "C:/Users/Public/Documents");
        file_explorer.explorer_set_quick_access_path(lv.EXPLORER_FS_DIR, "D:");

# linux 
file_explorer.explorer_open_dir("A:/")

if LV_FILE_EXPLORER_QUICK_ACCESS :
    home_dir = "A:" + os.getenv('HOME')    
    print("quick access: " + home_dir)
    file_explorer.explorer_set_quick_access_path(lv.EXPLORER.HOME_DIR, home_dir)
    file_explorer.explorer_set_quick_access_path(lv.EXPLORER.VIDEO_DIR, home_dir + "/Videos")
    file_explorer.explorer_set_quick_access_path(lv.EXPLORER.PICTURES_DIR, home_dir + "/Pictures")
    file_explorer.explorer_set_quick_access_path(lv.EXPLORER.MUSIC_DIR, home_dir + "/Music")
    file_explorer.explorer_set_quick_access_path(lv.EXPLORER.DOCS_DIR, home_dir + "/Documents")
    file_explorer.explorer_set_quick_access_path(lv.EXPLORER.FS_DIR, "A:/")

    file_explorer.add_event_cb(file_explorer_event_handler, lv.EVENT.ALL, None)
