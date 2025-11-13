#include "console.h"

//#include <chrono>
//#include <fmt/chrono.h>

void console_impl::print(
    const fmt::text_style& style_text, const fmt::text_style& style_title, const fmt::text_style& style_separator,
    const str& type_name, const str& owner_name, fmt::string_view fmt, fmt::format_args args
) {
    using namespace fmt;//{:%r} //styled(std::chrono::system_clock::now(), fg(DATE_COLOUR)),
    fmt::print(
        "{}{}{}{}{} {}\n",
        styled("[", style_separator),
        styled(type_name, style_title | emphasis::bold),
        styled(owner_name.empty() ? "" : "|", style_separator),
        styled(owner_name, style_title | emphasis::italic),
        styled("]", style_separator),
        styled(vformat(fmt, args), style_text)
    );
}