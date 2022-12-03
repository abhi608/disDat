/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/29/2022
 */

#include <iostream>
#include "datamanager.h"

DataManager::DataManager(const int32_t site_id_): site_id(site_id_) {
    for(int32_t i=1; i<=20; i++) {
        if((i%2 == 0) or (1+(i%10) == site_id_)) {
            std::string name_ = "x"+std::to_string(i);
            Variable* variable_ = new Variable(i, name_, 10*i, site_id_);
            variable_map[name_] = variable_;
        }
    }
}

void DataManager::add_variables(const std::string& name_, Variable* variable_) {
    variable_map[name_] = variable_;
}

std::optional<Variable*> DataManager::get_variable(const std::string& name_) {
    if(variable_map.find(name_) == variable_map.end()) return {};
    return {variable_map[name_]};
}

bool DataManager::has_variable(const std::string& name_) {
    return (variable_map.find(name_) != variable_map.end());
}

void DataManager::clear_lock(Lock* lock_, const std::string& variable_) {
    lock_table.clear_lock(lock_, variable_);
}

LockTable& DataManager::get_lock_table() {
    return lock_table;
}

bool DataManager::get_lock(Transaction* transaction_, const LockType lock_type_, const std::string& variable_) {
    bool is_locked_by_txn = lock_table.is_locked_by_transaction(transaction_, variable_);
    if(is_locked_by_txn) {
        if(lock_table.get_len_locks(variable_) == 1) {
            lock_table.set_lock(transaction_, lock_type_, variable_);
            return true;
        }
        return false;
    }
    if(lock_type_ == WRITE and !lock_table.is_locked(variable_)) {
        lock_table.set_lock(transaction_, lock_type_, variable_);
        return true;
    }
    if(lock_type_ == READ and !lock_table.is_write_locked(variable_)) {
        lock_table.set_lock(transaction_, lock_type_, variable_);
        return true;
    }
    if(lock_type_ == WRITE) std::cout << "DEBUG| " << transaction_->get_name() << " did not get write lock on " << variable_ << " site: " << site_id << std::endl;
    else std::cout << "DEBUG| " << transaction_->get_name() << " did not get read lock on " << variable_ << " site: " << site_id << std::endl;
    return false;
}

bool DataManager::write_variable(Transaction* transaction_, const std::string& variable_name_, int64_t value_) {
    if(lock_table.is_locked_by_transaction(transaction_, variable_name_, WRITE)) {
        variable_map[variable_name_]->set_value(value_);
        std::cout << transaction_->get_name() << " write the value " << value_ << " of variable " << variable_name_ << std::endl;
        return true;
    }
    return false;
}

std::unordered_map<std::string, Variable*>& DataManager::get_variables() {
    return variable_map;
}
