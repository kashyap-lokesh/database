#include "utils/logger.h"
#include <iostream>

namespace mvcc
{

    void Logger::log(const std::string &msg)
    {
        std::cout << "[LOG] " << msg << std::endl;
    }

}