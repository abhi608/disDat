/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/29/2022
 */

#ifndef INSTRUCTION_1234_H
#define INSTRUCTION_1234_H

#include <string>
#include <vector>

/**
 * This class is a wrapper class for the input instructions
 */
class Instruction {
private:
    std::string instruction_type;
    std::vector<std::string> params;

public:
    /**
     * Instruction 
     * Constructor
     * @param  {std::string} instruction : raw instruction string to be processed 
     */
    Instruction(const std::string& instruction);
    
    /**
     * get instruction type (begin, fail, write, ...)
     * @return {std::string}  : string representing the instruction type
     */
    std::string& get_instruction_type();
    
    /**
     * get parameter of the instruction 
     * @return {std::vector<std::string>}  : all the parameters in form of a string vector
     */
    std::vector<std::string>& get_params();
};

#endif