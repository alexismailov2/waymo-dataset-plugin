#pragma once

#include <vector>
#include <cstdint>
#include <string>

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