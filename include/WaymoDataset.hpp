#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <functional>

class WaymoDataset
{
public:
  struct Item
  {
      struct Box
      {
          using List = std::vector<Box>;
          int32_t classIndex;
          double centerX;
          double centerY;
          double width;
          double height;
      };
      uint32_t frameId;
      std::string imageData;
      Box::List boxes;
  };

public:
  WaymoDataset(std::string const& filePath);

  auto Get() const -> std::vector<Item> const&;

  static void Filter(std::vector<Item>& dataset, std::function<bool(Item::Box::List const& dataset, Item::Box const& box)> const& filter);

private:
  class Impl;
  std::shared_ptr<Impl> _impl;
};