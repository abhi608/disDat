#ifndef TRANSACTIONMANAGER_1234_H
#define TRANSACTIONMANAGER_1234_H

#include <map>
#include "transaction.h"
#include "lock.h"
#include "locktable.h"
#include "sitemanager.h"
#include "variable.h"
#include "constants.h"

enum InstructionType {READ_INSTR, READ_ONLY_INSTR, WRITE_INSTR};

struct TransactionInfo {
    size_t len;
    std::string transaction;
    InstructionType instruction_type;
    std::string variable;
    int64_t value;
};

class TransactionManager {
private:
    int32_t number_of_variables;
    int32_t number_of_sites;
    std::unordered_map<std::string, Transaction*> transaction_map;
    LockTable* lock_table;
    SiteManager* site_manager;
    int64_t current_time;
    std::map<std::int64_t, std::unordered_map<std::string, TransactionInfo>> blocked_transactions;
    std::map<std::int64_t, std::unordered_map<std::string, TransactionInfo>> waiting_transactions;

public:
    TransactionManager(const int32_t num_vars_, const int32_t num_sites_, LockTable* lock_table_, SiteManager* site_manager_);
    void clear_locks(Transaction* transaction);
    void abort(const std::string& name);
    void clear_aborted();
    std::unordered_map<std::string, std::vector<TransactionInfo>> get_squashed_blocked_transactions();
    std::unordered_map<std::string, std::vector<TransactionInfo>> get_squashed_waiting_transactions();
    void clear_deadlock(std::vector<std::string>& transaction_list, const int32_t index);
    void detect_deadlock(const std::string& transaction, std::unordered_map<std::string, int32_t>& visited, std::vector<std::string>& current, std::unordered_map<std::string, std::vector<TransactionInfo>>& blocked_dict);
    void detect_and_clear_deadlocks();
    void blocked_to_waiting();
    void begin(std::vector<std::string>& params);
    void begin_read_only(std::vector<std::string>& params);
    void write_request(std::vector<std::string>& params);
    void read_request_read_only(Transaction* transaction, const std::string& variable, const std::string& transaction_name, bool try_waiting);
    void read_request(std::vector<std::string>& params, bool try_waiting=false);
    void try_waiting();
    void commit_transaction(const std::string& name);
    void end(const std::vector<std::string>& params);
    void tick(Instruction& instruction);
};

#endif