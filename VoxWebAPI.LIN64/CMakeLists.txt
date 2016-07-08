cmake_minimum_required(VERSION 3.5)
project(VoxWebAPI_LIN64)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC -g -rdynamic")

set(SOURCE_FILES main.cpp Connection.cpp Connection.h main.cpp HttpRequest.cpp HttpRequest.h VoxWebAPI.h Webserver.h Site.cpp Site.h Page.cpp Page.h WebSocket.cpp WebSocket.h voxstring.h Webserver.cpp voxcrypt.h voxchain.h NetworkHelper.cpp NetworkHelper.h SimplePage.cpp SimplePage.h DataFileStorage.cpp DataFileStorage.h ProcessPage.cpp ProcessPage.h voxtimemap.h DataTable.cpp DataTable.h Postgres.cpp Postgres.h)
add_library(VoxWebAPI_LIN64 STATIC ${SOURCE_FILES})
target_link_libraries(VoxWebAPI_LIN64 pthread)
