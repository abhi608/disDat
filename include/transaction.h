/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 12/02/2022
 */

#ifndef TRANSACTION_1234_H
#define TRANSACTION_1234_H

#include <string>
#include <unordered_map>
#include <vector>

enum TransactionStatus {RUNNING, WAITING, BLOCKED, ABORTED, COMMITTED};

/**
 * Transaction represents a running process that was started from an input file. 
 * A transaction can read and write to variables, as well as have a status assigned to it.
 */
class Transaction {
private:
    TransactionStatus status;
    int32_t id;
    std::string name;
    bool read_only;
    std::unordered_map<std::string, int64_t> uncommitted_variables;
    std::unordered_map<std::string, std::vector<int64_t>> read_variables;
    std::unordered_map<std::string, int64_t> variable_values;

public:
    /**
     * Transaction 
     * Constructor
     * @param  {int32_t} id_           : id of this transaction
     * @param  {std::string} name_     : name of this transaction
     * @param  {bool} read_only_=false : is the transaction read only
     */
    Transaction(const int32_t id_, const std::string& name_, bool read_only_=false);  
    
    /**
     * get the id of this transaction
     * @return {int32_t}  : id of this transaction
     */
    int32_t get_id() const;
    
    /**
     * get the name of this transaction
     * @return {std::string}  : name of this transaction
     */
    std::string& get_name();
    
    /**
     * get the status of this transaction
     * @return {TransactionStatus}  : status of this transaction
     */
    TransactionStatus get_status() const;
    
    /**
     * check if this transaction is read only
     * @return {bool}  : if RO, returns true, otherwise false
     */
    bool is_read_only() const;
    
    /**
     * set the status of this transaction
     * @param  {TransactionStatus} status_ : desired status of this transaction
     */
    void set_status(const TransactionStatus status_);
    
    /**
     * get all read variables of this transaction
     * @return {std::unordered_map<std::string, std::vector<int64_t>>&}  : map of variables read 
     */
    std::unordered_map<std::string, std::vector<int64_t>>& get_read_variables();

    /**
     * gets all variables written by this transaction
     * @return {std::unordered_map<std::string, int64_t>&}  : map of variables written
     */
    std::unordered_map<std::string, int64_t>& get_uncommitted_variables();
    
    /**
     * clears the uncommitted variables of this transaction
     */
    void clear_uncommitted_variables();
    
    /**
     * returns a refence to variable_values
     * @return {std::unordered_map<std::string, int64_t>&}  : variable_values
     */
    std::unordered_map<std::string, int64_t>& get_variable_values();
    
    /**
     * sets the variable_values map
     * @param  {std::unordered_map<std::string, int64_t>} variable_values_ : desired variable_values
     */
    void set_variable_values(std::unordered_map<std::string, int64_t> variable_values_);
    
    /**
     * compares this transaction object with another
     * @param  {Transaction} transaction_ : transaction object to be compared with this transaction object
     * @return {bool}                     : true if the two transaction objects are same, false otherwise
     */
    bool operator==(const Transaction& transaction_) const;
};

#endif