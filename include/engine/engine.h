#pragma once

#include <memory>
#include <unordered_map>
#include <string>

#include "storage/version.h"
#include "transaction/transaction.h"

namespace mvcc
{

    class Table;

    class Engine
    {
    public:
        Engine();
        ~Engine();

        int beginTransaction();
        void commitTransaction(int txn_id);
        void rollbackTransaction(int txn_id);

        void insert(int id, const std::string &data, int txn_id);
        std::string read(int id, int txn_id);
        void update(int id, const std::string &data, int txn_id);
        void remove(int id, int txn_id);

        void printState() const;

    private:
        VersionVisibility classifyVersion(const Version &version, int reader_txn_id) const;
        const Transaction *findTransaction(int txn_id) const;

        std::unique_ptr<Table> table_;
        std::unordered_map<int, Transaction> transactions_;
        int next_txn_id_;
        int next_commit_ts_;
    };

} // namespace mvcc
