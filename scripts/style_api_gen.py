#!/usr/bin/env python3

import os
import re
import sys

props = [
{'section': 'Size and position', 'dsc':'Properties related to size, position, alignment and layout of the objects.' },
{'name': 'WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t' , 'default':'Widget dependent', 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets the width of object. Pixel, percentage and `LV_SIZE_CONTENT` values can be used. Percentage values are relative to the width of the parent's content area."},

{'name': 'MIN_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t' , 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets a minimal width. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area."},

{'name': 'MAX_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t' , 'default':'LV_COORD_MAX', 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets a maximal width. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area."},

{'name': 'HEIGHT',
 'style_type': 'num',   'var_type': 'lv_coord_t' , 'default':'Widget dependent', 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets the height of object. Pixel, percentage and `LV_SIZE_CONTENT` can be used. Percentage values are relative to the height of the parent's content area."},

{'name': 'MIN_HEIGHT',
 'style_type': 'num',   'var_type': 'lv_coord_t' , 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets a minimal height. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area."},

{'name': 'MAX_HEIGHT',
 'style_type': 'num',   'var_type': 'lv_coord_t' , 'default':'LV_COORD_MAX', 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets a maximal height. Pixel and percentage values can be used. Percentage values are relative to the height of the parent's content area."},

{'name': 'X',
 'style_type': 'num',   'var_type': 'lv_coord_t' , 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the X coordinate of the object considering the set `align`. Pixel and percentage values can be used. Percentage values are relative to the width of the parent's content area."},

{'name': 'Y',
 'style_type': 'num',   'var_type': 'lv_coord_t', 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the Y coordinate of the object considering the set `align`. Pixel and percentage values can be used. Percentage values are relative to the height of the parent's content area."},

{'name': 'ALIGN',
 'style_type': 'num',   'var_type': 'lv_align_t', 'default':'`LV_ALIGN_DEFAULT`', 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the alignment which tells from which point of the parent the X and Y coordinates should be interpreted. The possible values are: `LV_ALIGN_DEFAULT`, `LV_ALIGN_TOP_LEFT/MID/RIGHT`, `LV_ALIGN_BOTTOM_LEFT/MID/RIGHT`, `LV_ALIGN_LEFT/RIGHT_MID`, `LV_ALIGN_CENTER`. `LV_ALIGN_DEFAULT` means `LV_ALIGN_TOP_LEFT` with LTR base direction and `LV_ALIGN_TOP_RIGHT` with RTL base direction."},

{'name': 'TRANSFORM_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Make the object wider on both sides with this value. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's width." },

{'name': 'TRANSFORM_HEIGHT',
  'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Make the object higher on both sides with this value. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's height." },

{'name': 'TRANSLATE_X',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Move the object with this value in X direction. Applied after layouts, aligns and other positioning. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's width." },

{'name': 'TRANSLATE_Y',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Move the object with this value in Y direction. Applied after layouts, aligns and other positioning. Pixel and percentage (with `lv_pct(x)`) values can be used. Percentage values are relative to the object's height." },

{'name': 'TRANSFORM_ZOOM',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 1,
 'dsc': "Zoom an objects. The value 256 (or `LV_IMG_ZOOM_NONE`) means normal size, 128 half size, 512 double size, and so on" },

{'name': 'TRANSFORM_ANGLE',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 1,
 'dsc': "Rotate an objects. The value is interpreted in 0.1 degree units. E.g. 450 means 45 deg."},

{'name': 'TRANSFORM_PIVOT_X',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the pivot point's X coordinate for transformations. Relative to the object's top left corner'"},

{'name': 'TRANSFORM_PIVOT_Y',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the pivot point's Y coordinate for transformations. Relative to the object's top left corner'"},

{'section': 'Padding', 'dsc' : "Properties to describe spacing between the parent's sides and the children and among the children. Very similar to the padding properties in HTML."},
{'name': 'PAD_TOP',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets the padding on the top. It makes the content area smaller in this direction."},

{'name': 'PAD_BOTTOM',
 'style_type': 'num',   'var_type': 'lv_coord_t', 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets the padding on the bottom. It makes the content area smaller in this direction."},

{'name': 'PAD_LEFT',
 'style_type': 'num',   'var_type': 'lv_coord_t', 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets the padding on the left. It makes the content area smaller in this direction."},

{'name': 'PAD_RIGHT',
  'style_type': 'num',   'var_type': 'lv_coord_t', 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets the padding on the right. It makes the content area smaller in this direction."},

{'name': 'PAD_ROW',
 'style_type': 'num',   'var_type': 'lv_coord_t', 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets the padding between the rows. Used by the layouts."},

{'name': 'PAD_COLUMN',
 'style_type': 'num',   'var_type': 'lv_coord_t', 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Sets the padding between the columns. Used by the layouts."},

{'section': 'Background', 'dsc':'Properties to describe the background color and image of the objects.' },
{'name': 'BG_COLOR',
 'style_type': 'color', 'var_type': 'lv_color_t', 'default':'`0xffffff`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set the background color of the object."},

{'name': 'BG_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t',  'default':'`LV_OPA_TRANSP`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the opacity of the background. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency."},

{'name': 'BG_GRAD_COLOR',
 'style_type': 'color', 'var_type': 'lv_color_t',  'default':'`0x000000`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set the gradient color of the background. Used only if `grad_dir` is not `LV_GRAD_DIR_NONE`"},

{'name': 'BG_GRAD_DIR',
 'style_type': 'num',   'var_type': 'lv_grad_dir_t',  'default':'`LV_GRAD_DIR_NONE`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the direction of the gradient of the background. The possible values are `LV_GRAD_DIR_NONE/HOR/VER`."},

{'name': 'BG_MAIN_STOP',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the point from which the background color should start for gradients. 0 means to top/left side, 255 the bottom/right side, 128 the center, and so on"},

{'name': 'BG_GRAD_STOP',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':255, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the point from which the background's gradient color should start. 0 means to top/left side, 255 the bottom/right side, 128 the center, and so on"},

{'name': 'BG_GRAD',
 'style_type': 'ptr',   'var_type': 'const lv_grad_dsc_t *',  'default':'`NULL`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the gradient definition. The pointed instance must exist while the object is alive. NULL to disable. It wraps `BG_GRAD_COLOR`, `BG_GRAD_DIR`, `BG_MAIN_STOP` and `BG_GRAD_STOP` into one descriptor and allows creating gradients with more colors too."},

{'name': 'BG_DITHER_MODE',
 'style_type': 'num',   'var_type': 'lv_dither_mode_t',  'default':'`LV_DITHER_NONE`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the dithering mode of the gradient of the background. The possible values are `LV_DITHER_NONE/ORDERED/ERR_DIFF`."},

{'name': 'BG_IMG_SRC',
 'style_type': 'ptr',   'var_type': 'const void *',  'default':'`NULL`', 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set a background image. Can be a pointer to `lv_img_dsc_t`, a path to a file or an `LV_SYMBOL_...`"},

{'name': 'BG_IMG_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t',  'default':'`LV_OPA_COVER`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the opacity of the background image. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency."},

{'name': 'BG_IMG_RECOLOR',
 'style_type': 'color', 'var_type': 'lv_color_t',  'default':'`0x000000`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set a color to mix to the background image."},

{'name': 'BG_IMG_RECOLOR_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t',  'default':'`LV_OPA_TRANSP`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the intensity of background image recoloring. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means no mixing, 255, `LV_OPA_100` or `LV_OPA_COVER` means full recoloring, other values or LV_OPA_10, LV_OPA_20, etc are interpreted proportionally."},

{'name': 'BG_IMG_TILED',
 'style_type': 'num',   'var_type': 'bool',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "If enabled the background image will be tiled. The possible values are `true` or `false`."},

{'section': 'Border', 'dsc':'Properties to describe the borders' },
{'name': 'BORDER_COLOR',
 'style_type': 'color', 'var_type': 'lv_color_t',  'default':'`0x000000`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set the color of the border"},

{'name': 'BORDER_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t' ,  'default':'`LV_OPA_COVER`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the opacity of the border. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency."},

{'name': 'BORDER_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the width of the border. Only pixel values can be used."},

{'name': 'BORDER_SIDE',
 'style_type': 'num',   'var_type': 'lv_border_side_t',  'default':'`LV_BORDER_SIDE_NONE`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set only which side(s) the border should be drawn. The possible values are `LV_BORDER_SIDE_NONE/TOP/BOTTOM/LEFT/RIGHT/INTERNAL`. OR-ed values can be used as well, e.g. `LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_LEFT`."},

{'name': 'BORDER_POST',
'style_type': 'num',   'var_type': 'bool' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Sets whether the border should be drawn before or after the children are drawn. `true`: after children, `false`: before children"},

{'section': 'Outline', 'dsc':'Properties to describe the outline. It\'s like a border but drawn outside of the rectangles.' },
{'name': 'OUTLINE_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set the width of the outline in pixels. "},

{'name': 'OUTLINE_COLOR',
 'style_type': 'color', 'var_type': 'lv_color_t' ,  'default':'`0x000000`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set the color of the outline."},

{'name': 'OUTLINE_OPA',
'style_type': 'num',   'var_type': 'lv_opa_t' ,  'default':'`LV_OPA_COVER`', 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set the opacity of the outline. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency."},

{'name': 'OUTLINE_PAD',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set the padding of the outline, i.e. the gap between object and the outline."},

{'section': 'Shadow', 'dsc':'Properties to describe the shadow drawn under the rectangles.' },
{'name': 'SHADOW_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set the width of the shadow in pixels. The value should be >= 0."},

{'name': 'SHADOW_OFS_X',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set an offset on the shadow in pixels in X direction. "},

{'name': 'SHADOW_OFS_Y',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set an offset on the shadow in pixels in Y direction. "},

{'name': 'SHADOW_SPREAD',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Make the shadow calculation to use a larger or smaller rectangle as base. The value can be in pixel to make the area larger/smaller"},

{'name': 'SHADOW_COLOR',
  'style_type': 'color', 'var_type': 'lv_color_t' ,  'default':'`0x000000`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set the color of the shadow"},

{'name': 'SHADOW_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t' ,  'default':'`LV_OPA_COVER`', 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set the opacity of the shadow. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency."},

{'section': 'Image', 'dsc':'Properties to describe the images' },
{'name': 'IMG_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t' ,  'default':'`LV_OPA_COVER`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the opacity of an image. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency."},

{'name': 'IMG_RECOLOR',
 'style_type': 'color', 'var_type': 'lv_color_t',  'default':'`0x000000`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set color to mixt to the image."},

{'name': 'IMG_RECOLOR_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the intensity of the color mixing. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency."},

{'section': 'Line', 'dsc':'Properties to describe line-like objects' },
{'name': 'LINE_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set the width of the lines in pixel."},

{'name': 'LINE_DASH_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the width of dashes in pixel. Note that dash works only on horizontal and vertical lines"},

{'name': 'LINE_DASH_GAP',
 'style_type': 'num',   'var_type': 'lv_coord_t',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the gap between dashes in pixel. Note that dash works only on horizontal and vertical lines"},

{'name': 'LINE_ROUNDED',
 'style_type': 'num',   'var_type': 'bool' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Make the end points of the lines rounded. `true`: rounded, `false`: perpendicular line ending "},

{'name': 'LINE_COLOR',
 'style_type': 'color', 'var_type': 'lv_color_t' ,  'default':'`0x000000`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set the color fo the lines."},

{'name': 'LINE_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t' ,  'default':'`LV_OPA_COVER`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the opacity of the lines."},

{'section': 'Arc', 'dsc':'TODO' },
{'name': 'ARC_WIDTH',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 1,
 'dsc': "Set the width (thickness) of the arcs in pixel."},

{'name': 'ARC_ROUNDED',
 'style_type': 'num',   'var_type': 'bool' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Make the end points of the arcs rounded. `true`: rounded, `false`: perpendicular line ending "},

{'name': 'ARC_COLOR',
 'style_type': 'color', 'var_type': 'lv_color_t',  'default':'`0x000000`', 'inherited': 0, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Set the color of the arc."},

{'name': 'ARC_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t' ,  'default':'`LV_OPA_COVER`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the opacity of the arcs."},

{'name': 'ARC_IMG_SRC',
 'style_type': 'ptr',   'var_type': 'const void *',  'default':'`NULL`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set an image from which the arc will be masked out. It's useful to display complex effects on the arcs. Can be a pointer to `lv_img_dsc_t` or a path to a file"},

{'section': 'Text', 'dsc':'Properties to describe the properties of text. All these properties are inherited.' },
{'name': 'TEXT_COLOR',
'style_type': 'color', 'var_type': 'lv_color_t',  'default':'`0x000000`', 'inherited': 1, 'layout': 0, 'ext_draw': 0, 'filtered': 1,
 'dsc': "Sets the color of the text."},

{'name': 'TEXT_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t',  'default':'`LV_OPA_COVER`', 'inherited': 1, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the opacity of the text. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency."},

{'name': 'TEXT_FONT',
 'style_type': 'ptr',   'var_type': 'const lv_font_t *',  'default':'`LV_FONT_DEFAULT`', 'inherited': 1, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the font of the text (a pointer `lv_font_t *`). "},

{'name': 'TEXT_LETTER_SPACE',
'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 1, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the letter space in pixels"},

{'name': 'TEXT_LINE_SPACE',
 'style_type': 'num',   'var_type': 'lv_coord_t' ,  'default':0, 'inherited': 1, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the line space in pixels."},

{'name': 'TEXT_DECOR',
 'style_type': 'num',   'var_type': 'lv_text_decor_t' ,  'default':'`LV_TEXT_DECOR_NONE`', 'inherited': 1, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set decoration for the text. The possible values are `LV_TEXT_DECOR_NONE/UNDERLINE/STRIKETHROUGH`. OR-ed values can be used as well." },

{'name': 'TEXT_ALIGN',
'style_type': 'num',   'var_type': 'lv_text_align_t' ,  'default':'`LV_TEXT_ALIGN_AUTO`', 'inherited': 1, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set how to align the lines of the text. Note that it doesn't align the object itself, only the lines inside the object. The possible values are `LV_TEXT_ALIGN_LEFT/CENTER/RIGHT/AUTO`. `LV_TEXT_ALIGN_AUTO` detect the text base direction and uses left or right alignment accordingly"},

{'section': 'Miscellaneous', 'dsc':'Mixed properties for various purposes.' },
{'name': 'RADIUS',
 'style_type': 'num', 'var_type': 'lv_coord_t', 'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Set the radius on every corner. The value is interpreted in pixel (>= 0) or `LV_RADIUS_CIRCLE` for max. radius"},

{'name': 'CLIP_CORNER',
 'style_type': 'num',   'var_type': 'bool',  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Enable to clip the overflowed content on the rounded corner. Can be `true` or `false`." },

{'name': 'OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t',  'default':'`LV_OPA_COVER`', 'inherited': 1, 'layout': 0, 'ext_draw': 0,
 'dsc': "Scale down all opacity values of the object by this factor. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency." },

{'name': 'OPA_LAYERED',
 'style_type': 'num',   'var_type': 'lv_opa_t',  'default':'`LV_OPA_COVER`', 'inherited': 1, 'layout': 0, 'ext_draw': 0,
 'dsc': "First draw the object on the layer, then scale down layer opacity factor. Value 0, `LV_OPA_0` or `LV_OPA_TRANSP` means fully transparent, 255, `LV_OPA_100` or `LV_OPA_COVER` means fully covering, other values or LV_OPA_10, LV_OPA_20, etc means semi transparency." },

{'name': 'COLOR_FILTER_DSC',
 'style_type': 'ptr',   'var_type': 'const lv_color_filter_dsc_t *',  'default':'`NULL`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Mix a color to all colors of the object." },

{'name': 'COLOR_FILTER_OPA',
 'style_type': 'num',   'var_type': 'lv_opa_t' ,  'default':'`LV_OPA_TRANSP`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "The intensity of mixing of color filter."},

 {'name': 'ANIM',
 'style_type': 'ptr',   'var_type': 'const lv_anim_t *',  'default':'`NULL`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "The animation template for the object's animation. Should be a pointer to `lv_anim_t`. The animation parameters are widget specific, e.g. animation time could be the E.g. blink time of the cursor on the text area or scroll time of a roller. See the widgets' documentation to learn more."},

{'name': 'ANIM_TIME',
 'style_type': 'num',   'var_type': 'uint32_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "The animation time in milliseconds. Its meaning is widget specific. E.g. blink time of the cursor on the text area or scroll time of a roller. See the widgets' documentation to learn more."},

{'name': 'ANIM_SPEED',
 'style_type': 'num',   'var_type': 'uint32_t' ,  'default':0, 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "The animation speed in pixel/sec. Its meaning is widget specific. E.g. scroll speed of label. See the widgets' documentation to learn more."},

{'name': 'TRANSITION',
 'style_type': 'ptr',   'var_type': 'const lv_style_transition_dsc_t *' ,  'default':'`NULL`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "An initialized `lv_style_transition_dsc_t` to describe a transition."},

{'name': 'BLEND_MODE',
 'style_type': 'num',   'var_type': 'lv_blend_mode_t' ,  'default':'`LV_BLEND_MODE_NORMAL`', 'inherited': 0, 'layout': 0, 'ext_draw': 0,
 'dsc': "Describes how to blend the colors to the background. The possible values are `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE/MULTIPLY`"},

{'name': 'LAYOUT',
 'style_type': 'num',   'var_type': 'uint16_t', 'default':0, 'inherited': 0, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the layout of the object. The children will be repositioned and resized according to the policies set for the layout. For the possible values see the documentation of the layouts."},

{'name': 'BASE_DIR',
 'style_type': 'num',   'var_type': 'lv_base_dir_t', 'default':'`LV_BASE_DIR_AUTO`', 'inherited': 1, 'layout': 1, 'ext_draw': 0,
 'dsc': "Set the base direction of the object. The possible values are `LV_BIDI_DIR_LTR/RTL/AUTO`."},
]


def style_get_cast(style_type, var_type):
  cast = ""
  if style_type != 'color':
    cast = "(" + var_type + ")"
  return cast


def obj_style_get(p):
  if 'section' in p: return

  cast = style_get_cast(p['style_type'], p['var_type'])
  print("static inline " + p['var_type'] + " lv_obj_get_style_" + p['name'].lower() +"(const struct _lv_obj_t * obj, uint32_t part)")
  print("{")
  print("    lv_style_value_t v = lv_obj_get_style_prop(obj, part, LV_STYLE_" + p['name'] + ");")
  print("    return " + cast + "v." + p['style_type'] + ";")
  print("}")
  print("")

  if 'filtered' in p and p['filtered']:
    print("static inline " + p['var_type'] + " lv_obj_get_style_" + p['name'].lower() +"_filtered(const struct _lv_obj_t * obj, uint32_t part)")
    print("{")
    print("    lv_style_value_t v = _lv_obj_style_apply_color_filter(obj, part, lv_obj_get_style_prop(obj, part, LV_STYLE_" + p['name'] + "));")
    print("    return " + cast + "v." + p['style_type'] + ";")
    print("}")
    print("")



def style_set_cast(style_type):
  cast = ""
  if style_type == 'num':
    cast = "(int32_t)"
  return cast


def style_set_c(p):
  if 'section' in p: return

  cast = style_set_cast(p['style_type'])
  print("")
  print("void lv_style_set_" + p['name'].lower() +"(lv_style_t * style, "+ p['var_type'] +" value)")
  print("{")
  print("    lv_style_value_t v = {")
  print("        ." + p['style_type'] +" = " + cast + "value")
  print("    };")
  print("    lv_style_set_prop(style, LV_STYLE_" + p['name'] +", v);")
  print("}")


def style_set_h(p):
  if 'section' in p: return

  print("void lv_style_set_" + p['name'].lower() +"(lv_style_t * style, "+ p['var_type'] +" value);")


def local_style_set_c(p):
  if 'section' in p: return

  cast = style_set_cast(p['style_type'])
  print("")
  print("void lv_obj_set_style_" + p['name'].lower() + "(struct _lv_obj_t * obj, " + p['var_type'] +" value, lv_style_selector_t selector)")
  print("{")
  print("    lv_style_value_t v = {")
  print("        ." + p['style_type'] +" = " + cast + "value")
  print("    };")
  print("    lv_obj_set_local_style_prop(obj, LV_STYLE_" + p['name'] +", v, selector);")
  print("}")


def local_style_set_h(p):
  if 'section' in p: return
  print("void lv_obj_set_style_" + p['name'].lower() + "(struct _lv_obj_t * obj, " + p['var_type'] +" value, lv_style_selector_t selector);")


def style_const_set(p):
  if 'section' in p: return

  cast = style_set_cast(p['style_type'])
  print("")
  print("#define LV_STYLE_CONST_" + p['name'] + "(val) \\")
  print("    { \\")
  print("        .prop = LV_STYLE_" + p['name'] + ", .value = { ." + p['style_type'] +" = " + cast + "val } \\")
  print("    }")


def docs(p):
  if "section" in p:
    print("")
    print("## " + p['section'])
    print(p['dsc'])
    return

  if "default" not in p: return

  d = str(p["default"])

  i = "No"
  if p["inherited"]: i = "Yes"

  l = "No"
  if p["layout"]: l = "Yes"

  e = "No"
  if p["ext_draw"]: e = "Yes"

  li_style = "style='display:inline; margin-right: 20px; margin-left: 0px"

  dsc = p['dsc']

  print("")
  print("### " + p["name"].lower())
  print(dsc)

  print("<ul>")
  print("<li " + li_style + "'><strong>Default</strong> " + d + "</li>")
  print("<li " + li_style + "'><strong>Inherited</strong> " + i + "</li>")
  print("<li " + li_style + "'><strong>Layout</strong> " + l + "</li>")
  print("<li " + li_style + "'><strong>Ext. draw</strong> " + e + "</li>")
  print("</ul>")


base_dir = os.path.abspath(os.path.dirname(__file__))
sys.stdout = open(base_dir + '/../src/core/lv_obj_style_gen.h', 'w')

for p in props:
  obj_style_get(p)

for p in props:
  local_style_set_h(p)

sys.stdout = open(base_dir + '/../src/core/lv_obj_style_gen.c', 'w')

print("#include \"lv_obj.h\"")
for p in props:
  local_style_set_c(p)

sys.stdout = open(base_dir + '/../src/misc/lv_style_gen.c', 'w')

print("#include \"lv_style.h\"")
for p in props:
  style_set_c(p)

sys.stdout = open(base_dir + '/../src/misc/lv_style_gen.h', 'w')

for p in props:
  style_set_h(p)

for p in props:
  style_const_set(p)

sys.stdout = open(base_dir + '/../docs/overview/style-props.md', 'w')

print('# Style properties')
for p in props:
  docs(p)
