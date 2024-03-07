# ESP-IDF component file for make based commands

COMPONENT_SRCDIRS := . \
                  src \
                  src/core \
                  src/draw \
                  src/extra \
                  src/font \
                  src/hal \
                  src/misc \
                  src/widgets \
                  src/draw/arm2d \
                  src/draw/nxp \
                  src/draw/sdl \
                  src/draw/stm32_dma2d \
                  src/draw/sw \
                  src/draw/swm342_dma2d \
                  src/extra/layouts \
                  src/extra/libs \
                  src/extra/others \
                  src/extra/themes \
                  src/extra/widgets \
                  src/extra/layouts/flex \
                  src/extra/layouts/grid \
                  src/extra/libs/bmp \
                  src/extra/libs/ffmpeg \
                  src/extra/libs/freetype \
                  src/extra/libs/fsdrv \
                  src/extra/libs/gif \
                  src/extra/libs/png \
                  src/extra/libs/qrcode \
                  src/extra/libs/rlottie \
                  src/extra/libs/sjgp \
                  src/extra/others/fragment \
                  src/extra/others/gridnav \
                  src/extra/others/ime \
                  src/extra/others/imgfont \
                  src/extra/others/monkey \
                  src/extra/others/msg \
                  src/extra/others/snapshot \
                  src/extra/themes/basic \
                  src/extra/themes/default \
                  src/extra/themes/mono \
                  src/extra/widgets/animimg \
                  src/extra/widgets/calendar \
                  src/extra/widgets/chart \
                  src/extra/widgets/colorwheel \
                  src/extra/widgets/imgbtn \
                  src/extra/widgets/keyboard \
                  src/extra/widgets/led \
                  src/extra/widgets/list \
                  src/extra/widgets/menu \
                  src/extra/widgets/meter \
                  src/extra/widgets/msgbox \
                  src/extra/widgets/span \
                  src/extra/widgets/spinbox \
                  src/extra/widgets/spinner \
                  src/extra/widgets/tabview \
                  src/extra/widgets/tileview \
                  src/extra/widgets/win

ifeq ($(CONFIG_LV_USE_THORVG_INTERNAL),y)
COMPONENT_SRCDIRS += src/extra/libs/thorvg 
endif

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS) .
