ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

define PINFO
PINFO DESCRIPTION = Light and Versatile Graphics Library
endef
INSTALLDIR=
NAME=lvgl
USEFILE=

SRC_ROOT=$(PROJECT_ROOT)/../../src
EXTRA_SRCVPATH=$(SRC_ROOT) \
$(SRC_ROOT)/libs \
$(SRC_ROOT)/libs/bmp \
$(SRC_ROOT)/libs/libjpeg_turbo \
$(SRC_ROOT)/libs/fsdrv \
$(SRC_ROOT)/libs/libpng \
$(SRC_ROOT)/libs/bin_decoder \
$(SRC_ROOT)/libs/tiny_ttf \
$(SRC_ROOT)/libs/barcode \
$(SRC_ROOT)/libs/rlottie \
$(SRC_ROOT)/libs/qrcode \
$(SRC_ROOT)/libs/lz4 \
$(SRC_ROOT)/libs/ffmpeg \
$(SRC_ROOT)/libs/tjpgd \
$(SRC_ROOT)/libs/thorvg \
$(SRC_ROOT)/libs/thorvg/rapidjson \
$(SRC_ROOT)/libs/thorvg/rapidjson/internal \
$(SRC_ROOT)/libs/thorvg/rapidjson/error \
$(SRC_ROOT)/libs/lodepng \
$(SRC_ROOT)/libs/rle \
$(SRC_ROOT)/libs/gif \
$(SRC_ROOT)/libs/freetype \
$(SRC_ROOT)/draw \
$(SRC_ROOT)/draw/vg_lite \
$(SRC_ROOT)/draw/sw \
$(SRC_ROOT)/draw/sw/arm2d \
$(SRC_ROOT)/draw/sw/blend \
$(SRC_ROOT)/draw/sw/blend/helium \
$(SRC_ROOT)/draw/sw/blend/arm2d \
$(SRC_ROOT)/draw/sw/blend/neon \
$(SRC_ROOT)/misc \
$(SRC_ROOT)/misc/cache \
$(SRC_ROOT)/font \
$(SRC_ROOT)/stdlib \
$(SRC_ROOT)/stdlib/builtin \
$(SRC_ROOT)/stdlib/rtthread \
$(SRC_ROOT)/stdlib/clib \
$(SRC_ROOT)/stdlib/micropython \
$(SRC_ROOT)/drivers \
$(SRC_ROOT)/drivers/qnx \
$(SRC_ROOT)/themes \
$(SRC_ROOT)/themes/simple \
$(SRC_ROOT)/themes/mono \
$(SRC_ROOT)/themes/default \
$(SRC_ROOT)/display \
$(SRC_ROOT)/indev \
$(SRC_ROOT)/core \
$(SRC_ROOT)/tick \
$(SRC_ROOT)/others \
$(SRC_ROOT)/others/monkey \
$(SRC_ROOT)/others/ime \
$(SRC_ROOT)/others/snapshot \
$(SRC_ROOT)/others/file_explorer \
$(SRC_ROOT)/others/imgfont \
$(SRC_ROOT)/others/fragment \
$(SRC_ROOT)/others/observer \
$(SRC_ROOT)/others/vg_lite_tvg \
$(SRC_ROOT)/others/sysmon \
$(SRC_ROOT)/others/gridnav \
$(SRC_ROOT)/widgets \
$(SRC_ROOT)/widgets/objx_templ \
$(SRC_ROOT)/widgets/tabview \
$(SRC_ROOT)/widgets/scale \
$(SRC_ROOT)/widgets/checkbox \
$(SRC_ROOT)/widgets/slider \
$(SRC_ROOT)/widgets/calendar \
$(SRC_ROOT)/widgets/bar \
$(SRC_ROOT)/widgets/win \
$(SRC_ROOT)/widgets/dropdown \
$(SRC_ROOT)/widgets/switch \
$(SRC_ROOT)/widgets/span \
$(SRC_ROOT)/widgets/canvas \
$(SRC_ROOT)/widgets/lottie \
$(SRC_ROOT)/widgets/textarea \
$(SRC_ROOT)/widgets/arc \
$(SRC_ROOT)/widgets/msgbox \
$(SRC_ROOT)/widgets/property \
$(SRC_ROOT)/widgets/chart \
$(SRC_ROOT)/widgets/table \
$(SRC_ROOT)/widgets/list \
$(SRC_ROOT)/widgets/button \
$(SRC_ROOT)/widgets/image \
$(SRC_ROOT)/widgets/line \
$(SRC_ROOT)/widgets/animimage \
$(SRC_ROOT)/widgets/roller \
$(SRC_ROOT)/widgets/spinner \
$(SRC_ROOT)/widgets/imagebutton \
$(SRC_ROOT)/widgets/led \
$(SRC_ROOT)/widgets/spinbox \
$(SRC_ROOT)/widgets/keyboard \
$(SRC_ROOT)/widgets/buttonmatrix \
$(SRC_ROOT)/widgets/menu \
$(SRC_ROOT)/widgets/label \
$(SRC_ROOT)/widgets/tileview \
$(SRC_ROOT)/layouts \
$(SRC_ROOT)/layouts/grid \
$(SRC_ROOT)/layouts/flex \
$(SRC_ROOT)/osal

PRE_TARGET=$(PROJECT_ROOT)/lv_conf.h

include $(MKFILES_ROOT)/qtargets.mk

# Generate the lv_conf.h file from the template
$(PROJECT_ROOT)/lv_conf.h: $(PROJECT_ROOT)/../../lv_conf_template.h
	cp $< $@
	sed -i -e "s/#if 0/#if 1/" $@
	sed -i -e "s/#define LV_COLOR_DEPTH 16/#define LV_COLOR_DEPTH 32/" $@
	sed -i -e "s/#define LV_USE_QNX.*/#define LV_USE_QNX 1/" $@
	sed -i -e "s/#define LV_QNX_BUF_COUNT.*/#define LV_QNX_BUF_COUNT 2/" $@
