#pragma once

#include <format>
#include <iostream>
#include <util/console/ansi.h>

namespace console {

    #define __DEF_CONSOLE_OUTPUT_FUNC(name, prefix) \
    template<class... Args> void name(std::format_string<Args...> fmt, Args&&... args) { \
        std::cout << prefix << std::vformat(fmt.get(), std::make_format_args(args...)) << '\n' << ANSI::Reset; \
    }
    #define __DEF_CONSOLE_OUTPUT_FUNC_EMPTY(name) \
    template<class... Args> void name(std::format_string<Args...> fmt, Args&&... args) {}

    __DEF_CONSOLE_OUTPUT_FUNC(info,  ANSI::Colour::FG::White    << "[INFO] ")
    __DEF_CONSOLE_OUTPUT_FUNC(warn,  ANSI::Colour::FG::Yellow   << "[WARN] ")
    __DEF_CONSOLE_OUTPUT_FUNC(error, ANSI::Colour::FG::Red      << "[ERROR] ")

    #ifdef DEBUG
    __DEF_CONSOLE_OUTPUT_FUNC(debug, ANSI::Colour::FG::Purple   << "[DEBUG] ")
    #endif
    #ifndef DEBUG
    __DEF_CONSOLE_OUTPUT_FUNC_EMPTY(debug)
    #endif
}