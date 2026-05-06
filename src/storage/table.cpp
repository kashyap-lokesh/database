#include "storage/table.h"

#include <algorithm>
#include <iostream>

namespace mvcc
{

    void Table::insert(int id, const std::string &data, int txn_id)
    {
        auto it = rows_.find(id);

        if (it == rows_.end())
        {
            Row row(id);
            row.versions.emplace_back(txn_id, data);
            rows_.emplace(id, std::move(row));
        }
        else
        {
            it->second.versions.emplace_back(txn_id, data);
        }
    }

    std::string Table::read(
        int id,
        int txn_id,
        const std::function<VersionVisibility(const Version &, int)> &visibility) const
    {
        auto it = rows_.find(id);
        if (it == rows_.end())
        {
            return "NOT_FOUND";
        }

        for (auto vit = it->second.versions.rbegin(); vit != it->second.versions.rend(); ++vit)
        {
            const VersionVisibility state = visibility(*vit, txn_id);
            if (state == VersionVisibility::VISIBLE)
            {
                return vit->data;
            }
            if (state == VersionVisibility::DELETED)
            {
                return "NOT_FOUND";
            }
        }

        return "NOT_FOUND";
    }

    void Table::update(int id, const std::string &data, int txn_id)
    {
        auto it = rows_.find(id);
        if (it == rows_.end())
        {
            return;
        }

        it->second.versions.emplace_back(txn_id, data);
    }

    void Table::remove(int id, int txn_id)
    {
        auto it = rows_.find(id);
        if (it == rows_.end())
        {
            return;
        }

        if (!it->second.versions.empty())
        {
            it->second.versions.back().deleted_txn = txn_id;
        }
    }

    void Table::rollback(int txn_id)
    {
        for (auto row_it = rows_.begin(); row_it != rows_.end();)
        {
            auto &versions = row_it->second.versions;

            versions.erase(
                std::remove_if(
                    versions.begin(),
                    versions.end(),
                    [txn_id](const Version &version)
                    {
                        return version.created_txn == txn_id;
                    }),
                versions.end());

            for (auto &version : versions)
            {
                if (version.deleted_txn == txn_id)
                {
                    version.deleted_txn = -1;
                }
            }

            if (versions.empty())
            {
                row_it = rows_.erase(row_it);
            }
            else
            {
                ++row_it;
            }
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
