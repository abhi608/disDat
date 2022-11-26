#ifndef LOCK_1234_H
#define LOCK_1234_H

#include "variable.h"
#include "transaction.h"


class Lock {
private:
    LockType lock_type;
    Transaction transaction;

public:
    Lock(const LockType lock_type_, const Transaction& transaction_);
    Transaction& get_transaction();
    LockType get_lock_type() const;
    void set_lock_type(const LockType lock_type_);
    void set_transaction(const Transaction& transaction_);
    bool operator==(const Lock& lock_) const;
};

#endif