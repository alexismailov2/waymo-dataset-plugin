#include "AudiElectronicsVentureDataset.hpp"

#include <algorithm>
#include <cstring>
#include <experimental/filesystem>
#include <iomanip>
#include <iostream>

int main(int argc, char* argv[])
{
   auto dataset = AudiElectronicsVentureDataset{"/media/oleksandr_ismailov/6a7d7e84-3bc5-48c9-b9f1-4ba4a67e60f5/Videos/YOLO_on_darknet/waymo-dataset-plugin/camera_lidar_semantic_bboxes"};
   return 0;
}
