/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/30/2022
 */

#ifndef LOCKTABLE_1234_H
#define LOCKTABLE_1234_H

#include <map>
#include <string>
#include <vector>
#include "variable.h"
#include "lock.h"

/**
 * The LockTable holds information about locks present on variables. 
 * Its main attribute is lock_map that contains variables, keys, and values. 
 * It assists in setting, testing, and clearing locks.
 */
class LockTable {
private:
    std::map<std::string, std::vector<Lock*>> lock_map;

public:
    /**
     * LockTable 
     * Constructor
     */
    LockTable();
    
    /**
     * gets the reference to the lock map
     * @return {std::map<std::string, std::vector<Lock*>>&}  : reference to the lock map 
     */
    std::map<std::string, std::vector<Lock*>>& get_lock_map();
    
    /**
     * returns number of locks on a variable
     * @param  {std::string} variable_ : variable that needs to be checked
     * @return {size_t}                : number of locks on this variable
     */
    size_t get_len_locks(const std::string& variable_);
    
    /**
     * tries to set particular type of lock on variable for a transaction
     * @param  {Transaction*} transaction_ : transaction that needs the lock
     * @param  {LockType} lock_type_       : type of the lock
     * @param  {std::string} variable_     : variable on which lock is needed
     */
    void set_lock(Transaction* transaction_, const LockType lock_type_, const std::string& variable_);
    
    /**
     * checks if a lock is set on a variable
     * @param  {std::string} variable_ : variable for which lock needs to be checked
     * @return {bool}                  : true if the lock is set, false otherwise
     */
    bool is_locked(const std::string& variable_);
    
    /**
     * checks if a write lock is set on a variable
     * @param  {std::string} variable_ : variable for which lock needs to be checked
     * @return {bool}                  : true if the lock is set, false otherwise
     */
    bool is_write_locked(const std::string& variable_);
    
    /**
     * checks if a read lock is set on a variable
     * @param  {std::string} variable_ : variable for which lock needs to be checked
     * @return {bool}                  : true if the lock is set, false otherwise
     */
    bool is_read_locked(const std::string& variable_);
    
    /**
     * frees all lock s on a variable
     * @param  {std::string} variable_ : variable for which locks need to be freed
     */
    void free(const std::string& variable_);
    
    /**
     * tries to clear a lock on a variable
     * @param  {Lock*} lock_           : lock that needs to be cleared 
     * @param  {std::string} variable_ : variable for which lock needs to be cleared
     * @return {bool}                  : true if the lock was cleared, false otherwise
     */
    bool clear_lock(Lock* lock_, const std::string& variable_);
    
    /**
     * sets the lock map of this locktable
     * @param  {std::map<std::string, std::vector<Lock*>>} lock_map_ : locks map that needs to be set 
     */
    void set_lock_map(std::map<std::string, std::vector<Lock*>> lock_map_);
    
    /**
     * cheks if a variable is locked by a transaction
     * @param  {Transaction*} current_transaction_ : transaction that needs to be checked
     * @param  {std::string} variable_             : variable that needs to be checked for the lock
     * @param  {LockType} lock_type_=NOLOCK        : optional lock type of the locks that needs checking
     * @return {bool}                              : true if success, false otherwise
     */
    bool is_locked_by_transaction(Transaction* current_transaction_, const std::string& variable_, const LockType lock_type_=NOLOCK);
};

#endif