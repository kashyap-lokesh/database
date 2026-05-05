#pragma once

#include <vector>
#include "storage/version.h"

namespace mvcc
{

    struct Row
    {
        int id;
        std::vector<Version> versions;

        Row() : id(-1) {}
        Row(int id) : id(id) {}
    };

}