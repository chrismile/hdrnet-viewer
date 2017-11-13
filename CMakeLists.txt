cmake_minimum_required (VERSION 2.6)
project (hdrnetviewer)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
#MESSAGE(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
add_executable(hdrnetviewer ${SOURCES})
include_directories(src)

#make VERBOSE=1

find_package(SGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(TensorFlow REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
target_link_libraries(hdrnetviewer GL)
target_link_libraries(hdrnetviewer ${OpenCV_LIBS})
target_link_libraries(hdrnetviewer ${TF_LIBRARIES})
target_link_libraries(hdrnetviewer protobuf)
target_link_libraries(hdrnetviewer sgl)
