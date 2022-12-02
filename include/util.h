#ifndef UTIL_1234_H
#define UTIL_1234_H

#include <string>
#include <vector>


class Util {
public:
    static std::string trim(const std::string& str, const std::string& whitespace = " \t");
    static std::vector<std::string> tokenize(std::string str, const std::string& delimiter = ",");
};

#endif