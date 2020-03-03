cmake_minimum_required(VERSION 3.15)

include(ExternalProject)

ExternalProject_Add(protobuf_build
    PREFIX protobuf
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third-party/protobuf/cmake"
    CMAKE_CACHE_ARGS
    -Dprotobuf_BUILD_TESTS:BOOL=OFF
    -Dprotobuf_WITH_ZLIB:BOOL=OFF
    -Dprotobuf_MSVC_STATIC_RUNTIME:BOOL=OFF
    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/protobuf)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/dataset.pb.cc
           ${CMAKE_CURRENT_BINARY_DIR}/label.pb.cc
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/protobuf/bin/protoc ${CMAKE_CURRENT_SOURCE_DIR}/third-party/waymo-open-dataset/waymo_open_dataset/dataset.proto
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/third-party/waymo-open-dataset/waymo_open_dataset/label.proto
    --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/third-party/waymo-open-dataset
    --cpp_out=${CMAKE_CURRENT_BINARY_DIR}/
)

project(waymo-dataset)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(third-party/glm)

include_directories(
   include
   third-party/glm
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}/protobuf/include)

file(GLOB WAYMO_DATASET_LIBRARY_SOURCE src/*.cpp src/*.hpp)
file(GLOB WAYMO_DATASET_LIBRARY_PUBLIC_HEADERS include/*.hpp)

add_library(${PROJECT_NAME}
   ${CMAKE_CURRENT_BINARY_DIR}/waymo_open_dataset/dataset.pb.cc
   ${CMAKE_CURRENT_BINARY_DIR}/waymo_open_dataset/label.pb.cc
   ${WAYMO_DATASET_LIBRARY_SOURCE}
   ${WAYMO_DATASET_LIBRARY_PUBLIC_HEADERS})

target_link_libraries(${PROJECT_NAME}
   -L${CMAKE_CURRENT_BINARY_DIR}/protobuf/lib
   protobuf)

add_subdirectory(test)