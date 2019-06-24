cmake_minimum_required(VERSION 2.8.4)
find_package(OpenCV REQUIRED)


get_filename_component(parent_path ${PROJECT_SOURCE_DIR} PATH)
message(STATUS "git root: ${parent_path}")

set(imgs_path "${parent_path}/imgs")
message(STATUS "imgs dir: ${imgs_path}")

list( APPEND CMAKE_CXX_FLAGS "-std=c++0x")

set(IMGS_FOLDER "${imgs_path}")

configure_file (
  "${PROJECT_SOURCE_DIR}/pathconfig.h.in"
  "${PROJECT_BINARY_DIR}/pathconfig.h"
  )
  
include_directories(${PROJECT_BINARY_DIR})

add_executable(Sobel sobel.cpp)
target_link_libraries(Sobel ${OpenCV_LIBS}) 

add_executable(RobertsCross RobertsCross.cpp)
target_link_libraries(RobertsCross ${OpenCV_LIBS}) 

add_executable(CannyEdgeDetection canny.cpp)
target_link_libraries(CannyEdgeDetection ${OpenCV_LIBS}) 

add_executable(Laplacian Laplacian.cpp)
target_link_libraries(Laplacian ${OpenCV_LIBS}) 

add_executable(Prewitt prewitt.cpp)
target_link_libraries(Prewitt ${OpenCV_LIBS})

add_executable(test test.cpp)
target_link_libraries(test ${OpenCV_LIBS})

add_executable(LoG LoG.cpp)
target_link_libraries(LoG ${OpenCV_LIBS})
