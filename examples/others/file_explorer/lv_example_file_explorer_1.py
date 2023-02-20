import fs_driver
import os

LV_FILE_EXPLORER_QUICK_ACCESS = True
LV_USE_FS_WIN32 = False
LV_FILE_EXPLORER_PATH_MAX_LEN = 128

def file_explorer_event_handler(e) :

    code = e.get_code()
    obj = e.get_target_obj()

    if code == lv.EVENT.VALUE_CHANGED:
        cur_path =  obj.explorer_get_current_path()
        sel_fn = obj.explorer_get_selected_file_name()
        # print("cur_path: " + str(cur_path), " sel_fn: " + str(sel_fn))
        print(str(cur_path) + str(sel_fn))

file_explorer = lv.file_explorer(lv.scr_act())
file_explorer.explorer_set_sort(lv.EXPLORER_SORT.KIND)


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

file_explorer.add_event(file_explorer_event_handler, lv.EVENT.ALL, None)

