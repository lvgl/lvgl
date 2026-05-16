find_library(G2D_LIBRARY NAMES g2d QUIET)
find_path(G2D_INCLUDE_DIR NAMES g2d.h g2dExt.h QUIET)

if(G2D_LIBRARY AND G2D_INCLUDE_DIR)
  message(STATUS "lvgl: G2D: found at ${G2D_LIBRARY}")
  target_link_libraries(lvgl PRIVATE ${G2D_LIBRARY})
  target_include_directories(lvgl PRIVATE ${G2D_INCLUDE_DIR})
  return()
endif()

message(FATAL_ERROR "lvgl: G2D not found. " "G2D_LIBRARY=${G2D_LIBRARY} "
                    "G2D_INCLUDE_DIR=${G2D_INCLUDE_DIR}")
