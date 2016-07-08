cmake_minimum_required(VERSION 3.5)
project(VoxWebAPI_LIN64)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -g -rdynamic")

set(SOURCE_FILES main.cpp RequestTypes/Connection.cpp RequestTypes/Connection.h main.cpp RequestTypes/HttpRequest.cpp RequestTypes/HttpRequest.h VoxWebAPI.h Network/Webserver.h PageTypes/Site.cpp PageTypes/Site.h PageTypes/Page.cpp PageTypes/Page.h RequestTypes/WebSocket.cpp RequestTypes/WebSocket.h Ext/voxstring.h Network/Webserver.cpp Ext/voxcrypt.h Ext/voxchain.h Network/NetworkHelper.cpp Network/NetworkHelper.h PageTypes/SimplePage.cpp PageTypes/SimplePage.h Database/DataFileStorage.cpp Database/DataFileStorage.h PageTypes/ProcessPage.cpp PageTypes/ProcessPage.h Ext/voxtimemap.h Database/DataTable.cpp Database/DataTable.h Database/Postgres.cpp Database/Postgres.h)
add_library(VoxWebAPI_LIN64 STATIC ${SOURCE_FILES})
target_link_libraries(VoxWebAPI_LIN64 pthread)
include_directories(${PROJECT_SOURCE_DIR})