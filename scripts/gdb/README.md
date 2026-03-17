# lvglgdb

lvglgdb is a GDB script for LVGL.

# Installation

```bash
pip install lvglgdb
```

# Simple Usage

In your GDB session, run:
```bash
py import lvglgdb

dump obj
dump display -f png
dump cache image
dump cache image_header
check cache image
dump anim
dump timer
dump indev
dump group
dump image_decoder
dump fs_drv
dump draw_task <layer_expr>

# Inspect a single lv_style_t variable
info style my_style

# Inspect all styles of an lv_obj_t
info style --obj my_obj

# Show draw unit information
info draw_unit

# Show object class hierarchy
info obj_class obj->class_p

# Show subject and its observers
info subject &my_subject
```

# Structure

```mermaid
graph TD
    lvgl["lvgl<br/>(mem→python object)"]
    gdb_cmds["gdb_cmds<br/>(gdb commands)"]
    lvglgdb["lvglgdb"]

    lvglgdb --> lvgl
    lvglgdb --> gdb_cmds
    gdb_cmds --> lvgl

    classDef pkg fill:white,stroke:gray
    classDef core fill:white,stroke:gray
    class lvglgdb,lvgl,gdb_cmds pkg
```
