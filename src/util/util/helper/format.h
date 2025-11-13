#pragma once

#include <alias/str.h>
#include <fmt/format.h>

#define __INTERNAL_UNWRAP_DIRECT_FROM_BRACES(...) __VA_ARGS__

#define DEF_TEMPLATED_SIMPLE_FORMATTER(BracedType, VarName, ...) \
template<__INTERNAL_UNWRAP_DIRECT_FROM_BRACES BracedType> \
struct fmt::formatter<__INTERNAL_UNWRAP_DIRECT_FROM_BRACES BracedType>: formatter<string_view> { \
    inline auto format(const __INTERNAL_UNWRAP_DIRECT_FROM_BRACES BracedType& VarName, format_context& ctx) const { \
        return formatter<str>().format(fmt::format(__VA_ARGS__), ctx); \
    } \
}

#define DEF_SIMPLE_FORMATTER(Type, VarName, ...) \
template<> struct fmt::formatter<Type>: formatter<string_view> { \
    inline auto format(const Type& VarName, format_context& ctx) const { \
        return formatter<str>().format(fmt::format(__VA_ARGS__), ctx); \
    } \
}