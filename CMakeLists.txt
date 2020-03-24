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
    DEPENDS protobuf_build
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/protobuf/bin/protoc ${CMAKE_CURRENT_SOURCE_DIR}/third-party/waymo-open-dataset/waymo_open_dataset/dataset.proto
                                                            ${CMAKE_CURRENT_SOURCE_DIR}/third-party/waymo-open-dataset/waymo_open_dataset/label.proto
    --proto_path=${CMAKE_CURRENT_SOURCE_DIR}/third-party/waymo-open-dataset
    --cpp_out=${CMAKE_CURRENT_BINARY_DIR}/
)

add_custom_target(waymo_protobuf ALL
   COMMAND echo "This is ALL target 'waymo_protobuf', and it depends on ${CMAKE_CURRENT_BINARY_DIR}/dataset.pb.cc and ${CMAKE_CURRENT_BINARY_DIR}/label.pb.cc"
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dataset.pb.cc
           ${CMAKE_CURRENT_BINARY_DIR}/label.pb.cc
   VERBATIM)

file(GLOB_RECURSE FILE_LIST ./waymo/*.tfrecord)

list(LENGTH FILE_LIST FILE_LIST_COUNT)
string(REPLACE ";" "\",\"" FILE_LIST "\"${FILE_LIST}\"")

add_definitions(
   -DFILE_LIST=${FILE_LIST}
   -DFILE_LIST_COUNT=${FILE_LIST_COUNT})

project(waymo-dataset)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third-party/glm)

include_directories(
   include
   third-party/glm
   third-party/
   ${CMAKE_CURRENT_BINARY_DIR}
   ${CMAKE_CURRENT_BINARY_DIR}/protobuf/include)

file(GLOB WAYMO_DATASET_LIBRARY_SOURCE src/*.cpp src/*.hpp)
file(GLOB WAYMO_DATASET_LIBRARY_PUBLIC_HEADERS include/*.hpp)

add_library(${PROJECT_NAME}
   ${CMAKE_CURRENT_BINARY_DIR}/waymo_open_dataset/dataset.pb.cc
   ${CMAKE_CURRENT_BINARY_DIR}/waymo_open_dataset/label.pb.cc
   ${WAYMO_DATASET_LIBRARY_SOURCE}
   ${WAYMO_DATASET_LIBRARY_PUBLIC_HEADERS})

add_dependencies(${PROJECT_NAME} waymo_protobuf)

target_link_libraries(${PROJECT_NAME}
   -L${CMAKE_CURRENT_BINARY_DIR}/protobuf/lib
   protobuf)

add_subdirectory(test)