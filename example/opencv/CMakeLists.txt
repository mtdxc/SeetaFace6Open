
cmake_minimum_required(VERSION 2.8)
project(sf6Demo)

#set(OpenCV_DIR D:\\opencv\\opencv\\build\\install) # xxxx目录包含OpenCVConfig.cmake
find_package(OpenCV REQUIRED)
set(Build_DIR ${CMAKE_SOURCE_DIR}/../../build)
include_directories(${OpenCV_INCLUDE_DIRS} ${Build_DIR}/include)
link_directories(${Build_DIR}/lib/x64)
set(EXECUTABLE_OUTPUT_PATH ${Build_DIR}/bin/x64)

add_executable(${PROJECT_NAME} main.cpp FaceEngine2.cpp)	# *.cpp指要编译的那些源文件
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
