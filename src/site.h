#ifndef SITE_1234_H
#define SITE_1234_H

#include <unordered_set>
#include <string>
#include <vector>
#include "datamanager.h"
#include "transaction.h"
#include "variable.h"

enum SiteStatus {UP, DOWN, RECOVERING};

class Site {
private:
    int32_t id;
    SiteStatus status;
    DataManager data_manager;
    std::unordered_set<std::string> recovered_variables;

public:
    Site(const int32_t id_);
    void set_status(SiteStatus status_);
    SiteStatus get_status() const;
    int32_t get_id() const;
    bool get_lock(Transaction& transaction_, const LockType lock_type_, const std::string& variable_);
    void clear_lock(const Lock& lock_, const std::string& variable_);
    void write_variable(const Transaction& transaction_, const std::string& variable_name_, int64_t value_);
    void listen();
    void fail();
    void recover();
    void dump_site();
    std::vector<Variable> get_all_variables();
};

#endif