/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/30/2022
 */

#include <iostream>
#include "io.h"
#include "util.h"

IO::IO(const std::string& file_name_, SiteManager* site_manager_, TransactionManager* transaction_manager_, LockTable* lock_table_): file_name(file_name_), file(file_name_, std::ifstream::in), site_manager(site_manager_), transaction_manager(transaction_manager_), lock_table(lock_table_) {
    if(!file) {
        std::cout << "File " << file_name_ << " does not exist." << std::endl;
        exit(1);
    }
}

std::optional<std::vector<Instruction>> IO::get_next_instruction() {
    std::vector<Instruction> instr;
    std::optional<std::string> line = get_next_line();
    if(!line.has_value()) return {};
    return {std::move(process_instruction(line.value()))};
}

std::optional<std::string> IO::get_next_line() {
    std::string line;
    if(std::getline(file, line)) {
        return {std::move(line)};
    }
    return {};
}

std::vector<Instruction> IO::process_instruction(std::string& line) {
    std::vector<Instruction> instructions;
    line = Util::trim(line);
    if(line == "") return instructions;
    std::vector<std::string> processed_line = Util::tokenize(line, ";");
    for(auto& instruction : processed_line) {
        if(instruction.find("//") == 0) continue;
        instructions.emplace_back(Instruction(instruction));
    }
    return instructions;
}

void IO::run() {
    std::optional<std::vector<Instruction>> instructions = get_next_instruction();
    while(instructions.has_value()) {
        for(auto& instruction : instructions.value()) {
            bool found = false;
            for(auto& site_manager_func : SITE_MANAGER_FUNCS) {
                if(instruction.get_instruction_type() == site_manager_func) {
                    site_manager->tick(instruction);
                    found = true;
                    break;
                } 
            }
            if(!found) {
                transaction_manager->tick(instruction);
            }
        }
        instructions = get_next_instruction();
    }
}