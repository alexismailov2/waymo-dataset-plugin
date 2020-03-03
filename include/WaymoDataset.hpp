#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

class WaymoDataset
{
public:
  struct Item
  {
      struct Box
      {
          int32_t classIndex;
          double centerX;
          double centerY;
          double width;
          double height;
      };
      uint32_t frameId;
      std::string imageData;
      std::vector<Box> boxes;
  };

public:
  WaymoDataset(std::string const& filePath);

  auto Get() const -> std::vector<Item> const&;

private:
  class Impl;
  std::shared_ptr<Impl> _impl;
};