cmake_minimum_required(VERSION 3.12)

project(python-dlopen LANGUAGES CXX)

find_package(Python2 2.7 COMPONENTS Interpreter Development)
find_package(Python3 3.0 COMPONENTS Interpreter Development)

if(Python2_FOUND)
  add_library(python2-dlopen PythonFuncHandler.cpp)
  target_include_directories(python2-dlopen PUBLIC ${CMAKE_CURRENT_LIST_DIR})
  target_link_libraries(python2-dlopen PUBLIC Python2::Module)
endif()

if(Python3_FOUND)
  add_library(python3-dlopen PythonFuncHandler.cpp)
  target_include_directories(python3-dlopen PUBLIC ${CMAKE_CURRENT_LIST_DIR})
  target_link_libraries(python3-dlopen PUBLIC Python3::Module)
endif()

# ------------------------------------------------------------------------------

add_custom_target(test)

if(Python2_FOUND)
  add_executable(main_test_py2 ${CMAKE_CURRENT_LIST_DIR}/test/main.cpp)
  target_link_libraries(main_test_py2 PUBLIC python2-dlopen ${CMAKE_DL_LIBS})

  add_custom_target(try_run_py2 COMMAND main_test_py2 ${Python2_LIBRARIES})
  add_dependencies(test try_run_py2)
endif()

if(Python3_FOUND)
  add_executable(main_test_py3 ${CMAKE_CURRENT_LIST_DIR}/test/main.cpp)
  target_link_libraries(main_test_py3 PUBLIC python3-dlopen ${CMAKE_DL_LIBS})
  add_custom_target(try_run_py3 COMMAND main_test_py3 ${Python3_LIBRARIES})
  add_dependencies(test try_run_py3)
endif()
