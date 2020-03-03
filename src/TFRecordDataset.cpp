#include "TFRecordDataset.hpp"

#include <functional>
#include <fstream>

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

TFRecordDataset::const_iterator::const_iterator(std::vector<std::pair<int, uint64_t>> const &indexes, size_t index,
                                                std::string const &filePath)
    : _index{index}
    , _indexes(indexes)
    , _filePath{filePath}
{
}

auto TFRecordDataset::const_iterator::operator++() -> TFRecordDataset::const_iterator::self_type {
    self_type i = *this;
    _index++;
    return i;
}

auto TFRecordDataset::const_iterator::operator++(int junk) -> TFRecordDataset::const_iterator::self_type {
    _index++;
    return *this;
}

auto TFRecordDataset::const_iterator::operator*() -> TFRecordDataset::const_iterator::reference const{
    auto file = std::ifstream{_filePath, std::ios::binary};
    _data.resize(_indexes[_index].second);
    file.seekg(_indexes[_index].first, std::ios::beg);
    file.read(_data.data(), _indexes[_index].second);
    return _data;
}

auto TFRecordDataset::const_iterator::operator->() -> TFRecordDataset::const_iterator::pointer const {
    auto file = std::ifstream{_filePath, std::ios::binary};
    file.seekg(_indexes[_index].first, std::ios::beg);
    _data.resize(_indexes[_index].second);
    file.read(_data.data(), _indexes[_index].second);
    return &_data;
}

bool TFRecordDataset::const_iterator::operator==(const TFRecordDataset::const_iterator::self_type &rhs) {
    return _index != rhs._index;
}

bool TFRecordDataset::const_iterator::operator!=(const TFRecordDataset::const_iterator::self_type &rhs) {
    return _index != rhs._index;
}

TFRecordDataset::TFRecordDataset(std::string const &filePath)
    : _filePath{filePath}
{
    parseTFRecordDataset(filePath, [&](int offset, uint64_t size) {
        _indexes.push_back(std::make_pair(offset, size));
    });
}

auto TFRecordDataset::begin() const -> const TFRecordDataset::const_iterator
{
    return const_iterator(_indexes, 0, _filePath);
}

auto TFRecordDataset::end() const -> const TFRecordDataset::const_iterator
{
    return const_iterator(_indexes, _indexes.size(), _filePath);
}

auto TFRecordDataset::cbegin() const -> const const_iterator
{
    return const_iterator(_indexes, 0, _filePath);
}

auto TFRecordDataset::cend() const -> const const_iterator
{
    return const_iterator(_indexes, _indexes.size(), _filePath);
}
