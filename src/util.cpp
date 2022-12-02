#include <iostream>
#include "util.h"

std::string Util::trim(const std::string& str, const std::string& whitespace/*=" \t"*/) {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    return std::move(str.substr(strBegin, strRange));
}

std::vector<std::string> Util::tokenize(std::string str, const std::string& delimiter/*=","*/) {
    size_t pos = 0;
    std::vector<std::string> params;
    std::string token;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        params.push_back(Util::trim(token));
        str.erase(0, pos + delimiter.length());
    }
    params.push_back(Util::trim(str));
    return std::move(params);
}