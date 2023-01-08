#!/bin/bash
# Version: 1.0.0
# Date: 2023-01-05
# This bash script check the content of the CMSIS Software Pack:
# * The script will start by checking for missing files inside the PDSC file
#   - File extensions are: *.c, *.h, *.md, *.png and *.gif
#   - PACK_EXCLUDED_DIRS is used to ignore directories from the search
#   - PACK_EXCLUDED_FILES is used to ignore specific files from the search
#   - results/ignored_files.txt will be generated for ignored files
#   - results/missing_files.txt will be generated for missing files
#
# * Then it will check for missing defines inside the PDSC file
#   - Defines are starting with 'LV_USE_'
#   - PACK_EXCLUDED_DEFINES is used to ignore specific defines from the search
#   - results/ignored_defines.txt will be generated for ignored defines
#   - results/missing_defines.txt will be generated for missing defines
#
# Missing files and defines need to be either implemented inside the PDSC or added
# to the EXCLUDED arrays
#
# Pre-requisites:
# - bash shell (for Windows: install git for Windows)

############### EDIT BELOW ###############
# DEBUG TRACES
if [ -z $DEBUG_TRACE ]; then
  declare DEBUG_TRACE="n"
fi

# Outputs directory
CWD=`pwd`
declare OUT_DIR=$CWD/results

# Specify directories included in pack relative to base directory
declare -A PACK_DIRS="../../src
../../demos
../../env_support/pikascript
"

# Specify directories included in pack relative to base directory
declare -A PACK_FILES="../../LICENCE.txt
../../README.md
../../lvgl.h
../../lv_conf.h
../../lv_cmsis_pack.c
../../src/lv_conf_internal.h
../../src/lv_conf_kconfig.h
../../src/lv_api_map.h
"

# Specify directories excluded from pack relative to base directory
declare -A PACK_EXCLUDED_DIRS="../../src/draw/sdl
../../demos/benchmark/assets
../../demos/widgets/assets
"

# Specify files excluded from the pack relative to base directory
declare -A PACK_EXCLUDED_FILES="../../src/libs/fsdrv/lv_fs_win32.c
../../demos/README.md
../../demos/widgets/assets/avatar.png
"

# Specify defines excluded from the pack
declare -A PACK_EXCLUDED_DEFINES="LV_USE_BUILTIN_MALLOC \
LV_USE_BUILTIN_MEMCPY \
LV_USE_BUILTIN_SNPRINTF \
LV_USE_DRAW_MASKS \
LV_USE_DRAW_SW \
LV_USE_DRAW_SDL \
LV_USE_GPU_NXP_PXP_AUTO_INIT \
LV_USE_LOG \
LV_USE_ASSERT_NULL \
LV_USE_ASSERT_MALLOC \
LV_USE_ASSERT_STYLE \
LV_USE_ASSERT_MEM_INTEGRITY \
LV_USE_ASSERT_OBJ \
LV_USE_PERF_MONITOR \
LV_USE_PERF_MONITOR_POS \
LV_USE_MEM_MONITOR \
LV_USE_MEM_MONITOR_POS \
LV_USE_REFR_DEBUG \
LV_USE_USER_DATA \
LV_USE_LARGE_COORD \
LV_USE_FONT_COMPRESSED \
LV_USE_FONT_PLACEHOLDER \
LV_USE_BIDI \
LV_USE_ARABIC_PERSIAN_CHARS \
LV_USE_CALENDAR_HEADER_ARROW \
LV_USE_CALENDAR_HEADER_DROPDOWN \
"

############ DO NOT EDIT BELOW ###########
# Locate Package Description file
# check whether there is more than one pdsc file
NUM_PDSCS=`ls -1 *.pdsc | wc -l`
PACK_DESCRIPTION_FILE=`ls *.pdsc`
LV_CONF_FILE=./lv_conf_cmsis.h
PACK_PDSC_FILE=./$PACK_VENDOR.$PACK_NAME.pdsc
if [ $NUM_PDSCS -lt 1 ]
  then
  echo "Error: No *.pdsc file found in current directory"
  echo " "
elif [ $NUM_PDSCS -gt 1 ]
  then
  echo "Error: Only one PDSC file allowed in directory structure:"
  echo "Found:"
  echo "$PACK_DESCRIPTION_FILE"
  echo "Action: Delete unused pdsc files"
  echo " "
  exit
fi

SAVEIFS=$IFS
IFS=.
set $PACK_DESCRIPTION_FILE
# Pack Vendor
PACK_VENDOR=$1
# Pack Name
PACK_NAME=$2
echo Checking content of $PACK_VENDOR.$PACK_NAME CMSIS pack
echo " "
IFS=$SAVEIFS

# Spin chars
sp="/-\|"
sc=0
function spin() {
   printf "\b${sp:sc++:1}"
   ((sc==${#sp})) && sc=0
}

function endspin() {
   printf "\b \r"
   sc=0
}

# Debug Traces
function debug_trace() {
  if [[ "${DEBUG_TRACE}" == "y" ]]; then
    printf "$*"
  fi
}

############################
# Check PDSC files         #
############################
for d in $PACK_EXCLUDED_DIRS
do
    PACK_EXCLUDED_FILES+=`find $d -type f`
done

function check_pack_files ()
{
  local file_count=0
  local found_count=0
  local ignore_count=0
  local missing_count=0
  [ ! -d "$OUT_DIR" ] && mkdir -p $OUT_DIR
  echo "" > $OUT_DIR/ignored_files.txt
  echo "" > $OUT_DIR/missing_files.txt

  for d in $PACK_DIRS
  do
    local files=`find $d -type f \( -iname \*.c -o -iname \*.h -o -iname \*.md -o -iname \*.png -o -iname \*.gif \)`
    files+="${PACK_FILES}"
    for f in $files
    do
      ((file_count++))
      file_name="${f#../../}"
      spin
      if fgrep -q $file_name $PACK_PDSC_FILE
      then
        ((found_count++))
      elif [[ "${PACK_EXCLUDED_FILES[*]}" =~ (^|[^[:alpha:]])$f([^[:alpha:]]|$) ]];
      then
          ((ignore_count++))
          endspin
          debug_trace "${file_name} is ignored.\n\r"
          echo "${file_name}" >> $OUT_DIR/ignored_files.txt
      else
          ((missing_count++))
          endspin
          debug_trace "${file_name} is missing !!\n\r"
          echo "${file_name}" >> $OUT_DIR/missing_files.txt
      fi
    done
    endspin
  done
  echo -ne "\r\n\rTotal checked files ${file_count}, found files ${found_count}, missing files ${missing_count}, ignored files ${ignore_count}\n\r"
}

############################
# Check LV_USE_XXX defines #
############################
function check_pack_defines ()
{
  local define_count=0
  local found_count=0
  local ignore_count=0
  local missing_count=0
  [ ! -d "$OUT_DIR" ] && mkdir -p $OUT_DIR
  echo "" > $OUT_DIR/ignored_defines.txt
  echo "" > $OUT_DIR/missing_defines.txt

  local _PACK_DEFINES=`grep -oP "#define LV_USE_\w+" $PACK_PDSC_FILE | cut -d' ' -f2`
  local _CONF_DEFINES=`grep -oP "#define LV_USE_\w+" $LV_CONF_FILE | cut -d' ' -f2`
  local PACK_DEFINES=($_PACK_DEFINES)
  local CONF_DEFINES=($_CONF_DEFINES)

  # echo -ne "\n\rPACK_DEFINES:\n\r"
  # echo -ne "${PACK_DEFINES[@]}\n\r"
  # echo -ne "\n\rCONF_DEFINES:\n\r"
  # echo -ne "${CONF_DEFINES[@]}\n\r"
  # echo -ne "\n\rPACK_EXCLUDED_DEFINES:\n\r"
  # echo -ne "${PACK_EXCLUDED_DEFINES[@]}\n\r"

  for d in "${CONF_DEFINES[@]}"
  do
    ((define_count++))
    spin
    if [[ "${PACK_DEFINES[@]}" =~ "$d " ]];
    then
      ((found_count++))
    elif [[ "${PACK_EXCLUDED_DEFINES[@]}" =~ "$d " ]];
    then
        ((ignore_count++))
        endspin
        debug_trace "${d} is ignored.\n\r"
        echo "${d}" >> $OUT_DIR/ignored_defines.txt
    else
        ((missing_count++))
        endspin
        debug_trace "${d} is missing !!\n\r"
        echo "${d}" >> $OUT_DIR/missing_defines.txt
    fi
  done
  endspin
  echo -ne "\r\n\rTotal checked defines ${define_count}, found defines ${found_count}, missing defines ${missing_count}, ignored defines ${ignore_count}\n\r"
}

echo -ne "\n\rChecking content of CMSIS-Pack: `date`\n\r"
check_pack_files
check_pack_defines
echo -ne "\n\rCompleted CMSIS-Pack check: `date`\n\r"
