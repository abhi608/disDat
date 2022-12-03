/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/30/2022
 */

#ifndef IO_1234_H
#define IO_1234_H

#include <fstream>
#include "instruction.h"
#include "variable.h"
#include "constants.h"
#include "sitemanager.h"
#include "transactionmanager.h"
#include "locktable.h"

/**
 * Class to read instructions from stdin and forward them to SiteManager and TransactionManager.
 */
class IO{
private:
    std::string file_name;
    std::ifstream file;
    SiteManager* site_manager;
    TransactionManager* transaction_manager;
    LockTable* lock_table;

public:
    /**
     * IO 
     * Constructor
     * @param  {std::string} file_name_                   : input file
     * @param  {SiteManager*} site_manager_               : instance of the site manager
     * @param  {TransactionManager*} transaction_manager_ : instance of the transaction manager
     * @param  {LockTable*} lock_table_                   : instance of the lock table
     */
    IO(const std::string& file_name_, SiteManager* site_manager_, TransactionManager* transaction_manager_, LockTable* lock_table_);
    
    /**
     * get the input instructions on the same line separated by a ';'
     * @return {std::optional<std::vector<Instruction>>}  : instructions wrapped in the Instruction class object
     */
    std::optional<std::vector<Instruction>> get_next_instruction();
    
    /**
     * reads the next line in the input file
     * @return {std::optional<std::string>}  : a line in the input file
     */
    std::optional<std::string> get_next_line();
    
    /**
     * breaks an input line to vector of Instruction objects
     * @param  {std::string} line          : a line in the input file
     * @return {std::vector<Instruction>}  : instructions wrapped in Instruction object
     */
    std::vector<Instruction> process_instruction(std::string& line);
    
    /**
     * calls site manager and transaction manager depending on the type of instruction
     */
    void run();
};

#endif