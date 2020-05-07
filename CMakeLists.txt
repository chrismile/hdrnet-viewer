cmake_minimum_required (VERSION 3.1)
project (hdrnetviewer)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
#MESSAGE(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

set (CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
include_directories(src)

add_executable(hdrnetviewer ${SOURCES})

#make VERBOSE=1

cmake_policy(SET CMP0012 NEW)
find_package(SGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(TensorflowCC REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
target_link_libraries(hdrnetviewer GL)
target_link_libraries(hdrnetviewer ${OpenCV_LIBS})
target_link_libraries(hdrnetviewer TensorflowCC::TensorflowCC)
target_link_libraries(hdrnetviewer sgl)

include_directories(${Boost_INCLUDES} ${GLEW_INCLUDES} ${TensorflowCC_INCLUDES})
