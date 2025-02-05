execute_process(
  COMMAND ${PYTHON_EXECUTABLE}
  ${LVGL_ROOT_DIR}/scripts/kconfig.py
  ${LVGL_ROOT_DIR}/Kconfig
  ${OUTPUT_DOTCONFIG}
  ${AUTOCONF_H}
  ${KCONFIG_LIST_OUT}
  ${DOTCONFIG}
  WORKING_DIRECTORY ${LVGL_ROOT_DIR}
  # The working directory is set to the app dir such that the user
  # can use relative paths in CONF_FILE, e.g. CONF_FILE=nrf5.conf
  RESULT_VARIABLE ret
  )
if(NOT "${ret}" STREQUAL "0")
  message(FATAL_ERROR "command failed with return code: ${ret}")
endif()

import_kconfig(CONFIG_ ${DOTCONFIG} keys)
