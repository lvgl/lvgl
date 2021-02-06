# ESP-IDF component file for make based commands

ifdef $(IDF_VER)

$(info Adding LVGL as ESP-IDF component)

COMPONENT_SRCDIRS := .
COMPONENT_ADD_INCLUDEDIRS := .

else

$(info IDF_VER not defined)

endif
