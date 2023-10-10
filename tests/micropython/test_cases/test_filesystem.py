#! ../../../../../../../lv_micropython/ports/unix/build-standard/micropython ../run_testcase.py
#LVGL MicroPython binding tester script: FileSystem - file-creation/writing/reading by lv.fs_*()

import fs_driver


LV_TESTFILE_NAME = lv_test.FOLDER + "assets/testfile"


#test-objects:

lv_testdriver = lv.fs_drv_t()
fs_driver.fs_register(lv_testdriver, 'S')

lv_testfile = lv.fs_file_t()

lv_testlist = [ 0x12, 0x34, 0x56, 0x78 ]
lv_testarray = bytearray( lv_testlist )
LV_TESTLIST_SIZE = len(lv_testarray)

lv_readback_list = [ 0 ] * LV_TESTLIST_SIZE
lv_readback_array = bytearray(lv_readback_list)
lv_match_count = 0


#tests:

try: os.remove(LV_TESTFILE_NAME) #delete before creation to be sure the file is really created
except: pass
lv_filestatus = lv_testfile.open( "S:" + LV_TESTFILE_NAME, lv.FS_MODE.WR )

lv_test.assert_equal( lv.FS_RES.OK, lv_filestatus, "File-creation (with open)" )


lv_written_bytes = [0]
lv_filestatus = lv_testfile.write( lv_testarray , LV_TESTLIST_SIZE, bytearray(lv_written_bytes) )

lv_test.assert_equal( lv.FS_RES.OK, lv_filestatus, "Writing into file (status)" )
#lv_test.assert_equal( LV_TESTLIST_SIZE, bytearray(lv_written_bytes)[0], "Writing into file (count)" )

lv_testfile.close()


lv_filestatus = lv_testfile.open( "S:" + LV_TESTFILE_NAME, lv.FS_MODE.RD )

lv_test.assert_equal( lv.FS_RES.OK, lv_filestatus, "Opening file for reading" )


lv_read_bytes = 0
lv_filestatus = lv_testfile.read( lv_readback_array, LV_TESTLIST_SIZE, bytearray(lv_read_bytes) )

lv_test.assert_equal( lv.FS_RES.OK, lv_filestatus, "Reading back data from file (status)" )
#lv_test.assert_equal( LV_TESTLIST_SIZE, lv_read_bytes, "Reading back data from file (count)" )

lv_testfile.close()


for i in range(LV_TESTLIST_SIZE):
    if list(lv_readback_array)[i] == lv_testlist[i]: lv_match_count += 1

lv_test.assert_equal( lv_match_count, LV_TESTLIST_SIZE, "Match-count of data that was read back" )

