#include <WaymoDataset.hpp>

#include "TFRecordDataset.hpp"

#include <waymo_open_dataset/dataset.pb.h>

#include <functional>
#include <vector>

namespace {
    auto extractImageAndBoundingBox(std::vector<char> const& data) -> WaymoDataset::Item
    {
        static auto frameCounter = 0;
        WaymoDataset::Item datasetItem{};

        datasetItem.frameId = frameCounter;

        waymo::open_dataset::Frame frame;
        frame.ParseFromArray(data.data(), data.size());
#if 0 // Not needed for now
        if (frame.has_context())
        {
            auto const& context = frame.context();
            if (context.has_name())
            {
                std::cout << "name: " << context.name() << std::endl;
            }
            for (auto const& cameraCalibration : context.camera_calibrations())
            {

            }
            for (auto const& laserCalibration : context.laser_calibrations())
            {

            }
            if (context.has_stats())
            {
                auto contextStats = context.stats();
            }
        }
        if (frame.has_timestamp_micros())
        {
            auto timestamp_micros = frame.timestamp_micros();
            std::cout << "timestamp_micros: " << timestamp_micros << std::endl;
        }
        if (frame.has_pose())
        {
            // Vechicle pose
        }
#endif
        auto frameCameraLabelsIt = frame.camera_labels().cbegin();
        auto frameImagesIt = frame.images().cbegin();
        while ((frameImagesIt != frame.images().cend()) &&
               (frameCameraLabelsIt != frame.camera_labels().cend()))
        {
            if (frameImagesIt->has_name() && (frameImagesIt->name() == waymo::open_dataset::CameraName_Name_FRONT) &&
                frameImagesIt->has_image() &&
                frameCameraLabelsIt->has_name() && (frameCameraLabelsIt->name() ==  waymo::open_dataset::CameraName_Name_FRONT))
            {
                //if (frame.has_context() && frame.context().has_name() && (cameraName == "FRONT"))
                //{
                datasetItem.imageData = frameImagesIt->image();
                //auto imageFile = std::ofstream{std::string("FRONT/") + frame.context().name() + "_" + std::to_string(frameCounter) + "_" + ".jpg", std::ios::binary};
                //imageFile.write(frameImage.image().data(), frameImage.image().size());
                //}
                for(auto const& label : frameCameraLabelsIt->labels())
                {
                    if (label.has_box() && label.has_type())
                    {
                        auto const& box = label.box();
                        if (!box.has_center_x() || !box.has_center_y() || /*!box.has_center_z() ||*/
                            !box.has_width() || !box.has_length() || /*!box.has_height()*/
                            (box.length() < 32) || (box.width() < 32))
                        {
                            continue;
                        }
                        auto classIndex = 0;
                        switch(label.type())
                        {
                            case waymo::open_dataset::Label_Type_TYPE_VEHICLE:
                                classIndex = 1;
                                break;
                            case waymo::open_dataset::Label_Type_TYPE_PEDESTRIAN:
                                classIndex = 0;
                                break;
                            case waymo::open_dataset::Label_Type_TYPE_SIGN:
                                classIndex = 2;
                                break;
                            case waymo::open_dataset::Label_Type_TYPE_CYCLIST:
                                classIndex = 3;
                                break;
                            case waymo::open_dataset::Label_Type_TYPE_UNKNOWN:
                                classIndex = 4;
                            default:
                                break;
                        }
                        datasetItem.boxes.push_back({classIndex, box.center_x(), box.center_y(), box.length(), box.width()});
                    }
                }
                //excludeIfSomeBoxIntersectedWithAnotherMoreThan2Div3(dataset.back().boxes);
//                auto annotationFile = std::ofstream{std::string("FRONT/") + frame.context().name() + "_" + std::to_string(frameCounter) + "_" + ".txt"};
//                for (auto const& boundingBox : boundingBoxes)
//                {
//                    annotationFile << boundingBox.classIndex << " "
//                                   << boundingBox.center_x/1920.0 << " "
//                                   << boundingBox.center_y/1280.0 << " "
//                                   << boundingBox.width/1920.0 << " "
//                                   << boundingBox.height/1280.0 << std::endl;
//                }
            }
            ++frameImagesIt;
            ++frameCameraLabelsIt;
        }
        frameCounter++;
        return datasetItem;
    }
} /// end namespace anonymous

class WaymoDataset::Impl
{
  public:
    Impl(std::string const &filePath)
      : _tfRecordDataset{filePath}
    {
        for(auto const& item : _tfRecordDataset)
        {
            _dataset.push_back(extractImageAndBoundingBox(item));
        }
    }

    auto Get() -> std::vector<Item> const&
    {
        return _dataset;
    }

  private:
    std::string _filePath;
    TFRecordDataset _tfRecordDataset;
    std::vector<Item> _dataset;
};

WaymoDataset::WaymoDataset(std::string const &filePath)
  : _impl{std::make_shared<Impl>(filePath)}
{
}

auto WaymoDataset::Get() const -> std::vector<Item> const&
{
  return _impl->Get();
}

void WaymoDataset::Filter(std::vector<WaymoDataset::Item>& dataset, std::function<bool(WaymoDataset::Item::Box::List const&, WaymoDataset::Item::Box const&)> const& filter)
{
   dataset.erase(std::remove_if(dataset.begin(), dataset.end(), [&](auto& item) {
      item.boxes.erase(std::remove_if(item.boxes.begin(), item.boxes.end(), [&](auto& box) {
         return filter(item.boxes, box);
      }), item.boxes.end());
      return item.boxes.empty();
   }), dataset.end());
}
