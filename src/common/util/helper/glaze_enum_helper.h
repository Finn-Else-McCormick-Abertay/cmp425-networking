#pragma once
#include <glaze/glaze.hpp>
#include <util/helper/enum_serialization.h>

#define ENUM_GLAZE(EnumType, Values) \
template <> struct glz::meta<EnumType> { \
    using enum EnumType; \
    static constexpr auto value = enumerate Values; \
}


#define ENUM_GLAZE_AND_FORMATTER(EnumType, Values, ...) ENUM_GLAZE(EnumType, Values); ENUM_FORMATTER(EnumType, Values __VA_OPT__(,)__VA_ARGS__)