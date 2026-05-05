#pragma once

#include <string>

namespace mvcc
{

    struct Version
    {
        int created_txn;
        int deleted_txn; // -1 means alive
        std::string data;

        Version(int c, const std::string &d)
            : created_txn(c), deleted_txn(-1), data(d) {}
    };

}