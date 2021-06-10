cmake_minimum_required (VERSION 3.1)
project (hdrnetviewer)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)
#MESSAGE(STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH})

set (CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.c)
include_directories(src)

#if(MSVC)
#	set(CMAKE_CXX_FLAGS "-W3 /EHsc")
#elseif(CMAKE_COMPILER_IS_GNUCXX)
#	set(CMAKE_CXX_FLAGS "-Wall")
#elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#	set(CMAKE_CXX_FLAGS "-Wall")
#endif()

# Prefer GLVND by default when available.
if (${CMAKE_VERSION} VERSION_GREATER "3.11")
    cmake_policy(SET CMP0072 NEW)
endif()

set(DATA_PATH "${CMAKE_SOURCE_DIR}/Data" CACHE PATH "location of folder 'Data'")
add_definitions(-DDATA_PATH=\"${DATA_PATH}\")

if(WIN32)
    add_executable(hdrnetviewer WIN32 ${SOURCES})
else()
    add_executable(hdrnetviewer ${SOURCES})
endif()

#make VERBOSE=1

cmake_policy(SET CMP0012 NEW)
find_package(OpenCV REQUIRED)
find_package(TensorflowCC REQUIRED)
find_package(Boost COMPONENTS system filesystem REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
set(FPHSA_NAME_MISMATCHED TRUE)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(PNG REQUIRED)
find_package(sgl REQUIRED)
if((${CMAKE_GENERATOR} STREQUAL "MinGW Makefiles") OR (${CMAKE_GENERATOR} STREQUAL "MSYS Makefiles"))
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
    target_link_libraries(hdrnetviewer PUBLIC mingw32)
endif()
target_link_libraries(hdrnetviewer SDL2::Main)
target_link_libraries(hdrnetviewer ${Boost_LIBRARIES} ${OPENGL_LIBRARIES} GLEW::GLEW)
target_link_libraries(hdrnetviewer ${OpenCV_LIBS})
target_link_libraries(hdrnetviewer TensorflowCC::TensorflowCC)
target_link_libraries(hdrnetviewer sgl)

include_directories(${sgl_INCLUDES} ${Boost_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDES} ${TensorflowCC_INCLUDES})
