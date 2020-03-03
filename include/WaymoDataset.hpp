#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <vector>

class WaymoDataset
{
public:
//  class TFRecordParser
//  {
//  public:
//    TFRecordParser(std::string const& filePath);
//    bool Next(size_t index = 1);
//    void Reset();
//    auto GetCurrentRecord() const -> std::vector<char> const&;
//
//  private:
//    size_t _fileLength{};
//    size_t _fileLengthLeft{};
//    std::vector<char> _data;
//    std::ifstream _file;
//  };

public:
  class const_iterator
  {
  public:
    using self_type         = const_iterator;
    using value_type        = std::vector<char>;
    using reference         = std::vector<char>&;
    using pointer           = std::vector<char>*;
    using difference_type   = int;
    using iterator_category = std::forward_iterator_tag;

    const_iterator(std::vector<std::pair<int, uint64_t>> const& indexes, size_t index, std::string const& filePath)
      : _index{index}
      , _indexes(indexes)
      , _filePath{filePath}
    {
    }

    auto operator++() -> self_type
    {
      self_type i = *this;
      _index++;
      //auto file = std::ifstream{filePath, std::ios::binary};
      //_file.seekg(_indexes[_index].first, std::ios::beg);
      return i;
    }

    auto operator++(int junk) -> self_type
    {
      _index++;
      //auto file = std::ifstream{filePath, std::ios::binary};
      //file.seekg(_indexes[_index].first, std::ios::beg);
      return *this;
    }

    auto operator*() -> const reference
    {
      auto file = std::ifstream{_filePath, std::ios::binary};
      _data.resize(_indexes[_index].second);
      file.read(_data.data(), _indexes[_index].second);
      return _data;
    }

    auto operator->() -> const pointer
    {
      auto file = std::ifstream{_filePath, std::ios::binary};
      _data.resize(_indexes[_index].second);
      file.read(_data.data(), _indexes[_index].second);
      return &_data;
    }

    bool operator==(const self_type& rhs)
    {
      return _index != rhs._index;
    }

    bool operator!=(const self_type& rhs)
    {
      return _index != rhs._index;
    }

  private:
    size_t _index;
    std::vector<std::pair<int, uint64_t>> const& _indexes;
    std::string const& _filePath;
    std::vector<char> _data;
  };

public:
  WaymoDataset(std::string const& filePath);

  auto begin() const -> const_iterator const;
  auto end() const -> const_iterator const;
  auto cbegin() const -> const_iterator const;
  auto cend() const -> const_iterator const;

private:
  class Impl;
  std::shared_ptr<Impl> _impl;
};