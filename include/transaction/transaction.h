#pragma once

namespace mvcc
{

    struct Transaction
    {
        int txn_id;
        bool active;

        Transaction(int id) : txn_id(id), active(true) {}
    };

}