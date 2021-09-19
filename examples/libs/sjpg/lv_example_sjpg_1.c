#include "../../lv_examples.h"
#if LV_USE_SJPG && LV_BUILD_EXAMPLES

LV_IMG_DECLARE( small_image_sjpg );
LV_IMG_DECLARE( wallpaper_jpg );

void demo_jpg_sjpg( void )
{
  lv_fs_if_init();
  lv_obj_t * img1;
  lv_obj_t * img2;

  lv_split_jpeg_init();
  img1 = lv_img_create(lv_scr_act());
  img2 = lv_img_create(lv_scr_act());

  //jpg from c array
  lv_img_set_src( img1,  &wallpaper_jpg);

  //sjpg from file (with lv_fs)
  //On Windows
  lv_img_set_src(img2,  "S.\\lv_lib_split_jpg\\example\\images\\small_image.sjpg");

  //On Linux
  //lv_img_set_src(img2,  "S/lv_lib_split_jpg/example/images/small_image.sjpg");
}

#endif
