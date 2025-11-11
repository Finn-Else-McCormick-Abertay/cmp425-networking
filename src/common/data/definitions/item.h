#pragma once

#include <util/std_aliases.h>
#include <util/glaze_prelude.h>
#include <util/helper/format.h>
#include <fmt/std.h>
#include <unordered_map>

namespace data { namespace definition {
    struct Item {
        opt<str> texture;
    };
    static_assert(glz::reflectable<Item>);
}}