#pragma once

#include <alias/str.h>
#include <alias/opt.h>
#include <fmt/std.h>

namespace fmt {
    template<typename T> str opt(const ::opt<T>& opt) { return opt ? fmt::format("{}", opt.value()) : "null"; }
    template<typename T, typename O> str opt(const ::opt<T>& opt, const O& fallback) { return opt ? fmt::format("{}", opt.value()) : fmt::format("{}", fallback); }
}