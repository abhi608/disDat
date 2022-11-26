#include <iostream>
#include "lock.h"

Lock::Lock(LockType lock_type_, Transaction& transaction_): lock_type(lock_type_), transaction(transaction_) {}

Transaction& Lock::get_transaction() {
    return transaction;
}

LockType Lock::get_lock_type() const {
    return lock_type;
}

void Lock::set_lock_type(const LockType lock_type_) {
    lock_type = lock_type_;
}

void Lock::set_transaction(const Transaction& transaction_) {
    transaction = transaction_;
}
