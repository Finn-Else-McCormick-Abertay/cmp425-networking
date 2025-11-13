#pragma once

#include <prelude/data_definition.h>

namespace data { namespace definition {
    struct Item {
        opt<str> texture;
    };
    static_assert(glz::reflectable<Item>);
}}