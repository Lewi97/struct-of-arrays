﻿# CMakeList.txt : CMake project for StructofArrays, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (StructofArrays "StructofArrays.cpp" "StructofArrays.h" "Iterable.h"  "TupleTools.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET StructofArrays PROPERTY CXX_STANDARD 23)
endif()

# TODO: Add tests and install targets if needed.
