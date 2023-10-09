#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: FileSystem - file-creation/writing/reading by lv.fs_*()

import fs_driver

LV_TESTFILE_NAME = lv_test.FOLDER + "assets/testfile"


#test-objects:

lv_testdriver = lv.fs_drv_t()
fs_driver.fs_register(lv_testdriver, 'S')

lv_testfile = lv.fs_file_t()

lv_testlist = [ 12, 34, 56, 78 ]
lv_testarray = bytearray( lv_testlist )
LV_TESTLIST_SIZE = len(lv_testarray)

lv_readback_list = [ 0 ] * LV_TESTLIST_SIZE
lv_readback_array = bytearray(lv_readback_list)
lv_match_count = 0


#tests:

lv_filestatus = lv_testfile.open( "S:" + LV_TESTFILE_NAME, lv.FS_MODE.WR )

lv_test.assert_equal( lv_filestatus, lv.FS_RES.OK, "File-creation (with open)" )


#lv_filestatus = lv_testfile.write( whatpointer? , LV_TESTLIST_SIZE, whatpointer? )

#lv_test.assert_equal( lv_filestatus, lv.FS_RES.OK, "Writing into file" )

lv_testfile.close()


lv_filestatus = lv_testfile.open( "S:" + LV_TESTFILE_NAME, lv.FS_MODE.RD )

lv_test.assert_equal( lv_filestatus, lv.FS_RES.OK, "Opening file for reading" )


#lv_filestatus = lv_testfile.read( whatpointer?, LV_TESTLIST_SIZE, whatpointer? )

#lv_test.assert_equal( lv_filestatus, lv.FS_RES.OK, "Reading back data from file" )

lv_testfile.close()

for i in range(LV_TESTLIST_SIZE):
    if lv_readback_list[i] == lv_testlist[i]: lv_match_count += 1

#lv_test.assert_equal( lv_match_count, LV_TESTLIST_SIZE, "Match-count of data that was read back" )

