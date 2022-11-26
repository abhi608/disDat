#ifndef INSTRUCTION_1234_H
#define INSTRUCTION_1234_H

#include <string>
#include <vector>

class Instruction {
private:
    std::string instruction_type;
    std::vector<std::string> params;

public:
    Instruction(const std::string& instruction);
    std::string& get_instruction_type();
    std::vector<std::string>& get_params();
};

#endif