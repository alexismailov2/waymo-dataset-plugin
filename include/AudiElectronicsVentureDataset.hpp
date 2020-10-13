#pragma once

#include <Item.hpp>

#include <string>
#include <memory>
#include <vector>
#include <functional>

class AudiElectronicsVentureDataset {
public:
    AudiElectronicsVentureDataset(std::string const& filePath);

    auto Get() const -> std::vector<Item> const&;

    static void Filter(std::vector<Item>& dataset, std::function<bool(Item::Box::List const& dataset, Item::Box const& box)> const& filter);

private:
    class Impl;
    std::shared_ptr<Impl> _impl;
};
