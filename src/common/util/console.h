#pragma once

#include <format>
#include <iostream>
#include <string>

namespace console {
    namespace ANSI {
        class Effect {
        public:
            Effect(std::string code, std::string opposite);
            Effect reverse() const; Effect operator !() const;

            std::string to_string() const;
            std::string code() const;
        private:
            const std::string _code, _opposite;
        };

        std::ostream& operator<<(std::ostream& os, const Effect& effect);
        
        #define ANSI_EFFECT(name, value, opp) static const Effect name = Effect(#value, #opp);
        #define ANSI_COLOUR(name, r, g, b) namespace Colour { \
            namespace FG { ANSI_EFFECT(name, 38;2;r;g;b, 39) } \
            namespace BG { ANSI_EFFECT(name, 48;2;r;g;b, 49) } \
        }

        ANSI_EFFECT(Reset, 0, 0)
        ANSI_EFFECT(Bold, 1, 0)
        ANSI_EFFECT(Faint, 2, 22)
        ANSI_EFFECT(Italic, 3, 23)
        ANSI_EFFECT(Underline, 4, 24)
        ANSI_EFFECT(Strikethrough, 9, 29)
        ANSI_EFFECT(Blink, 5, 25)
        ANSI_EFFECT(Invert, 7, 27)

        ANSI_COLOUR(White, 255, 255, 255)
        ANSI_COLOUR(Grey, 192, 192, 192)
        ANSI_COLOUR(Red, 128, 0, 0)
        ANSI_COLOUR(Green, 0, 128, 0)
        ANSI_COLOUR(Blue, 0, 0, 128)
        ANSI_COLOUR(Purple, 255, 85, 255)
    }

    template<class... Args> void info(std::format_string<Args...> fmt, Args&&... args) {
        std::cout << ANSI::Colour::FG::White << "[INFO] " <<
            std::vformat(fmt.get(), std::make_format_args(args...))
            << '\n' << ANSI::Reset;
    }

    template<class... Args> void debug(std::format_string<Args...> fmt, Args&&... args) {
        #ifdef DEBUG
        std::cout << ANSI::Colour::FG::Purple << "[DEBUG] " <<
            std::vformat(fmt.get(), std::make_format_args(args...))
            << '\n' << ANSI::Reset;
        #endif
    }
}