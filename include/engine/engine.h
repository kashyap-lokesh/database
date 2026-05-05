#pragma once

#include <string>
#include <memory>

namespace mvcc
{

    class Table;
    class Transaction;

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
        std::unique_ptr<Table> table_;
        int next_txn_id_;
    };

} // namespace mvcc