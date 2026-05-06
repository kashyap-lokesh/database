#pragma once

namespace mvcc
{

    enum class TxnState
    {
        ACTIVE,
        COMMITTED,
        ABORTED
    };

    enum class VersionVisibility
    {
        INVISIBLE,
        VISIBLE,
        DELETED
    };

    struct Transaction
    {
        int txn_id;
        int begin_ts;
        int commit_ts;
        TxnState state;

        Transaction(int id, int begin)
            : txn_id(id), begin_ts(begin), commit_ts(-1), state(TxnState::ACTIVE) {}
    };

}
