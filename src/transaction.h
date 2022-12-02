#ifndef TRANSACTION_1234_H
#define TRANSACTION_1234_H

#include <string>
#include <unordered_map>
#include <vector>

enum TransactionStatus {RUNNING, WAITING, BLOCKED, ABORTED, COMMITTED};

class Transaction {
private:
    TransactionStatus status;
    int32_t id;
    std::string name;
    bool read_only;
    std::unordered_map<std::string, int64_t> uncommitted_variables;
    std::unordered_map<std::string, std::vector<int64_t>> read_variables;
    std::unordered_map<std::string, int64_t> variable_values;

public:
    Transaction(const int32_t id_, const std::string& name_, bool read_only_=false);  
    int32_t get_id() const;
    std::string& get_name();
    TransactionStatus get_status() const;
    bool is_read_only() const;
    void set_status(const TransactionStatus status_);
    std::unordered_map<std::string, std::vector<int64_t>>& get_read_variables();
    std::unordered_map<std::string, int64_t>& get_uncommitted_variables();
    void clear_uncommitted_variables();
    std::unordered_map<std::string, int64_t>& get_variable_values();
    void set_variable_values(std::unordered_map<std::string, int64_t> variable_values_);
    bool operator==(const Transaction& transaction_) const;
};

#endif