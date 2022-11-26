#ifndef CONSTANTS_1234_H
#define CONSTANTS_1234_H

#include <string_view>
#include <array>

constexpr std::string_view BEGIN_FUNC{ "begin" };
constexpr std::string_view BEGIN_READ_ONLY_FUNC{ "beginRO" };
constexpr std::string_view READ_FUNC{ "R" };
constexpr std::string_view WRITE_FUNC{ "W" };
constexpr std::string_view DUMP_FUNC{ "dump" };
constexpr std::string_view END_FUNC{ "end" };
constexpr std::string_view FAIL_FUNC{ "fail" };
constexpr std::string_view RECOVER_FUNC{ "recover" };
constexpr std::array<std::string_view, 3> SITE_MANAGER_FUNCS{{DUMP_FUNC, FAIL_FUNC, RECOVER_FUNC}};

#endif