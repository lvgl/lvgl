#!/opt/bin/lv_micropython-ffmpeg -i
import sys
import lvgl as lv
import display_driver

#
# Open a video from a file
#

# birds.mp4 is downloaded from http://www.videezy.com (Free Stock Footage by Videezy!)
# https://www.videezy.com/abstract/44864-silhouettes-of-birds-over-the-sunset
player = lv.ffmpeg_player(lv.scr_act())
player.player_set_src("birds.mp4")
player.player_set_auto_restart(True)
player.player_set_cmd(lv.ffmpeg_player.PLAYER_CMD.START)
# player.player_set_cmd(0)
player.center()
