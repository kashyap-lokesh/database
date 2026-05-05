#pragma once

#include <string>

namespace mvcc
{

    class Logger
    {
    public:
        static void log(const std::string &msg);
    };

}