from imagetools import get_png_info, open_png

# Register PNG image decoder
decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

# Create an image from the png file
try:
    with open('../../assets/img_caret_down.png','rb') as f:
        png_data = f.read()
except:
    print("Could not find img_caret_down.png")
    sys.exit()

img_caret_down_argb = lv.img_dsc_t({
  'data_size': len(png_data),
  'data': png_data
})

def event_cb(e):
    dropdown = e.get_target()
    option = " "*64 # should be large enough to store the option
    dropdown.get_selected_str(option, len(option))
    print(option.strip() +" is selected")
#
# Create a menu from a drop-down list and show some drop-down list features and styling
#

# Create a drop down list
dropdown = lv.dropdown(lv.scr_act())
dropdown.align(lv.ALIGN.TOP_LEFT, 10, 10)
dropdown.set_options("\n".join([
    "New project",
    "New file",
    "Open project",
    "Recent projects",
    "Preferences",
    "Exit"]))

# Set a fixed text to display on the button of the drop-down list
dropdown.set_text("Menu")

# Use a custom image as down icon and flip it when the list is opened
# LV_IMG_DECLARE(img_caret_down)
dropdown.set_symbol(img_caret_down_argb)
dropdown.set_style_transform_angle(1800, lv.PART.INDICATOR | lv.STATE.CHECKED)

# In a menu we don't need to show the last clicked item
dropdown.set_selected_highlight(False)

dropdown.add_event_cb(event_cb, lv.EVENT.VALUE_CHANGED, None)

