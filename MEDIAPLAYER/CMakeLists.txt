CMAKE_MINIMUM_REQUIRED(VERSION 3.6)
SET(SPAWNING_NAME ENGINE)
SET(PROJECT_NAME MEDIAPLAYER)
PROJECT(${PROJECT_NAME})
PROJECT(${PROJECT_NAME})
FIND_PACKAGE(OpenCV 4 REQUIRED)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -ldl -lpthread")
SET(BUILD_SHARED_LIBS ON) # need to be turn off for demo
SET(CMAKE_CXX_FLAGS_DEBUG  "-g")

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS}
        ${PROJECT_SOURCE_DIR}/include)

find_package(Eigen3 REQUIRED)
MESSAGE( [Main] " EIGEN3_INCLUDE_DIRS = ${EIGEN3_INCLUDE_DIRS}") 
#EIGEN3_INCLUDE_DIRS = /usr/include/eigen3

include_directories(${EIGEN3_INCLUDE_DIRS})

include_directories("/usr/local/include/mongocxx/v_noabi")
include_directories("/usr/local/include/libmongoc-1.0")
include_directories("/usr/local/include/bsoncxx/v_noabi")
include_directories("/usr/local/include/libbson-1.0")

find_package(gflags REQUIRED)
# gflags_INCLUDE_DIR = /usr/include/gflags
include_directories (${gflags_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${REDIS_PLUS_PLUS_HEADER}
                    ${REDIS_PLUS_PLUS_LIB})

find_path(HIREDIS_HEADER hiredis)
#target_include_directories(${PROJECT_NAME} PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
#target_link_libraries(${PROJECT_NAME} ${HIREDIS_LIB})

# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)
INCLUDE_DIRECTORIES(${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
INCLUDE_DIRECTORIES(${REDIS_PLUS_PLUS_LIB})

ADD_EXECUTABLE(${SPAWNING_NAME} engine.cpp)
TARGET_LINK_LIBRARIES(${SPAWNING_NAME} ${OpenCV_LIBS} -lmongocxx -lbsoncxx)

find_package(Iconv REQUIRED)
if(NOT Iconv_IS_BUILT_IN)
  set(ICONV_LIB Iconv::Iconv)
endif()

ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

#TARGET_LINK_LIBRARIES(${PROJECT_NAME} -lredis++ -lhiredis -lstdc++fs)
TARGET_LINK_LIBRARIES(${PROJECT_NAME}  ${OpenCV_LIBS} -lavformat -lavcodec -lswscale  -lz -lavutil -ltbb -lcurl -lredis++ -lhiredis -lstdc++fs -lpthread -lboost_thread -lboost_system -lmongocxx -lbsoncxx  pthread dl ssl)
