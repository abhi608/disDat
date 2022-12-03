/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 12/01/2022
 */

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

/**
 * SiteManager manages sites and their interactions, such as failure and recovery, 
 * within the whole system. For any queries related to the site, including access 
 * to the lock table through the data manager, it is the central contact for the 
 * transaction manager. SiteManager provides all kinds of access to different 
 * sites and acts as proxy for them.
 */
class SiteManager {
private:
    std::size_t num_sites;
    std::size_t num_variables;
    std::vector<Site*> sites;
    struct UtilFuncs;

public:
    /**
     * SiteManager 
     * Constructor
     * @param  {std::size_t} num_sites_     : number of sites
     * @param  {std::size_t} num_variables_ : number of variables
     */
    SiteManager(const std::size_t num_sites_, const std::size_t num_variables_);
    
    /**
     * get the sites on which a variable copies are present
     * @param  {int32_t} id_           : index of the variable
     * @return {std::vector<int32_t>}  : list of sites
     */
    std::vector<int32_t> get_sites(const int32_t id_);
    
    /**
     * get the sites on which a variable is present
     * @param  {std::string} variable_ : name of the variable
     * @return {std::vector<int32_t>}  : list of sites
     */
    std::vector<int32_t> get_sites(const std::string& variable_);
    
    /**
     * gets a site on an index
     * @param  {int32_t} index_ : index of the site
     * @return {Site*}          : site instance
     */
    Site* get_site(const int32_t index_);
    
    /**
     * fails a site
     * @param  {int32_t} index_ : index of site to be failed
     */
    void fail(const int32_t index_);
    
    /**
     * recovers a site
     * @param  {int32_t} index_ : index of the site to be recovered
     */
    void recover(const int32_t index_);
    
    /**
     * If the Instruction has something to do with sites, such as fail, 
     * recover, or dump, IO calls the tick of Site Manager.
     * @param  {Instruction} instruction_ : Instruction object for next instruction
     */
    void tick(Instruction& instruction_);
    
    /**
     * Locks a variable for a transaction. Various checks are 
     * performed to ensure the lock is legitimate.
     * @param  {Transaction*} transaction_ : transaction that wants the lock
     * @param  {LockType} lock_type_       : type of the lock
     * @param  {std::string} variable_     : variable on which lock is required
     * @return {LockAcquireStatus}         : status of the lock acquired
     */
    LockAcquireStatus get_locks(Transaction* transaction_, const LockType lock_type_, const std::string& variable_);
    
    /**
     * returns currently set variables and their values
     * @param  {std::string} var_                          : variable name
     * @return {std::unordered_map<std::string, int64_t>}  : map of variables to their set values
     */
    std::unordered_map<std::string, int64_t> get_current_variables(const std::string& var_);
    
    /**
     * returns all the set locks in evry site's data manager
     * @return {LockTable}  : 
     */
    LockTable get_set_locks();
    
    /**
     *  clears a lock for a variable
     * @param  {Lock*} lock_                : lock to be cleared
     * @param  {std::string} variable_name_ : variable for which lock is to be cleared
     */
    void clear_locks(Lock* lock_, const std::string& variable_name_);
    
    /**
     * starts all the sites
     */
    void start();
};

#endif