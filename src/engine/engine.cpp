#include "engine/engine.h"

#include "storage/table.h"
#include "utils/logger.h"

namespace mvcc
{

    Engine::Engine() : next_txn_id_(1), next_commit_ts_(0)
    {
        table_ = std::make_unique<Table>();
    }

    Engine::~Engine() {}

    const Transaction *Engine::findTransaction(int txn_id) const
    {
        auto it = transactions_.find(txn_id);
        if (it == transactions_.end())
        {
            return nullptr;
        }

        return &it->second;
    }

    int Engine::beginTransaction()
    {
        int id = next_txn_id_++;
        transactions_.emplace(id, Transaction(id, next_commit_ts_));
        Logger::log("Begin txn " + std::to_string(id));
        return id;
    }

    void Engine::commitTransaction(int txn_id)
    {
        const Transaction *txn = findTransaction(txn_id);
        if (txn == nullptr || txn->state != TxnState::ACTIVE)
        {
            Logger::log("Commit txn " + std::to_string(txn_id) + " ignored");
            return;
        }

        Transaction &mutable_txn = transactions_.at(txn_id);
        mutable_txn.state = TxnState::COMMITTED;
        mutable_txn.commit_ts = ++next_commit_ts_;
        Logger::log("Commit txn " + std::to_string(txn_id));
    }

    void Engine::rollbackTransaction(int txn_id)
    {
        const Transaction *txn = findTransaction(txn_id);
        if (txn == nullptr || txn->state != TxnState::ACTIVE)
        {
            Logger::log("Rollback txn " + std::to_string(txn_id) + " ignored");
            return;
        }

        table_->rollback(txn_id);
        transactions_.at(txn_id).state = TxnState::ABORTED;
        Logger::log("Rollback txn " + std::to_string(txn_id));
    }

    void Engine::insert(int id, const std::string &data, int txn_id)
    {
        const Transaction *txn = findTransaction(txn_id);
        if (txn == nullptr || txn->state != TxnState::ACTIVE)
        {
            Logger::log("Insert ignored for txn " + std::to_string(txn_id));
            return;
        }

        table_->insert(id, data, txn_id);
    }

    std::string Engine::read(int id, int txn_id)
    {
        const Transaction *txn = findTransaction(txn_id);
        if (txn == nullptr || txn->state == TxnState::ABORTED)
        {
            return "NOT_FOUND";
        }

        return table_->read(
            id,
            txn_id,
            [this](const Version &version, int reader_txn_id)
            {
                return classifyVersion(version, reader_txn_id);
            });
    }

    void Engine::update(int id, const std::string &data, int txn_id)
    {
        const Transaction *txn = findTransaction(txn_id);
        if (txn == nullptr || txn->state != TxnState::ACTIVE)
        {
            Logger::log("Update ignored for txn " + std::to_string(txn_id));
            return;
        }

        table_->update(id, data, txn_id);
    }

    void Engine::remove(int id, int txn_id)
    {
        const Transaction *txn = findTransaction(txn_id);
        if (txn == nullptr || txn->state != TxnState::ACTIVE)
        {
            Logger::log("Remove ignored for txn " + std::to_string(txn_id));
            return;
        }

        table_->remove(id, txn_id);
    }

    void Engine::printState() const
    {
        table_->printState();
    }

    VersionVisibility Engine::classifyVersion(const Version &version, int reader_txn_id) const
    {
        const Transaction *reader_ptr = findTransaction(reader_txn_id);
        if (reader_ptr == nullptr)
        {
            return VersionVisibility::INVISIBLE;
        }

        const Transaction &reader = *reader_ptr;
        if (reader.state == TxnState::ABORTED)
        {
            return VersionVisibility::INVISIBLE;
        }

        const Transaction *creator_ptr = findTransaction(version.created_txn);
        if (creator_ptr == nullptr || creator_ptr->state == TxnState::ABORTED)
        {
            return VersionVisibility::INVISIBLE;
        }

        const Transaction &creator = *creator_ptr;
        const bool creator_visible =
            creator.txn_id == reader.txn_id ||
            (creator.state == TxnState::COMMITTED && creator.commit_ts <= reader.begin_ts);

        if (!creator_visible)
        {
            return VersionVisibility::INVISIBLE;
        }

        if (version.deleted_txn == -1)
        {
            return VersionVisibility::VISIBLE;
        }

        const Transaction *deleter_ptr = findTransaction(version.deleted_txn);
        if (deleter_ptr == nullptr || deleter_ptr->state == TxnState::ABORTED)
        {
            return VersionVisibility::VISIBLE;
        }

        const Transaction &deleter = *deleter_ptr;
        const bool delete_visible =
            deleter.txn_id == reader.txn_id ||
            (deleter.state == TxnState::COMMITTED && deleter.commit_ts <= reader.begin_ts);

        return delete_visible ? VersionVisibility::DELETED : VersionVisibility::VISIBLE;
    }

}
