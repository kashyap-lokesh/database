#pragma once

#include <unordered_map>
#include <string>
#include "storage/row.h"

namespace mvcc
{

    class Table
    {
    public:
        void insert(int id, const std::string &data, int txn_id);
        std::string read(int id, int txn_id);
        void update(int id, const std::string &data, int txn_id);
        void remove(int id, int txn_id);

        void printState() const;

    private:
        std::unordered_map<int, Row> rows_;
    };

}