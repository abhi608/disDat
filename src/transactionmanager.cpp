/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 12/02/2022
 */

#include <iostream>
#include "transactionmanager.h"

TransactionManager::TransactionManager(const int32_t num_vars_, const int32_t num_sites_, LockTable* lock_table_, SiteManager* site_manager_): number_of_variables(num_vars_), number_of_sites(num_sites_), lock_table(lock_table_), site_manager(site_manager_), current_time(0) {}

void TransactionManager::clear_locks(Transaction* transaction) {
    LockTable lock_table_ = site_manager->get_set_locks();
    std::map<std::string, std::vector<Lock *>>& lock_map = lock_table_.get_lock_map();
    for(auto& [var_name, locks] : lock_map) {
        for(auto* lock : locks) {
            if((*(lock->get_transaction())) == (*transaction)) {
                site_manager->clear_locks(lock, var_name);
                std::cout << "DEBUG| " << "Clearing site locks for " << transaction->get_name() << " variable: " << var_name << std::endl;
                if(lock_table->clear_lock(lock, var_name)) {
                    std::cout << "Clearing locks for " << transaction->get_name() << " variable: " << var_name << std::endl;
                }
            }
        }
    }
}

void TransactionManager::abort(const std::string& name) {
    std::vector<std::pair<int64_t, std::string>> to_pop_blocked;
    std::vector<std::pair<int64_t, std::string>> to_pop_waiting;
    for(auto& [time, v] : blocked_transactions) {
        v.erase(name);
    }
    for(auto& [time, v] : waiting_transactions) {
        v.erase(name);
    }
    Transaction* transaction_ = transaction_map[name];
    transaction_->set_status(ABORTED);
    clear_locks(transaction_);
}

void TransactionManager::clear_aborted() {
    for(const auto& [trn_name, transaction] : transaction_map) {
        if(transaction->get_status() == ABORTED) {
            abort(trn_name);
        }
    }
}

std::unordered_map<std::string, std::vector<TransactionInfo>> TransactionManager::get_squashed_blocked_transactions() {
    std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_blocked_transactions;
    for(auto& [_, blocked_dicts] : blocked_transactions) {
        for(auto& [transaction_name, blocked_transaction_info] : blocked_dicts) {
            squashed_blocked_transactions[transaction_name].push_back(blocked_transaction_info);
        }
    }
    return squashed_blocked_transactions;
}

std::unordered_map<std::string, std::vector<TransactionInfo>> TransactionManager::get_squashed_waiting_transactions() {
    std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_waiting_transactions;
    for(auto& [_, waiting_dicts] : waiting_transactions) {
        for(auto& [transaction_name, waiting_transaction_info] : waiting_dicts) {
            squashed_waiting_transactions[transaction_name].push_back(waiting_transaction_info);
        }
    }
    return squashed_waiting_transactions;
}

void TransactionManager::clear_deadlock(std::vector<std::string>& transaction_list_, const int32_t index) {
    std::vector<std::string> transaction_list(transaction_list_.begin()+index, transaction_list_.end());
    int32_t max_id = -1;
    std::string max_name = "";
    for(const auto& name : transaction_list) {
        Transaction* transaction = transaction_map[name];
        if(transaction->get_status() == ABORTED or transaction->get_status() == COMMITTED) return;
        if(max_id < transaction->get_id()) {
            max_id = transaction->get_id();
            max_name = name;
        }
    }
    std::cout << max_name << " aborted as it is youngest in a deadlock" << std::endl;
    abort(max_name);
}

void TransactionManager::detect_deadlock(const std::string& transaction, std::unordered_map<std::string, int32_t>& visited, std::vector<std::string>& current, std::unordered_map<std::string, std::vector<TransactionInfo>>& blocked_dict) {
    bool is_aborted = (transaction_map[transaction]->get_status() == TransactionStatus::ABORTED);
    bool is_committed = (transaction_map[transaction]->get_status() == TransactionStatus::COMMITTED);
    if(blocked_dict.find(transaction) != blocked_dict.end() and !is_aborted and !is_committed) {
        visited[transaction] = current.size()+1;
        current.push_back(transaction);
        for(const auto& block_ : blocked_dict[transaction]) {
            const std::string& block = block_.transaction;
            if(transaction_map[block]->get_status() == ABORTED) continue;
            if(visited.find(block) != visited.end()) {
                clear_deadlock(current, visited[block]-1);
            } else {
                detect_deadlock(block, visited, current, blocked_dict);
            }
        }
        current.pop_back();
        visited.erase(transaction);
    }
}

void TransactionManager::detect_and_clear_deadlocks() {
    std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_blocked_transactions = get_squashed_blocked_transactions();
    for(auto& [transaction, _] : squashed_blocked_transactions) {
        std::unordered_map<std::string, int32_t> visited;
        std::vector<std::string> current;
        detect_deadlock(transaction, visited, current, squashed_blocked_transactions);
    }
}

void TransactionManager::blocked_to_waiting() {
    std::vector<std::pair<int64_t, std::string>> to_pop;
    std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_blocked_transactions = get_squashed_blocked_transactions();
    for(auto& [blocked_dict_key, items] : blocked_transactions) {
        for(auto& [key, blocked_transaction_info] : items) {
            bool is_clear = true;
            Transaction* block = transaction_map[blocked_transaction_info.transaction];
            bool is_aborted = block->get_status();
            bool is_committed = block->get_status();
            is_clear &= (is_aborted or is_committed);
            if(is_clear) {
                for(auto it = squashed_blocked_transactions[key].begin(); it != squashed_blocked_transactions[key].end(); it++) {
                    if((*it).transaction == block->get_name()) {
                        squashed_blocked_transactions[key].erase(it--);
                        break;
                    }
                }
                to_pop.push_back({blocked_dict_key, key});
                if(squashed_blocked_transactions[key].size() == 0) {
                    bool flag = false;
                    std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_waiting_transactions = get_squashed_waiting_transactions();
                    if(squashed_blocked_transactions.find(key) != squashed_waiting_transactions.end()) {
                        for(auto& bl_transaction_info : squashed_waiting_transactions[key]) {
                            if(bl_transaction_info.len != blocked_transaction_info.len - 1) continue;
                            if(bl_transaction_info.len == 3 and (bl_transaction_info.instruction_type != blocked_transaction_info.instruction_type or bl_transaction_info.variable != blocked_transaction_info.variable or bl_transaction_info.value != blocked_transaction_info.value)) continue;
                            if(bl_transaction_info.len == 2 and (bl_transaction_info.instruction_type != blocked_transaction_info.instruction_type or bl_transaction_info.variable != blocked_transaction_info.variable)) continue;
                            flag = true;
                            break;
                        }
                    }
                    if(flag) continue;
                    TransactionInfo waiting_info = blocked_transaction_info;
                    waiting_info.len--;
                    waiting_transactions[current_time][key] = std::move(waiting_info);
                    Transaction* transaction = transaction_map[key];
                    transaction->set_status(WAITING);
                }
            }
        }
    }
    for(const auto& [k1, k2] : to_pop) {
        blocked_transactions[k1].erase(k2);
    }
}

void TransactionManager::begin(std::vector<std::string>& params) {
    int32_t current_index = transaction_map.size();
    std::cout << "Starting " << params[0] << std::endl;
    transaction_map[params[0]] = new Transaction(current_index, params[0]);
}

void TransactionManager::begin_read_only(std::vector<std::string>& params) {
    int32_t current_index = transaction_map.size();
    std::cout << "Starting read only transaction " << params[0] << std::endl;
    transaction_map[params[0]] = new Transaction(current_index, params[0], true);
    transaction_map[params[0]]->set_variable_values(site_manager->get_current_variables(""));
}

void TransactionManager::write_request(std::vector<std::string>& params) {
    std::string transaction_name = params[0], variable = params[1];
    int64_t value = std::stoi(params[2]);
    if(transaction_map.find(transaction_name) == transaction_map.end()) return;
    Transaction* transaction = transaction_map[transaction_name];
    bool is_waiting = (transaction->get_status() == WAITING);
    bool is_running = (transaction->get_status() == RUNNING);
    if(!(is_waiting or is_running)) return;
    if(lock_table->is_locked_by_transaction(transaction, variable, LockType::WRITE)) {
        lock_table->set_lock(transaction, LockType::WRITE, variable);
        std::cout << transaction->get_name() << " already has a write lock on " << variable << std::endl;
        transaction->get_uncommitted_variables()[variable] = value;
        transaction->set_status(TransactionStatus::RUNNING);
        return;
    }
    LockAcquireStatus lock_acquire_status = site_manager->get_locks(transaction, LockType::WRITE, variable);
    if(lock_acquire_status == LockAcquireStatus::GOT_LOCK) {
        std::cout << transaction->get_name() << " got write lock on " << variable << std::endl;
        lock_table->set_lock(transaction, LockType::WRITE, variable);
        transaction->get_uncommitted_variables()[variable] = value;
        std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_waiting_transactions = get_squashed_waiting_transactions();
        if(squashed_waiting_transactions.find(transaction->get_name()) != squashed_waiting_transactions.end()) {
            for(auto& waiting_transaction_info : squashed_waiting_transactions[transaction->get_name()]) {
                if(waiting_transaction_info.variable != variable) return;
            }
        }
        transaction->set_status(TransactionStatus::RUNNING);
    } else if(lock_acquire_status == LockAcquireStatus::ALL_SITES_DOWN) {
        TransactionInfo waiting_transaction_info;
        waiting_transaction_info.instruction_type = InstructionType::WRITE_INSTR;
        waiting_transaction_info.variable = variable;
        waiting_transaction_info.value = value;
        waiting_transaction_info.len = 3;

        std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_waiting_transactions = get_squashed_waiting_transactions();
        if(squashed_waiting_transactions.find(transaction->get_name()) != squashed_waiting_transactions.end()) {
            for(auto& wt_transaction_info : squashed_waiting_transactions[transaction->get_name()]) {
                if(wt_transaction_info.len == waiting_transaction_info.len and wt_transaction_info.instruction_type == waiting_transaction_info.instruction_type and wt_transaction_info.variable == waiting_transaction_info.variable and wt_transaction_info.value == waiting_transaction_info.value) return;
            }
        }
        std::cout << transaction->get_name() << " is waiting on " << variable << std::endl;
        transaction->set_status(TransactionStatus::WAITING);
        waiting_transactions[current_time][transaction->get_name()] = std::move(waiting_transaction_info);
    } else {
        LockTable lock_table_ = site_manager->get_set_locks();
        for(auto* lock : lock_table_.get_lock_map()[variable]) {
            const std::string& blocking_transaction = lock->get_transaction()->get_name();
            if((*(lock->get_transaction())) == (*transaction)) continue;
            
            TransactionInfo blocking_transaction_info;
            blocking_transaction_info.transaction = blocking_transaction;
            blocking_transaction_info.instruction_type = InstructionType::WRITE_INSTR;
            blocking_transaction_info.variable = variable;
            blocking_transaction_info.value = value;
            blocking_transaction_info.len = 4;

            std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_blocking_transactions = get_squashed_blocked_transactions();
            if(squashed_blocking_transactions.find(transaction->get_name()) != squashed_blocking_transactions.end()) {
                for(auto& bk_transaction_info : squashed_blocking_transactions[transaction->get_name()]) {
                    if(bk_transaction_info.len == blocking_transaction_info.len and bk_transaction_info.transaction == blocking_transaction_info.transaction and bk_transaction_info.instruction_type == blocking_transaction_info.instruction_type and bk_transaction_info.variable == blocking_transaction_info.variable and bk_transaction_info.value == blocking_transaction_info.value) return;
                }
            }
            std::cout << transaction->get_name() << " is blocked for a write lock by " << blocking_transaction << " on " << variable << std::endl;
            transaction->set_status(TransactionStatus::BLOCKED);
            current_time++;
            blocked_transactions[current_time][transaction_name] = std::move(blocking_transaction_info);
        }
    }
}

void TransactionManager::read_request_read_only(Transaction* transaction, const std::string& variable, const std::string& transaction_name, bool try_waiting) {
    if(try_waiting) {
        std::unordered_map<std::string, int64_t> val = site_manager->get_current_variables(variable);
        if(val.size() == 0) transaction->get_variable_values()[variable] = INT64_MIN;
        else return;
        if(transaction->get_variable_values().find(variable) != transaction->get_variable_values().end()) {
            transaction->get_read_variables()[variable].push_back(transaction->get_variable_values()[variable]);
            std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_waiting_transactions = get_squashed_waiting_transactions();
            if(squashed_waiting_transactions.find(transaction->get_name()) != squashed_waiting_transactions.end()) {
                for(auto& waiting_transaction_info : squashed_waiting_transactions[transaction->get_name()]) {
                    if(waiting_transaction_info.variable != variable) return;
                }
            }
            transaction->set_status(TransactionStatus::RUNNING);
        } else {
            TransactionInfo waiting_transaction_info;
            waiting_transaction_info.instruction_type = InstructionType::READ_ONLY_INSTR;
            waiting_transaction_info.variable = variable;
            waiting_transaction_info.len = 2;

            std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_waiting_transactions = get_squashed_waiting_transactions();
            if(squashed_waiting_transactions.find(transaction_name) != squashed_waiting_transactions.end()) {
                for(auto& wt_transaction_info : squashed_waiting_transactions[transaction_name]) {
                    if(wt_transaction_info.len == waiting_transaction_info.len and wt_transaction_info.instruction_type == waiting_transaction_info.instruction_type and wt_transaction_info.variable == waiting_transaction_info.variable) return;
                }
            }
            transaction->set_status(TransactionStatus::WAITING);
            std::cout << transaction->get_name() << " is waiting on " << variable << std::endl;
            waiting_transactions[current_time][transaction_name] = std::move(waiting_transaction_info);
        }
    }
}

void TransactionManager::read_request(std::vector<std::string>& params, bool try_waiting/*=false*/) {
    std::string transaction_name = params[0], variable = params[1];
    if(transaction_map.find(transaction_name) == transaction_map.end()) return;
    Transaction* transaction = transaction_map[transaction_name];
    bool is_waiting = (transaction->get_status() == WAITING);
    bool is_running = (transaction->get_status() == RUNNING);
    if(!(is_waiting or is_running)) return;
    if(transaction->is_read_only()) {
        read_request_read_only(transaction, variable, transaction_name, try_waiting);
    } else {
        if(lock_table->is_locked_by_transaction(transaction, variable, LockType::WRITE)) {
            int64_t val = transaction->get_uncommitted_variables()[variable];
            std::cout << transaction->get_name() << " got read lock on " << variable << " having value " << val << std::endl;
            transaction->get_read_variables()[variable].push_back(val);
            return;
        }
        if(lock_table->is_locked_by_transaction(transaction, variable, LockType::READ)) {
            lock_table->set_lock(transaction, LockType::READ, variable);
            std::cout << transaction->get_name() << " already has a read lock on " << variable << std::endl;
            transaction->set_status(TransactionStatus::RUNNING);
            return;
        }
        for(auto& [time, blocked_info_dict] : blocked_transactions) {
            for(auto& [key, blocked_transaction_info] : blocked_info_dict) {
                if(blocked_transaction_info.len == 4 and blocked_transaction_info.variable == variable) {
                    for(auto* lock : lock_table->get_lock_map()[variable]) {
                        const std::string& blocking_transaction = lock->get_transaction()->get_name();
                        if((*(lock->get_transaction())) == (*transaction)) continue;
                        TransactionInfo blocking_transaction_info;
                        blocking_transaction_info.transaction = blocking_transaction;
                        blocking_transaction_info.instruction_type = InstructionType::READ_INSTR;
                        blocking_transaction_info.variable = variable;
                        blocking_transaction_info.len = 3;

                        transaction->set_status(TransactionStatus::BLOCKED);
                        blocked_transactions[current_time][transaction_name] = std::move(blocking_transaction_info);
                        std::cout << transaction->get_name() << " will not get a read lock on " << variable << " because " << key << " is already waiting for a write lock" << std::endl;
                        return; 
                    }
                }
            }
        }
        LockAcquireStatus lock_acquire_status = site_manager->get_locks(transaction, LockType::READ, variable);
        if(lock_acquire_status == LockAcquireStatus::GOT_LOCK or lock_acquire_status == LockAcquireStatus::GOT_LOCK_RECOVERING) {
            if(lock_acquire_status == LockAcquireStatus::GOT_LOCK) {
                std::cout << transaction->get_name() << " got read lock on " << variable << " having value " << site_manager->get_current_variables(variable)[variable] << std::endl;
            } else {
                std::cout << "Though the site holding " << variable << " is recovering, " << transaction->get_name() << " got read lock on " << variable << " having value " << site_manager->get_current_variables(variable)[variable] << " since its the only copy" << std::endl;
            }
            int64_t curr_variable = site_manager->get_current_variables(variable)[variable];
            transaction->get_read_variables()[variable].push_back(curr_variable);
            lock_table->set_lock(transaction, LockType::READ, variable);
            std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_waiting_transactions = get_squashed_waiting_transactions();
            if(squashed_waiting_transactions.find(transaction->get_name()) != squashed_waiting_transactions.end()) {
                for(auto& wt_transaction_info : squashed_waiting_transactions[transaction->get_name()]) {
                    if(wt_transaction_info.variable == variable) return;    
                }
            }
            transaction->set_status(TransactionStatus::RUNNING);
        } else if(lock_acquire_status == LockAcquireStatus::ALL_SITES_DOWN) {
            TransactionInfo waiting_transaction_info;
            waiting_transaction_info.instruction_type = InstructionType::READ_INSTR;
            waiting_transaction_info.variable = variable;
            waiting_transaction_info.len = 2;

            std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_waiting_transactions = get_squashed_waiting_transactions();
            if(squashed_waiting_transactions.find(transaction->get_name()) != squashed_waiting_transactions.end()) {
                for(auto& wt_transaction_info : squashed_waiting_transactions[transaction->get_name()]) {
                    if(wt_transaction_info.len == waiting_transaction_info.len and wt_transaction_info.instruction_type == waiting_transaction_info.instruction_type and wt_transaction_info.variable == waiting_transaction_info.variable) return;
                }
            }
            std::cout << transaction->get_name() << " is waiting on " << variable << std::endl;
            transaction->set_status(TransactionStatus::WAITING);
            waiting_transactions[current_time][transaction->get_name()] = std::move(waiting_transaction_info);
        } else {
            LockTable lock_table_ = site_manager->get_set_locks();
            for(Lock* lock : lock_table_.get_lock_map()[variable]) {
                const std::string& blocking_transaction = lock->get_transaction()->get_name();
                if((*(lock->get_transaction())) == (*transaction)) continue;
                TransactionInfo blocking_transaction_info;
                blocking_transaction_info.transaction = blocking_transaction;
                blocking_transaction_info.instruction_type = InstructionType::READ_INSTR;
                blocking_transaction_info.variable = variable;
                blocking_transaction_info.len = 3;

                std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_blocking_transactions = get_squashed_blocked_transactions();
                if(squashed_blocking_transactions.find(transaction->get_name()) != squashed_blocking_transactions.end()) {
                    for(auto& bk_transaction_info : squashed_blocking_transactions[transaction->get_name()]) {
                        if(bk_transaction_info.len == blocking_transaction_info.len and bk_transaction_info.transaction == blocking_transaction_info.transaction and bk_transaction_info.instruction_type == blocking_transaction_info.instruction_type and bk_transaction_info.variable == blocking_transaction_info.variable) return;
                    }
                }
                std::cout << transaction->get_name() << " is blocked by " << blocking_transaction << " on " << variable << std::endl;
                transaction->set_status(TransactionStatus::BLOCKED);
                blocked_transactions[current_time][transaction_name] = std::move(blocking_transaction_info);
                current_time += 1;
            }
        }
    }
}

void TransactionManager::try_waiting() {
    std::vector<std::pair<int64_t, std::string>> to_pop;
    for(auto& [time, waiting_dicts] : waiting_transactions) {
        for(auto& [transaction, params] : waiting_dicts) {
            Transaction* transaction_obj = transaction_map[transaction];
            transaction_obj->set_status(TransactionStatus::WAITING);
            if(params.instruction_type == InstructionType::WRITE_INSTR) {
                std::vector<std::string> args{transaction, params.variable, std::to_string(params.value)};
                write_request(args);
            } else if(params.instruction_type == InstructionType::READ_INSTR) {
                std::vector<std::string> args{transaction, params.variable};
                read_request(args);
            } else if(params.instruction_type == InstructionType::READ_ONLY_INSTR) {
                std::vector<std::string> args{transaction, params.variable};
                read_request(args, true);
            }
            if(transaction_map[transaction]->get_status() == TransactionStatus::RUNNING) {
                to_pop.push_back({time, transaction});
            }
        }
    }
    for(const auto& [key1, key2] : to_pop) {
        waiting_transactions[key1].erase(key2);
    }
}

void TransactionManager::commit_transaction(const std::string& name) {
    TransactionStatus status = transaction_map[name]->get_status();
    if(status == TransactionStatus::COMMITTED or status == TransactionStatus::ABORTED) return;
    Transaction* transaction = transaction_map[name];
    std::unordered_map<std::string, std::vector<int64_t>>& read_variables = transaction->get_read_variables();
    for(const auto& [variable, values] : read_variables) {
        for(const auto& value : values) {
            std::cout << name << " read the value " << value << " of variable " << variable << std::endl; 
        }
    }
    std::unordered_map<std::string, int64_t>& uncommited_variables = transaction->get_uncommitted_variables();
    for(auto& [variable, value] : uncommited_variables) {
        for(int32_t i=1; i<= number_of_sites; i++) {
            int32_t var = std::stoi(variable.substr(1, variable.size()-1));
            if(var%2 == 0 or (1+(var%10)) == i) {
                Site* site = site_manager->get_site(i);
                site->write_variable(transaction, variable, value);
            }
        }
    }
    transaction_map[name]->set_status(TransactionStatus::COMMITTED);
}

void TransactionManager::end(const std::vector<std::string>& params) {
    TransactionStatus status = transaction_map[params[0]]->get_status();
    if(status == TransactionStatus::COMMITTED or status == TransactionStatus::ABORTED) return;
    commit_transaction(params[0]);
    std::cout << params[0] << " committed" << std::endl;
    clear_locks(transaction_map[params[0]]);
    std::unordered_map<std::string, std::vector<TransactionInfo>> squashed_blocked_transactions = get_squashed_blocked_transactions();
    if(squashed_blocked_transactions.find(params[0]) != squashed_blocked_transactions.end()) {
        squashed_blocked_transactions.erase(params[0]);
        for(auto& [blocked_dict_key, blocked_dict_val] : blocked_transactions) {
            blocked_dict_val.erase(params[0]);
        }
    }
    for(auto& [time, waiting_dict_val] : waiting_transactions) {
        waiting_dict_val.erase(params[0]);
    }
    detect_and_clear_deadlocks();
    blocked_to_waiting();
    try_waiting();
}

void TransactionManager::tick(Instruction& instruction) {
    current_time++;
    clear_aborted();
    detect_and_clear_deadlocks();
    blocked_to_waiting();
    try_waiting();
    std::vector<std::string> &params = instruction.get_params();
    if(instruction.get_instruction_type() == BEGIN_FUNC) begin(params);
    else if(instruction.get_instruction_type() == BEGIN_READ_ONLY_FUNC) begin_read_only(params);
    else if(instruction.get_instruction_type() == READ_FUNC) read_request(params);
    else if(instruction.get_instruction_type() == WRITE_FUNC) write_request(params);
    else if(instruction.get_instruction_type() == END_FUNC) end(params);
    else std::cout << "Instruction can't be recognized" << std::endl;
}