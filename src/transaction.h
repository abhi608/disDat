#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>

enum TransactionStatus {RUNNING, WAITING, BLOCKED, ABORTED, COMMITTED};

class Transaction {
private:
    TransactionStatus status;
    uint32_t id;
    std::string name;
    bool read_only;
    std::unordered_map<std::string, int64_t> uncommitted_variables;
    std::unordered_map<std::string, std::vector<int64_t>> read_variables;
    std::unordered_map<std::string, int64_t> variable_values;

public:
    Transaction(const uint32_t id_, const std::string& name_, bool read_only_=false);  
    uint32_t get_id() const;
    TransactionStatus get_status() const;
    bool is_read_only() const;
    void set_status(const TransactionStatus status_);
    std::unordered_map<std::string, std::vector<int64_t>>& get_read_variables();
    std::unordered_map<std::string, int64_t>& get_uncommitted_variables();
    void clear_uncommitted_variables();
};

#endif