/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/30/2022
 */

#ifndef LOCK_1234_H
#define LOCK_1234_H

#include "variable.h"
#include "transaction.h"

/**
 * This class represent a lock placed by a transaction on a variable
 */
class Lock {
private:
    LockType lock_type;
    Transaction* transaction;

public:
    /**
     * Lock 
     * Constructor
     * @param  {LockType} lock_type_       : LockType of the lock
     * @param  {Transaction*} transaction_ : transactionn that holds the lock
     */
    Lock(const LockType lock_type_, Transaction* transaction_);
    
    /**
     * get the transaction for this lock
     * @return {Transaction*}  : transaction for this lock
     */
    Transaction* get_transaction();
    
    /**
     * get the type of this lock
     * @return {LockType}  : type of lock
     */
    LockType get_lock_type() const;
    
    /**
     * sets the lock type
     * @param  {LockType} lock_type_ : desired lock type of the lock 
     */
    void set_lock_type(const LockType lock_type_);
    
    /**
     * sets the transaction of this lock
     * @param  {Transaction*} transaction_ : transaction that needs this lock
     */
    void set_transaction(Transaction* transaction_);
    
    /**
     * to compare two transaction objects
     * @param  {Lock} lock_ : lock object that needs to be compared with this object
     * @return {bool}       : true if the two locks are same, false otherwise
     */
    bool operator==(const Lock& lock_) const;
};

#endif