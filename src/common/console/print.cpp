#include "print.h"

bool console_detail::Settings::no_ansi = false;
#ifdef NDEBUG
bool console_detail::Settings::hide_debug = true;
#else
bool console_detail::Settings::hide_debug = false;
#endif

void console_detail::print(
    const fmt::text_style& style_text, const fmt::text_style& style_title, const fmt::text_style& style_separator,
    const str& type_name, const str& owner_name, fmt::string_view fmt, fmt::format_args args
) {
    using namespace fmt;
    if (console_detail::Settings::no_ansi) fmt::print(
        "[{}{}{}] {}\n",
        type_name,
        owner_name.empty() ? "" : "|",
        owner_name,
        vformat(fmt, args)
    );
    else fmt::print(
        "{}{}{}{}{} {}\n",
        styled("[", style_separator),
        styled(type_name.c_str(), style_title | emphasis::bold),
        styled(owner_name.empty() ? "" : "|", style_separator),
        styled(owner_name.c_str(), style_title | emphasis::italic),
        styled("]", style_separator),
        styled(vformat(fmt, args), style_text)
    );
}