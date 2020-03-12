project(waymo-dataset-test)

add_executable(${PROJECT_NAME}
   WaymoDatasetTest_.cpp)

target_link_libraries(${PROJECT_NAME}
   waymo-dataset
   stdc++fs)