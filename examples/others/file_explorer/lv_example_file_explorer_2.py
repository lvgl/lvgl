import fs_driver
import os

LV_FILE_EXPLORER_QUICK_ACCESS = True
LV_USE_FS_WIN32 = False
LV_FILE_EXPLORER_PATH_MAX_LEN = 128

def file_explorer_event_handler(e):
    code = e.get_code()
    obj = e.get_target()

    if code == lv.EVENT.VALUE_CHANGED:
        cur_path =  obj.explorer_get_current_path()
        sel_fn = obj.explorer_get_selected_file_name()
        print(str(cur_path) + str(sel_fn))

def btn_event_handler(e,fe_quick_access_obj):

    code = e.get_code()
    btn = e.get_target()
    # lv_obj_t * file_explorer = lv_event_get_user_data(e);

    if code == lv.EVENT.VALUE_CHANGED :
        if btn.has_state(lv.STATE.CHECKED) :
            fe_quick_access_obj.add_flag(lv.obj.FLAG.HIDDEN)
        else :
            fe_quick_access_obj.clear_flag(lv.obj.FLAG.HIDDEN)

def dd_event_handler(e, file_explorer):

    code = e.get_code()
    dd = e.get_target()
    # fe_quick_access_obj = lv_event_get_user_data(e);

    if code == lv.EVENT.VALUE_CHANGED :
        buf = bytearray(32)
        lv.dropdown.get_selected_str(dd,buf,len(buf))

        if buf[:4] == b"NONE":
            # print("set sort to NONE")
            file_explorer.explorer_set_sort(lv.EXPLORER_SORT.NONE)
        elif buf[:4] == b"KIND" :
            # print("set sort to KIND")
            file_explorer.explorer_set_sort(lv.EXPLORER_SORT.KIND)

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

# Quick access status control button
fe_quick_access_obj = file_explorer.explorer_get_quick_access_area()
fe_header_obj = file_explorer.explorer_get_header()
btn = lv.btn(fe_header_obj)
btn.set_style_radius(2, 0)
btn.set_style_pad_all(4, 0)
btn.align(lv.ALIGN.LEFT_MID, 0, 0)
btn.add_flag(lv.obj.FLAG.CHECKABLE)

label = lv.label(btn)
label.set_text(lv.SYMBOL.LIST)
label.center()

btn.add_event_cb(lambda evt: btn_event_handler(evt,fe_quick_access_obj), lv.EVENT.VALUE_CHANGED, None)
#btn.add_event_cb(lambda evt: btn_event_handler(evt,file_explorer), lv.EVENT.VALUE_CHANGED, None)

# Sort control
opts = "NONE\nKIND"

dd = lv.dropdown(fe_header_obj)
dd.set_style_radius(4, 0)
dd.set_style_pad_all(0, 0)
dd.set_size(lv.pct(30), lv.SIZE_CONTENT)
dd.set_options_static(opts)
dd.align(lv.ALIGN.RIGHT_MID, 0, 0)
# lv_obj_align_to(dd, btn, LV_ALIGN_OUT_RIGHT_MID, 0, 0);

dd.add_event_cb(lambda evt: dd_event_handler(evt,file_explorer), lv.EVENT.VALUE_CHANGED, None)
#dd.add_event_cb(lambda evt: dd_event_handler(evt,fe_quick_access_obj), lv.EVENT.VALUE_CHANGED, None)

