find_library(G2D_LIBRARY NAMES g2d QUIET)
find_path(G2D_INCLUDE_DIR NAMES g2d.h g2dExt.h QUIET)

if(G2D_LIBRARY AND G2D_INCLUDE_DIR)
  message(STATUS "lvgl: G2D: found at ${G2D_LIBRARY}")
  # Create an interface target for G2D since it's found manually
  add_library(G2D::G2D INTERFACE IMPORTED)
  set_target_properties(
    G2D::G2D PROPERTIES INTERFACE_LINK_LIBRARIES "${G2D_LIBRARY}"
                        INTERFACE_INCLUDE_DIRECTORIES "${G2D_INCLUDE_DIR}")
  lvgl_link_libraries(PRIVATE TARGETS G2D::G2D)
  return()
endif()

message(FATAL_ERROR "lvgl: G2D not found. " "G2D_LIBRARY=${G2D_LIBRARY} "
                    "G2D_INCLUDE_DIR=${G2D_INCLUDE_DIR}")
