/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/29/2022
 */

#ifndef VARIABLE_1234_H
#define VARIABLE_1234_H

#include <cstdint>
#include <string>
#include <vector>

enum LockType {READ, WRITE, NOLOCK};

/**
 * This class represents our sites' data, which is accessible by transactions.
 */
class Variable {
private:
    int32_t index;
    std::string name;
    int32_t current_site_id;
    int64_t value;
    LockType lock_type;
    
public:
    /**
     * Variable 
     * Default Constructor to enable this class to be used as value in unordered_map
     */
    Variable();
    
    /**
     * Variable 
     * Constructor
     * @param  {int32_t} index_           : index of the variable
     * @param  {std::string} name_        : name of the variable
     * @param  {int64_t} value_           : value of the variable
     * @param  {int32_t} current_side_id_ : id of the site holding this variable
     */
    Variable(const int32_t index_, const std::string& name_, const int64_t value_, const int32_t current_side_id_);
    
    /**
     * get index of this variable
     * @return {int32_t}  : id of this variable
     */
    int32_t get_current_site() const;
    
    /**
     * get value of this variable
     * @return {int64_t}  : value of the current variable
     */
    int64_t get_value() const;
    
    /**
     * set value of this variable
     * @param  {int64_t} value_ : desired value
     */
    void set_value(int64_t value_);
    
    /**
     * checks for any lock on this variable
     * @return {LockType}  : type of the lock
     */
    LockType is_locked();
    
    /**
     * sets the desired type of lock on this variable
     * @param  {LockType} lock_type_ : desired lock type
     */
    void set_lock_type(const LockType lock_type_);
    
    /**
     * returns the name of this variable
     * @return {std::string}  : name of this variable
     */
    std::string& get_name();
}; 

#endif