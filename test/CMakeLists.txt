project(waymo-dataset-test)

add_executable(${PROJECT_NAME}
   WaymoDatasetTest.cpp)

target_link_libraries(${PROJECT_NAME}
   waymo-dataset)