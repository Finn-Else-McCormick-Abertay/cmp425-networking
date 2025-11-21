#pragma once

#include <fmt/format.h>
#include <fmt/color.h>

#include <concepts>
#include <typeinfo>

#include <alias/str.h>
#include <alias/utility.h>

namespace console_impl {
    static constexpr auto DATE_COLOUR = fmt::color::turquoise;
    static constexpr auto UNKNOWN_COLOUR = fmt::color::red;

    template<typename T> concept has_name = requires(T) { { T::NAME } -> std::convertible_to<str>; };
    template<typename T> concept has_text_colour = requires(T) { { T::TEXT_COLOUR } -> std::convertible_to<fmt::detail::color_type>; };
    template<typename T> concept has_title_colour = requires(T) { { T::TITLE_COLOUR } -> std::convertible_to<fmt::detail::color_type>; };
    template<typename T> concept has_separator_colour = requires(T) { { T::SEPARATOR_COLOUR } -> std::convertible_to<fmt::detail::color_type>; };

    void print(
        const fmt::text_style& style_text, const fmt::text_style& style_title, const fmt::text_style& style_separator,
        const str& type_name, const str& owner_name, fmt::string_view fmt, fmt::format_args args
    );

    constexpr str clean_type_name(const char* type_name) {
        str working(type_name);
        if      (working.starts_with("class "))  working = working.substr(6);
        else if (working.starts_with("struct ")) working = working.substr(7);
        return working;
    }
}

template<typename Ctx, typename Owner, typename... Args> void print(fmt::format_string<Args...> fmt, Args&&... args) {
    str ctx_name;
    if constexpr(console_impl::has_name<Ctx>) ctx_name = Ctx::NAME;
    else ctx_name = console_impl::clean_type_name(typeid(Ctx).name());
    
    str owner_name;
    if constexpr(!std::same_as<Owner, void>) owner_name = console_impl::clean_type_name(typeid(Owner).name());

    fmt::detail::color_type text_colour = console_impl::UNKNOWN_COLOUR;
    if constexpr (console_impl::has_text_colour<Ctx>) text_colour = Ctx::TEXT_COLOUR;
    fmt::detail::color_type title_colour = text_colour;
    if constexpr (console_impl::has_title_colour<Ctx>) title_colour = Ctx::TITLE_COLOUR;
    fmt::detail::color_type separator_colour = title_colour;
    if constexpr (console_impl::has_separator_colour<Ctx>) separator_colour = Ctx::SEPARATOR_COLOUR;

    console_impl::print(
        fmt::fg(text_colour), fmt::fg(title_colour), fmt::fg(separator_colour),
        ctx_name, owner_name, fmt, fmt::make_format_args(args...)
    );
}

template<typename Ctx, typename... Args> void print(fmt::format_string<Args...> fmt, Args&&... args) { print<Ctx, void>(move(fmt), std::forward<Args>(args)...); }
template<typename Ctx, typename Owner, typename Arg> void print(Arg val) { print<Ctx, Owner>("{}", val); }
template<typename Ctx, typename Arg> void print(Arg val) { print<Ctx, void>("{}", val); }