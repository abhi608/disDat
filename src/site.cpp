#include <iostream>
#include "site.h"
#include "locktable.h"
#include "lock.h"
#include "transaction.h"

Site::Site(const int32_t id_): id(id_), status(UP), data_manager(id) {
    for(int32_t i=1; i<=20; i++) {
        if((i%2 == 0) or (1+(i%10) == id_)) {
            recovered_variables.insert("x"+std::to_string(i));
        }
    }
}

void Site::set_status(SiteStatus status_) {
    status = status_;
}

SiteStatus Site::get_status() const {
    return status;
}

int32_t Site::get_id() const {
    return id;
}

bool Site::get_lock(Transaction& transaction_, const LockType lock_type_, const std::string& variable_) {
    if(data_manager.get_lock(transaction_, lock_type_, variable_)) {
        recovered_variables.insert(variable_);
        if(recovered_variables.size() == data_manager.get_variables().size()) {
            status = UP;
        }
        return true;
    }
    return false;
}

void Site::clear_lock(const Lock& lock_, const std::string& variable_) {
    data_manager.clear_lock(lock_, variable_);
}

void Site::write_variable(const Transaction& transaction_, const std::string& variable_name_, int64_t value_) {
    if(status != DOWN and recovered_variables.find(variable_name_) != recovered_variables.end()) {
        data_manager.write_variable(transaction_, variable_name_, value_);
    }
}

void Site::listen() {
    set_status(UP);
}

void Site::fail() {
    set_status(DOWN);
    recovered_variables.clear();
    LockTable& lock_table = data_manager.get_lock_table();
    std::unordered_map<std::string, std::vector<Lock>>& lock_map = lock_table.get_lock_map();
    for(auto& [variable, locks] : lock_map) {
        for(auto& lock : locks) {
            std::cout << lock.get_transaction().get_name() << " aborted as site " << id << " failed" << std::endl;
            lock.get_transaction().set_status(ABORTED);
        }
    }
}

void Site::recover() {
    std::unordered_map<std::string, Variable>& variable_map = data_manager.get_variables();
    for(auto& [variable_name, variable] : variable_map) {
        if(std::stoi(variable_name.substr(1, variable_name.size()-1))%2 != 0) {
            recovered_variables.insert(variable_name);
        }
    }
    set_status(RECOVERING);
}

void Site::dump_site() {
    std::cout << "=== Site " << id << " ===" << std::endl;
    if(status == DOWN) {
        std::cout << "This site is down" << std:: endl;
        return;
    }
    size_t count = 0;
    std::unordered_map<std::string, Variable>& variable_map = data_manager.get_variables();
    for(auto& [variable_name, variable] : variable_map) {
        if(status == RECOVERING) {
            count++;
            if(recovered_variables.find(variable.get_name()) == recovered_variables.end()) {
                std::cout << variable.get_name() << " is not available for reading" << std::endl;
            } else {
                std::cout << variable.get_name() << ": " << variable.get_value() << " (available at site " << id << " for reading as it is the only copy or has been written after recovery)" << std::endl;
            }
            continue;            
        }
        if(variable.get_value() != std::stoi(variable_name.substr(1, variable_name.size()-1))*10) {
            count++;
            std::cout << variable.get_name() << ": " << variable.get_value() << " at site " << id << std::endl;
        }
    }
    if(count != data_manager.get_variables().size()) {
        std::cout << "All other variables have same initial value" << std::endl;
    }
}

std::vector<Variable> Site::get_all_variables() {
    std::vector<Variable> variables;
    std::unordered_map<std::string, Variable>& variable_map = data_manager.get_variables();
    for(auto& [variable_name, variable] : variable_map) {
        variables.emplace_back(variable);
    }
    return variables;
}