/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 12/02/2022
 */

#ifndef TRANSACTIONMANAGER_1234_H
#define TRANSACTIONMANAGER_1234_H

#include <map>
#include "transaction.h"
#include "lock.h"
#include "locktable.h"
#include "sitemanager.h"
#include "variable.h"
#include "constants.h"

enum InstructionType {READ_INSTR, READ_ONLY_INSTR, WRITE_INSTR};

struct TransactionInfo {
    size_t len;
    std::string transaction;
    InstructionType instruction_type;
    std::string variable;
    int64_t value;
};

/**
 * TransactionManager class is responsible for handling transactions and deadlock detection and resolution. 
 * When a transaction commits, it calls the siteManager to clear locks.
 */
class TransactionManager {
private:
    int32_t number_of_variables;
    int32_t number_of_sites;
    std::unordered_map<std::string, Transaction*> transaction_map;
    LockTable* lock_table;
    SiteManager* site_manager;
    int64_t current_time;
    std::map<std::int64_t, std::unordered_map<std::string, TransactionInfo>> blocked_transactions;
    std::map<std::int64_t, std::unordered_map<std::string, TransactionInfo>> waiting_transactions;

public:
    /**
     * TransactionManager 
     * Constructor
     * @param  {int32_t} num_vars_          : number of variables
     * @param  {int32_t} num_sites_         : number of sites
     * @param  {LockTable*} lock_table_     : global instance of lock table
     * @param  {SiteManager*} site_manager_ : global instance of site manager
     */
    TransactionManager(const int32_t num_vars_, const int32_t num_sites_, LockTable* lock_table_, SiteManager* site_manager_);
    
    /**
     * Responsible for clearing locks on transactions. 
     * It is capable of clearing locks on transactions in both the global 
     * lock table and those controlled by the site manager.
     * @param  {Transaction*} transaction : transaction for which locks need to be cleared
     */
    void clear_locks(Transaction* transaction);
    
    /**
     * Responsible for aborting transactions. 
     * Also reoves the transaction from blocked and waiting lists
     * @param  {std::string} name : name of transaction to be aborted
     */
    void abort(const std::string& name);
    
    /**
     * Clears transactions aborted due to site failures by calling the abort function explicitly.
     */
    void clear_aborted();
    
    /**
     * Squashes blocked_transactions (based on the current time as the key) 
     * into a map based on the transaction name.
     * @return {std::unordered_map<std::string, std::vector<TransactionInfo>>}  : the resulting map
     */
    std::unordered_map<std::string, std::vector<TransactionInfo>> get_squashed_blocked_transactions();
    
    /**
     * Squashes waiting_transactions (based on the current time as the key) 
     * into a map based on the transaction name.
     * @return {std::unordered_map<std::string, std::vector<TransactionInfo>>}  : the resulting map
     */
    std::unordered_map<std::string, std::vector<TransactionInfo>> get_squashed_waiting_transactions();
    
    /**
     * resolves deadlock after it has been detected by aborting the youngest transaction
     * @param  {std::vector<std::string>} transaction_list : visited transactions
     * @param  {int32_t} index                             : index of the first transaction in deadlock
     */
    void clear_deadlock(std::vector<std::string>& transaction_list, const int32_t index);
    
    /**
     * detects deadlock and calls clear_deadlock if deadlock found
     * @param  {std::string} transaction                                                      : blocked transaction
     * @param  {std::unordered_map<std::string, int32_t>&} visited                            : index of each transaction visited
     * @param  {std::vector<std::string>} current                                             : transactions with possibility of being involved in a deadlock
     * @param  {std::unordered_map<std::string, std::vector<TransactionInfo>>} blocked_dict   : stores information about blocked transaction
     */
    void detect_deadlock(const std::string& transaction, std::unordered_map<std::string, int32_t>& visited, std::vector<std::string>& current, std::unordered_map<std::string, std::vector<TransactionInfo>>& blocked_dict);
    
    /**
     * Detects and clears deadlocks by traversing through blocked transactions.
     */
    void detect_and_clear_deadlocks();
    
    /**
     * Checks if the transaction blocking the blocked transaction has been 
     * committed or aborted, and, if so, changes its status to waiting.
     */
    void blocked_to_waiting();
    
    /**
     * initializes a transaction by making a new instance of Transaction class
     * @param  {std::vector<std::string>} params : parameters of the parsed instruction along with instruction name
     */
    void begin(std::vector<std::string>& params);
    
    /**
     * initializes a read only transaction by making a new instance of Transaction class
     * @param  {std::vector<std::string>} params : parameters of the parsed instruction along with instruction name
     */
    void begin_read_only(std::vector<std::string>& params);
    
    /**
     * Write request processing procedure that gets write locks on variables to be written, 
     * and if it cannot, changes them to blocked or waiting depending on the situation. 
     * It also inserts transactions into waiting or blocked transaction dictionaries as required.
     * @param  {std::vector<std::string>} params : parameters of the parsed instruction along with instruction name
     */
    void write_request(std::vector<std::string>& params);
    
    /**
     * Responsible for processing read requests from read-only transactions. 
     * When the site holding the variable is unavailable, this module waits 
     * for the site to go online before reading the variable.
     * @param  {Transaction*} transaction     : transaction to be processed
     * @param  {std::string} variable         : variable that needs to be read
     * @param  {std::string} transaction_name : name of the current transaction
     * @param  {bool} try_waiting             : whether try to wait or not
     */
    void read_request_read_only(Transaction* transaction, const std::string& variable, const std::string& transaction_name, bool try_waiting);
    
    /**
     * method responsible for processing a read request, obtaining read locks on the variable to be written, and changing the status to blocked or waiting depending on the situation. It also inserts transactions into waiting or blocked transactions dicts.
     * @param  {std::vector<std::string>} params : 
     * @param  {bool} try_waiting=false          : 
     */
    void read_request(std::vector<std::string>& params, bool try_waiting=false);
    
    /**
     * Tries to get locks for waiting transactions. 
     * It traverses all waiting transactions and tries to get locks 
     * for them so that their status can be changed to running.
     */
    void try_waiting();
    
    /**
     * In this method, uncommitted variables are traversed and the values 
     * are written to the respective sites when an end instruction is received.
     * @param  {std::string} name : name of the transaction to be committed
     */
    void commit_transaction(const std::string& name);
    
    /**
     * This is called when end instruction is recieved. 
     * Calls commit_transaction and clears the transaction from blocked and waiting dicts. 
     * Also clears locks held by transaction.
     * @param  {std::vector<std::string>} params : transaction to be committed
     */
    void end(const std::vector<std::string>& params);
    
    /**
     * calls other procedures based on instruction type.
     * Additionally, checks for deadlock at every tick
     * @param  {Instruction} instruction : Instruction object of next instruction
     */
    void tick(Instruction& instruction);
};

#endif