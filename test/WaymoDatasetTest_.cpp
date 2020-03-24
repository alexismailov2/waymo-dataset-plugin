#include "WaymoDataset.hpp"

#include <algorithm>
#include <cstring>
#include <experimental/filesystem>
#include <iomanip>
#include <iostream>
#include <cmath>
#include <sstream>

auto intersection(WaymoDataset::Item::Box const& r1, WaymoDataset::Item::Box const& r2) -> WaymoDataset::Item::Box
{
    auto xmax = std::max(r1.centerX - r1.width/2,  r2.centerX - r2.width/2);
    auto ymax = std::max(r1.centerY - r1.height/2, r2.centerY - r2.height/2);
    auto xmin = std::min(r1.centerX + r1.width/2,  r2.centerX + r2.width/2);
    auto ymin = std::min(r1.centerY + r1.height/2, r2.centerY + r2.height/2);

    auto result = WaymoDataset::Item::Box{};
    if ((xmax > xmin) || (ymax > ymin))
    {

    }
    else
    {
        result.width   = std::fabs(xmin - xmax);
        result.height  = std::fabs(ymin - ymax);
        result.centerX = xmax + result.width/2;
        result.centerY = ymin - result.height/2;
    }
    return result;
}

int main(int argc, char* argv[])
{
    //const std::array<std::string, FILE_LIST_COUNT> waymoDataset{{ FILE_LIST }};

    //for (auto const& tfrecord : waymoDataset)
    //{
        if ((argc < 2) || (argc > 3))
        {
          std::cout << "Usage: waymo-dataset-test <path-to-waymo-dataset-file>.tfrecord [<folder name>]" << std::endl;
        }
        auto tfrecord = std::string{argv[1]};
        auto waymoDataset = WaymoDataset(tfrecord).Get();
/*
        WaymoDataset::Filter(waymoDataset, [](WaymoDataset::Item::Box::List const& boxes, WaymoDataset::Item::Box const& box) {
            WaymoDataset::Item::Box intersected{};
            for (auto const& item : boxes)
            {
                if (&box == &item)
                {
                  continue;
                }
                intersected = intersection(item, box);
                if ((intersected.width > 0) && (intersected.height > 0))
                {
                    break;
                }
            }
            bool neededToBeRemoved = (box.width < 32) || (box.height < 32) || (((box.width * box.height) * 1.0 / 3.0) <= (intersected.width * intersected.height));
            return neededToBeRemoved;
        });
        */
        auto outputDirectory = std::string((argc == 3) ? argv[2] : std::string(argv[1]).substr(0, std::strlen(argv[1]) - 9));
        if (!std::experimental::filesystem::create_directory(outputDirectory))
        {
          std::cout << "Could not be created directory" << std::endl;
          return -1;
        }
        for (auto const& item : waymoDataset)
        {
            auto filePath = std::stringstream{};
            filePath << outputDirectory << "/" << std::setw(5) << std::setfill('0') << item.frameId;
            auto imageFile = std::ofstream{filePath.str() + ".jpg", std::ios::binary};
            imageFile.write(item.imageData.data(), item.imageData.size());
            auto annotationFile = std::ofstream{filePath.str() + ".txt"};
            for (auto const& boundingBox : item.boxes)
            {
                annotationFile << boundingBox.classIndex << " "
                               << boundingBox.centerX/1920.0 << " "
                               << boundingBox.centerY/1280.0 << " "
                               << boundingBox.width/1920.0 << " "
                               << boundingBox.height/1280.0 << std::endl;
            }
        }
    //}
    return 0;
}

