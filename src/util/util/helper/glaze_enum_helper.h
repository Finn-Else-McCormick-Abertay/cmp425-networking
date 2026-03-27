#pragma once
#include <glaze/glaze.hpp>
#include <util/helper/enum_serialization.h>

#define ENUM_GLAZE(EnumType, Values) \
template <> struct glz::meta<EnumType> { \
    using enum EnumType; \
    static constexpr auto value = enumerate Values; \
}

#define ENUM_GLAZE_AND_FORMATTER(EnumType, Values, ...) \
    ENUM_GLAZE(EnumType, Values); ENUM_FORMATTER(EnumType, Values __VA_OPT__(,)__VA_ARGS__)

#define DECL_ENUM_FORMATTED_GLAZED(Signature, EnumName, Values, ...) \
    DECL_ENUM_FORMATTED(Signature, EnumName, Values __VA_OPT__(,) __VA_ARGS__); ENUM_GLAZE(EnumName, Values)

#define DECL_NAMESPACED_ENUM_FORMATTED_GLAZED(Namespace, Signature, EnumName, Values, ...) \
    DECL_NAMESPACED_ENUM_FORMATTED(Namespace, Signature, EnumName, Values __VA_OPT__(,) __VA_ARGS__); ENUM_GLAZE(Namespace::EnumName, Values)


// A 'complex enum' is a class wrapping an enum so it can be used like one but can also have methods etc
#define GLAZE_COMPLEX_ENUM_AS_VALUE(EnumName) \
    template <> struct glz::meta<EnumName> { static constexpr auto value = [](EnumName self) -> EnumName::Value { return (EnumName::Value)self; }; }

#define FORMAT_AND_GLAZE_COMPLEX_ENUM_AS_VALUE(EnumName) \
    FORMAT_COMPLEX_ENUM_AS_VALUE(EnumName); \
    GLAZE_COMPLEX_ENUM_AS_VALUE(EnumName)
