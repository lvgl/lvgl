#!/bin/bash
# Version: 1.1
# Date: 2022-01-11
# This bash script generates a CMSIS Software Pack:
#
# Pre-requisites:
# - bash shell (for Windows: install git for Windows)
# - 7z in path (zip archiving utility)
#   e.g. Ubuntu: sudo apt-get install p7zip-full p7zip-rar)
# - PackChk in path with execute permission
#   (see CMSIS-Pack: CMSIS/Utilities/<os>/PackChk)
# - xmllint in path (XML schema validation)
#   e.g. Ubuntu: sudo apt-get install libxml2-utils
#   Windows: download from https://www.zlatkovic.com/pub/libxml/

############### EDIT BELOW ###############
# Extend Path environment variable locally
#
if [ `uname -s` = "Linux" ]
  then
  CMSIS_PACK_PATH="/home/$USER/.arm/Packs/ARM/CMSIS/5.7.0/"
  PATH_TO_ADD="$CMSIS_PACK_PATH/CMSIS/Utilities/Linux64/"
else
  CMSIS_PACK_PATH="/C/Users/$USER/AppData/Local/Arm/Packs/ARM/CMSIS/5.7.0"
  PATH_TO_ADD="/C/Program Files (x86)/7-Zip/:/C/Program Files/7-Zip/:$CMSIS_PACK_PATH/CMSIS/Utilities/Win32/:/C/xmllint/"
fi
[[ ":$PATH:" != *":$PATH_TO_ADD}:"* ]] && PATH="${PATH}:${PATH_TO_ADD}"
echo $PATH_TO_ADD appended to PATH
echo " "

# Pack warehouse directory - destination
PACK_WAREHOUSE=./

# Temporary pack build directory
PACK_BUILD=build/

# Specify directories included in pack relative to base directory
# All directories:
# PACK_DIRS=`ls -d */`
# Do not include the build directory if it is local
# PACK_DIRS=${PACK_DIRS//$PACK_BUILD/}
# PACK_DIRS=${PACK_DIRS//$PACK_WAREHOUSE/}

# alternative: specify directory names to be added to pack base directory
PACK_DIRS="
  ../../src
  ../../docs
  ../../demos
"


# Specify file names to be added to pack base directory
PACK_BASE_FILES="
  ../../LICENCE.txt
  ../../README.md
  ../../README_zh.md
  ../../lvgl.h
  lv_conf_cmsis.h
  lv_cmsis_pack.txt
"

############ DO NOT EDIT BELOW ###########
echo Starting CMSIS-Pack Generation: `date`
# Zip utility check
ZIP=7z
type -a $ZIP
errorlevel=$?
if [ $errorlevel -gt 0 ]
  then
  echo "Error: No 7zip Utility found"
  echo "Action: Add 7zip to your path"
  echo " "
  exit
fi

# Pack checking utility check
PACKCHK=PackChk
type -a $PACKCHK
errorlevel=$?
if [ $errorlevel != 0 ]
  then
  echo "Error: No PackChk Utility found"
  echo "Action: Add PackChk to your path"
  echo "Hint: Included in CMSIS Pack:"
  echo "<pack_root_dir>/ARM/CMSIS/<version>/CMSIS/Utilities/<os>/"
  echo " "
  exit
fi
echo " "

# XML syntax checking utility check
XMLLINT=xmllint
type -a $XMLLINT
errorlevel=$?
if [ $errorlevel != 0 ]
  then
  echo "Error: No xmllint found"
  echo "Action: Add xmllint to your path"
  echo " "
  exit
fi
echo " "

# Locate Package Description file
# check whether there is more than one pdsc file
NUM_PDSCS=`ls -1 *.pdsc | wc -l`
PACK_DESCRIPTION_FILE=`ls *.pdsc`
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
echo Generating Pack Version: for $PACK_VENDOR.$PACK_NAME
echo " "
IFS=$SAVEIFS

#if $PACK_BUILD directory does not exist, create it.
if [ ! -d $PACK_BUILD ]; then
  mkdir -p $PACK_BUILD
fi

mkdir -p ${PACK_BUILD}/examples
mkdir -p ${PACK_BUILD}/examples/porting

# Copy files into build base directory: $PACK_BUILD
# pdsc file is mandatory in base directory:
cp -f  ./$PACK_VENDOR.$PACK_NAME.pdsc ${PACK_BUILD}
cp -f ../../examples/porting/* ${PACK_BUILD}/examples/porting


# directories
echo Adding directories to pack:
echo $PACK_DIRS
echo " "
for d in ${PACK_DIRS}
do
  cp -r "$d" ${PACK_BUILD}
done

# files for base directory
echo Adding files to pack:
echo $PACK_BASE_FILES
echo " "
for f in $PACK_BASE_FILES
do
  cp -f  "$f" $PACK_BUILD/
done

mv "${PACK_BUILD}/lv_cmsis_pack.txt" "${PACK_BUILD}/lv_cmsis_pack.c"

# Run Schema Check (for Linux only):
# sudo apt-get install libxml2-utils

echo Running schema check for $PACK_VENDOR.$PACK_NAME.pdsc
$XMLLINT --noout --schema ${CMSIS_PACK_PATH}/CMSIS/Utilities/PACK.xsd $PACK_BUILD/$PACK_VENDOR.$PACK_NAME.pdsc
errorlevel=$?
if [ $errorlevel -ne 0 ]; then
  echo "build aborted: Schema check of $PACK_VENDOR.$PACK_NAME.pdsc against PACK.xsd failed"
  echo " "
  exit
fi

# Run Pack Check and generate PackName file with version
$PACKCHK $PACK_BUILD/$PACK_VENDOR.$PACK_NAME.pdsc -n PackName.txt -x M362
errorlevel=$?
if [ $errorlevel -ne 0 ]; then
  echo "build aborted: pack check failed"
  echo " "
  exit
fi

PACKNAME=`cat PackName.txt`
rm -rf PackName.txt

echo remove unrequired files and folders...
rm -rf $PACK_BUILD/demos/keypad_encoder
rm -rf $PACK_BUILD/demos/music
rm -rf $PACK_BUILD/demos/stress
rm -rf $PACK_BUILD/demos/widgets/screenshot1.gif

# echo apply patches...
# rm -rf $PACK_BUILD/demos/lv_demos.h
# cp -f ./lv_demos.h $PACK_BUILD/demos/

# Archiving
# $ZIP a $PACKNAME
echo creating pack file $PACKNAME
#if $PACK_WAREHOUSE directory does not exist create it
if [ ! -d $PACK_WAREHOUSE ]; then
  mkdir -p $PACK_WAREHOUSE
fi
pushd $PACK_WAREHOUSE
PACK_WAREHOUSE=`pwd`
popd
pushd $PACK_BUILD
"$ZIP" a $PACK_WAREHOUSE/$PACKNAME -tzip
popd
errorlevel=$?
if [ $errorlevel -ne 0 ]; then
  echo "build aborted: archiving failed"
  exit
fi

# cp -f ./$PACK_VENDOR.$PACK_NAME.pdsc ${PACK_WAREHOUSE}


echo "build of pack succeeded"
# Clean up
echo "cleaning up ..."

rm -rf $PACK_BUILD

echo " "

echo Completed CMSIS-Pack Generation: `date`