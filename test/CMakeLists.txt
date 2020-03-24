project(waymo-dataset-test)

add_executable(${PROJECT_NAME}
   WaymoDatasetTest_.cpp)

target_link_libraries(${PROJECT_NAME}
   waymo-dataset)
   #stdc++fs)

add_executable(wider-person-dataset-test
   WiderPersonTest.cpp)

target_link_libraries(wider-person-dataset-test
   waymo-dataset)
   #stdc++fs)