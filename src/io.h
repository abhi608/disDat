#ifndef IO_1234_H
#define IO_1234_H

#include "instruction.h"
#include "variable.h"
#include "constants.h"
#include "sitemanager.h"
#include "transactionmanager.h"
#include "locktable.h"

class IO{
private:
    std::string file_name;
    std::ifstream file;
    SiteManager* site_manager;
    TransactionManager* transaction_manager;
    LockTable* lock_table;

public:
    IO(const std::string& file_name_, SiteManager* site_manager_, TransactionManager* transaction_manager_, LockTable* lock_table_);
    std::optional<std::vector<Instruction>> get_next_instruction();
    std::optional<std::string> get_next_line();
    std::vector<Instruction> process_instruction(std::string& line);
    void run();
};

#endif