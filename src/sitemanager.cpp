#include <iostream>
#include "sitemanager.h"

struct SiteManager::UtilFuncs {
    static void _check_index_sanity(SiteManager& self, int32_t index_) {
        if(index_ > static_cast<int32_t>(self.num_sites) or index_ <= 0) {
            std::cout << "Index must be in the range 1 to " << self.num_sites << std::endl;
            exit(1);
        }
    }
};

SiteManager::SiteManager(const std::size_t num_sites_, const std::size_t num_variables_): num_sites(num_sites_), num_variables(num_variables_) {
    Site *site0 = new Site(0);
    sites.push_back(site0);
    for(int32_t i=1; i<= static_cast<int32_t>(num_sites_); i++) {
        Site *site_i = new Site(i);
        sites.push_back(site_i);
    }
}

std::vector<int32_t> SiteManager::get_sites(const int32_t id_) {
    std::vector<int32_t> sites_;
    if(id_%2 == 0) {
        for(int32_t i=1; i<= static_cast<int32_t>(num_sites); i++) {
            sites_.emplace_back(i);
        }
    } else {
        sites_.emplace_back(1 + (id_%10));
    }
    return sites_;
}

std::vector<int32_t> SiteManager::get_sites(const std::string& variable_) {
    return get_sites(std::stoi(variable_.substr(1, variable_.size()-1)));
}

Site* SiteManager::get_site(const int32_t index_) {
    UtilFuncs::_check_index_sanity(*this, index_);
    return sites[index_];
}

void SiteManager::fail(const int32_t index_) {
    Site* site_ = get_site(index_);
    std::cout << "Site: " << index_ << " failed" << std::endl;
    site_->fail();
}

void SiteManager::recover(const int32_t index_) {
    Site* site_ = get_site(index_);
    std::cout << "Site: " << index_ << " recovered" << std::endl;
    site_->recover();
}

void SiteManager::tick(Instruction& instruction_) {
    std::vector<std::string>& params = instruction_.get_params();
    if(instruction_.get_instruction_type() == DUMP_FUNC) {
        if(params[0].size() == 0) {
            for(int32_t i=1; i<= num_sites; i++) {
                get_site(i)->dump_site();
            }
        } else if(params[0][0] == 'x') {
            std::vector<int32_t> sites_ = get_sites(std::stoi(params[0].substr(1, params[0].size()-1)));
            for(int32_t site_ : sites_) {
                std::vector<Variable*> variables_ = get_site(site_)->get_all_variables();
                for(Variable* variable_ : variables_) {
                    if(variable_->get_name() == params[0]) {
                        std::cout << variable_->get_value();
                    }
                }
            }
        } else if(params[0].size() == 2) {
            get_site(std::stoi(params[0]))->dump_site();
        }
    } else if(instruction_.get_instruction_type() == FAIL_FUNC) {
        fail(std::stoi(params[0]));
    } else if(instruction_.get_instruction_type() == RECOVER_FUNC) {
        recover(std::stoi(params[0]));
    }
}

LockAcquireStatus SiteManager::get_locks(Transaction* transaction_, const LockType lock_type_, const std::string& variable_) {
    std::vector<int32_t> sites_ = get_sites(variable_);
    bool flag = true, recovering_flag = false, all_sites_down = true, even_index = (std::stoi(variable_.substr(1, variable_.size()-1))%2 == 0);
    for(int32_t site_ : sites_) {
        SiteStatus status_ = get_site(site_)->get_status();
        if(status_ == DOWN) continue;
        if(status_ == RECOVERING and lock_type_ == READ) {
            if(!get_site(site_)->is_in_recovered_variables(variable_)) continue;
            if(!even_index) recovering_flag = true;
        }
        all_sites_down = false;
        bool state_ = get_site(site_)->get_lock(transaction_, lock_type_, variable_);
        if(state_ and lock_type_ == READ) {
            if(recovering_flag) return GOT_LOCK_RECOVERING;
            else return GOT_LOCK;
        }
        flag &= state_;
    }
    if(all_sites_down) return ALL_SITES_DOWN;
    else if(!flag) return NO_LOCK;
    return GOT_LOCK; 
}

std::unordered_map<std::string, int64_t> SiteManager::get_current_variables(const std::string& var_) {
    std::unordered_map<std::string, int64_t> variable_values;
    for(int32_t i=1; i<=num_sites; i++) {
        Site* site_ = get_site(i);
        if(site_->get_status() == UP) {
            std::vector<Variable*> variables_ = site_->get_all_variables();
            for(Variable* variable_ : variables_) {
                if(var_ != "" and variable_->get_name() == var_) {
                    variable_values.clear();
                    variable_values[var_] = variable_->get_value();
                    return std::move(variable_values);
                }
                variable_values[variable_->get_name()] = variable_->get_value();
            }
            if(variable_values.size() == num_variables) return variable_values;
        } else if(site_->get_status() == RECOVERING) {
            std::vector<Variable*> variables_ = site_->get_all_variables();
            for(Variable* variable_ : variables_) {
                if(site_->is_in_recovered_variables(variable_->get_name())) {
                    if(var_ != "" and variable_->get_name() == var_) {
                        variable_values.clear();
                        variable_values[var_] = variable_->get_value();
                        return std::move(variable_values);
                    }
                    variable_values[variable_->get_name()] = variable_->get_value();
                }
            }
            if(variable_values.size() == num_variables) return variable_values;
        }
    }
    if(var_ == "") return variable_values;
    variable_values.clear();
    return variable_values;
}

LockTable SiteManager::get_set_locks() {
    std::map<std::string, std::vector<Lock*>> locks;
    for(int32_t i=1; i<=num_sites; i++) {
        // std::cout << "Site: " << i << std::endl;
        Site* site_ = get_site(i);
        std::map<std::string, std::vector<Lock*>>& lock_map = site_->get_data_manager().get_lock_table().get_lock_map();
        for(auto& [var_, curr_locks_] : lock_map) {
            if(locks.find(var_) == locks.end()) {
                std::vector<Lock*> tmp;
                locks[var_] = tmp;
            }  
            for(auto lock_ : curr_locks_) {
                bool is_present = false;
                for(auto* l : locks[var_]) {
                    if((*l) == (*lock_)) {
                        is_present = true;
                        break;
                    }
                }
                if(!is_present) {
                    // std::cout << var_ << ":" << lock_->get_lock_type() << " " << lock_->get_transaction()->get_id() <<std::endl;
                    locks[var_].push_back(lock_);
                }
            }
        }
    }
    LockTable lock_table;
    lock_table.set_lock_map(locks);
    // std::cout << "return lock table\n"; 
    // for(auto& l : lock_table.get_lock_map()["x2"]) {
    //     std::cout << "QWI: " << l->get_lock_type() << " ";
    // }
    // std::cout << "xyz\n";
    return lock_table;
}

// LockTable SiteManager::get_set_locks() {
//     LockTable lock_table;
//     for(int32_t i=1; i<=num_sites; i++) {
//         std::cout << "Site: " << i << std::endl;
//         Site* site_ = get_site(i);
//         std::map<std::string, std::vector<Lock*>>& lock_map = site_->get_data_manager().get_lock_table().get_lock_map();
//         for(auto& [var_, curr_locks_] : lock_map) {
//             for(Lock* lock_ : curr_locks_) {
//                 std::cout << var_ << ":" << lock_->get_lock_type() << std::endl;
//                 if()
//                 lock_table.get_lock_map()[var_].push_back(lock_);
//             }
//         }
//     }
//     return std::move(lock_table);
// }

void SiteManager::clear_locks(Lock* lock_, const std::string& variable_name_) {
    std::vector<int32_t> sites_ = get_sites(variable_name_);
    for(int32_t index : sites_) {
        get_site(index)->clear_lock(lock_, variable_name_);;
    }
}

void SiteManager::start() {
    for(int32_t i=1; i<=num_sites; i++) {
        get_site(i)->listen();
    }
}
