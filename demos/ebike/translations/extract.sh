#!/bin/bash
# Get the _("...") strings from the c files add add them to the yml files

lv_i18n extract -s '../src/**/*.c' -t './*.yml'

