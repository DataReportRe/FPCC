cmake_minimum_required(VERSION 2.8.8)
set(PROJECT_NAME_STR direct_opt)
set(PROJECT_SOURCE_DIR /home/star001/projects/dockerfiles/softTar/DiRect/src)
project(${PROJECT_NAME_STR} C CXX)

if(CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wno-deprecated)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static -O3")
endif()

if(MSVC)
  # TODO: add optimization options for msvc
  # vc 2012 fix for vararg templates
  add_definitions("-D_VARIADIC_MAX=10")
endif()

set(SCL_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin CACHE STRING "Output directory")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${SCL_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SCL_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${SCL_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SCL_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${SCL_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SCL_OUTPUT_DIRECTORY})

# set common include folder for module
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/src)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.c)
#add_executable(${PROJECT_NAME_STR}_test ${SRC_FILES})
#target_link_libraries(${PROJECT_NAME_STR}_test PRIVATE m)

#add_library(direct_opt SHARED ${SRC_FILES})
add_library(direct_opt STATIC ${SRC_FILES})
target_link_libraries(${PROJECT_NAME_STR} PRIVATE m)
set(CMAKE_INSTALL_PREFIX /opt/DiRect/)
install(TARGETS ${PROJECT_NAME_STR} DESTINATION lib)
