#ifndef LOCK_H
#define LOCK_H

#include "variable.h"
#include "transaction.h"


class Lock {
private:
    LockType lock_type;
    Transaction transaction;
public:
    Lock(LockType lock_type_, Transaction& transaction_);
    Transaction& get_transaction();
    LockType get_lock_type() const;
    void set_lock_type(const LockType lock_type_);
    void set_transaction(const Transaction& transaction_);
};

#endif