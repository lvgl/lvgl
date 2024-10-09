#!/bin/bash
#Create lv_i18n.c and lv_i18n.h from the yml files.


lv_i18n compile -t "./*.yml" -o .
