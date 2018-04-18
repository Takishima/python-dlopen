cmake_minimum_required(VERSION 2.8.7)

project(python-dlopen CXX)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_LIST_DIR})
add_library(python-dlopen PythonFuncHandler.cpp)

# ------------------------------------------------------------------------------

include(CTest)
enable_testing()

if(BUILD_TESTING)
  add_executable(main_test ${CMAKE_CURRENT_LIST_DIR}/test/main.cpp)
  target_link_libraries(main_test python-dlopen ${PYTHON_LIBRARIES} ${CMAKE_DL_LIBS})

  add_test(
    NAME try_run
    COMMAND main_test ${PYTHON_LIBRARIES})
endif(BUILD_TESTING)

