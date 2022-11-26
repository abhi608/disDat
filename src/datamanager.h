#ifndef DATAMANAGER_1234_H
#define DATAMANAGER_1234_H

#include <unordered_map>
#include <string>
#include "variable.h"
#include "locktable.h"

class DataManager {
private:
    int32_t site_id;
    LockTable lock_table;
    std::unordered_map<std::string, Variable> variable_map;

public:
    DataManager(const int32_t site_id_);
    void add_variables(const std::string& name_, const Variable& variable_);
    std::optional<Variable> get_variable(const std::string& name_);
    bool has_variable(const std::string& name_);
    void clear_lock(const Lock& lock_, const std::string& variable_);
    LockTable& get_lock_table();
    bool get_lock(Transaction& transaction_, const LockType lock_type_, const std::string& variable_);
    bool write_variable(const Transaction& transaction_, const std::string& variable_name_, int64_t value_);
    std::unordered_map<std::string, Variable>& get_variables();
};

#endif