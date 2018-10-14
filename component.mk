#
# Component Makefile
#

COMPONENT_SRCDIRS := . \
    lv_core \
    lv_draw \
    lv_fonts \
    lv_hal \
    lv_misc \
    lv_objx \
    lv_themes
    
COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS) ..
