#include "engine/engine.h"
#include "storage/table.h"
#include "utils/logger.h"

namespace mvcc
{

    Engine::Engine() : next_txn_id_(1)
    {
        table_ = std::make_unique<Table>();
    }

    Engine::~Engine() {}

    int Engine::beginTransaction()
    {
        int id = next_txn_id_++;
        Logger::log("Begin txn " + std::to_string(id));
        return id;
    }

    void Engine::commitTransaction(int txn_id)
    {
        Logger::log("Commit txn " + std::to_string(txn_id));
    }

    void Engine::rollbackTransaction(int txn_id)
    {
        Logger::log("Rollback txn " + std::to_string(txn_id));
    }

    void Engine::insert(int id, const std::string &data, int txn_id)
    {
        table_->insert(id, data, txn_id);
    }

    std::string Engine::read(int id, int txn_id)
    {
        return table_->read(id, txn_id);
    }

    void Engine::update(int id, const std::string &data, int txn_id)
    {
        table_->update(id, data, txn_id);
    }

    void Engine::remove(int id, int txn_id)
    {
        table_->remove(id, txn_id);
    }

    void Engine::printState() const
    {
        table_->printState();
    }

}