cmake_minimum_required (VERSION 2.6)
project (school)

set(school_VERSION_MAJOR 0)
set(school_VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_SOURCE_DIR}/config.h"
  )

add_executable (multiplication multiplication.cxx)

