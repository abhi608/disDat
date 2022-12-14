/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/30/2022
 */

#include <iostream>
#include "lock.h"

Lock::Lock(const LockType lock_type_, Transaction* transaction_): lock_type(lock_type_), transaction(transaction_) {}

Transaction* Lock::get_transaction() {
    return transaction;
}

LockType Lock::get_lock_type() const {
    return lock_type;
}

void Lock::set_lock_type(const LockType lock_type_) {
    lock_type = lock_type_;
}

void Lock::set_transaction(Transaction* transaction_) {
    transaction = transaction_;
}

bool Lock::operator==(const Lock& lock_) const {
    if(lock_type == lock_.lock_type and (*transaction) == *(lock_.transaction)) {
        return true;
    }
    return false;
}