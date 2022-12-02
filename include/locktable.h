#ifndef LOCKTABLE_1234_H
#define LOCKTABLE_1234_H

#include <map>
#include <string>
#include <vector>
#include "variable.h"
#include "lock.h"

class LockTable {
private:
    std::map<std::string, std::vector<Lock*>> lock_map;

public:
    LockTable();
    std::map<std::string, std::vector<Lock*>>& get_lock_map();
    size_t get_len_locks(const std::string& variable_);
    void set_lock(Transaction* transaction_, const LockType lock_type_, const std::string& variable_);
    bool is_locked(const std::string& variable_);
    bool is_write_locked(const std::string& variable_);
    bool is_read_locked(const std::string& variable_);
    void free(const std::string& variable_);
    bool clear_lock(Lock* lock_, const std::string& variable_);
    void set_lock_map(std::map<std::string, std::vector<Lock*>> lock_map_);
    bool is_locked_by_transaction(Transaction* current_transaction_, const std::string& variable_, const LockType lock_type_=NOLOCK);
};

#endif