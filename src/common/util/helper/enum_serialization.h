#pragma once

#include <fmt/format.h>
#include <util/macro/wrap.h>
#include <util/macro/remove_parentheses.h>

#define __INTERNAL_ENUM_CASE_FORMAT(ValueName) case ValueName: value_name = #ValueName; break;

#define __INTERNAL_ENUM_FINAL_FORMATNO_TYPE_NAME(EnumName, ValueName) ValueName
#define __INTERNAL_ENUM_FINAL_FORMAT(EnumName, ValueName) fmt::format("{}::{}", EnumName, ValueName)

#define ENUM_FORMATTER(EnumName, Values, ...) \
template <> struct fmt::formatter<EnumName>: formatter<string_view> { \
    inline auto format(EnumName v, format_context& ctx) const { \
        using enum EnumName; \
        std::string value_name; \
        switch (v) { \
            WRAP_CALL(__INTERNAL_ENUM_CASE_FORMAT, DEPAREN(Values)) \
            default: value_name = fmt::format("unknown({})", fmt::underlying(v)); \
        } \
        bool use_type_name = true; \
        WRAP(,;,__VA_ARGS__) \
        if (use_type_name) return formatter<std::string>().format(fmt::format("{}::{}", #EnumName, value_name), ctx); \
        else return formatter<std::string>().format(value_name, ctx); \
    } \
}

#define DECL_SERIALIZED_ENUM_CLASS(EnumName, Values) enum class EnumName { DEPAREN(Values) }; ENUM_FORMATTER(EnumName, Values)