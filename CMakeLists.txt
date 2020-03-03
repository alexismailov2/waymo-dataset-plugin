cmake_minimum_required(VERSION 3.16)

project(waymo-dataset)

add_subdirectory(third-party/glm)

include_directories(
   include
   third-party/glm)

file(GLOB WAYMO_DATASET_LIBRARY_SOURCE src/*.cpp src/*.hpp)
file(GLOB WAYMO_DATASET_LIBRARY_PUBLIC_HEADERS include/*.hpp)

add_library(${PROJECT_NAME}
   ${WAYMO_DATASET_LIBRARY_SOURCE}
   ${WAYMO_DATASET_LIBRARY_PUBLIC_HEADERS})

target_link_libraries(${PROJECT_NAME}
   )

add_subdirectory(test)