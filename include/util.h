/**
 * Author: Abhishek Verma
 * Author: Shiv Sinha
 * Date: 11/30/2022
 */

#ifndef UTIL_1234_H
#define UTIL_1234_H

#include <string>
#include <vector>

/**
 * Class for utility functions
 */
class Util {
public:
    /**
     * removes whitespace characters from beginning and end of the input string
     * @param  {std::string} str        : input string that needs trimming
     * @param  {std::string} whitespace : characters to be deleted
     * @return {std::string}            : trimmed string
     */
    static std::string trim(const std::string& str, const std::string& whitespace = " \t");
    
    /**
     * splits the input string by delimiter
     * @param  {std::string} str           : input string to be split
     * @param  {std::string} delimiter     : character that causes the split
     * @return {std::vector<std::string>}  : vector of tokenized strings
     */
    static std::vector<std::string> tokenize(std::string str, const std::string& delimiter = ",");
};

#endif