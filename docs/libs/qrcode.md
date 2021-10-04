```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/libs/qrcode.md
```

# QR code

QR code generation with LVGL. Uses [QR-Code-generator](https://github.com/nayuki/QR-Code-generator) by [nayuki](https://github.com/nayuki).

## Get started
- Download or clone this repository
  - [Download](https://github.com/littlevgl/lv_lib_qrcode.git) from GitHub
  - Clone: git clone https://github.com/littlevgl/lv_lib_qrcode.git
- Include the library: `#include "lv_lib_qrcode/lv_qrcode.h"`
- Test with the following code:
```c
const char * data = "Hello world";

/*Create a 100x100 QR code*/
lv_obj_t * qr = lv_qrcode_create(lv_scr_act(), 100, lv_color_hex3(0x33f), lv_color_hex3(0xeef));

/*Set data*/
lv_qrcode_update(qr, data, strlen(data));
```

## Notes
- QR codes with less data are smaller but they scaled by an integer numbers number to best fit to the given size

