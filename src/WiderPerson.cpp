#include <WiderPerson.hpp>

#include <fstream>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace {
    template<typename T>
    auto split(std::string const& input) -> std::vector<T>
    {
        auto delimiter = [](auto const& c) -> bool { return !std::isdigit(c); };
        auto not_delimiter = [&](auto const& c) -> bool { return !delimiter(c); };

        std::vector<T> result;
        auto itSubstrEnd = input.cbegin();
        while(true)
        {
            auto itSubstrBegin = std::find_if(itSubstrEnd, input.cend(), not_delimiter);
            if (itSubstrBegin == input.cend())
            {
                break;
            }

            itSubstrEnd = std::find_if(itSubstrBegin, input.cend(), delimiter);
            auto word = std::string(itSubstrBegin, itSubstrEnd);
//            std::transform(word.cbegin(),
//                           word.cend(),
//                           word.begin(),
//                           [](unsigned char c){ return std::tolower(c); });
            result.emplace_back(std::move(word));
        }
        return result;
    }

    auto readImageNames(std::string const& imageListFilePath) -> std::vector<std::string>
    {
        auto result = std::vector<std::string>{};
        result.reserve(10000);
        auto file = std::ifstream(imageListFilePath);
        auto line = std::string{};
        while (getline(file, line))
        {
            result.push_back(line.substr(0, line.size() - 1));
        }
        return result;
    }

    auto readAnnotation(std::string const& annotationFilePath, std::string const& imageFilePath) -> WiderPerson::Item::Box::List
    {
        auto imageWidth = 0;
        auto imageHeight = 0;
        auto comp = 0;
        auto info = stbi_info(imageFilePath.c_str(), &imageWidth, &imageHeight, &comp);

        auto result = WiderPerson::Item::Box::List{};
        result.reserve(100);
        auto annotationFile = std::ifstream{annotationFilePath};
        auto line = std::string{};
        while (getline(annotationFile, line))
        {
          auto splitedData = split<std::string>(line);
          if (splitedData.size() != 5)
          {
              continue;
          }
          auto x1 = std::strtod(splitedData[1].c_str(), nullptr);
          auto y1 = std::strtod(splitedData[2].c_str(), nullptr);
          auto x2 = std::strtod(splitedData[3].c_str(), nullptr);
          auto y2 = std::strtod(splitedData[4].c_str(), nullptr);
          auto width = std::fabs(x1 - x2);
          auto height = std::fabs(y1 - y2);
          result.emplace_back(WiderPerson::Item::Box{std::atoi(splitedData[0].c_str()),
                                                     (x1 + (width/2.0f))/imageWidth,
                                                     (y1 + (height/2.0f))/imageHeight,
                                                     width/imageWidth,
                                                     height/imageHeight});
        }
        return result;
    }

    void saveAnnotationInDarknetFormat(std::string const& pathToAnnotation, WiderPerson::Item::Box::List const& objectList)
    {
        auto annotationFile = std::ofstream{pathToAnnotation};
        for (auto const& boundingBox : objectList)
        {
            annotationFile << boundingBox.classIndex << " "
                           << boundingBox.centerX << " "
                           << boundingBox.centerY << " "
                           << boundingBox.width << " "
                           << boundingBox.height << std::endl;
        }
    }
} /// end namespace anonymous

class WiderPerson::Impl
{
public:
  Impl(std::string const &directoryPath)
      : _directoryPath{directoryPath}
  {
    auto const trainList = readImageNames(_directoryPath + "/train.txt");
    auto const validList = readImageNames(_directoryPath + "/val.txt");
    for (auto const& item : trainList)
    {
        auto objectList = readAnnotation(_directoryPath + "/Annotations/" + item + ".jpg.txt",
                                         _directoryPath + "/Images/" + item + ".jpg");
        saveAnnotationInDarknetFormat(_directoryPath + "/Images/" + item + ".txt", objectList);
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

WiderPerson::WiderPerson(std::string const &directoryPath)
    : _impl{std::make_shared<Impl>(directoryPath)}
{
}

auto WiderPerson::Get() const -> std::vector<Item> const&
{
  return _impl->Get();
}

void WiderPerson::Filter(std::vector<WiderPerson::Item>& dataset, std::function<bool(WiderPerson::Item::Box::List const&, WiderPerson::Item::Box const&)> const& filter)
{
  dataset.erase(std::remove_if(dataset.begin(), dataset.end(), [&](auto& item) {
    item.boxes.erase(std::remove_if(item.boxes.begin(), item.boxes.end(), [&](auto& box) {
      return filter(item.boxes, box);
    }), item.boxes.end());
    return item.boxes.empty();
  }), dataset.end());
}