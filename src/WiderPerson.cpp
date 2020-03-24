#include <WiderPerson.hpp>

#include <experimental/filesystem>

namespace {
  auto extractImageAndBoundingBox(std::vector<char> const& data) -> WiderPerson::Item
  {
    return {};
  }
} /// end namespace anonymous

class WiderPerson::Impl
{
public:
  Impl(std::string const &directoryPath)
      : _directoryPath{directoryPath}
  {
    //std::vector<std::string>
    for(auto const& item : std::experimental::filesystem::directory_iterator(_directoryPath))
    {
      //_dataset.push_back(extractImageAndBoundingBox(item));
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