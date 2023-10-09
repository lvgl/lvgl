#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: Image - test loading from file and array


#test-objects:

lv_testimage = lv.image( lv.scr_act() )
lv_testlabel = lv.label( lv_testimage )

LV_TESTIMAGE_FILENAME = lv_test.FOLDER + "assets/image_cogwheel_argb.png"
try:
    with open( LV_TESTIMAGE_FILENAME, 'rb' ) as f: lv_pngdata = f.read()
except:
    print( "Could not find", LV_TESTIMAGE_FILENAME )
    sys.exit( lv_const.ERROR_TESTCASE_FAILED )

lv_imagedata = lv.image_dsc_t ({ 'data_size': len(lv_pngdata), 'data': lv_pngdata })


lv_testimage2 = lv.image( lv.scr_act() )
lv_testlabel2 = lv.label( lv_testimage2 )
lv_testlabel2.set_text("")

lv_bitmap = bytearray( [ 0x23,0x45,0x67, 0x23,0x45,0x67,  0x23,0x45,0x67,  0x23,0x45,0x67 ] )

lv_imageheader2 = lv.image_header_t( { 'always_zero':0, 'w':4, 'h':4, 'cf':lv.COLOR_FORMAT.RGB888  } )

lv_imagedata2 = lv.image_dsc_t ({ 'header': lv_imageheader2, 'data_size': len(lv_bitmap), 'data': lv_bitmap })

lv_testimage2.set_y(200)


#tests:

lv_testimage.set_src( lv_imagedata )
lv_testlabel.set_align( lv.ALIGN.CENTER )
lv_test.wait( lv_const.TRANSITION_WAIT_TIME )

lv_test.assert_greater( 0, lv_testlabel.get_x(), "Image-loading from PNG-file " + LV_TESTIMAGE_FILENAME )


lv_testimage2.set_src( lv_imagedata2 )
lv_testlabel2.set_align( lv.ALIGN.CENTER )
lv_test.wait( lv_const.TRANSITION_WAIT_TIME )

lv_test.assert_greater( 0, lv_testlabel2.get_x(), "Image-loading from built-in array" )

