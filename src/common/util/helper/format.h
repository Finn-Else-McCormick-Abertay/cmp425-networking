#pragma once

#include <fmt/format.h>

#define DEF_SIMPLE_FORMATTER(Type, VarName, ...) \
template<> struct fmt::formatter<Type>: formatter<string_view> { \
    inline auto format(const Type& VarName, format_context& ctx) const { \
        return formatter<std::string>().format(fmt::format(__VA_ARGS__), ctx); \
    } \
}