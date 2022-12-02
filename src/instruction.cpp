#include <iostream>
#include <regex>
#include <csignal>
#include "instruction.h"
#include "util.h"

Instruction::Instruction(const std::string& instruction) {
    std::regex rex("(.+)\\((.*?)\\)");
    std::smatch match;
    if(std::regex_search(instruction, match, rex) and match.size() == 3) {
        instruction_type = Util::trim(match.str(1));
        params = Util::tokenize(std::move(match.str(2)));
    } else {
        std::cout << "Invalid instruction syntax: " << instruction << std::endl;
        exit(1);
    }
}

std::string& Instruction::get_instruction_type() {
    return instruction_type;
}

std::vector<std::string>& Instruction::get_params() {
    return params;
}

