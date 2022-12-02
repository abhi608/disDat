#ifndef SITEMANAGER_1234_H
#define SITEMANAGER_1234_H

#include <cstddef>
#include <vector>
#include "site.h"
#include "variable.h"
#include "locktable.h"
#include "constants.h"
#include "instruction.h"

enum LockAcquireStatus {ALL_SITES_DOWN, NO_LOCK, GOT_LOCK, GOT_LOCK_RECOVERING};

class SiteManager {
private:
    std::size_t num_sites;
    std::size_t num_variables;
    std::vector<Site*> sites;
    struct UtilFuncs;

public:
    SiteManager(const std::size_t num_sites_, const std::size_t num_variables_);
    std::vector<int32_t> get_sites(const int32_t id_);
    std::vector<int32_t> get_sites(const std::string& variable_);
    Site* get_site(const int32_t index_);
    void fail(const int32_t index_);
    void recover(const int32_t index_);
    void tick(Instruction& instruction_);
    LockAcquireStatus get_locks(Transaction* transaction_, const LockType lock_type_, const std::string& variable_);
    std::unordered_map<std::string, int64_t> get_current_variables(const std::string& var_);
    LockTable get_set_locks();
    void clear_locks(Lock* lock_, const std::string& variable_name_);
    void start();
};

#endif