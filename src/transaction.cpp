#include <iostream>
#include "transaction.h"

Transaction::Transaction(const int32_t id_, const std::string& name_, bool read_only_/*=false*/): id(id_), name(name_), read_only(read_only_) {}

int32_t Transaction::get_id() const {
    return id;
}

std::string& Transaction::get_name() {
    return name;
}

TransactionStatus Transaction::get_status() const {
    return status;
}

bool Transaction::is_read_only() const {
    return read_only;
}

void Transaction::set_status(const TransactionStatus status_) {
    status = status_;
}

std::unordered_map<std::string, std::vector<int64_t>>& Transaction::get_read_variables() {
    return read_variables;
}

std::unordered_map<std::string, int64_t>& Transaction::get_uncommitted_variables() {
    return uncommitted_variables;
}

void Transaction::clear_uncommitted_variables() {
    uncommitted_variables.clear();
}

std::unordered_map<std::string, int64_t>& Transaction::get_variable_values() {
    return variable_values;
}

void Transaction::set_variable_values(std::unordered_map<std::string, int64_t> variable_values_) {
    variable_values = std::move(variable_values_);
}

bool Transaction::operator==(const Transaction& transaction_) const {
    if(id == transaction_.id and name == transaction_.name and read_only == transaction_.read_only) {
        return true;
    }
    return false;
}