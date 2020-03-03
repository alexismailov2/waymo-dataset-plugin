#include <iostream>

#include "WaymoDataset.hpp"

int main(int argc, char* argv[])
{
    auto waymoDataset = WaymoDataset(argv[1]);
    for (auto const& item : waymoDataset)
    {

    }
    return 0;
}

