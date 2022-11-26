#ifndef VARIABLE_1234_H
#define VARIABLE_1234_H

#include <cstdint>
#include <string>
#include <vector>

enum LockType {READ, WRITE, NOLOCK};

class Variable {
private:
    int32_t index;
    std::string name;
    int32_t current_site_id;
    int64_t value;
    LockType lock_type;
    
public:
    Variable(const int32_t index_, const std::string& name_, const int64_t value_, const int32_t current_side_id_);
    int32_t get_current_site() const;
    int64_t get_value() const;
    void set_value(int64_t value_);
    LockType is_locked();
    void set_lock_type(const LockType lock_type_);
}; 

#endif