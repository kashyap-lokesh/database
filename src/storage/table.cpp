#include "storage/table.h"
#include <iostream>

namespace mvcc
{

    void Table::insert(int id, const std::string &data, int txn_id)
    {
        auto it = rows_.find(id);

        if (it == rows_.end())
        {
            // create row properly
            Row row(id);
            row.versions.emplace_back(txn_id, data);
            rows_.emplace(id, std::move(row));
        }
        else
        {
            it->second.versions.emplace_back(txn_id, data);
        }
    }

    std::string Table::read(int id, int txn_id)
    {
        if (rows_.find(id) == rows_.end())
        {
            return "NOT_FOUND";
        }

        auto &versions = rows_[id].versions;

        if (!versions.empty())
        {
            return versions.back().data;
        }

        return "NOT_FOUND";
    }

    void Table::update(int id, const std::string &data, int txn_id)
    {
        if (rows_.find(id) == rows_.end())
            return;

        rows_[id].versions.emplace_back(txn_id, data);
    }

    void Table::remove(int id, int txn_id)
    {
        if (rows_.find(id) == rows_.end())
            return;

        if (!rows_[id].versions.empty())
        {
            rows_[id].versions.back().deleted_txn = txn_id;
        }
    }

    void Table::printState() const
    {
        for (const auto &[id, row] : rows_)
        {
            std::cout << "Row " << id << ":\n";
            for (const auto &v : row.versions)
            {
                std::cout << "  [created: " << v.created_txn
                          << ", deleted: " << v.deleted_txn
                          << ", data: " << v.data << "]\n";
            }
        }
    }

}