#pragma once

struct info {
    static constexpr auto TEXT_COLOUR = fmt::color::white;
    static constexpr auto TITLE_COLOUR = fmt::rgb(255, 220, 255);
};
struct debug {
    static constexpr auto TEXT_COLOUR = fmt::color::violet;
    static constexpr auto TITLE_COLOUR = fmt::rgb(236, 149, 236);

    struct header {
        static constexpr auto NAME = "debug";
        static constexpr auto TEXT_COLOUR = fmt::rgb(255, 176, 255);
        static constexpr auto TITLE_COLOUR = debug::TITLE_COLOUR;
    };
};

struct success { static constexpr auto TEXT_COLOUR = fmt::color::lawn_green; };
struct failure { static constexpr auto TEXT_COLOUR = fmt::color::tomato; };

struct warning { static constexpr auto TEXT_COLOUR = fmt::rgb(255, 225, 58); };
struct error { static constexpr auto TEXT_COLOUR = fmt::color::tomato; };