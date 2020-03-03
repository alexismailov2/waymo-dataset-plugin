#include <WaymoDataset.hpp>

#include <glm/glm.hpp>

#include <functional>
#include <vector>

namespace {
  void parseTFRecordDataset(std::string const& filePath, std::function<void(int offset, uint64_t size)>&& newRecord)
  {
    auto file = std::ifstream(filePath, std::ios::binary);

    file.seekg(0, std::ios::end);
    auto fileLength = file.tellg();
    file.seekg(0, std::ios::beg);

    while(fileLength >= 8)
    {
      uint64_t length{};
      file.read((char*)&length, sizeof(uint64_t));
      if (fileLength >= length + 4 + 4)
      {
        uint32_t masked_crc32_of_length{};
        file.read((char*)&masked_crc32_of_length, sizeof(uint32_t));

        newRecord(file.tellg(), length);
        file.seekg(length, std::ios::cur);

        uint32_t masked_crc32_of_data{};
        file.read((char*)&masked_crc32_of_data, sizeof(uint32_t));
      }
      fileLength -= length + 4 + 4 + 8;
    }
  }
} /// end namespace anonymous

//WaymoDataset::TFRecordParser::TFRecordParser(std::string const &filePath)
//    : _file{filePath, std::ios::binary}
//{
//  _file.seekg(0, std::ios::end);
//  _fileLength = _file.tellg();
//  _fileLengthLeft = _fileLength;
//  _file.seekg(0, std::ios::beg);
//}
//
//bool WaymoDataset::TFRecordParser::Next(size_t index)
//{
//  while((_fileLengthLeft >= 8) && (index-- > 0))
//  {
//    uint64_t length{};
//    _file.read((char*)&length, sizeof(uint64_t));
//    if (_fileLengthLeft >= length + 4 + 4)
//    {
//      uint32_t masked_crc32_of_length{};
//      _file.read((char*)&masked_crc32_of_length, sizeof(uint32_t));
//
//      if (index)
//      {
//        _file.seekg(length, std::ios::cur);
//      }
//      else
//      {
//        _data.resize(length);
//        _file.read(_data.data(), length);
//      }
//
//      uint32_t masked_crc32_of_data{};
//      _file.read((char*)&masked_crc32_of_data, sizeof(uint32_t));
//    }
//    _fileLengthLeft -= length + 4 + 4 + 8;
//  }
//}
//
//void WaymoDataset::TFRecordParser::Reset()
//{
//  _file.seekg(0, std::ios::beg);
//  _fileLengthLeft = _fileLength;
//  _data.clear();
//}
//
//auto WaymoDataset::TFRecordParser::GetCurrentRecord() const -> std::vector<char> const&
//{
//  return _data;
//}

class WaymoDataset::Impl
{
  public:
    Impl(std::string const &filePath)
      : _filePath{filePath}
    {
      parseTFRecordDataset(filePath, [&](int offset, uint64_t size) {
        _indexes.push_back(std::make_pair(offset, size));
      });
    }

    auto begin() const -> const_iterator const
    {
      return const_iterator(_indexes, 0, _filePath);
    }

    auto end() const -> const_iterator const
    {
      return const_iterator(_indexes, _indexes.size(), _filePath);
    }

  private:
    std::string _filePath;
    std::vector<std::pair<int, uint64_t>> _indexes;
};

WaymoDataset::WaymoDataset(std::string const &filePath)
  : _impl{std::make_shared<Impl>(filePath)}
{
}

auto WaymoDataset::begin() const -> const WaymoDataset::const_iterator
{
  return _impl->begin();
}

auto WaymoDataset::end() const -> const WaymoDataset::const_iterator
{
  return _impl->end();
}

auto WaymoDataset::cbegin() const -> const WaymoDataset::const_iterator
{
  return _impl->begin();
}

auto WaymoDataset::cend() const -> const WaymoDataset::const_iterator
{
  return _impl->end();
}
