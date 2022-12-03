/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 12/01/2022
 */

#ifndef SITE_1234_H
#define SITE_1234_H

#include <unordered_set>
#include <string>
#include <vector>
#include "datamanager.h"
#include "transaction.h"
#include "variable.h"

enum SiteStatus {UP, DOWN, RECOVERING};

/**
 * Site illustrates a specific site. 
 * Users will have to navigate through SiteManager and then Site in order 
 * to access any variables that are present on this site. 
 * It contains a data manager that contains lock tables and variables.
 */
class Site {
private:
    int32_t id;
    SiteStatus status;
    DataManager data_manager;
    std::unordered_set<std::string> recovered_variables;

public:
    /**
     * Site 
     * Constructor
     * @param  {int32_t} id_ : index of the current site
     */
    Site(const int32_t id_);
    
    /**
     * change the status of this site
     * @param  {SiteStatus} status_ : desired status 
     */
    void set_status(SiteStatus status_);
    
    /**
     * get the current status of this site
     * @return {SiteStatus}  : current status of this site
     */
    SiteStatus get_status() const;
    
    /**
     * get the index of this site
     * @return {int32_t}  : index
     */
    int32_t get_id() const;
    
    /**
     * Tries to get a lock on a variable by a transaction
     * @param  {Transaction*} transaction_ : transaction that wants the lock
     * @param  {LockType} lock_type_       : type of the lock
     * @param  {std::string} variable_     : name of the variable that needs the lock
     * @return {bool}                      : true if lock was acquired, false otherwise
     */
    bool get_lock(Transaction* transaction_, const LockType lock_type_, const std::string& variable_);
    
    /**
     * clear a lock on a variable
     * @param  {Lock*} lock_           : lock that needs to be cleared
     * @param  {std::string} variable_ : variable on which lock is to be cleared
     */
    void clear_lock(Lock* lock_, const std::string& variable_);
    
    /**
     * writes value toa  variable for a transaction
     * @param  {Transaction*} transaction_  : transaction that wants to erite a value
     * @param  {std::string} variable_name_ : variable to which the value needs to be written
     * @param  {int64_t} value_             : value that needs to be written
     */
    void write_variable(Transaction* transaction_, const std::string& variable_name_, int64_t value_);
    
    /**
     * starts this site
     */
    void listen();
    
    /**
     * fails this site
     */
    void fail();

    /**
     * recover this site
     */
    void recover();
    
    /**
     * dump the variable values on this site
     */
    void dump_site();
    
    /**
     * gets a list of all the variables present in the data manager of this site
     * @return {std::vector<Variable*>}  : list of variables on this site
     */
    std::vector<Variable*> get_all_variables();
    
    /**
     * checks if a variable is recovered
     * @param  {std::string} variable_="" : variable that needs to be checked
     * @return {bool}                     : true if variable is recovered, otherwise false
     */
    bool is_in_recovered_variables(const std::string& variable_) const;
    
    /**
     * returns a reference to the data manager of this site
     * @return {DataManager}  : reference to the data manager of this site
     */
    DataManager& get_data_manager();
};

#endif