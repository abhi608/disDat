#include <iostream>
#include "variable.h"

Variable::Variable(int32_t index_, const std::string& name_, int64_t value_, int32_t current_side_id_): index(index_), name(name_), value(value_), current_site_id(current_side_id_), lock_type(NOLOCK) {}

int32_t Variable::get_current_site() const {
    return current_site_id;
}

int64_t Variable::get_value() const {
    return value;
}

void Variable::set_value(int64_t value_) {
    value = value_;
}

LockType Variable::is_locked() {
    return lock_type;
}

void Variable::set_lock_type(LockType lock_type_) {
    lock_type = lock_type_;
}