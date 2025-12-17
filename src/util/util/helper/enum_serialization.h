#pragma once

#include <alias/str.h>
#include <fmt/format.h>
#include <util/macro/wrap.h>
#include <util/macro/remove_parentheses.h>

namespace helper_detail {
    struct EnumFormatOptions {
        bool use_type_name = false;
    };
}

#define __INTERNAL_ENUM_CASE_FORMAT(ValueName) case ValueName: value_name = #ValueName; break;

#define ENUM_FORMATTER(EnumName, Values, ...) \
template <> struct fmt::formatter<EnumName>: formatter<str> { \
    inline auto format(EnumName v, format_context& ctx) const { \
        using enum EnumName; \
        str value_name; \
        switch (v) { \
            WRAP_CALL(__INTERNAL_ENUM_CASE_FORMAT, DEPAREN(Values)) \
            default: value_name = fmt::format("unknown({})", fmt::underlying(v)); \
        } \
        static constexpr helper_detail::EnumFormatOptions options __VA_ARGS__; \
        if constexpr (options.use_type_name) return formatter<str>().format(fmt::format("{}::{}", #EnumName, value_name), ctx); \
        else return formatter<str>().format(value_name, ctx); \
    } \
}

#define DECL_ENUM_FORMATTED(Signature, EnumName, Values, ...) \
    Signature EnumName { DEPAREN(Values) }; \
    ENUM_FORMATTER(EnumName, Values __VA_OPT__(,) __VA_ARGS__)