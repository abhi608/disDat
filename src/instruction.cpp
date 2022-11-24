#include <iostream>
#include <regex>
#include <csignal>
#include "instruction.h"

std::string trim(const std::string& str, const std::string& whitespace = " \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return std::move(str.substr(strBegin, strRange));
}

std::vector<std::string> tokenize(std::string str, const std::string& delimiter = ",") {
    size_t pos = 0;
    std::vector<std::string> params;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        params.push_back(trim(token));
        str.erase(0, pos + delimiter.length());
    }
    params.push_back(trim(str));
    return std::move(params);
}

Instruction::Instruction(const std::string& instruction) {
    std::regex rex("(.+)\\((.*?)\\)");
    std::smatch match;
    if(std::regex_search(instruction, match, rex) and match.size() == 3) {
        instruction_type = trim(match.str(1));
        params = tokenize(std::move(match.str(2)));
    } else {
        std::cout << "Invalid instruction syntax: " << instruction << std::endl;
        std::raise(SIGTERM);
    }
}

std::string& Instruction::get_instruction_type() {
    return instruction_type;
}

std::vector<std::string>& Instruction::get_params() {
    return params;
}

