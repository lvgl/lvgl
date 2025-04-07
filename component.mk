# ESP-IDF component file for make based commands

COMPONENT_SRCDIRS := . \
                  src \
                  src/core \
                  src/extra \
                  src/font \
                  src/hal \
                  src/tick \
                  src/display \
                  src/indev \
                  src/osal \
                  src/misc \
                  src/misc/cache \
                  src/draw \
                  src/draw/arm2d \
                  src/draw/nxp \
                  src/draw/sdl \
                  src/draw/stm32_dma2d \
                  src/draw/sw \
                  src/draw/sw/blend \
                  src/draw/swm342_dma2d \
                  src/others \
                  src/layouts \
                  src/layouts/flex \
                  src/layouts/grid \
                  src/libs \
                  src/libs/bin_decoder \
                  src/libs/bmp \
                  src/libs/ffmpeg \
                  src/libs/freetype \
                  src/libs/fsdrv \
                  src/libs/gif \
                  src/libs/libpng \
                  src/libs/lodepng \
                  src/libs/png \
                  src/libs/qrcode \
                  src/libs/rlottie \
                  src/libs/rle \
                  src/libs/sjgp \
                  src/libs/tiny_ttf \
                  src/libs/tjpgd \
                  src/libs/libjpeg_turbo \
                  src/others/fragment \
                  src/others/gridnav \
                  src/others/ime \
                  src/others/imgfont \
                  src/others/monkey \
                  src/others/msg \
                  src/others/observer   \
                  src/others/snapshot \
                  src/others/sysmon \
                  src/others/test \
                  src/themes \
                  src/themes/basic \
                  src/themes/default \
                  src/themes/mono \
                  src/themes/simple \
                  src/widgets \
                  src/widgets/animimg \
                  src/widgets/button \
                  src/widgets/buttonmatrix \
                  src/widgets/calendar \
                  src/widgets/canvas \
                  src/widgets/chart \
                  src/widgets/colorwheel \
                  src/widgets/dropdown \
                  src/widgets/image \
                  src/widgets/imgbtn \
                  src/widgets/keyboard \
                  src/widgets/label \
                  src/widgets/led \
                  src/widgets/list \
                  src/widgets/menu \
                  src/widgets/meter \
                  src/widgets/msgbox \
                  src/widgets/span \
                  src/widgets/spinbox \
                  src/widgets/spinner \
                  src/widgets/tabview \
                  src/widgets/textarea \
                  src/widgets/tileview \
                  src/widgets/win     \
                  src/stdlib \
                  src/stdlib/builtin

ifeq ($(CONFIG_LV_USE_THORVG_INTERNAL),y)
COMPONENT_SRCDIRS += src/libs/thorvg
endif

COMPONENT_ADD_INCLUDEDIRS := $(COMPONENT_SRCDIRS) .
