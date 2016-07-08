cmake_minimum_required(VERSION 3.5)
project(VoxWebserver_LIN64)

add_subdirectory(VoxWebAPI.LIN64)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -rdynamic -fpermissive")

set(SOURCE_FILES VoxWebserver/main.cpp VoxWebserver/Processors/ConnectManager.cpp VoxWebserver/Processors/ConnectManager.h VoxWebserver/Loaders/SiteLoader.cpp VoxWebserver/Loaders/SiteLoader.h VoxWebserver/Processors/PacketProcessor.cpp VoxWebserver/Processors/PacketProcessor.h settings.h VoxWebserver/Processors/FileManager.cpp VoxWebserver/Processors/FileManager.h VoxWebserver/Custom/thpool.cpp VoxWebserver/Custom/thpool.h)
add_executable(VoxWebserver_LIN64 ${SOURCE_FILES})

set(Boost_INCLUDE_DIR $ENV{BoostInc})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.61.0 COMPONENTS filesystem system)
find_package(OpenSSL REQUIRED)

include_directories(${OpenSSL_INCLUDE_DIR})
list(APPEND LIB_LIST ${OPENSSL_LIBRARIES})

include_directories(${Boost_INCLUDE_DIR})
include_directories(VoxWebAPI.LIN64/)
target_link_libraries(VoxWebserver_LIN64 ${OPENSSL_LIBRARIES})

#VoxWebBin - path to voxwebapi bin
target_link_libraries(VoxWebserver_LIN64 $ENV{VoxWebBin}/Release/libVoxWebAPI_LIN64.a)
target_link_libraries(VoxWebserver_LIN64 pthread)
target_link_libraries(VoxWebserver_LIN64 dl)
target_link_libraries(VoxWebserver_LIN64 pqxx)
target_link_libraries(VoxWebserver_LIN64 pq)