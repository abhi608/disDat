/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/29/2022
 */

#ifndef DATAMANAGER_1234_H
#define DATAMANAGER_1234_H

#include <unordered_map>
#include <string>
#include "variable.h"
#include "locktable.h"

/**
 * This program is local to each site and manages all variables and locks for the site 
 */
class DataManager {
private:
    int32_t site_id;
    LockTable lock_table;
    std::unordered_map<std::string, Variable*> variable_map;

public:
    /**
     * DataManager 
     * Constructor
     * @param  {int32_t} site_id_ : id of the site to which this data manager belongs
     */
    DataManager(const int32_t site_id_);

    /**
     * Inserts variable into variable map
     * @param  {std::string} name_   : name of the variable
     * @param  {Variable*} variable_ : instance of the variable
     */
    void add_variables(const std::string& name_, Variable* variable_);

    /**
     * returns variable instance looking up the name of the variable in variable map  
     * @param  {std::string} name_         : name of the variable
     * @return {std::optional<Variable>*}  : variable instance if found, otherwise empty
     */
    std::optional<Variable*> get_variable(const std::string& name_);
    
    /**
     * checks for the presense of a variable in variable map
     * @param  {std::string} name_ : name of the variable
     * @return {bool}              : true if found, otherwise false
     */
    bool has_variable(const std::string& name_);
    
    /**
     * clear lock on the variable
     * @param  {Lock*} lock_           : lock to be cleared 
     * @param  {std::string} variable_ : variable name on which lock needs to be cleared
     */
    void clear_lock(Lock* lock_, const std::string& variable_);
    
    /**
     * 
     * @return {LockTable}  : returns lock table of this data manager 
     */
    LockTable& get_lock_table();
    
    /**
     * Tries to get lock on a variable for a transaction
     * @param  {Transaction*} transaction_ : transaction that wants the lock
     * @param  {LockType} lock_type_       : type of lock needed
     * @param  {std::string} variable_     : variable for which lock is needed
     * @return {bool}                      : true if lock was acquired, false otherwise
     */
    bool get_lock(Transaction* transaction_, const LockType lock_type_, const std::string& variable_);
    
    /**
     * writes a value of a variable for a transaction
     * @param  {Transaction*} transaction_  : transaction that wants the lock
     * @param  {std::string} variable_name_ : variable name whose value needs to be written
     * @param  {int64_t} value_             : the value of the variable
     * @return {bool}                       : true if the write was success, otherwise false
     */
    bool write_variable(Transaction* transaction_, const std::string& variable_name_, int64_t value_);
    
    /**
     * returns the variable map
     * @return {std::unordered_map<std::string,Variable*>}  : the variable map of this data manager 
     */
    std::unordered_map<std::string, Variable*>& get_variables();
};

#endif