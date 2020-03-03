#pragma once

#include <vector>
#include <string>

class TFRecordDataset
{
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

        const_iterator(std::vector<std::pair<int, uint64_t>> const& indexes, size_t index, std::string const& filePath);

        auto operator++() -> self_type;
        auto operator++(int junk) -> self_type;
        auto operator*() -> const reference;
        auto operator->() -> const pointer;
        bool operator==(const self_type& rhs);
        bool operator!=(const self_type& rhs);

    private:
        size_t _index;
        std::vector<std::pair<int, uint64_t>> const& _indexes;
        std::string const& _filePath;
        std::vector<char> _data;
    };

public:
    TFRecordDataset(std::string const& filePath);

    auto begin() const -> const_iterator const;
    auto end() const -> const_iterator const;
    auto cbegin() const -> const const_iterator;
    auto cend() const -> const const_iterator;

private:
    std::string _filePath;
    std::vector<std::pair<int, uint64_t>> _indexes;
};
