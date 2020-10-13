#include <AudiElectronicsVentureDataset.hpp>

#include <boost/property_tree/json_parser.hpp>

#include <experimental/filesystem>

#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <set>

namespace {
  template <typename T>
  std::vector<T> as_vector(boost::property_tree::ptree const& pt, boost::property_tree::ptree::key_type const& key)
  {
    std::vector<T> r;
    for (auto& item : pt.get_child(key))
    {
      r.push_back(item.second.get_value<T>());
    }
    return r;
  }

  struct LabelItem
  {
    LabelItem(boost::property_tree::ptree const& pt)
      : _2d_bbox{as_vector<float>(pt, "2d_bbox")}
      , _class(pt.get<std::string>("class"))
      , _id{pt.get<int32_t>("id")}
      , _occlusion{pt.get<float>("occlusion")}
      , _rot_angle{pt.get<float>("rot_angle")}
      , _size{as_vector<float>(pt, "size")}
      , _truncation{pt.get<float>("truncation")}
    {
    }

    operator Item::Box() const
    {
      auto width = std::fabs(_2d_bbox[0] - _2d_bbox[2]);
      auto height = std::fabs(_2d_bbox[1] - _2d_bbox[3]);
      auto centerX = _2d_bbox[0] + width/2.0;
      auto centerY = _2d_bbox[1] + height/2.0;
      return Item::Box{getClassIndex(_class), centerX/1920.0, centerY/1208.0, width/1920.0, height/1208.0};
    }

    static
    auto getClassIndex(std::string const& key) -> int32_t
    {
      auto const found = _classes.find(key);
      if (found != _classes.cend())
      {
        return found->second;
      }
      auto currentIndex = _classes.size();
      _classes[key] = currentIndex;
      return _classes.size() - 1;
    }

    std::vector<float> _2d_bbox;
    std::string        _class;
    int32_t            _id;
    float              _occlusion;
    float              _rot_angle;
    std::vector<float> _size;
    float              _truncation;
    static std::map<std::string, int32_t> _classes;
  };

  std::map<std::string, int32_t> LabelItem::_classes;

  auto getItems(std::string const& pathToFileWithLabels) -> Item::Box::List
  {
    auto fileStream = std::ifstream{pathToFileWithLabels};
    boost::property_tree::ptree _labelItem;
    boost::property_tree::read_json(fileStream, _labelItem);

    Item::Box::List boxes;
    auto index = 0;
    while(auto const& annotations = _labelItem.get_child_optional(std::string("box_") + std::to_string(index)))
    {
      auto bbox = LabelItem{annotations.get()};
      if (bbox._truncation == 0.0f)
      {
        boxes.push_back(bbox);
      }
      index++;
    }

    return boxes;
  }
} /// end namespace anonymoous



class AudiElectronicsVentureDataset::Impl
{
public:
  Impl(std::string const &directoryPath)
      : _directoryPath{directoryPath}
  {
    for (auto const& item : std::experimental::filesystem::directory_iterator(_directoryPath + "/images/"))
    {
      if (item.path().extension().string() == ".json")
      {
        auto boxes = getItems(item.path().string());
        auto annotationFilePathJson = item.path().string();
        auto annotationFilePathTxt = annotationFilePathJson.substr(0, annotationFilePathJson.size() - 4) + "txt";
        auto annotationFile = std::ofstream(annotationFilePathTxt/*, std::ios_base::app*/);
        for(auto const& box : boxes)
        {
          annotationFile << box.classIndex << " " << box.centerX << " " << box.centerY << " " << box.width << " " << box.height << std::endl;
        }
      }
    }

    auto classNamesFile = std::ofstream(_directoryPath + "/images/_.names"/*, std::ios_base::app*/);
    std::vector<std::string> sortedClassesByIndex(LabelItem::_classes.size());
    for (auto const& _class : LabelItem::_classes)
    {
      sortedClassesByIndex[_class.second] = _class.first;
    }
    for (auto const& _class : sortedClassesByIndex)
    {
      classNamesFile << _class << std::endl;
    }
  }

  auto Get() -> std::vector<Item> const&
  {
    return _dataset;
  }

private:
  std::string _directoryPath;
  std::vector<Item> _dataset;
};

AudiElectronicsVentureDataset::AudiElectronicsVentureDataset(std::string const &directoryPath)
    : _impl{std::make_shared<Impl>(directoryPath)}
{
}

auto AudiElectronicsVentureDataset::Get() const -> std::vector<Item> const&
{
  return _impl->Get();
}

void AudiElectronicsVentureDataset::Filter(std::vector<Item>& dataset, std::function<bool(Item::Box::List const&, Item::Box const&)> const& filter)
{
  dataset.erase(std::remove_if(dataset.begin(), dataset.end(), [&](auto& item) {
    item.boxes.erase(std::remove_if(item.boxes.begin(), item.boxes.end(), [&](auto& box) {
      return filter(item.boxes, box);
    }), item.boxes.end());
    return item.boxes.empty();
  }), dataset.end());
}
