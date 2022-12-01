#include <iostream>
#include "locktable.h"

LockTable::LockTable() = default;

std::unordered_map<std::string, std::vector<Lock*>>& LockTable::get_lock_map() {
    return lock_map;
}

size_t LockTable::get_len_locks(const std::string& variable_) {
    if(lock_map.find(variable_) == lock_map.end()) return 0;
    return lock_map[variable_].size();
}

void LockTable::set_lock(Transaction& transaction_, const LockType lock_type_, const std::string& variable_) {
    Lock* lock = new Lock(lock_type_, &transaction_);
    for(const Lock* lck : lock_map[variable_]) {
        if((*lck) == (*lock)) return;
    }
    lock_map[variable_].push_back(lock);
}

bool LockTable::is_locked(const std::string& variable_) {
    if(lock_map.find(variable_) == lock_map.end()) return false;
    return (lock_map[variable_].size() != 0);
}

bool LockTable::is_write_locked(const std::string& variable_) {
    if(lock_map.find(variable_) == lock_map.end()) return false;
    for(const Lock* lck : lock_map[variable_]) {
        if(lck->get_lock_type() == WRITE) return true;
    }
    return false;
}

bool LockTable::is_read_locked(const std::string& variable_) {
    if(lock_map.find(variable_) == lock_map.end()) return false;
    for(const Lock* lck : lock_map[variable_]) {
        if(lck->get_lock_type() == READ) return true;
    }
    return false;
}

void LockTable::free(const std::string& variable_) {
    lock_map.erase(variable_);
}

bool LockTable::clear_lock(Lock& lock_, const std::string& variable_) {
    if(lock_map.find(variable_) == lock_map.end()) return false;
    std::erase_if(lock_map[variable_], [&lock_](Lock* lck) {return (*lck) == lock_;});
    if(lock_map[variable_].size() == 0) lock_map.erase(variable_);
    return true;
}

bool LockTable::is_locked_by_transaction(const Transaction& current_transaction_, const std::string& variable_, const LockType lock_type_/*=NOLOCK*/) {
    if(lock_map.find(variable_) == lock_map.end()) return false;
    for(Lock* lck : lock_map[variable_]) {
        Transaction* transaction_ = lck->get_transaction();
        if(current_transaction_.get_id() == transaction_->get_id()) {
            if(lock_type_ == NOLOCK or lock_type_ == lck->get_lock_type()) return true;
        }
    }
    return false;
}
