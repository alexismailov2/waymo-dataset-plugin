#include "WaymoDataset.hpp"

int main(int argc, char* argv[])
{
    auto waymoDataset = WaymoDataset(argv[1]).Get();
    for (auto const& item : waymoDataset)
    {
        auto imageFile = std::ofstream{std::string("FRONT/") + std::to_string(item.frameId) + ".jpg", std::ios::binary};
        imageFile.write(item.imageData.data(), item.imageData.size());
        auto annotationFile = std::ofstream{std::string("FRONT/") + std::to_string(item.frameId) + ".txt"};
        for (auto const& boundingBox : item.boxes)
        {
            annotationFile << boundingBox.classIndex << " "
                           << boundingBox.centerX/1920.0 << " "
                           << boundingBox.centerY/1280.0 << " "
                           << boundingBox.width/1920.0 << " "
                           << boundingBox.height/1280.0 << std::endl;
        }
    }
    return 0;
}

